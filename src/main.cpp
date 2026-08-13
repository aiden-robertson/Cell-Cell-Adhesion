#include "config.h"

/* Constants */
#pragma region Constants

/* Program Related */
const char* programName = "Cell-Cell Adhesion Studies";

const std::array<int, 2> aspectRatio = {16, 9};
const float aspectScale = 100;

/* Studies Related */

const int particleCount = 10;
const int cellCount = 10;

#pragma endregion

/* Functions for Graphics Rendering */
#pragma region Graphics Functions

/* Shader Compiler*/
// Compiles GLSL source code and returns the OpenGL shader object ID
static unsigned int CompileShader(const std::string& source, unsigned int type)
{
    // Creates a shader of given type to load shader onto
    unsigned int id = glCreateShader(type);

    // Sets the source of the shader
    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);

    // Compiles shader
    glCompileShader(id);

    // Checks if shader actually compiled
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    // Retrieve and print the shader compilation log
    if (!result)
    {
        // Message length
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        // Actual message (in CString form)
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);

        // Prints shader
        std::cout << "Shader compile error:\n" << message << std::endl;
        delete[] message;

        // Ends process of trying to get shader
        glDeleteShader(id);
        return 0;
    }

    return id;
}

/* Shader Initialization */
// Links shader to program then removes the intermidiate compiled shader
static unsigned int CreateShader(const std::string& vertex, const std::string& fragment)
{
    // Creates program to create shader under
    unsigned int program = glCreateProgram();

    // Compiles both shaders and gets their id
    unsigned int vs = CompileShader(vertex, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragment, GL_FRAGMENT_SHADER);

    // Attaches shader to state machine
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    // Connects shader to program
    glLinkProgram(program);
    glValidateProgram(program);

    // Removes intermediates
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

/* Shader Loader */
// Turns shader file of given path to a string
static std::string LoadShaderFile(const std::string& path)
{
    // Gets file contents in form of a string
    std::ifstream file(path);
    std::stringstream stream;
    stream << file.rdbuf();

    return stream.str();
}

/* Shader Initialization */
// Creates shader and mounts it to state machine
static unsigned int InitializeShader(const std::string path)
{
    // Shader source: change path if moving or renaming basic.shader
    std::string shaderSrc = LoadShaderFile(path);

    const std::string vertexMarker = "#shader vertex";
    const std::string fragmentMarker = "#shader fragment";

    const std::size_t vertexPos = shaderSrc.find(vertexMarker);
    const std::size_t fragmentPos = shaderSrc.find(fragmentMarker);

    // Gets vertex shader from shader file
    std::string vs = shaderSrc.substr(vertexPos + vertexMarker.size());
    vs = vs.substr(0, fragmentPos - (vertexPos + vertexMarker.size()));

    // Gets fragment shader from shader file
    std::string fs = shaderSrc.substr(fragmentPos + fragmentMarker.size());

    // Initializes shader from vertex and fragment shader
    unsigned int shader = CreateShader(vs, fs);

    // Tells state machine to use that shader
    glUseProgram(shader);

    return shader;
}

#pragma endregion

int main()
{
    /* Window Creation */
    #pragma region Window Creation

    // Ensure GLFW exists by attempting to initialize it, and exiting if it fails
    if (!glfwInit())
    {
        std::cout << "GLFW failed to start!\n";
        return -1;
    }

    // Create window and set as current context (one to be modified)
    GLFWwindow* window = glfwCreateWindow(aspectRatio[0]*aspectScale, aspectRatio[1]*aspectScale, programName, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Try to initialize GLEW & exit if it fails
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to start!\n";
        return -1;
    }

    #pragma endregion

    /* Shader Setup */
    #pragma region Shader Setup

    // Find path to executable from root
    std::filesystem::path projectRoot;

    std::error_code error;
    std::filesystem::path executablePath = std::filesystem::canonical("/proc/self/exe", error);

    projectRoot = executablePath.parent_path();

    // If it was defaulted to build, just move it down
    if(projectRoot.filename() == "build")
        projectRoot = projectRoot.parent_path();

    // Initialize each shader from its path
    GLuint worldShader = InitializeShader((projectRoot / "res/shaders/world.shader").string());
    GLuint uiShader = InitializeShader((projectRoot / "res/shaders/UI.shader").string());
    GLuint textShader = InitializeShader((projectRoot / "res/shaders/text.shader").string());

    #pragma endregion

    /* Cell Initialization */
    #pragma region Cell Initialization

    // Initializes new CircleRenderer object for rendering particles
    gfx::CircleRenderer circleRenderer;
    circleRenderer.CreateBuffer(worldShader, 6);

    // Spawns cells at random normalized positions and adds them to cell list
    /* CREATION OF A LIST OF EACH PARTICLE */

    // Farthest distance in which cells can interact
    /* CALCULATIONS FOR HOW FAR EACH CELL CAN HYPOTHETICALLY REACH ANOTHER CELL */

    // Cell diameter for big each grid cell is
    /* CELL DIAMETER CALCULATION */

    // Calculate how many spaces are needed (in the standard OpenGL -1 to 1 world space)
    /* TOTAL WORLD SIZE (1 - -1 = 2) / SIZE OF EACH CELL -> HOW MANY CELLS ARE NEEDED TO FILL THE WORLD */

    // Total number of cells needed
    /* PREVIOUS NUMBER SQUARED */

    // Range of check per cell
    /* CELL RADIUS / GRID CELL (not to be confused with particle cell) SIZE AS AN INTEGER */

    // Create 2d vector of grid cells
    /* VECTOR INITIALIZATION HERE */

    // Calculate on average how many cells will exist per grid space
    /* TOTAL CELL COUNT / GRID CELL COUNT + 1; AS A BUFFER SET TO AT LEAST 10 */

    // For each grid cell, allow for enough particles within it
    /* LOOP THROUGH EACH GRID CELL AND .reserve() THE PREVIOUS NUMBER PER */

    #pragma endregion

    /* Particle Initialization */
    #pragma region Particle Initialization

    // Spawns particles at random normalized positions and adds them to particle list
    std::vector<bodies::Particle> particles;

    // Farthest distance in which particles can interact
    const float maxQueryRadius = bodies::GlobalParticle::repelRadius;

    // Particle diameter for big each grid cell is
    const float spatialCellSize = std::max(bodies::GlobalParticle::radius * 2, maxQueryRadius * 0.5f); // Each particle must be at least one particle diameter, and also small enough to keep neighbor checks local

    // Calculate how many spaces are needed (in the standard OpenGL -1 to 1 world space)
    const int gridCols = static_cast<int>(std::ceil(2.0f / spatialCellSize));

    // Total number of cells needed
    const int gridCellCount = gridCols * gridCols;

    // Range of check per particle
    const int maxCellOffset = static_cast<int>(std::ceil(maxQueryRadius / spatialCellSize));

    // Create 2d vector of grid cells
    std::vector<std::vector<int>> gridCells(gridCellCount);

    // Calculate on average how many particles will exist per grid space
    int reserveAmount = static_cast<int>(particleCount / gridCellCount) + 1; // Estimate how many particles will go into each cell
    if (reserveAmount < 10) reserveAmount = 10;

    // For each grid cell, allow for enough particles within it
    for (auto &cell : gridCells)
        cell.reserve(reserveAmount);

    #pragma endregion

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

    // Safely end program
    glfwTerminate();
    return 0;
}