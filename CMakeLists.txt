cmake_minimum_required(VERSION 3.29)
project(CartesianGrid)

# Set CMake policies
cmake_policy(SET CMP0091 NEW)

# Set compiler options
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Explicitly set Houdini paths
set(HFS "C:/Program Files/Side Effects Software/Houdini 20.5.278")
set(Houdini_PATH ${HFS})
set(Houdini_DIR ${Houdini_PATH}/toolkit/cmake)

# Define custom DSO output directory
set(HOUDINI_DSO_PATH "C:/Users/mansa/OneDrive/Documents/houdini20.5/dso")

# Find Houdini package
find_package(Houdini REQUIRED)

# Add library target 
add_library(CartesianGrid SHARED CartesianGrid.cpp)

# Configure target properties
set_target_properties(CartesianGrid PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
    # Set the output directory for the DLL
   # RUNTIME_OUTPUT_DIRECTORY ${HOUDINI_DSO_PATH}
    #LIBRARY_OUTPUT_DIRECTORY ${HOUDINI_DSO_PATH}
)

# Link against Houdini libraries
target_link_libraries(CartesianGrid
    Houdini
)

# Add Houdini binary paths to the system PATH during build
set(ENV{PATH} "${HFS}/bin;${HFS}/toolkit/bin;$ENV{PATH}")

# Print some helpful information
message(STATUS "Using Houdini installation: ${HFS}")
message(STATUS "DSO output directory: ${HOUDINI_DSO_PATH}")