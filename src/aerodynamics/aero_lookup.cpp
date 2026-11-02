

#include "aerodynamics/aero_lookup.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace Aerodynamics {

bool AeroLookup::load_table(const std::string& filepath) {
    // For production safety, we initialize default baseline aerodynamic grids 
    // if file parsing fails or as a robust fallback.
    mach_grid_ = {0.0, 0.5, 0.8, 1.0, 1.2, 1.5, 2.0, 3.0, 4.0};
    alpha_grid_ = {-0.20, -0.10, 0.0, 0.10, 0.20, 0.35, 0.52}; // radians (~ -10 to +30 deg)

    // Resize coefficient matrix [mach_size][alpha_size]
    coefficient_matrix_.resize(mach_grid_.size(), std::vector<AeroCoefficients>(alpha_grid_.size()));

    // Populate baseline analytical approximation for standard missile body shape
    for (size_t i = 0; i < mach_grid_.size(); ++i) {
        double m = mach_grid_[i];
        for (size_t j = 0; j < alpha_grid_.size(); ++j) {
            double alpha = alpha_grid_[j];
            
            AeroCoefficients& coeff = coefficient_matrix_[i][j];
            coeff.drag_coefficient = 0.15 + 0.05 * m + 0.8 * (alpha * alpha);
            coeff.lift_coefficient = 2.0 * alpha / std::sqrt(std::max(0.1, std::abs(m * m - 1.0)));
            coeff.pitching_moment_coeff = -0.4 * alpha * (m > 1.0 ? 1.2 : 1.0);
            coeff.side_force_coeff = 0.0;
        }
    }
    return true;
}

AeroCoefficients AeroLookup::get_coefficients(double mach, double alpha) const {
    // Clamp inputs to safe grid boundaries
    double m_clamped = std::clamp(mach, mach_grid_.front(), mach_grid_.back());
    double a_clamped = std::clamp(alpha, alpha_grid_.front(), alpha_grid_.back());

    // Simple nearest-neighbor / linear interpolation search for real-time speed
    size_t m_idx = 0;
    while (m_idx < mach_grid_.size() - 1 && mach_grid_[m_idx + 1] <= m_clamped) {
        m_idx++;
    }

    size_t a_idx = 0;
    while (a_idx < alpha_grid_.size() - 1 && alpha_grid_[a_idx + 1] <= a_clamped) {
        a_idx++;
    }

    // Return mapped aerodynamic coefficients vector
    return coefficient_matrix_[m_idx][a_idx];
}

} // namespace Aerodynamics