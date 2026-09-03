#include "config.h"

/* Constants */
#pragma region Constants

/* Program Related */
const char* programName = "Cell-Cell Adhesion Studies";

const std::array<int, 2> aspectRatio = {16, 9};
const float aspectScale = 100;

const float timeScale = 1.0f;

/* Studies Related */

const int particleCount = 10000;
const int cellCount = 0;

/* Visuals Related */

const float particleRadius = .005f;
const float cellRadius = .05f;

const std::array<float, 4> particleColor = {0.0f, 0.0f, 0.5f, 1.0f};
const std::array<float, 4> cellColor = {0.4f, 0.0f, 0.9f, 1.0f};

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

/* Interactions Function */
// Calculates interactions between Particles based on their current spacial position
void UpdateParticles(
    std::vector<bodies::Particle>& particles, std::vector<bodies::Particle>& particlesWrite, std::vector<bodies::Particle*>& nearbyParticles, 
    std::vector<std::vector<int>>& particleGridCells, const int& gridCols, const float& cellSize, float deltaTime
)
{
    particlesWrite = particles;

    // Clear previous frame's grid
    for(auto& cell : particleGridCells)
        cell.clear();

    // Place particles in their grid cells
    for(int i = 0; i < static_cast<int>(particles.size()); ++i)
    {
        // Get current world coordinates of particles
        const float x = particles[i].GetX();
        const float y = particles[i].GetY();

        // Convert world coordinates ([-1, 1]) to grid coordinates ([0, 2])
        // Round the particle position to it's nearest grid cell (essentially, grid locking)
        int cellX = static_cast<int>((x + 1.0f) / cellSize);
        int cellY = static_cast<int>((y + 1.0f) / cellSize);

        cellX = std::clamp(cellX, 0, gridCols - 1);
        cellY = std::clamp(cellY, 0, gridCols - 1);

        // Convert from 2D coordinates to 1D inex
        const int cellIndex = cellY * gridCols + cellX;

        // Re-store the particle's new index
        particleGridCells[cellIndex].push_back(i);
    }

    // Max distance one particle can repel another
    const float queryRadius = bodies::GlobalParticle::repelRadius;
    const float queryRadiusSquared = queryRadius * queryRadius;

    // A particle may have to search multiple cells in each direction
    const int maxCellOffset = static_cast<int>(
        std::ceil(queryRadius / cellSize)
    );

    // For each particle:
    for(int i = 0; i < static_cast<int>(particles.size()); ++i)
    {
        // Get particle positions
        const float x = particles[i].GetX();
        const float y = particles[i].GetY();

        // Recalculate the current cell of a particle
        int cellX = static_cast<int>((x + 1.0f) / cellSize);
        int cellY = static_cast<int>((y + 1.0f) / cellSize);

        cellX = std::clamp(cellX, 0, gridCols - 1);
        cellY = std::clamp(cellY, 0, gridCols - 1);

        // Search for nearby particles within the given range
        for(int offsetY = -maxCellOffset; offsetY <= maxCellOffset; ++offsetY)
            for(int offsetX = -maxCellOffset; offsetX <= maxCellOffset; ++offsetX)
            {
                // Check for particles in neighboring cells
                const int neighborCellX = cellX + offsetX;
                const int neighborCellY = cellY + offsetY;

                // Ignore cells not in the sim
                if(neighborCellX < 0 || neighborCellX >= gridCols ||
                   neighborCellY < 0 || neighborCellY >= gridCols)
                    continue;
                
                // Get index of neighboring cell
                const int neighborCellIndex = neighborCellY * gridCols + neighborCellX;

                // Get all the particles within grid cell
                for(int j : particleGridCells[neighborCellIndex])
                {
                    // Skip if particle is itself
                    if(i == j)
                        continue;

                    // Calculate distance beween two particles
                    const float dx = particles[j].GetX() - x;
                    const float dy = particles[j].GetY() - y;

                    const float distanceSquared = dx*dx + dy*dy;

                    // If particles are close enough
                    if(distanceSquared <= queryRadiusSquared)
                        nearbyParticles.emplace_back(&particles[j]);
                }
            }
        
        // Call particle's update function
        particlesWrite[i].Update(deltaTime*timeScale, nearbyParticles);

        nearbyParticles.clear();
    }

    particles = particlesWrite;
}

