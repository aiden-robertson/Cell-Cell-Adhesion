#include "particleRenderer.h"

#include <bodies.h>
namespace gfx
{
    #pragma region CircleRenderer
    // Initializes empty circle buffer
    CircleRenderer::CircleRenderer(): vao(0), vbo(0), instanceVbo(0), vertexCount(0), aspectLoc(-1), instanceBufferSize(0) {}

    /* Buffer Initialization */
    // Initializes vertex array for circle
    void CircleRenderer::CreateBuffer(GLuint shader, int segments)
    {
        // Allows for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Initialization of new vertecies
        std::vector<float> vertices;

        // Center of circle (for triangle fan)
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        // Generate unit circle
        for (int i = 0; i <= segments; i++)
        {
            // Generates angle for each segment (distributes evenly throughout circle)
            float angle = 2.0f * std::numbers::pi * i / segments;

            // Puts points at x and y positions based on given angle
            vertices.push_back(cos(angle));
            vertices.push_back(sin(angle));
        }

        // Number of vertices in the triangle fan: center + one vertex per segment + repeat first vertex
        vertexCount = segments + 2;

        // Creates vertex array & binds to OpenGL
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Creates vertex buffer & binds to OpenGL
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Passes necessary data into vertex buffer object(vbo)
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(float),
                     vertices.data(),
                     GL_STATIC_DRAW);

        // The circle shape is stored as a vertex list; attribute 0 is the per-vertex position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Create instance buffer for per-particle attributes
        glGenBuffers(1, &instanceVbo);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // allocate an empty dynamic buffer first

