cmake_minimum_required(VERSION 3.20)
project(Hybrid6DOFSimulator CXX C)

find_package (Eigen3 REQUIRED)
include_directories (${EIGEN3_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optimization flags for 5kHz high-frequency real-time loop
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/O2 /arch:AVX2 /W4)
endif()

# Include Directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/drivers_c)

# --- Rust Subsystem Integration via Cargo ---
find_program(CARGO_EXECUTABLE cargo REQUIRED)

set(RUST_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gnc_rust/target)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_BUILD_ARGS "")
    set(RUST_LIB_PATH ${RUST_TARGET_DIR}/debug/libgnc_rust.a)
else()
    set(CARGO_BUILD_ARGS "--release")
    set(RUST_LIB_PATH ${RUST_TARGET_DIR}/release/libgnc_rust.a)
endif()

# Custom command to build Rust static library before compiling C++
add_custom_target(
    gnc_rust_target
    ALL
    COMMAND ${CARGO_EXECUTABLE} build ${CARGO_BUILD_ARGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/gnc_rust
    COMMENT "Building Rust GNC memory-safe subsystem..."
)

# Source Files Collection
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE C_SOURCES "drivers_c/*.c")

# Main Executable Target
add_executable(hybrid_sim 
    ${CPP_SOURCES} 
    ${C_SOURCES}
)

# Ensure Rust lib builds before C++ linking
add_dependencies(hybrid_sim gnc_rust_target)

# Link Libraries (Rust static lib, pthread, math library)
target_link_libraries(hybrid_sim 
    PRIVATE 
    ${RUST_LIB_PATH}
    pthread
    m
)