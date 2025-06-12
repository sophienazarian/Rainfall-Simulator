import numpy as np

class Platform:
    def __init__(self, center=(0.5, 0.25), width=0.4, angle=-np.pi / 6):
        self.center = np.array(center, dtype=float)
        self.width = width
        self.angle = angle

        dx = width / 2
        dy = np.tan(angle) * dx
        self.start = self.center + np.array([-dx, -dy])
        self.end   = self.center + np.array([ dx,  dy])
        self.slope = (self.end[1] - self.start[1]) / (self.end[0] - self.start[0])

    def height_at(self, x):
        return self.start[1] + self.slope * (x - self.start[0])

    def check_collision(self, particle):
        px, py = particle.pos
        r = particle.radius
        if self.start[0] <= px <= self.end[0]:
            platform_y = self.height_at(px)
            return py - r <= platform_y
        return False
