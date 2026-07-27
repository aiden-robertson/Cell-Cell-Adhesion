# ----------------------------------------
# Cell - Cell Adhesion Studies Project
# ----------------------------------------

# The code in this project is designed to work on Ubuntu with all necessary graphics libraries.
# It serves as a computational agent-based simulation of how adhesion can effect emergent 
# Behaviors in multicellular life.

# Project information
cmake_minimum_required(VERSION 3.10)
project(AdhesionStudies)

# C++ version
set(CMAKE_CXX_STANDARD 20)

# Custom libs as subdirectories
add_subdirectory(dependencies/ParticleRenderer)

# Main executable
add_executable(AdhesionStudies src/main.cpp)

# All necessary dependencies
target_include_directories(AdhesionStudies PRIVATE
    src
    dependencies
    dependencies/ParticleRenderer
)

# Required third party packages (must be installed globally on system)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

# Link everything to executable
target_link_libraries(AdhesionStudies PRIVATE
    ParticleRenderer
    glfw
    OpenGL::GL
    GLEW::GLEW
    Freetype::Freetype
)