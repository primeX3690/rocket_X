


use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct TelemetryPacket {
    pub timestamp_ms: u64,
    pub position: [f64; 3],
    pub velocity: [f64; 3],
    pub quaternion: [f64; 4],
    pub body_rates: [f64; 3],
    pub control_deflections: [f64; 3],
    pub mach_number: f64,
}

impl TelemetryPacket {
    pub fn new(
        timestamp_ms: u64,
        pos: [f64; 3],
        vel: [f64; 3],
        quat: [f64; 4],
        rates: [f64; 3],
        defs: [f64; 3],
        mach: f64,
    ) -> Self {
        Self {
            timestamp_ms,
            position: pos,
            velocity: vel,
            quaternion: quat,
            body_rates: rates,
            control_deflections: defs,
            mach_number: mach,
        }
    }

    pub fn serialize_to_json(&self) -> Result<String, serde_json::Error> {
        serde_json::to_string(self)
    }
}