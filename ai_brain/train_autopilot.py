import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
from sim_gym_env import FlightSimGymEnv

class NeuralAutopilotPolicy(nn.Module):
    """
    Neural Network Actor-Critic Architecture for Missile/Drone Autopilot Brain.
    Takes flight state telemetry and outputs optimal fin deflection actions.
    """
    def __init__(self, state_dim, action_dim):
        super(NeuralAutopilotPolicy, self).__init__()
        # Actor Network (Policy)
        self.actor = nn.Sequential(
            nn.Linear(state_dim, 128),
            nn.Tanh(),
            nn.Linear(128, 128),
            nn.Tanh(),
            nn.Linear(128, action_dim),
            nn.Tanh() # Output actions between -1 and 1
        )
        # Critic Network (Value Function)
        self.critic = nn.Sequential(
            nn.Linear(state_dim, 128),
            nn.Tanh(),
            nn.Linear(128, 128),
            nn.Tanh(),
            nn.Linear(128, 1)
        )

    def forward(self, state):
        return self.actor(state), self.critic(state)

def train_neural_autopilot():
    env = FlightSimGymEnv()
    state_dim = env.state_dim
    action_dim = env.action_dim
    
    policy = NeuralAutopilotPolicy(state_dim, action_dim)
    optimizer = optim.Adam(policy.parameters(), lr=3e-4)
    
    print("🚀 Training Neural Autopilot Brain (Phase 2 AI Integration)...")
    
    num_episodes = 200
    for episode in range(num_episodes):
        state = env.reset()
        episode_reward = 0
        
        for step in range(env.max_steps):
            state_tensor = torch.FloatTensor(state).unsqueeze(0)
            action_logits, value = policy(state_tensor)
            
            # Sample action with exploration noise
            action = action_logits.detach().numpy()[0] + np.random.normal(0, 0.1, size=action_dim)
            
            next_state, reward, done, _ = env.step(action)
            episode_reward += reward
            
            state = next_state
            if done:
                break
                
        if (episode + 1) % 10 == 0:
            print(f"Episode [{episode+1}/{num_episodes}] | Total Reward: {episode_reward:.2f}")

    # Save trained Neural Autopilot weights for edge deployment
    torch.save(policy.state_dict(), "neural_autopilot_weights.pth")
    print("✅ Training Complete! Neural Autopilot Brain saved as 'neural_autopilot_weights.pth'.")

if __name__ == "__main__":
    train_neural_autopilot()




import struct

def export_weights_for_embedded_c_rust(model_path="neural_autopilot_weights.pth", output_bin="model_weights.bin"):
    """
    Exports PyTorch model weights into a raw binary format (.bin) 
    so that the C++/Rust air-gapped flight computer can load them directly.
    """
    print("📦 Exporting weights for air-gapped embedded flight computer...")
    state_dict = torch.load(model_path)
    
    with open(output_bin, "wb") as f:
        for name, param in state_dict.items():
            # Flatten tensor weights to 1D array
            weights_np = param.detach().cpu().numpy().flatten()
            # Write raw float32 bytes directly to binary file
            for w in weights_np:
                f.write(struct.pack('f', float(w)))
                
    print(f"✅ Success! Embedded-ready binary weights saved as '{output_bin}'.")
    print("   (Yeh .bin file ab kisi bhi air-gapped C/C++ flight computer par flash ki ja sakti hai.)")

# Training khatam hone ke baad is function ko call kar lo:
# export_weights_for_embedded_c_rust()