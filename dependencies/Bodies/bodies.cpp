#include "bodies.h"

/* Helper Functions */
#pragma region Helper Functions

/* Vector Normalization */
// Normalize a vector to set all values to percent
template <std::size_t N>
std::array<float, N> NormalizeArray(std::array<float, N> array)
{
    // Calculate the total length defined by the vector
    float length = 0.0f;

    for(float& value : array)
        length += value * value;

    length = std::sqrt(length);

    // Update each value of the vector to normalize
    if(length > 0.0f)
        for(float& value : array)
            value /= length;

    return array;
}

#pragma endregion

/* Overloads */
#pragma region Overloads

/* std::array Addition Overload Between 2 Arrays */
// Add the first value of the first vector to the first of the second, etc.
template<typename T, std::size_t N>
std::array<T, N>& operator+=(std::array<T, N>& lhs, const std::array<T, N>& rhs)
{
    for(std::size_t i = 0; i < N; ++i)
        lhs[i] += rhs[i];

    return lhs;
}

/* std::array Addition Overload Between 2 Arrays */
// Add the first value of the first vector to the first of the second, etc.
template<typename T, std::size_t N>
std::array<T, N> operator+(const std::array<T, N>& lhs, const std::array<T, N>& rhs)
{
    std::array<T, N> result = lhs;
    result += rhs;
    return result;
}

/* std::array Subtraction Overload Between 2 Arrays */
// Subtract the first value of the first vector to the first of the second, etc.
template<typename T, std::size_t N>
std::array<T, N>& operator-=(std::array<T, N>& lhs, const std::array<T, N>& rhs)
{
    for(std::size_t i = 0; i < N; ++i)
        lhs[i] -= rhs[i];

    return lhs;
}

/* std::array Subtraction Overload Between 2 Arrays */
// Subtract the first value of the first vector to the first of the second, etc.
template<typename T, std::size_t N>
std::array<T, N> operator-(const std::array<T, N>& lhs, const std::array<T, N>& rhs)
{
    std::array<T, N> result = lhs;
    result -= rhs;
    return result;
}

/* std::array Multiplication Overload Between 1 Array and 1 Other */
// Multiply every value of an array with another value
template<typename T, std::size_t N>
std::array<T, N>& operator*=(std::array<T, N>& lhs, T rhs)
{
    for(std::size_t i = 0; i < N; ++i)
        lhs[i] *= rhs;
    
    return lhs;
}

/* std::array Multiplication Overload Between 1 Array and 1 Other */
// Multiply every value of an array with another value
template<typename T, std::size_t N>
std::array<T, N> operator*(const std::array<T, N>& lhs, T rhs)
{
    std::array<T, N> result = lhs;
    result *= rhs;
    return result;
}

/* std::array Division Overload Between 1 Array and 1 Other */
// Divide every value of an array with another value
template<typename T, std::size_t N>
std::array<T, N>& operator/=(std::array<T, N>& lhs, T rhs)
{
    for(std::size_t i = 0; i < N; ++i)
        lhs[i] /= rhs;
    
    return lhs;
}

/* std::array Division Overload Between 1 Array and 1 Other */
// Divide every value of an array with another value
template<typename T, std::size_t N>
std::array<T, N> operator/(const std::array<T, N>& lhs, T rhs)
{
    std::array<T, N> result = lhs;
    result /= rhs;
    return result;
}

#pragma endregion

namespace bodies
{
    Body::Body(float x, float y, float size, float mass, std::array<float, 2> velocity)
        : position({x, y}), radius(size), mass(mass), velocity{velocity}
    {}

    /* Particles */
    #pragma region Particles

    /* Particle */
    #pragma region Particle

