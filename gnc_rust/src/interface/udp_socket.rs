


use tokio::net::UdpSocket;
use std::net::SocketAddr;
use std::error::Error;

pub struct SafeUdpSocket {
    socket: Option<UdpSocket>,
    target_addr: SocketAddr,
}

impl SafeUdpSocket {
    pub async fn new(bind_addr: &str, target_addr_str: &str) -> Result<Self, Box<dyn Error>> {
        let socket = UdpSocket::bind(bind_addr).await?;
        let target_addr: SocketAddr = target_addr_str.parse()?;
        Ok(Self {
            socket: Some(socket),
            target_addr,
        })
    }

    pub async fn send_packet(&self, data: &[u8]) -> Result<usize, Box<dyn Error>> {
        if let Some(ref sock) = self.socket {
            let len = sock.send_to(data, &self.target_addr).await?;
            Ok(len)
        } else {
            Err("UDP socket not initialized".into())
        }
    }
}