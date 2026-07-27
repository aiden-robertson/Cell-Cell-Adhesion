cmake_minimum_required(VERSION 3.10)
project(ParticleRenderer)

# C++ version
set(CMAKE_CXX_STANDARD 23)

add_library(ParticleRenderer particleRenderer.cpp)

# FORCE the compiler to check your thirdparty folder first
target_include_directories(ParticleRenderer BEFORE PRIVATE 
    "${CMAKE_SOURCE_DIR}/thirdparty"
    "${CMAKE_SOURCE_DIR}/dependencies"
)

target_include_directories(ParticleRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_compile_definitions(ParticleRenderer PRIVATE GLFW_DLL)
    target_link_libraries(ParticleRenderer PRIVATE
        "${CMAKE_SOURCE_DIR}/thirdparty/glfw3.dll"
        "${CMAKE_SOURCE_DIR}/thirdparty/glew32.dll"
        opengl32
        gdi32
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)
    target_link_libraries(ParticleRenderer PRIVATE OpenGL::GL GLEW::GLEW glfw)
endif()
