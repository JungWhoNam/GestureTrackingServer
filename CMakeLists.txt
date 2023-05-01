cmake_minimum_required(VERSION 3.15)

project(GestureTrackingServer)

set (CMAKE_CXX_STANDARD 14)
message (STATUS "This build is for C++${CMAKE_CXX_STANDARD}")

add_subdirectory(external)

option(BUILD_EXAMPLES "Build networking examples" ON)
if (BUILD_EXAMPLES)
  add_executable("client" client.cpp)
  add_executable("server" server.cpp)

  target_link_libraries("client" network json)
  target_link_libraries("server" network json)
endif()

### the rest is only for Windows ###
if (NOT WIN32)
  message (WARNING "The Kinect part is only supported in Windows.")
  return()
endif()

### Build GestureTrackingServer ###
add_executable(${CMAKE_PROJECT_NAME} kinectexample.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} network json)

# Add other dependencies
list(APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
)
include(functions)

### Kinect SDK - k4a ###
option(USE_k4a "Enable k4a" ON)
set("k4a_DIR_PATH" "C:/Program Files/Azure Kinect SDK v1.4.1/")
if (USE_k4a)
  include(k4a)
  target_link_libraries(${CMAKE_PROJECT_NAME} k4a::k4a)

  # install dll files from Azure Kinect SDK
  install_files(k4a::k4a "${CMAKE_SHARED_LIBRARY_SUFFIX}" "Release")
  install_files(k4a::k4a "${CMAKE_SHARED_LIBRARY_SUFFIX}" "Debug")
endif()

### Kinect Body Tracking SDK - k4abt ###
option(USE_k4abt "Enable k4abt" ON)
set("k4abt_DIR_PATH" "C:/Program Files/Azure Kinect Body Tracking SDK/")
if (USE_k4a AND USE_k4abt) # k4abt requires k4a
  include(k4abt)
  target_link_libraries(${CMAKE_PROJECT_NAME} k4a::k4abt)

  # install dll files from Azure Kinect Body Tracking SDK
  install_files(k4a::k4abt "${CMAKE_SHARED_LIBRARY_SUFFIX}" "Release")
  install_files(k4a::k4abt "${CMAKE_SHARED_LIBRARY_SUFFIX}" "Debug")

  # install onnx files from Azure Kinect Body Tracking SDK
  install_files(k4a::k4abt ".onnx" "Release")
  install_files(k4a::k4abt ".onnx" "Debug")
endif()

### GestureTracker ###
if (USE_k4a AND USE_k4abt)
  add_subdirectory(GestureTracker)
  target_link_libraries(${CMAKE_PROJECT_NAME} GestureTracker)
endif()