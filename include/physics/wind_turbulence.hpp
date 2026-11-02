


#ifndef WIND_TURBULENCE_HPP
#define WIND_TURBULENCE_HPP

#include <Eigen/Dense>
#include <random>

namespace Physics {

class WindTurbulence {
public:
    WindTurbulence(double turbulence_intensity);
    ~WindTurbulence() = default;

    // Generates stochastic wind velocity vector perturbations [m/s]
    Eigen::Vector3d get_wind_gust(double dt);

private:
    std::default_random_engine generator_;
    std::normal_distribution<double> noise_dist_;
    double intensity_;
    Eigen::Vector3d current_gust_;
};

} // namespace Physics

#endif // WIND_TURBULENCE_HPP