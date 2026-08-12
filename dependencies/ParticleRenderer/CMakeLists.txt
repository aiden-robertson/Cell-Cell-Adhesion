# ----------------------------------------
# Particle Renderer
# ----------------------------------------

# The code in this project serves as an optimized system for rendering particles as objects
# Using OpenGL and GLFW.

# Project information
cmake_minimum_required(VERSION 3.10)
project(ParticleRenderer)

# C++ version
set(CMAKE_CXX_STANDARD 23)

# Define as library, not primary executable
add_library(ParticleRenderer particleRenderer.cpp)

# All necessary dependencies
target_include_directories(ParticleRenderer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)