        // instancePosition (vec2) for each particle
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(0 * sizeof(float)));
        glVertexAttribDivisor(1, 1); // advance once per instance instead of once per vertex

        // instanceRadius (float) for each particle
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
        glVertexAttribDivisor(2, 1);

        // instanceColor (vec4) for each particle
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribDivisor(3, 1);

        // Unbinds intermediates
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Find uniforms and cache locations for faster per-draw updates
        aspectLoc = glGetUniformLocation(shader, "u_Aspect");
        posLoc = glGetUniformLocation(shader, "u_Position");
        scaleLoc = glGetUniformLocation(shader, "u_Scale");
        colorLoc = glGetUniformLocation(shader, "desiredColor");
    }

    /* Single Circle Drawing */
    // Draws circle based on already provided information and given circle information
    void CircleRenderer::Draw(const Circle &c, GLuint shader, float aspect)
    {
        // What shader and vertex array OpenGL should use to render
        glUseProgram(shader);
        glBindVertexArray(vao);

        // Use cached uniform locations when available
        if (posLoc == -1 || scaleLoc == -1 || aspectLoc == -1 || colorLoc == -1)
        {
            // fallback: still attempt to set uniforms by querying locations
            GLint pos = glGetUniformLocation(shader, "u_Position");
            GLint scale = glGetUniformLocation(shader, "u_Scale");
            GLint asp = glGetUniformLocation(shader, "u_Aspect");
            GLint col = glGetUniformLocation(shader, "desiredColor");
            if (pos != -1) glUniform2f(pos, c.x, c.y);
            if (scale != -1) glUniform1f(scale, c.radius);
            if (asp != -1) glUniform1f(asp, aspect);
            if (col != -1) glUniform4f(col, c.color[0], c.color[1], c.color[2], c.color[3]);
        }
        else
        {
            glUniform2f(posLoc, c.x, c.y);
            glUniform1f(scaleLoc, c.radius);
            glUniform1f(aspectLoc, aspect);
            glUniform4f(colorLoc, c.color[0], c.color[1], c.color[2], c.color[3]);
        }

        // Draws circle(aka triangle fan)
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

        // Unbind VAO to leave GL state clean
        glBindVertexArray(0);
    }

    /* Draw a Body directly without constructing a temporary Circle. */
    // Sets shader uniforms directly from the Body (position, radius, color) and draws.
    void CircleRenderer::Draw(const bodies::Body &b, GLuint shader, float aspect)
    {
        // Bind program and VAO; avoid temporaries to reduce allocations and branches
        glUseProgram(shader);
        glBindVertexArray(vao);

        if (posLoc == -1 || scaleLoc == -1 || aspectLoc == -1 || colorLoc == -1)
        {
            // Fallback behavior: query uniform locations if cache is invalid
            GLint pos = glGetUniformLocation(shader, "u_Position");
            GLint scale = glGetUniformLocation(shader, "u_Scale");
            GLint asp = glGetUniformLocation(shader, "u_Aspect");
            GLint col = glGetUniformLocation(shader, "desiredColor");
            if (pos != -1) glUniform2f(pos, b.GetX(), b.GetY());
            if (scale != -1) glUniform1f(scale, b.GetRadius());
            if (asp != -1) glUniform1f(asp, aspect);
            auto c = b.GetColor();
            if (col != -1) glUniform4f(col, c[0], c[1], c[2], c[3]);
        }
        else
        {
            glUniform2f(posLoc, b.GetX(), b.GetY());
            glUniform1f(scaleLoc, b.GetRadius());
            glUniform1f(aspectLoc, aspect);
            auto c = b.GetColor();
            glUniform4f(colorLoc, c[0], c[1], c[2], c[3]);
        }

        // Draw the unit circle mesh scaled and translated by the shader uniforms
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

        // Unbind VAO
        glBindVertexArray(0);
    }

    /* Multiple Circles Drawing (raw values) */
    // Draws multiple circles based on an array of raw per-instance attributes for efficiency.
    // Each instance is seven floats: x, y, radius, r, g, b, a
    void CircleRenderer::DrawBatch(const float* instanceValues, size_t instanceCount, GLuint shader, float aspect, const std::array<float,4>& renderColor, float renderRadiusOverride)
    {
        if (instanceCount == 0 || instanceValues == nullptr)
            return;

        // Compute number of bytes in the provided instance array and decide whether to reallocate
        size_t floatCount = instanceCount * 7;
        size_t byteSize = floatCount * sizeof(float);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        if (byteSize > instanceBufferSize)
        {
            // allocate or reallocate buffer with the provided data
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(byteSize), instanceValues, GL_DYNAMIC_DRAW);
            instanceBufferSize = byteSize;
        }
        else
        {
            // update only the used portion
            glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(byteSize), instanceValues);
        }

        // Bind shader and VAO ready for instanced rendering
        glUseProgram(shader);
        glBindVertexArray(vao);

        if (aspectLoc == -1)
        {
            std::cout << "Warning: shader uniform not found!" << std::endl;
        }

        // Upload aspect uniform then draw all instances in a single instanced draw call
        glUniform1f(aspectLoc, aspect);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, vertexCount, static_cast<GLsizei>(instanceCount));

        // Unbind VAO to leave GL state clean
        glBindVertexArray(0);
    }

    /** Build a packed instance buffer from a vector of `bodies::Particle` and render. */
    // Build instanceData (layout: x,y,radius,r,g,b,a) and issue an instanced draw
    void CircleRenderer::DrawBatch(const std::vector<bodies::Particle>& particles, GLuint shader, float aspect, const std::array<float,4>& renderColor, float renderRadiusOverride)
    {
        if (particles.empty())
            return;
        size_t count = particles.size();

        // Prepare instance attribute data for all particles in a single contiguous buffer
        instanceData.resize(count * 7);
        for (size_t i = 0; i < count; ++i)
        {
            const bodies::Particle& p = particles[i];
            float *dst = instanceData.data() + i * 7;
            dst[0] = p.GetX();
            dst[1] = p.GetY();

            // Use the override when provided so rendering size can differ from physics size
            dst[2] = (renderRadiusOverride > 0.0f) ? renderRadiusOverride : p.GetRadius();

            dst[3] = renderColor[0];
            dst[4] = renderColor[1];
            dst[5] = renderColor[2];
            dst[6] = renderColor[3];
        }

        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        size_t byteSize = instanceData.size() * sizeof(float);
        if (byteSize > instanceBufferSize)
        {
            // Allocate or reallocate buffer with the provided data
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(byteSize), instanceData.data(), GL_DYNAMIC_DRAW);
            instanceBufferSize = byteSize;
        }
        else
        {
            // Update only the used portion
            glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(byteSize), instanceData.data());
        }

        // What shader and vertex array OpenGL should use to render
        glUseProgram(shader);
        glBindVertexArray(vao);

        if (aspectLoc == -1)
        {
            std::cout << "Warning: shader uniform not found!" << std::endl;
        }

        glUniform1f(aspectLoc, aspect);

        // Draw the same circle mesh once per particle instance using the instance buffer
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, vertexCount, static_cast<GLsizei>(count));

        // Unbinds intermediates
        glBindVertexArray(0);
    }

    #pragma endregion
}