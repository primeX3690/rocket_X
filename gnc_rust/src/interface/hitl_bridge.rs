


use crate::interface::telemetry::TelemetryPacket;
use std::net::UdpSocket;

pub struct HitlBridge {
    socket: UdpSocket,
    target_addr: String,
}

impl HitlBridge {
    pub fn new(bind_addr: &str, target_addr: &str) -> Result<Self, std::io::Error> {
        let socket = UdpSocket::bind(bind_addr)?;
        socket.set_nonblocking(true)?;
        Ok(Self {
            socket,
            target_addr: target_addr.to_string(),
        })
    }

    pub fn transmit_telemetry(&self, packet: &TelemetryPacket) {
        if let Ok(json_data) = packet.serialize_to_json() {
            let _ = self.socket.send_to(json_data.as_bytes(), &self.target_addr);
        }
    }
}