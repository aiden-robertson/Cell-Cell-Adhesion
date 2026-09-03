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

            // Velocity
            std::array<float, 2> velocity;

            float maxVelocity;

        public:
            Body(float x = 0.0f, float y = 0.0f, float size = 0.0f, float mass = 0.0f, std::array<float, 2> velocity = {0.0f, 0.0f});
            virtual ~Body() = default;

            [[nodiscard]] const float& GetX() const { return position[0]; }
            [[nodiscard]] const float& GetY() const { return position[1]; }
            [[nodiscard]] const std::array<float, 2>& GetPosition() const { return position; }
            [[nodiscard]] const float& GetRadius() const { return radius; }
    };

    /* Particles */
    #pragma region Particles

    struct GlobalParticle
    {
        static constexpr float repelRadius = .005;
        static constexpr float radius = .05;

        static constexpr float reachGoalRadius = .1;

        static constexpr float repelStrength = 1;

        static constexpr float moveSpeed = .1;
        static constexpr float maxVelocity = 10;
    };

    class Particle : public Body
    { 
        // Distributions to use for generating new goals
        std::uniform_real_distribution<float> goalXDistribution, goalYDistribution;

        // Goal system
        std::array<float, 2> goal;

        public:
            Particle(
                float x, float y, float size, float mass, std::uniform_real_distribution<float>& goalXDistribution, 
                std::uniform_real_distribution<float>& goalYDistribution, float goalXMin, float goalXMax, float goalYMin, float goalYMax
            );

            void Update(
                float deltaTime = 1, const std::vector<Particle*>& nearbyParticles = std::vector<Particle*>(), 
                const std::vector<Cell*>& nearbyCells = std::vector<Cell*>()
            );

            [[nodiscard]] const float& GetMass() const { return mass; }
    };

    std::vector<Particle> SpawnParticles(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax
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

    /* Constant - Should NEVER be Changed */
    static constexpr std::array<float, 6> stageTimes = {
        .415, .335, .165, .065, .02 // Defined as the average percent of the cell cycle spent in each stage
    };

    struct GlobalCell
    {
        static constexpr float repelRadius = .1;
        static constexpr float radius = .05;

        static constexpr float repelStrength = .05;

        static constexpr float moveSpeed = 10;
        static constexpr float maxVelocity = 10;

        static constexpr int maxAdhesionSites = 100;

        static constexpr int cellCycleLength = 10000;
    };

    class Cell : public Body
    {
        // Rotation
        short int rotation;

        // Adhesion points
        std::vector<AdhesionSite> adhesivePoints;
        std::vector<short int> adhesionLocations;

        // Timing
        int time; // time++ per frame; time since stage start (actual stage lengths defined by global struct)
        short int stage; // 0 = G1, 1 = S, 2 = G2, 3 = M, 4 = Cytokinesis, 5 = G0

        public:
            Cell(
                float x, float y, float size, float mass
            );
    };

    std::vector<Cell> SpawnCells(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax
    );

    #pragma endregion
};

#endif