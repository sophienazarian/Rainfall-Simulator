# Rain It In

A real-time, physically-based rain simulation that models droplet dynamics, splash behavior, and surface interactions under varying rain intensities. Built using OpenGL, C++, and GLSL, the simulation integrates particle systems, gravity-driven motion, and procedural splash rendering to achieve visually and physically plausible rainfall behavior.

---

## Description

**Rain It In** simulates rain as a collection of discrete particles—each droplet represented by a dynamic entity governed by physical laws. We simulate and visualize:

- **Droplet motion** under gravity and wind forces
- **Collision responses** (e.g., bounce, splash, pooling) based on surface geometry and droplet momentum
- **Splash effects** dependent on droplet size, angle, and velocity
- **Secondary splash behavior** under high rainfall intensity

Key simulation goals include scalability under dense rain, believable splashing physics, and seamless integration with real-time rendering techniques. We implemented a custom integrator and collision pipeline to handle thousands of particles interacting with both flat and complex surfaces.

---

## Technical Approach

### Particle System

Each rain droplet is treated as a particle with:

- Position, velocity, acceleration vectors
- Mass proportional to size
- Lifecycle state (falling, impacting, splashing)

We solve the equations of motion using an explicit Euler or semi-implicit integrator.

### Collision & Surface Interaction

- Collision detection is performed against mesh geometry (ground plane, obstacles)
- Response varies by surface normal and rain intensity:
  - Low velocity → droplet sticks/drips
  - High velocity → splash with secondary particles

### Splash Modeling

Splash particles are procedurally generated on impact using:

- Angular dispersion kernels
- Energy-based radial velocity sampling
- Conservation of momentum (approximated)

### Rendering

- OpenGL with GLSL shaders
- Real-time shadowing and transparency for water droplets
- Camera controls for inspecting splash zones and droplet fields

---

## Getting Started

### Dependencies

- C++17 or later
- OpenGL 3.3+
- GLFW
- GLEW or glad
- Linux/macOS/Windows (makefile provided for Unix-like systems)

### Build & Run Instructions

1. Navigate to the `/3d_sim` directory:

```bash
cd 3d_sim
make
./3d_simulation
