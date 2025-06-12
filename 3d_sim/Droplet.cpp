#include "Droplet.h"
#include <cstdlib>
#include <iostream>
#include <random>
#include <cmath>

Droplet::Droplet(glm::vec3 pos, glm::vec3 vel, float sz)
    : position(pos), velocity(vel), size(sz), hasCollided(false), deformFactor(0.0f) {}

void Droplet::update(float deltaTime, std::vector<Particle>& particles) {
    // Apply gravity
    velocity.y -= 9.8f * deltaTime;
    
    // Droplet deformation during fall (becomes more elongated)
    if (velocity.y < -1.0f && !hasCollided) {
        deformFactor = std::min(deformFactor + deltaTime * 0.5f, 0.3f);
    }
    
    // Update position
    position += velocity * deltaTime;

    // Ground collision
    if (position.y - size < -2.0f) {
        position.y = -2.0f + size;

        // Generate splash particles
        if (!hasCollided) {
            hasCollided = true;
            createSplashEffect(particles);
        }
        
        velocity = glm::vec3(0.0f); // Stop moving after collision
    }
}

void Droplet::createSplashEffect(std::vector<Particle>& particles) {
    // Random number generators for realistic splash
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Parameters for the splash pattern
    const int numParticles = 60; // More particles for a better splash
    std::normal_distribution<float> angleDistribution(0.0f, 1.0f);
    std::lognormal_distribution<float> speedDistribution(0.5f, 0.3f);
    std::uniform_real_distribution<float> sizeDistribution(0.02f, 0.06f); // Varied sizes
    std::uniform_real_distribution<float> lifespanDistribution(0.5f, 2.0f); // Varied lifespans
    
    // Calculate impact velocity for splash energy
    float impactEnergy = std::min(std::abs(velocity.y) * 0.2f, 2.0f);
    
    // Create crown splash effect
    for (int i = 0; i < numParticles; i++) {
        // Angle in the horizontal plane (crown-like)
        float angle = (i / static_cast<float>(numParticles)) * 2.0f * 3.14159265359f;
        float angleVariation = angleDistribution(gen) * 0.3f;
        angle += angleVariation;
        
        // Speed varies with angle to create crown shape
        float speed = speedDistribution(gen) * impactEnergy;
        float upwardForce = 1.0f + std::abs(angleDistribution(gen)) * 0.5f;
        
        // Create velocity with crown-like shape
        glm::vec3 particleVel = glm::vec3(
            cos(angle) * speed,
            upwardForce, // Upward component
            sin(angle) * speed
        );
        
        // Offset position slightly for better visual
        glm::vec3 particlePos = position + glm::vec3(
            cos(angle) * 0.05f,
            0.0f,
            sin(angle) * 0.05f
        );
        
        // Create particle with varied size and lifespan
        float particleSize = sizeDistribution(gen);
        float lifespan = lifespanDistribution(gen);
        
        particles.emplace_back(particlePos, particleVel, particleSize, lifespan);
    }
    
    // Add a few vertical splash particles
    for (int i = 0; i < 10; i++) {
        float angle = angleDistribution(gen) * 3.14159265359f;
        float speed = speedDistribution(gen) * impactEnergy * 0.8f;
        
        glm::vec3 particleVel = glm::vec3(
            cos(angle) * speed * 0.2f,
            1.5f + std::abs(angleDistribution(gen)),
            sin(angle) * speed * 0.2f
        );
        
        float particleSize = sizeDistribution(gen) * 0.8f;
        float lifespan = lifespanDistribution(gen) * 0.8f;
        
        particles.emplace_back(position, particleVel, particleSize, lifespan);
    }
}