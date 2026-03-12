cmake_minimum_required(VERSION 3.10)
project(BrowserTabOptimizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link against Windows libraries
target_link_libraries(${PROJECT_NAME} psapi User32)
