


import streamlit as st
import plotly.graph_objects as go
import socket
import json
import pandas as pd
from collections import deque

# Page Configuration
st.set_page_config(
    page_title="Hybrid 6-DOF Ground Control Dashboard",
    page_icon="🚀",
    layout="wide"
)

st.title("🚀 HYBRID 6-DOF SIMULATOR - LIVE TELEMETRY COMMAND CENTER")
st.markdown("Real-time Flight Dynamics, GNC State Estimation, and Actuator Telemetry Stream.")

# Sidebar Controls
st.sidebar.header("Mission Control")
target_x = st.sidebar.slider("Target X Waypoint (m)", 5000.0, 20000.0, 10000.0)
target_z = st.sidebar.slider("Target Altitude (m)", 100.0, 5000.0, 1000.0)
connect_status = st.sidebar.empty()

# Initialize Session State for rolling data history
if "history" not in st.session_state:
    st.session_state.history = deque(maxlen=100)

# UDP Socket configuration to listen to Rust telemetry packets
UDP_IP = "127.0.0.1"
UDP_PORT = 8080

@st.cache_resource
def get_udp_socket():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))
    sock.setblocking(False)
    return sock

try:
    sock = get_udp_socket()
    connect_status.success("Status: Connected to Simulation UDP Stream 🟢")
except Exception as e:
    connect_status.warning("Status: Waiting for Simulator Backend... 🟡")

# Layout Columns for Metrics
col1, col2, col3, col4 = st.columns(4)
metric_alt = col1.metric("Altitude (MSL)", "0.0 m")
metric_mach = col2.metric("Mach Number", "0.00")
metric_vel = col3.metric("Velocity", "0.0 m/s")
metric_pitch = col4.metric("Pitch Cmd", "0.00 rad")

# Main Plot Area
chart_col1, chart_col2 = st.columns(2)

with chart_col1:
    st.subheader("📍 3D Trajectory Profile")
    trajectory_placeholder = st.empty()

with chart_col2:
    st.subheader("📈 Mach Number & Velocity Profile")
    velocity_placeholder = st.empty()

# Live Data Receiver & Plot Loop
if st.button("Start Live Telemetry Feed"):
    placeholder_text = st.empty()
    placeholder_text.text("Listening for live flight packets...")
    
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            packet = json.loads(data.decode("utf-8"))
            
            # Extract packet fields
            t = packet.get("timestamp_ms", 0) / 1000.0
            pos = packet.get("position", [0, 0, 0])
            vel = packet.get("velocity", [0, 0, 0])
            mach = packet.get("mach_number", 0.0)
            defs = packet.get("control_deflections", [0, 0, 0])
            
            v_mag = (vel[0]**2 + vel[1]**2 + vel[2]**2)**0.5
            
            # Append to rolling history
            st.session_state.history.append({
                "time": t,
                "x": pos[0],
                "y": pos[1],
                "z": pos[2],
                "mach": mach,
                "velocity": v_mag,
                "pitch": defs[0]
            })
            
            # Update Metrics UI
            col1.metric("Altitude (MSL)", f"{pos[2]:.2f} m")
            col2.metric("Mach Number", f"{mach:.2f}")
            col3.metric("Velocity", f"{v_mag:.2f} m/s")
            col4.metric("Pitch Cmd", f"{defs[0]:.4f} rad")
            
            # Convert history to DataFrame for plotting
            df = pd.DataFrame(st.session_state.history)
            
            if not df.empty:
                # 3D Trajectory Plot
                fig_3d = go.Figure(data=[go.Scatter3d(
                    x=df['x'], y=df['y'], z=df['z'],
                    mode='lines+markers',
                    marker=dict(size=3, color=df['z'], colorscale='Viridis'),
                    line=dict(color='red', width=4)
                )])
                fig_3d.update_layout(scene=dict(
                    xaxis_title='Downrange X (m)',
                    yaxis_title='Crossrange Y (m)',
                    zaxis_title='Altitude Z (m)'
                ), margin=dict(l=0, r=0, b=0, t=0), height=350)
                trajectory_placeholder.plotly_chart(fig_3d, use_container_width=True)
                
                # Velocity & Mach Line Chart
                fig_vel = go.Figure()
                fig_vel.add_trace(go.Scatter(x=df['time'], y=df['velocity'], mode='lines', name='Velocity (m/s)', line=dict(color='cyan')))
                fig_vel.update_layout(margin=dict(l=0, r=0, b=0, t=0), height=350, xaxis_title='Time (s)', yaxis_title='Speed')
                velocity_placeholder.plotly_chart(fig_vel, use_container_width=True)

        except BlockingIOError:
            pass
        except Exception as ex:
            break