cmake_minimum_required(VERSION 3.15)

project(GestureTrackingServer)

set (CMAKE_CXX_STANDARD 14)
message (STATUS "This build is for C++${CMAKE_CXX_STANDARD}")

add_subdirectory(external)

option(BUILD_EXAMPLES "Build networking examples" ON)
if (BUILD_EXAMPLES)
  add_executable("client" client.cpp)
  add_executable("server" server.cpp)

  target_link_libraries("client" network)
  target_link_libraries("server" network)
endif()

if (NOT WIN32)
  message (WARNING "The Kinect part is only supported in Windows.")
  return()
endif()

add_executable(${CMAKE_PROJECT_NAME} kinectexample.cpp)


list(APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
)

option(USE_k4a "Enable k4a" ON)
set("k4a_DIR_PATH" "C:/Program Files/Azure Kinect SDK v1.4.1/")
if (USE_k4a)
  include(k4a)
  target_link_libraries(${CMAKE_PROJECT_NAME} k4a::k4a)
endif()

option(USE_k4abt "Enable k4abt" ON)
set("k4abt_DIR_PATH" "C:/Program Files/Azure Kinect Body Tracking SDK/")
if (USE_k4a AND USE_k4abt)
  include(k4abt)
  target_link_libraries(${CMAKE_PROJECT_NAME} k4a::k4abt)
endif()

if (USE_k4a AND USE_k4abt)
  add_subdirectory(GestureTracker)
  target_link_libraries (${CMAKE_PROJECT_NAME} GestureTracker)
endif()

# install dll files from Azure Kinect SDK
if(USE_k4a)
    get_target_property(k4a_LIBNAME k4a::k4a IMPORTED_LOCATION_RELEASE)

    string(REGEX MATCH "^.*/" _sharedlib_glob "${k4a_LIBNAME}")
    string(APPEND _sharedlib_glob "*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
    # message(STATUS "_sharedlib_glob: ${_sharedlib_glob}")
    file(GLOB _sharedlibs LIST_DIRECTORIES false "${_sharedlib_glob}")
    # message(STATUS "_sharedlibs: ${_sharedlibs}")

    install(
      FILES ${_sharedlibs} 
      CONFIGURATIONS Debug
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/
    )
    install(
      FILES ${_sharedlibs} 
      CONFIGURATIONS Release
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release/
    )
    unset(_sharedlibs)
endif()

# install dll files from Azure Kinect Body Tracking SDK
if(USE_k4abt)
    get_target_property(k4abt_LIBNAME k4a::k4abt IMPORTED_LOCATION_RELEASE)

    string(REGEX MATCH "^.*/" _sharedlib_glob "${k4abt_LIBNAME}")
    string(APPEND _sharedlib_glob "*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
    # message(STATUS "_sharedlib_glob: ${_sharedlib_glob}")
    file(GLOB _sharedlibs LIST_DIRECTORIES false "${_sharedlib_glob}")
    # message(STATUS "_sharedlibs: ${_sharedlibs}")

    install(
      FILES ${_sharedlibs} 
      CONFIGURATIONS Debug
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/
    )
    install(
      FILES ${_sharedlibs} 
      CONFIGURATIONS Release
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release/
    )
    unset(_sharedlibs)
endif()

# install onnx files from Azure Kinect Body Tracking SDK
if(USE_k4abt)
    get_target_property(k4abt_LIBNAME k4a::k4abt IMPORTED_LOCATION_RELEASE)

    string(REGEX MATCH "^.*/" _sharedlib_glob "${k4abt_LIBNAME}")
    string(APPEND _sharedlib_glob "*.onnx*")
    # message(STATUS "_sharedlib_glob: ${_sharedlib_glob}")
    file(GLOB _sharedlibs LIST_DIRECTORIES false "${_sharedlib_glob}")
    # message(STATUS "_sharedlibs: ${_sharedlibs}")

    install(
      FILES ${_sharedlibs} 
      CONFIGURATIONS Debug
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/
    )
    install(
      FILES ${_sharedlibs} 
      CONFIGURATIONS Release
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release/
    )
    unset(_sharedlibs)
endif()