    Particle::Particle(
        float x, float y, float size, float mass, std::uniform_real_distribution<float>& goalXDistribution, 
        std::uniform_real_distribution<float>& goalYDistribution, float goalXMin, float goalXMax, float goalYMin, float goalYMax
    ) : Body(x, y, size, mass), goalXDistribution(goalXDistribution), goalYDistribution(goalYDistribution)
    {
        // Randomly generate a goal
        // goalDistribution will only exist temporarily, so storing is pointless
        goal[0] = this->goalXDistribution(gen);
        goal[1] = this->goalYDistribution(gen);

        // Calculate max velocity from GlobalParticle and existing mass
        maxVelocity = GlobalParticle::maxVelocity / mass;
    }

    /* Particle Movement */
    // Update particle positions per frame based on surrounding bodies and goal
    void Particle::Update(float deltaTime, const std::vector<Particle>& nearbyParticles, const std::vector<Cell>& nearbyCells)
    {
        // Move towards goal
        velocity += NormalizeArray(goal - position) * GlobalParticle::moveSpeed;
        velocity += NormalizeArray(goal - position) * GlobalParticle::moveSpeed;

        // Cap velocity
        if(velocity[0] > maxVelocity)
            velocity[0] = maxVelocity;
        
        if(velocity[1] > maxVelocity)
            velocity[1] = maxVelocity;

        // Update positions
        position += velocity / mass * deltaTime;

        // OPTIONAL: damping
        velocity *= 0.9f;

        // Calculate distance from goal
        float distance = std::sqrt(std::pow(goal[0] - position[0], 2) + std::pow(goal[1] - position[1], 2));

        if(distance < GlobalParticle::reachGoalRadius)
        {
            // Calculate new goal
            goal[0] = goalXDistribution(gen);
            goal[1] = goalYDistribution(gen);
        }
    }

    #pragma endregion

    /* Generate x Amount of Particles */
    // Spawns x amount of particles at random positions with given ranges
    std::vector<Particle> SpawnParticles(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax
    ) {
        // Particle vector to eventually be returned
        std::vector<Particle> particles;

        // Random number generation prep
        std::uniform_real_distribution<float> massDistribution(massMin, massMax);
        std::uniform_real_distribution<float> sizeDistribution(sizeMin, sizeMax);

        std::uniform_real_distribution<float> xDistribution(xMin, xMax);
        std::uniform_real_distribution<float> yDistribution(yMin, yMax);

        // Loop through each of needed amount of particles
        for(int i = 0; i < count; i++)
        {
            particles.push_back(Particle(
                xDistribution(gen), yDistribution(gen), sizeDistribution(gen), massDistribution(gen), xDistribution, 
                yDistribution, xMin, xMax, yMin, yMax
            ));
        }

        return particles;
    }

    #pragma endregion

    /* Cells */
    #pragma region Cells

    /* Cell */
    #pragma region Cell

    Cell::Cell(
        float x, float y, float size, float mass
    ) : Body(x, y, size, mass)
    {
        // Calculate max velocity from GlobalParticle and existing mass
        maxVelocity = GlobalParticle::maxVelocity / mass;

        // Initialize timing
        time = 0;
        stage = 0; // G1

        // Initialize rotation
        rotation = 0;

        /* TODO: INITIALIZE ADHESION SITES */
    }

    #pragma endregion

    /* Generate x Amount of Cells */
    // Spawns x amount of cells at random positions with given ranges
    std::vector<Cell> SpawnCells(
        int count, float xMin, float xMax, float yMin, float yMax, float massMin, float massMax, float sizeMin, float sizeMax
    ) {
        // Particle vector to eventually be returned
        std::vector<Cell> cells;

        // Random number generation prep
        std::uniform_real_distribution<float> massDistribution(massMin, massMax);
        std::uniform_real_distribution<float> sizeDistribution(sizeMin, sizeMax);

        std::uniform_real_distribution<float> xDistribution(xMin, xMax);
        std::uniform_real_distribution<float> yDistribution(yMin, yMax);

        // Loop through each of needed amount of particles
        for(int i = 0; i < count; i++)
        {
            cells.push_back(Cell(
                xDistribution(gen), yDistribution(gen), sizeDistribution(gen), massDistribution(gen)
            ));
        }

        return cells;
    }

    #pragma endregion
}