/* Random Number Generation Setup */
// Populate bodies::gen to be used globally
static std::random_device global_random_device;
namespace bodies { std::mt19937 gen(global_random_device()); }

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

    // Aspect to be used for graphics rendering
    float aspect = static_cast<float>(aspectRatio[0]) / static_cast<float>(aspectRatio[1]);

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

    /* Buffer Setup */
    #pragma region Buffer Setup

    // Initializes new CircleRenderer object for rendering particles
    gfx::CircleRenderer circleRenderer;
    circleRenderer.CreateBuffer(worldShader, 32);

    #pragma endregion

    /* Cell Initialization */
    #pragma region Cell Initialization

    // Spawns cells at random normalized positions and adds them to cell list
    std::vector<bodies::Cell> cells = bodies::SpawnCells(
        cellCount, // Amount of particles to spawn
        -1, 1, // X min / max
        -1, 1, // Y min / max
        1, 10, // Mass min / max
        .1, .1 // Size min / max
    );

    // Farthest distance in which cells can interact
    const float maxCellQueryRadius = bodies::GlobalCell::repelRadius;

    // Cell diameter for big each grid cell is
    const float cellSpatialCellSize = std::max(bodies::GlobalParticle::radius * 2, maxCellQueryRadius * 0.5f); // Each grid cell must be at least one cell diameter, and also small enough to keep neighbor checks local

    // Calculate how many spaces are needed (in the standard OpenGL -1 to 1 world space)
    const int cellGridCols = static_cast<int>(std::ceil(2.0f / cellSpatialCellSize));

    // Total number of cells needed
    const int cellGridCellCount = cellGridCols * cellGridCols;

    // Create 2d vector of grid cells
    std::vector<std::vector<int>> cellGridCells(cellGridCellCount);

    // Calculate on average how many cells will exist per grid space
    int cellReserveAmount = static_cast<int>(particleCount / cellGridCellCount) + 1; // Estimate how many particles will go into each cell
    if (cellReserveAmount < 10) cellReserveAmount = 10;

    // For each grid cell, allow for enough particles within it
    for (auto &cell : cellGridCells)
        cell.reserve(cellReserveAmount);

    #pragma endregion

    /* Particle Initialization */
    #pragma region Particle Initialization

    // Spawns particles at random normalized positions and adds them to particle list
    std::vector<bodies::Particle> particles = bodies::SpawnParticles(
        particleCount, // Amount of particles to spawn
        -1, 1, // X min / max
        -1, 1, // Y min / max
        1, 10, // Mass min / max
        .1, .1 // Size min / max
    );

    // Farthest distance in which particles can interact
    const float maxParticleQueryRadius = bodies::GlobalParticle::repelRadius;

    // Particle diameter for big each grid cell is
    const float particleSpatialCellSize = std::max(bodies::GlobalParticle::radius * 2, maxParticleQueryRadius * 0.5f); // Each grid cell must be at least one particle diameter, and also small enough to keep neighbor checks local

    // Calculate how many spaces are needed (in the standard OpenGL -1 to 1 world space)
    const int particleGridCols = static_cast<int>(std::ceil(2.0f / particleSpatialCellSize));

    // Total number of cells needed
    const int particleGridCellCount = particleGridCols * particleGridCols;

    // Create 2d vector of grid cells
    std::vector<std::vector<int>> particleGridCells(particleGridCellCount);

    // Calculate on average how many particles will exist per grid space
    int particleReserveAmount = static_cast<int>(particleCount / particleGridCellCount) + 1; // Estimate how many particles will go into each cell
    if (particleReserveAmount < 10) particleReserveAmount = 10;

    // For each grid cell, allow for enough particles within it
    for (auto &cell : particleGridCells)
        cell.reserve(particleReserveAmount);

    #pragma endregion

    /* Main Loop */
    #pragma region Main Loop

    // Delta time initialization
    std::chrono::time_point lastTime = std::chrono::steady_clock::now();
    std::chrono::time_point currentTime = std::chrono::steady_clock::now();
    float deltaTime;

    // Variables needed for neighboring bodies calculation
    std::vector<bodies::Particle> particlesWrite;
    std::vector<bodies::Particle*> nearbyParticles;

    while (!glfwWindowShouldClose(window))
    {
        /* DELTA TIME CALCULATIONS */
        currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        /* EVERY FRAME CALCULATIONS */

        // Update positioning of each particle
        UpdateParticles(particles, particlesWrite, nearbyParticles, particleGridCells, particleGridCols, particleSpatialCellSize, deltaTime*timeScale);

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);

        /* RENDER STUFF HERE */
        circleRenderer.DrawBatch(particles, worldShader, aspect, particleColor, particleRadius);
        circleRenderer.DrawBatch(cells, worldShader, aspect, cellColor, cellRadius);

        glfwPollEvents();

        // Swap read and write buffers
        glfwSwapBuffers(window);
    }

    #pragma endregion

    /* End Program */
    #pragma region End Program

    // Close shaders
    glDeleteProgram(worldShader);
    glDeleteProgram(uiShader);
    glDeleteProgram(textShader);

    // Safely end program
    glfwTerminate();
    return 0;

    #pragma endregion
}