cmake_minimum_required(VERSION 3.16)

project(CoreSim
    VERSION 0.1.0
    DESCRIPTION "Custom 16-bit CPU emulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CORESIM_BUILD_TESTS "Build CoreSim tests" ON)

# core emulator library
add_library(coresim_core
    src/CPU.cpp
    src/Memory.cpp
    src/Register.cpp
    src/CPUState.cpp
    src/Decoder.cpp
)
target_include_directories(coresim_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(coresim_core PRIVATE /W4)
else()
    target_compile_options(coresim_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# emulator executable
add_executable(coresim src/main.cpp)
target_link_libraries(coresim PRIVATE coresim_core)

if(CORESIM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
