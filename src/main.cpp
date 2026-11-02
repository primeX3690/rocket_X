#include <iostream>
#include <chrono>
#include <thread>
#include <Eigen/Dense>

#include "physics/eom_solver.hpp"
#include "physics/atmosphere_1976.hpp"
#include "aerodynamics/aero_lookup.hpp"
#include "aerodynamics/fin_efficiency.hpp"
#include "bridge/rust_gnc_ffi.hpp"
#include "physics/wind_turbulence.hpp"

// Initialize wind turbulence (intensity 3.5 m/s gust profile)
Physics::WindTurbulence wind_model(3.5);

// Inside the 5kHz loop:
// 1. Get wind gust perturbation
Eigen::Vector3d wind_gust = wind_model.get_wind_gust(dt);

// 2. Adjust relative velocity for aerodynamics
Eigen::Vector3d relative_velocity = state.velocity - wind_gust;
double v_mag = relative_velocity.norm();
double mach = v_mag / atm_data.speed_of_sound;
double alpha = std::atan2(relative_velocity.z(), std::max(1.0, relative_velocity.x()));




// C-driver extern declarations
extern "C" {
    #include "__init_hardware.h"
    void imu_spi_init(void);
    int8_t imu_spi_read_raw(int16_t* accel_out, int16_t* gyro_out);
    void pwm_actuator_init(void);
    void pwm_set_actuator_deflections(double pitch, double yaw, double roll);
}

int main() {
    std::cout << "=== HYBRID 6-DOF SIMULATOR INITIALIZING (5kHz Real-Time Core) ===" << std::endl;

    // 1. Initialize Bare-Metal Drivers
    imu_spi_init();
    pwm_actuator_init();

    // 2. Initialize Physics & Aero Modules
    double mass = 50.0; // [kg]
    Eigen::Matrix3d inertia;
    inertia << 2.5, 0.0, 0.0,
               0.0, 45.0, 0.0,
               0.0, 0.0, 45.0;

    Physics::EomSolver eom(mass, inertia);
    Physics::Atmosphere1976 atmos;
    Aerodynamics::AeroLookup aero_table;
    aero_table.load_table("data/aero_cfd_matrix.dat");
    Aerodynamics::FinEfficiency fins(0.12, 2.0); // Ref Area 0.12 m^2, Length 2.0 m

    // 3. Initialize Rust GNC Subsystem Context (Target at X=10000m, Y=0, Z=1000m)
    RustGncContext* gnc_ctx = rust_gnc_init(10000.0, 0.0, 1000.0);
    if (!gnc_ctx) {
        std::cerr << "CRITICAL ERROR: Failed to initialize Rust GNC context!" << std::endl;
        return -1;
    }

    // 4. Set Initial Flight State Vector
    Physics::StateVector state;
    state.position << 0.0, 0.0, 100.0;     // Altitude 100m
    state.velocity << 300.0, 0.0, 0.0;     // Mach ~1.0 initial velocity
    state.orientation = Eigen::Quaterniond::Identity();
    state.body_rates.setZero();

    // 5. 5kHz Loop Timing Constants (dt = 0.2 milliseconds = 200 microseconds)
    constexpr double dt = 0.0002;
    auto next_tick = std::chrono::high_resolution_clock::now();

    std::cout << "=== CORE LOOP RUNNING AT 5kHz. PRESS CTRL+C TO TERMINATE. ===" << std::endl;

    uint64_t iteration_count = 0;
    while (iteration_count < 25000) { // Run for 5 seconds as a test sequence
        auto start_time = std::chrono::high_resolution_clock::now();

        // Step A: Read raw sensors from bare-metal C drivers
        int16_t raw_accel[3], raw_gyro[3];
        imu_spi_read_raw(raw_accel, raw_gyro);

        // Step B: Get atmospheric properties based on current altitude
        double altitude = state.position.z();
        auto atm_data = atmos.get_properties(altitude);

        // Step C: Compute aerodynamic forces & moments
        double v_mag = state.velocity.norm();
        double mach = v_mag / atm_data.speed_of_sound;
        double alpha = std::atan2(state.velocity.z(), std::max(1.0, state.velocity.x()));
        
        auto aero_coeffs = aero_table.get_coefficients(mach, alpha);
        double dynamic_pressure = 0.5 * atm_data.density * v_mag * v_mag;

        Eigen::Vector3d aero_forces;
        aero_forces << -aero_coeffs.drag_coefficient * dynamic_pressure * 0.12,
                       aero_coeffs.side_force_coeff * dynamic_pressure * 0.12,
                       -aero_coeffs.lift_coefficient * dynamic_pressure * 0.12;

        Eigen::Vector3d aero_moments;
        aero_moments << 0.0, aero_coeffs.pitching_moment_coeff * dynamic_pressure * 0.12 * 2.0, 0.0;

        // Step D: Call Rust GNC via FFI for navigation and guidance commands
        FfiNavState ffi_state{
            state.position.x(), state.position.y(), state.position.z(),
            state.velocity.x(), state.velocity.y(), state.velocity.z(),
            state.orientation.w(), state.orientation.x(), state.orientation.y(), state.orientation.z(),
            state.body_rates.x(), state.body_rates.y(), state.body_rates.z()
        };

        FfiGuidanceCommands gnc_cmd;
        rust_gnc_update(gnc_ctx, &ffi_state, &gnc_cmd);

        // Step E: Compute fin control actuator loads from GNC commands
        Aerodynamics::ControlDeflections deflections{gnc_cmd.delta_pitch, gnc_cmd.delta_yaw, gnc_cmd.delta_roll};
        Eigen::Vector3d control_forces(Eigen::Vector3d::Zero());
        Eigen::Vector3d control_moments(Eigen::Vector3d::Zero());
        fins.compute_control_loads(dynamic_pressure, mach, deflections, control_forces, control_moments);

        // Send actuation signals to C driver PWM registers
        pwm_set_actuator_deflections(gnc_cmd.delta_pitch, gnc_cmd.delta_yaw, gnc_cmd.delta_roll);

        // Step F: Integrate 6-DOF Equations of Motion (RK4 Solver)
        Eigen::Vector3d total_forces = aero_forces + control_forces;
        Eigen::Vector3d total_moments = aero_moments + control_moments;
        eom.step(state, total_forces, total_moments, dt);

        // Periodic logging every 1000 iterations (0.2 seconds)
        if (iteration_count % 1000 == 0) {
            std::cout << "[Iter " << iteration_count << "] Alt: " << state.position.z() 
                      << " m | Mach: " << mach 
                      << " | Pitch Cmd: " << gnc_cmd.delta_pitch << " rad" << std::endl;
        }

        iteration_count++;

        // Strict 5kHz timing synchronization delay
        next_tick += std::chrono::microseconds(200);
        std::this_thread::sleep_until(next_tick);
    }

    // Cleanup FFI context
    rust_gnc_destroy(gnc_ctx);
    std::cout << "=== HYBRID 6-DOF SIMULATOR SHUTTING DOWN SAFELY ===" << std::endl;
    return 0;
}


