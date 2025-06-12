import numpy as np

GRAVITY = np.array([0, -9.8])

class Particle:
    def __init__(self, pos, vel, radius=0.02, is_splash=False):
        self.pos = np.array(pos, dtype=float)
        self.vel = np.array(vel, dtype=float)
        self.radius = radius
        self.is_splash = is_splash
        self.sliding = False

    def update(self, dt):
        next_vel = self.vel + GRAVITY * dt
        next_pos = self.pos + next_vel * dt

        if next_pos[1] <= self.radius:
            self.pos[1] = self.radius
            self.vel = np.zeros_like(self.vel)
        else:
            self.vel = next_vel
            self.pos = next_pos

        self.pos[0] = np.clip(self.pos[0], 0.0, 1.0)
