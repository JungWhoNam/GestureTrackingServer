# Gesture Tracking Server
> This application is part of a project called [Immersive OSPray Studio](https://github.com/jungwhonam/ImmersiveOSPRay).

## Overview
![](GestureTrackingServer.png)
`Gesture Tracking Server` processes the body tracking data from [Azure Kinect Body Tracking SDK](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md) and sends the processed data to connected clients.
* `k4abt_frame_t` is a struct from the SDK, and it contains `skeleton(s)` at the current frame. A `skeleton` contains a list of joints; each contains a position, an orientation, and a confidence level. 
* `GestureDetector` extracts important information from `k4abt_frame_t`, and packages the information into a JSON format. It extracts positions and confidence levels of all the joints provided by the SDK. 
* `async-sockets` opens a TCP socket and keeps track of connected clients. It sends the JSON string from `GestureDetector` to client(s).
  
> [!IMPORTANT]
> This application only runs in Windows due to its dependency to Azure Kinect SDK.

> [!NOTE]
> We provide [a plugin to OSPRay Studio](https://github.com/JungWhoNam/ospray_studio/tree/jungwho.nam-feature-plugin-gesture/plugins/gesture_plugin) that uses `Gesture Tracking Server` to support gesture-based interactions.

## Prerequisites
Install [Azure Kinect SDK v1.4.1](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md) and [Azure Kinect Body Tracking SDK v1.1.2](https://learn.microsoft.com/en-us/azure/kinect-dk/body-sdk-download).

## Setup
```shell
git clone https://github.com/jungwhonam/GestureTrackingServer.git
cd GestureTrackingServer

mkdir build
cd build
```


## CMake configuration and build
```shell
cmake \
-S .. \
-B . \
-DUSE_k4a=ON \
-Dk4a_DIR_PATH="C:/Program Files/Azure Kinect SDK v1.4.1/" \
-DUSE_k4abt=ON \
-Dk4abt_DIR_PATH="C:/Program Files/Azure Kinect Body Tracking SDK/"

cmake --build . --config Release

cmake --install . --config Release
```
CMake options to note (all have sensible defaults):

* Set the variable ```k4a_DIR_PATH``` to the folder where Azure Kinect SDK is installed, e.g., `C:/Program Files/Azure Kinect SDK v1.4.1/`. 
* Set the variable ```k4abt_DIR_PATH``` to the folder where Azure Kinect Body Tracking SDK is installed, e.g., `C:/Program Files/Azure Kinect Body Tracking SDK/`. Azure Kinect Body Tracking SDK requires Azure Kinect SDK.

## Run the application
```
./Release/GestureTrackingServer.exe
```
It can take seconds to start the sensor. When successfully started, the application will output `Sending the frame i...`.

## External Libraries
The application uses four external libraries:
1. [Azure Kinect SDK (currently using v1.4.1)](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md): ```cmake/k4a.cmake``` looks for the library installed in your computer. 
2. [Azure Kinect Body Tracking SDK (currently using v1.1.2)](https://learn.microsoft.com/en-us/azure/kinect-dk/body-sdk-download): ```cmake/k4abt.cmake``` looks for the library installed in your computer.
3. [JSON for Modern C++ (currently using v3.10.4)](https://github.com/nlohmann/json): It's a header-only library. v3.10.4 is used as it is the same version used by rkcommon, one of dependencies for OSPRay Studio. The library is placed under ```external/json```.
4. [async-sockets (using the version from the last commit on 2/21/2022)](https://github.com/eminfedar/async-sockets-cpp): We use this library for TCP networking. We update the library to support Windows. This is a header-only library, and is placed under ```external/async-sockets```.


> [!NOTE]
> In future, we plan to use Git Submodules for 
[JSON for Modern C++](https://github.com/nlohmann/json) and [async-sockets](https://github.com/eminfedar/async-sockets-cpp). Currently, these two repos are copied to ```external/json```.
