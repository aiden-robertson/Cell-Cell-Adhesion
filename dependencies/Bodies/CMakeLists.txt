# ----------------------------------------
# Bodies
# ----------------------------------------

# The code in this project defines three major bodies for the adhesion system: particles,
# Cells, and adhesion sites. Each of which is defined using OOP, and can easily be
# Implemented into a main.cpp file.

# Project information
cmake_minimum_required(VERSION 3.10)
project(Bodies)

# C++ version
set(CMAKE_CXX_STANDARD 23)

# Define as library, not primary executable
add_library(Bodies bodies.cpp)

# All necessary dependencies
target_include_directories(Bodies PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)