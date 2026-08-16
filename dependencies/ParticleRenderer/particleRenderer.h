#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include <vector>
#include <array>

#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include <numbers>

// Forward declare OpenGL types (avoids including glew in header)
// TODO: FIGURE OUT WHAT THIS DOES & COMMENT
typedef unsigned int GLuint;

// Bodies types
#include <bodies.h>

// Helper functions for circle rendering
namespace gfx
{
    // Circle definition(defining variables for individual circles)
    struct Circle
    {
        float x, y; // Positions; -1 to 1
        float radius; // Radius of circle
        std::array<float, 4> color; // Color of circle represented in RGBA where range is a floating point number between 0 exclusive and 1 inclusive
    };

    // Square definition(defining variables for individual squares)
    struct Rect
    {
        float x1, y1; // Location of first point
        float x2, y2; // Location of opposite point
        std::array<float, 4> color; // Color of square represented in RGBA where range is a floating point number between 0 exclusive and 1 inclusive
    };

    // Object for rendering circles
    class CircleRenderer
    {
        private:
            GLuint vao;
            GLuint vbo;
            GLuint instanceVbo;
            int vertexCount; // Number of vertecies on object

            // Circle rendering stuff
            int aspectLoc;
            int posLoc;
            int scaleLoc;
            int colorLoc;

            std::vector<float> instanceData;
            size_t instanceBufferSize;

        public:
            CircleRenderer(); // Default constructor

            void CreateBuffer(GLuint shader, int segments = 32); // Initialization for GPU data

            void Draw(const Circle& c, GLuint shader, float aspect); // Draws single circle

            void Draw(const bodies::Body& b, GLuint shader, float aspect);

            // If `renderRadiusOverride` > 0, the renderer will use it for all instance radii
            // instead of each particle's stored radius. This separates rendering size from
            // physics collision size stored in `bodies::GlobalParticle`.
            void DrawBatch(const std::vector<bodies::Particle>& particles, GLuint shader, float aspect, float renderRadiusOverride = 0.0f);

            void DrawBatch(const float* instanceValues, size_t instanceCount, GLuint shader, float aspect);
    };
}

#endif