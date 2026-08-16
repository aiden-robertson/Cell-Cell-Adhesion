#ifndef BODIES_H
#define BODIES_H

// Collections
#include <vector>
#include <array>
#include <unordered_map>

// Math
#include <random>

namespace bodies
{

    /* Always Initialize in Main */
    extern std::mt19937 gen;

    /* Class Initialization */
    // Allows for their use before they are defined
    class Particle;
    class Cell;

    class Body
    {
        protected:
            // Current body position
            float x, y;

            // Body size and mass
            float radius, mass;

            std::array<float, 4> rgba;

        public:
            Body(float x = 0.0f, float y = 0.0f, float size = 0.0f, float mass = 0.0f);
            virtual ~Body() = default;

            [[nodiscard]] const float& GetX() const { return x; }
            [[nodiscard]] const float& GetY() const { return y; }
            [[nodiscard]] const float& GetRadius() const { return radius; }
            [[nodiscard]] virtual const std::array<float,4>& GetColor() const { return rgba; }
    };

    /* Particles */
    #pragma region Particles
    struct GlobalParticle
    {
        static constexpr float repelRadius = .1;
        static constexpr float radius = .05;

        static constexpr float repelStrength = .05;
    };

    class Particle : public Body
    {
        // Velocity
        float velX, velY;

        // Goal system
        float goalX, goalY;

        float moveSpeed;

        public:
            Particle(float x, float y, float size, float mass, float moveSpeed, std::uniform_real_distribution<float>& goalDistributionX, std::uniform_real_distribution<float>& goalDistributionY);

            void Update(float deltaTime = 1, const std::vector<Particle>& nearbyParticles = std::vector<Particle>(), const std::vector<Cell>& nearbyCells = std::vector<Cell>());

            [[nodiscard]] const float& GetMass() const { return mass; }

            // TEMP: small helper to set a particle's goal from external code
            // (Added for debug; remove easily when no longer needed)
            void SetGoal(float gx, float gy) { goalX = gx; goalY = gy; }
    };

    std::vector<Particle> SpawnParticles(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax, float moveSpeedMin, float moveSpeedMax
    );

    #pragma endregion

    /* Adhesion Sites */
    #pragma region Adhesion Sites

    struct GlobalAdhesionSite
    {

    };

    class AdhesionSite
    {
        // Scaling and range
        float size, range;
    };

    #pragma endregion

    /* Cells */
    #pragma region Cells

    struct GlobalCell
    {

    };

    class Cell : public Body
    {
        // rotation
        short int rotation;

        // Velocity
        float velX, velY;

        // Adhesion points
        std::vector<AdhesionSite> adhesivePoints;
        std::vector<short int> adhesionLocations;

        // Timing
        int time; // time++ per frame; time since stage start (actual stage lengths defined by global struct)
        short int stage; // 0 = G1, 1 = S, 2 = G2, 3 = M, 4 = Cytokinesis, 5 = G0
    };

    #pragma endregion
};

#endif