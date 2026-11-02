


#include "physics/wind_turbulence.hpp"
#include <algorithm>

namespace Physics {

WindTurbulence::WindTurbulence(double turbulence_intensity)
    : generator_(1337), noise_dist_(0.0, 1.0), intensity_(turbulence_intensity) {
    current_gust_.setZero();
}

Eigen::Vector3d WindTurbulence::get_wind_gust(double dt) {
    // First-order Gauss-Markov process (Dryden turbulence approximation)
    double alpha_dec = std::exp(-dt * 2.0); // decay factor
    
    Eigen::Vector3d white_noise(
        noise_dist_(generator_),
        noise_dist_(generator_),
        noise_dist_(generator_)
    );

    current_gust_ = current_gust_ * alpha_dec + white_noise * (intensity_ * std::sqrt(dt));
    return current_gust_;
}

} // namespace Physics