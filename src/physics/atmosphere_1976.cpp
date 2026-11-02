


#include "physics/atmosphere_1976.hpp"
#include <cmath>
#include <algorithm>

namespace Physics {

// Standard Constants
constexpr double SEA_LEVEL_TEMP = 288.15;    // [K]
constexpr double SEA_LEVEL_PRESS = 101325.0;  // [Pa]
constexpr double SEA_LEVEL_DENSITY = 1.225;   // [kg/m^3]
constexpr double GRAVITY = 9.80665;           // [m/s^2]
constexpr double R_GAS = 287.05287;           // Specific gas constant for air [J/(kg*K)]
constexpr double GAMMA = 1.4;                 // Ratio of specific heats for air

AtmosphericData Atmosphere1976::get_properties(double altitude_meters) {
    AtmosphericData data;
    data.altitude = std::max(0.0, altitude_meters);

    // Troposphere layer model (0 - 11,000 meters)
    if (data.altitude <= 11000.0) {
        double lapse_rate = -0.0065; // Temperature lapse rate [K/m]
        data.temperature = SEA_LEVEL_TEMP + lapse_rate * data.altitude;
        data.pressure = SEA_LEVEL_PRESS * std::pow(data.temperature / SEA_LEVEL_TEMP, -GRAVITY / (lapse_rate * R_GAS));
    }
    // Lower Stratosphere layer model (11,000 - 20,000 meters)
    else if (data.altitude <= 20000.0) {
        double temp_11k = 216.65;
        double press_11k = 22632.1;
        data.temperature = temp_11k; // Isothermal
        data.pressure = press_11k * std::exp(-GRAVITY * (data.altitude - 11000.0) / (R_GAS * temp_11k));
    }
    // Upper Stratosphere (above 20,000 meters fallback)
    else {
        double temp_20k = 216.65;
        double press_20k = 5474.89;
        double lapse_rate = 0.001;
        data.temperature = temp_20k + lapse_rate * (data.altitude - 20000.0);
        data.pressure = press_20k * std::pow(data.temperature / temp_20k, -GRAVITY / (lapse_rate * R_GAS));
    }

    // Density computation: rho = P / (R * T)
    data.density = data.pressure / (R_GAS * data.temperature);

    // Local Speed of Sound: a = sqrt(gamma * R * T)
    data.speed_of_sound = std::sqrt(GAMMA * R_GAS * data.temperature);

    return data;
}

} // namespace Physics