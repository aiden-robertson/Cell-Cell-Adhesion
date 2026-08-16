#include "bodies.h"

namespace bodies
{

    /* Particles */
    #pragma region Particles

    /* Particle */
    #pragma region Particle

    Body::Body(float x, float y, float size, float mass)
        : x(x), y(y), radius(size), rgba{1.0f, 1.0f, 1.0f, 1.0f}, mass(mass)
    {}

    Particle::Particle(float x, float y, float size, float mass, float moveSpeed, std::uniform_real_distribution<float>& goalDistributionX, std::uniform_real_distribution<float>& goalDistributionY)
        : Body(x, y, size, mass), velX(0.0f), velY(0.0f), goalX(0.0f), goalY(0.0f), moveSpeed(moveSpeed)
    {
        // Randomly generate a goal
        // goalDistribution will only exist temporarily, so storing is pointless
        goalX = goalDistributionX(gen);
        goalY = goalDistributionX(gen);

    }

    /* Particle Movement */
    // Update particle positions per frame based on surrounding bodies and goal
    void Particle::Update(float deltaTime, const std::vector<Particle>& nearbyParticles, const std::vector<Cell>& nearbyCells)
    {
        // Move towards goal
        velX += (goalX - x) * moveSpeed;
        velY += (goalY - y) * moveSpeed;

        // Update positions
        x += velX * deltaTime;
        y += velY * deltaTime;

        // OPTIONAL: damping
        velX *= 0.9f;
        velY *= 0.9f;
    }


    /* Generate x Amount of Particles */
    // Spawns x amount of particles at random positions with given ranges
    std::vector<Particle> SpawnParticles(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax, float moveSpeedMin, float moveSpeedMax
    ) {
        // Particle vector to eventually be returned
        std::vector<Particle> particles;

        // Random number generation prep
        std::uniform_real_distribution<float> massDistribution(massMin, massMax);
        std::uniform_real_distribution<float> sizeDistribution(sizeMin, sizeMax);

        std::uniform_real_distribution<float> xDistribution(xMin, xMax);
        std::uniform_real_distribution<float> yDistribution(yMin, yMax);

        std::uniform_real_distribution<float> moveSpeedDistribution(moveSpeedMin, moveSpeedMax);

        // Loop through each of needed amount of particles
        for(int i = 0; i < count; i++)
        {
            particles.push_back(Particle(
                xDistribution(gen), yDistribution(gen), sizeDistribution(gen), massDistribution(gen), moveSpeedDistribution(gen), xDistribution, yDistribution
            ));
        }

        return particles;
    }

    #pragma endregion

    #pragma endregion
}