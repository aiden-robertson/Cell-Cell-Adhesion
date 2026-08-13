#include "bodies.h"

namespace bodies
{
    /* Particles */
    #pragma region Particles

    /* Particle */
    #pragma region Particle

    Particle::Particle(float x, float y, float size, float mass, float moveSpeed): x(x), y(y), size(size), mass(mass), moveSpeed(moveSpeed)
    {

    };

    /* Generate x Amount of Particles */
    std::vector<Particle> GenerateParticlesFromRandomPositions(
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