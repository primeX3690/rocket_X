


#ifndef RUST_GNC_FFI_HPP
#define RUST_GNC_FFI_HPP

#include <cstdint>

extern "C" {

// Opaque pointer to Rust GNC controller state
struct RustGncContext;

// FFI struct matching Rust's memory layout for navigation exchange
struct FfiNavState {
    double x, y, z;          // Position [m]
    double vx, vy, vz;       // Velocity [m/s]
    double q0, q1, q2, q3;   // Attitude Quaternion
    double p, q, r;          // Body rates [rad/s]
};

struct FfiGuidanceCommands {
    double delta_pitch;      // Canard pitch deflection command [rad]
    double delta_yaw;        // Fin yaw deflection command [rad]
    double delta_roll;       // Roll command [rad]
    uint8_t mode_flag;       // Flight mode status indicator
};

// Lifecycle functions
RustGncContext* rust_gnc_init(double target_x, double target_y, double target_z);
void rust_gnc_destroy(RustGncContext* ctx);

// Core GNC update step called at high frequency from C++ core loop
void rust_gnc_update(RustGncContext* ctx, const FfiNavState* state, FfiGuidanceCommands* out_cmd);

}

#endif // RUST_GNC_FFI_HPP