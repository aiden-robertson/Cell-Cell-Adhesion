#include "bodies.h"

namespace bodies
{

    /* Particles */
    #pragma region Particles

    /* Particle */
    #pragma region Particle

    Body::Body(float x, float y, float size, float /*mass*/)
        : x(x), y(y), radius(size), rgba{1.0f, 1.0f, 1.0f, 1.0f}
    {}

    Particle::Particle(float x, float y, float size, float mass, float moveSpeed)
        : Body(x, y, size, mass), velX(0.0f), velY(0.0f), size(size), mass(mass), goalX(0.0f), goalY(0.0f), moveSpeed(moveSpeed)
    {}


    /* Generate x Amount of Particles */
    // Spawns x amount of particles at random positions with given ranges
    std::vector<Particle> SpawnParticles(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax, float moveSpeedMin, float moveSpeedMax, std::mt19937 gen
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
                xDistribution(gen), yDistribution(gen), sizeDistribution(gen), massDistribution(gen), moveSpeedDistribution(gen)
            ));
        }

        return particles;
    }

    #pragma endregion

    #pragma endregion
}