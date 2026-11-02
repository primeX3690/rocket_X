

// Add inside EomSolver class:
public:
    void update_mass_and_inertia(double new_mass, const Eigen::Matrix3d& new_inertia);
    void apply_stage_separation_impulse(StateVector& state, double separation_delta_v);
