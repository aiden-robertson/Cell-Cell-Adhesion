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
            std::array<float, 2> position;

            // Body size and mass
            float radius, mass;

            std::array<float, 4> rgba;

        public:
            Body(float x = 0.0f, float y = 0.0f, float size = 0.0f, float mass = 0.0f);
            virtual ~Body() = default;

            [[nodiscard]] const float& GetX() const { return position[0]; }
            [[nodiscard]] const float& GetY() const { return position[1]; }
            [[nodiscard]] const float& GetRadius() const { return radius; }
            [[nodiscard]] virtual const std::array<float,4>& GetColor() const { return rgba; }
    };

    /* Particles */
    #pragma region Particles
    struct GlobalParticle
    {
        static constexpr float repelRadius = .1;
        static constexpr float radius = .05;

        static constexpr float reachGoalRadius = .1;

        static constexpr float repelStrength = .05;

        static constexpr float moveSpeed = 10;
        static constexpr float maxVelocity = 10;
    };

    class Particle : public Body
    {
        // Velocity
        std::array<float, 2> velocity;

        float maxVelocity;

        // Distributions to use for generating new goals
        std::uniform_real_distribution<float> goalXDistribution, goalYDistribution;

        // Goal system
        std::array<float, 2> goal;

        float moveSpeed;

        public:
            Particle(
                float x, float y, float size, float mass, float moveSpeed, std::uniform_real_distribution<float>& goalXDistribution, 
                std::uniform_real_distribution<float>& goalYDistribution, float goalXMin, float goalXMax, float goalYMin, float goalYMax
            );

            void Update(
                float deltaTime = 1, const std::vector<Particle>& nearbyParticles = std::vector<Particle>(), 
                const std::vector<Cell>& nearbyCells = std::vector<Cell>()
            );

            [[nodiscard]] const float& GetMass() const { return mass; }
    };

    std::vector<Particle> SpawnParticles(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax, float moveSpeedMin, 
        float moveSpeedMax
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