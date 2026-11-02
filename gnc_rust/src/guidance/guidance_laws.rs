


pub struct ProportionalNavigation {
    target_pos: [f64; 3],
    navigation_constant: f64,
}

impl ProportionalNavigation {
    pub fn new(tx: f64, ty: f64, tz: f64) -> Self {
        Self {
            target_pos: [tx, ty, tz],
            navigation_constant: 3.0, // Standard navigation gain N = 3.0
        }
    }

    pub fn compute_command(&self, x: f64, y: f64, z: f64, vx: f64, vy: f64, vz: f64) -> (f64, f64, f64) {
        let rx = self.target_pos[0] - x;
        let ry = self.target_pos[1] - y;
        let rz = self.target_pos[2] - z;

        let r_mag = (rx*rx + ry*ry + rz*rz).sqrt();
        if r_mag < 1e-3 {
            return (0.0, 0.0, 0.0);
        }

        let v_mag = (vx*vx + vy*vy + vz*vz).sqrt().max(1.0);
        
        let pitch_cmd = (rz / r_mag) * self.navigation_constant * (v_mag / 500.0);
        let yaw_cmd = (ry / r_mag) * self.navigation_constant * (v_mag / 500.0);
        let roll_cmd = 0.0;

        (pitch_cmd.clamp(-0.5, 0.5), yaw_cmd.clamp(-0.5, 0.5), roll_cmd)
    }
}