#include "config.h"

int main()
{
    #pragma region Window Creation

    // Ensure GLFW exists by attempting to initialize it, and exiting if it fails
    if (!glfwInit())
    {
        std::cout << "GLFW failed to start!\n";
        return -1;
    }

    // Create window and set as current context (one to be modified)
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Adhesion Simulation", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    #pragma endregion

    // Try to initialize GLEW & exit if it fails
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to start!\n";
        return -1;
    }

    /* Main Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* DELTA TIME CALCULATIONS */

        /* EVERY FRAME CALCULATIONS */

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);

        /* RENDER STUFF HERE */

        glfwPollEvents();

        // Swap read and write buffers
        glfwSwapBuffers(window);
    }

    /* CLOSE SHADERS HERE */

    // Safely end progra,
    glfwTerminate();
    return 0;
}