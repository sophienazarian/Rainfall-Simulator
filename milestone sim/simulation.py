import numpy as np
from particle import Particle
from platform import Platform
from matplotlib.patches import Ellipse

SIM_DT = 0.01
N_FRAMES = 200

def spawn_splash(center, velocity_magnitude, parent_radius):
    particles = []
    count = int(np.clip(velocity_magnitude**2 * 3, 10, 30))

    log_mean = np.log(parent_radius * 0.5)
    log_stddev = 0.4

    for _ in range(count):
        angle = np.random.normal(loc=np.pi / 2, scale=np.pi / 6)
        angle = np.clip(angle, 0, np.pi)

        speed = np.random.normal(velocity_magnitude * 0.7, 0.7)
        vel = speed * np.array([np.cos(angle), np.sin(angle)])

        radius = np.random.lognormal(mean=log_mean, sigma=log_stddev)
        radius = np.clip(radius, 0.002, parent_radius * 0.9)

        particles.append(Particle(center.copy(), vel, radius=radius, is_splash=True))

    return particles


def slide_particle(p, platform, friction=0.05):
    slope_vec = platform.end - platform.start
    slope_dir = slope_vec / np.linalg.norm(slope_vec)

    g_proj = np.dot(np.array([0, -9.8]), slope_dir)
    accel = g_proj * slope_dir

    p.vel += accel * SIM_DT

    tangent_vel = np.dot(p.vel, slope_dir)
    p.vel = tangent_vel * (1 - friction) * slope_dir

    p.pos += p.vel * SIM_DT
    p.pos[1] = platform.height_at(p.pos[0]) + p.radius
    p.sliding = True

def merge_particles(p1, p2):
    r1, r2 = p1.radius, p2.radius
    v1, v2 = (4/3) * np.pi * r1**3, (4/3) * np.pi * r2**3
    total_volume = v1 + v2
    new_radius = (total_volume * (3 / (4 * np.pi))) ** (1/3)

    mass1, mass2 = v1, v2
    total_mass = mass1 + mass2
    new_vel = (mass1 * p1.vel + mass2 * p2.vel) / total_mass
    new_pos = (mass1 * p1.pos + mass2 * p2.pos) / total_mass

    merged = Particle(new_pos, new_vel, radius=new_radius, is_splash=True)
    merged.resting = True
    return merged

def run_simulation():
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation

    platform = Platform()
    initial_drops = [
        Particle([0.45, 1.0], [0.0, 0.0], is_splash=False),
        Particle([0.50, 1.0], [0.0, 0.0], is_splash=False),
        Particle([0.55, 1.0], [0.0, 0.0], is_splash=False)
    ]
    particles = initial_drops.copy()
    splashed = set()

    fig, ax = plt.subplots()
    platform_x = np.linspace(platform.start[0], platform.end[0], 100)
    platform_y = [platform.height_at(x) for x in platform_x]
    ax.plot(platform_x, platform_y, 'k-', linewidth=2)
    scat = ax.scatter([], [], s=[])
    ground_level_eps = 1e-3

    def update(frame):
        new_positions = []
        sizes = []

        updated_particles = []

        for p in particles:
            if p in splashed and not p.is_splash:
                continue

            if getattr(p, "resting", False):
                updated_particles.append(p)
                continue

            if p.sliding:
                if platform.start[0] <= p.pos[0] <= platform.end[0]:
                    slide_particle(p, platform)
                    updated_particles.append(p)
                    continue
                else:
                    p.sliding = False

            p.update(SIM_DT)

            if p.pos[1] <= p.radius + ground_level_eps and p.vel[1] < 0:
                p.pos[1] = p.radius
                p.vel = np.zeros_like(p.vel)
                p.resting = True
                updated_particles.append(p)
                continue

            if (
                p.is_splash and
                platform.check_collision(p) and
                p.vel[1] < 0 and
                not p.sliding
            ):
                slide_particle(p, platform)
                updated_particles.append(p)
                continue

            if (
                not p.is_splash and
                p not in splashed and
                platform.check_collision(p) and
                p.vel[1] < 0
            ):
                splash_particles = spawn_splash(p.pos.copy(), np.linalg.norm(p.vel), p.radius)
                particles.extend(splash_particles)
                splashed.add(p)
                continue

            updated_particles.append(p)

        merged = set()
        new_merged_particles = []

        for i, p1 in enumerate(updated_particles):
            if i in merged or not p1.is_splash or not getattr(p1, "resting", False):
                continue
            for j in range(i + 1, len(updated_particles)):
                p2 = updated_particles[j]
                if j in merged or not p2.is_splash or not getattr(p2, "resting", False):
                    continue
                dist = np.linalg.norm(p1.pos - p2.pos)
                if dist < (p1.radius + p2.radius) * 1.5:
                    merged.add(i)
                    merged.add(j)
                    new_merged_particles.append(merge_particles(p1, p2))
                    break

        final_particles = [
            p for i, p in enumerate(updated_particles) if i not in merged
        ] + new_merged_particles

        particles.clear()
        particles.extend(final_particles)

        for patch in list(ax.patches):
            patch.remove()

        airborne_positions = []
        airborne_sizes = []

        for p in particles:
            if p.is_splash and getattr(p, "resting", False):
                ellipse = Ellipse(
                    xy=p.pos,
                    width=p.radius * 3.0,
                    height=p.radius * 0.5,
                    color='blue',
                    alpha=0.5
                )
                ax.add_patch(ellipse)
            elif not getattr(p, "resting", False):
                airborne_positions.append(p.pos)
                airborne_sizes.append(p.radius * 1000)

        if airborne_positions:
            scat.set_offsets(np.array(airborne_positions).reshape(-1, 2))
            scat.set_sizes(airborne_sizes)
        else:
            scat.set_offsets(np.empty((0, 2)))
            scat.set_sizes([])

        return scat,

    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1.2)
    ax.set_aspect('equal')
    ani = animation.FuncAnimation(fig, update, frames=N_FRAMES, interval=50, blit=True)
    plt.show()
