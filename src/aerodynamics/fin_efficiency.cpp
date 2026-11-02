


#include "aerodynamics/fin_efficiency.hpp"
#include <cmath>
#include <algorithm>

namespace Aerodynamics {

FinEfficiency::FinEfficiency(double reference_area, double reference_length)
    : ref_area_(reference_area), ref_length_(reference_length), effectiveness_factor_(2.5) {}

void FinEfficiency::compute_control_loads(double dynamic_pressure, double mach, 
                                         const ControlDeflections& deflections, 
                                         Eigen::Vector3d& out_forces_body, 
                                         Eigen::Vector3d& out_moments_body) {
    // Supersonic Mach effectiveness decay factor (Ackeret's rule approximation)
    double mach_factor = (mach > 1.0) ? (1.0 / std::sqrt(mach * mach - 1.0)) : 1.0;
    double effective_gain = effectiveness_factor_ * mach_factor * dynamic_pressure * ref_area_;

    // Control forces induced by fin deflection (primarily normal forces)
    double normal_force_z = effective_gain * deflections.delta_pitch;
    double side_force_y = effective_gain * deflections.delta_yaw;

    out_forces_body << 0.0, side_force_y, normal_force_z;

    // Control moments induced by fin deflection (Pitch, Yaw, Roll moments)
    // Moment = Force * Reference Length (moment arm)
    double pitching_moment = normal_force_z * ref_length_ * 0.5;
    double yawing_moment = -side_force_y * ref_length_ * 0.5;
    double rolling_moment = effective_gain * ref_length_ * 0.2 * deflections.delta_roll;

    out_moments_body << rolling_moment, pitching_moment, yawing_moment;
}

} // namespace Aerodynamics