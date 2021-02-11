cmake_minimum_required(VERSION 3.18)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/project.cmake")
    message(STATUS "Downloading project.cmake from github")
    file(DOWNLOAD "https://github.com/GabeRundlett/scripts/raw/v0.1/project.cmake" 
    "${CMAKE_BINARY_DIR}/project.cmake")
endif()
include(${CMAKE_BINARY_DIR}/project.cmake)

project(learn_opengl)

add_executable(${PROJECT_NAME} "main.cpp")

conan_cmake_run(
REQUIRES
    glfw/3.3.2
    assimp/5.0.1
    glad/0.1.34
    glm/0.9.9.5
    stb/20200203
    freetype/2.10.4
    fmt/7.1.3
OPTIONS
    BASIC_SETUP
    CMAKE_TARGETS
BUILD
    missing
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC 
    coel_project_options
    coel_project_warnings

    CONAN_PKG::assimp
    CONAN_PKG::glfw
    CONAN_PKG::glad
    CONAN_PKG::glm
    CONAN_PKG::stb
    CONAN_PKG::freetype
    CONAN_PKG::fmt
)

target_link_options(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:
        # "/ENTRY:mainCRTStartup"
        # "/SUBSYSTEM:WINDOWS"
    >
)