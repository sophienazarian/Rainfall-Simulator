#ifndef DROPLET_H
#define DROPLET_H

#include <glm/glm.hpp>
#include "Particle.h"
#include <vector>

class Droplet {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    bool hasCollided;
    float deformFactor; // How much the droplet is stretched during falling

    Droplet(glm::vec3 pos, glm::vec3 vel, float sz);
    void update(float deltaTime, std::vector<Particle>& particles);
    
private:
    void createSplashEffect(std::vector<Particle>& particles);
};

#endif