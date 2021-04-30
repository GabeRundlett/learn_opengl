cmake_minimum_required(VERSION 3.18)

project(coel)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")

add_library(${PROJECT_NAME} INTERFACE)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_link_libraries(${PROJECT_NAME} INTERFACE 
    glfw
    glad::glad
    glm::glm
    fmt::fmt
    assimp::assimp
    freetype
)
target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_SOURCE_DIR}
    ${STB_INCLUDE_DIRS}
)
