


void EomSolver::update_mass_and_inertia(double new_mass, const Eigen::Matrix3d& new_inertia) {
    mass_ = new_mass;
    inertia_tensor_ = new_inertia;
    inv_inertia_tensor_ = inertia_tensor_.inverse();
}

void EomSolver::apply_stage_separation_impulse(StateVector& state, double separation_delta_v) {
    // Stage separation transient kick (e.g., spring push-off or ullage motors)
    state.velocity.x() += separation_delta_v;
}
