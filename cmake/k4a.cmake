# set("k4a_DIR_PATH" "C:/Program Files/Azure Kinect SDK v1.4.1/")

if(NOT EXISTS "${k4a_DIR_PATH}" OR NOT IS_DIRECTORY "${k4a_DIR_PATH}")
    message(FATAL_ERROR "The directory for Azure Kinect SDK is not valid. Please specify k4a_DIR_PATH.")
endif()

add_library(k4a::k4a SHARED IMPORTED)

set_target_properties(k4a::k4a PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${k4a_DIR_PATH}/sdk/include/"
)

set_property(TARGET k4a::k4a APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(k4a::k4a PROPERTIES 
    IMPORTED_IMPLIB_RELEASE "${k4a_DIR_PATH}/sdk/windows-desktop/amd64/release/lib/k4a.lib"
    IMPORTED_LOCATION_RELEASE "${k4a_DIR_PATH}/sdk/windows-desktop/amd64/release/bin/k4a.dll"
)

set_property(TARGET k4a::k4a APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(k4a::k4a PROPERTIES 
    IMPORTED_IMPLIB_DEBUG "${k4a_DIR_PATH}/sdk/windows-desktop/amd64/release/lib/k4a.lib"
    IMPORTED_LOCATION_DEBUG "${k4a_DIR_PATH}/sdk/windows-desktop/amd64/release/bin/k4a.dll"
)