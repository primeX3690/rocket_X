


#ifndef FIN_EFFICIENCY_HPP
#define FIN_EFFICIENCY_HPP

#include <Eigen/Dense>

namespace Aerodynamics {

struct ControlDeflections {
    double delta_pitch; // Canard/Elevon pitch deflection [rad]
    double delta_yaw;   // Fin yaw deflection [rad]
    double delta_roll;  // Differential roll fin deflection [rad]
};

class FinEfficiency {
public:
    FinEfficiency(double reference_area, double reference_length);
    ~FinEfficiency() = default;

    // Computes control force and moment increments due to fin deflections
    void compute_control_loads(double dynamic_pressure, double mach, 
                               const ControlDeflections& deflections, 
                               Eigen::Vector3d& out_forces_body, 
                               Eigen::Vector3d& out_moments_body);

private:
    double ref_area_;
    double ref_length_;
    double effectiveness_factor_;
};

} // namespace Aerodynamics

#endif // FIN_EFFICIENCY_HPP