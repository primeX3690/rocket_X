


#ifndef AERO_LOOKUP_HPP
#define AERO_LOOKUP_HPP

#include <vector>
#include <utility>

namespace Aerodynamics {

struct AeroCoefficients {
    double drag_coefficient;      // CD
    double lift_coefficient;      // CL
    double pitching_moment_coeff; // Cm
    double side_force_coeff;      // CY
};

class AeroLookup {
public:
    AeroLookup() = default;
    ~AeroLookup() = default;

    // Load aerodynamic table data (Mach vs Alpha interpolation tables)
    bool load_table(const std::string& filepath);

    // Interpolate coefficients based on current Mach number and Angle of Attack [rad]
    AeroCoefficients get_coefficients(double mach, double alpha) const;

private:
    // Internal table representation for multi-dimensional interpolation
    std::vector<double> mach_grid_;
    std::vector<double> alpha_grid_;
    std::vector<std::vector<AeroCoefficients>> coefficient_matrix_;
};

} // namespace Aerodynamics

#endif // AERO_LOOKUP_HPP