#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include <vector>
#include <array>
#include <concepts>

#include <GL/glew.h>

#include <cmath>
#include <iostream>
#include <numbers>

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

            // Ensure inputed class type is derived from body
            template <typename BodyType>
                requires std::derived_from<BodyType, bodies::Body>
            void DrawBatch(
                const std::vector<BodyType>& bodies, GLuint shader, float aspect, const std::array<float,4>& renderColor,
                float renderRadiusOverride = 0.0f
            ) {
                // Loop through each to get a reference
                std::vector<const bodies::Body*> bodyReferences;

                bodyReferences.reserve(bodies.size());

                for (const BodyType& body : bodies)
                    bodyReferences.push_back(&body);

                // Run DrawBatch with said references
                DrawBatch(bodyReferences, shader, aspect, renderColor, renderRadiusOverride);
            }

            void DrawBatch(
                const std::vector<const bodies::Body*>& bodies, GLuint shader, float aspect, const std::array<float,4>& renderColor,
                float renderRadiusOverride = 0.0f
            );
    };
}

#endif