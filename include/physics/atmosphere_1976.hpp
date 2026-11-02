


#ifndef ATMOSPHERE_1976_HPP
#define ATMOSPHERE_1976_HPP

namespace Physics {

struct AtmosphericData {
    double altitude;     // Geopotential altitude [m]
    double temperature;  // Absolute temperature [K]
    double pressure;     // Static pressure [Pa]
    double density;      // Air density [kg/m^3]
    double speed_of_sound; // Local acoustic velocity [m/s]
};

class Atmosphere1976 {
public:
    Atmosphere1976() = default;
    ~Atmosphere1976() = default;

    // Computes atmospheric properties given geometric/geopotential altitude in meters
    AtmosphericData get_properties(double altitude_meters);
};

} // namespace Physics

#endif // ATMOSPHERE_1976_HPP