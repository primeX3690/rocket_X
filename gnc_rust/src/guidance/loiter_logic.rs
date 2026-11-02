


pub struct LoiterPattern {
    center_x: f64,
    center_y: f64,
    radius: f64,
    angle: f64,
}

impl LoiterPattern {
    pub fn new(cx: f64, cy: f64, r: f64) -> Self {
        Self {
            center_x: cx,
            center_y: cy,
            radius: r,
            angle: 0.0,
        }
    }

    pub fn compute_loiter_waypoint(&mut self, current_x: f64, current_y: f64, dt: f64) -> (f64, f64) {
        // Increment circular orbit angle
        self.angle += 0.5 * dt; // angular speed
        if self.angle > 2.0 * std::f64::consts::PI {
            self.angle -= 2.0 * std::f64::consts::PI;
        }

        // Target waypoint on the loiter circle perimeter
        let target_x = self.center_x + self.radius * self.angle.cos();
        let target_y = self.center_y + self.radius * self.angle.sin();

        (target_x, target_y)
    }
}