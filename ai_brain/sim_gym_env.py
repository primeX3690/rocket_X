
import socket
import json
import numpy as np

class FlightSimGymEnv:
    """
    100% Real UDP-Connected RL Environment.
    Talks directly to the C++ 5kHz Physics Engine via UDP Sockets.
    """
    def __init__(self, sim_host="127.0.0.1", sim_port=9090, local_port=8080):
        self.sim_address = (sim_host, sim_port)
        
        # UDP Socket for sending actions to C++ backend and receiving telemetry
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(("127.0.0.1", local_port))
        self.sock.settimeout(2.0) # 2 seconds timeout for safety
        
        self.state_dim = 8  # [x, y, z, vx, vy, vz, mach, dist_to_target]
        self.action_dim = 3 # [fin_pitch, fin_yaw, throttle]
        
        self.max_steps = 1000
        self.current_step = 0

    def reset(self):
        self.current_step = 0
        
        # Send RESET command to C++ backend
        reset_packet = json.dumps({"command": "RESET"})
        try:
            self.sock.sendto(reset_packet.encode('utf-8'), self.sim_address)
            data, _ = self.sock.recvfrom(1024)
            state_data = json.loads(data.decode('utf-8'))
            return self._parse_state(state_data)
        except Exception as e:
            print(f"⚠️ Warning: Could not connect to C++ Sim on reset ({e}). Returning zero state.")
            return np.zeros(self.state_dim)

    def step(self, action):
        self.current_step += 1
        action = np.clip(action, -1.0, 1.0)
        
        # Send CONTROL ACTION packet to C++ backend
        action_packet = json.dumps({
            "command": "STEP",
            "fin_pitch": float(action[0]),
            "fin_yaw": float(action[1]),
            "throttle": float(action[2])
        })
        
        try:
            self.sock.sendto(action_packet.encode('utf-8'), self.sim_address)
            data, _ = self.sock.recvfrom(1024)
            packet = json.loads(data.decode('utf-8'))
            
            state = self._parse_state(packet)
            dist_to_target = state[7]
            altitude = state[2]
            
            # Real Reward Function based on physical simulation outcomes
            reward = -0.001 * dist_to_target - 0.1 * np.sum(np.square(action))
            
            done = False
            if dist_to_target < 50.0:
                reward += 1000.0  # Target Hit!
                done = True
            elif altitude <= 0.0 or self.current_step >= self.max_steps:
                reward -= 500.0   # Crash or Timeout
                done = True
                
            return state, reward, done, packet

        except Exception as e:
            print(f"⚠️ UDP Communication Error in step(): {e}")
            return np.zeros(self.state_dim), -100.0, True, {}

    def _parse_state(self, packet):
        pos = packet.get("position", [0.0, 0.0, 100.0])
        vel = packet.get("velocity", [300.0, 0.0, 0.0])
        mach = packet.get("mach_number", 0.85)
        
        # Compute distance to fixed target (e.g., 10km downrange, 1km altitude)
        target_pos = np.array([10000.0, 0.0, 1000.0])
        current_pos = np.array(pos)
        dist = np.linalg.norm(target_pos - current_pos)
        
        return np.array([
            pos[0], pos[1], pos[2],
            vel[0], vel[1], vel[2],
            mach,
            dist
        ])
