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
    /* Particles */
    #pragma region Particles
    struct GlobalParticle
    {
        static constexpr float repelRadius = .1;
        static constexpr float radius = .05;

        // Shared color for all Particle instances. Particles no longer store per-instance color.
        static std::array<float,4> color;
    };

    class Body
    {
        protected:
            // Current body position
            float x, y;

            // Particle radius
            float radius;

            // Particle color (default for non-Particle bodies). Particles override this.
            std::array<float, 4> rgba;

        public:
            Body(float x = 0.0f, float y = 0.0f, float size = 0.0f, float mass = 0.0f);
            virtual ~Body() = default;

            float getX() const { return x; }
            float getY() const { return y; }
            float getRadius() const { return radius; }
            virtual const std::array<float,4>& getColor() const { return rgba; }
    };

    class Particle : public Body
    {
        // Velocity
        float velX, velY;

        // Size and mass
        float size, mass;

        // Goal system
        float goalX, goalY;

        float moveSpeed;

        public:
            Particle(float x, float y, float size, float mass, float moveSpeed);

            // Particles share a global color; this returns the shared color.
            const std::array<float,4>& getColor() const override;

            // Mass accessor
            float getMass() const { return mass; }
    };

    std::vector<Particle> SpawnParticles(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax, float moveSpeedMin, float moveSpeedMax, std::mt19937 gen
    );

    #pragma endregion

    /* Adhesion Sites */
    #pragma region Adhesion Sites

    struct GlobalAdhesionSite
    {

    };

    class AdhesionSite : public Body
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