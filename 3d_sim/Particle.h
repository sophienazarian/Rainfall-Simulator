#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    float life;       // Remaining lifetime of the particle
    float maxLife;    // Original lifetime (for fade calculations)
    float alpha;      // Transparency
    
    Particle(glm::vec3 pos, glm::vec3 vel, float sz, float lifespan)
        : position(pos), velocity(vel), size(sz), life(lifespan), maxLife(lifespan), alpha(0.9f) {}
        
    // Update particle and return true if still alive
    bool update(float deltaTime) {
        position += velocity * deltaTime;
        velocity.y -= 9.8f * deltaTime; // Gravity
        
        // Slow down due to air resistance
        velocity *= (1.0f - 0.3f * deltaTime);
        
        // Update lifetime
        life -= deltaTime;
        
        // Fade out as lifetime decreases
        alpha = (life / maxLife) * 0.9f;
        
        // Make particles smaller as they age
        size = size * (0.8f + 0.2f * (life / maxLife));
        
        return life > 0.0f;
    }
};

#endif