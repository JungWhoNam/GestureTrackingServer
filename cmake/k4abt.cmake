# set("k4abt_DIR_PATH" "C:/Program Files/Azure Kinect Body Tracking SDK/")

if(NOT EXISTS "${k4abt_DIR_PATH}" OR NOT IS_DIRECTORY "${k4abt_DIR_PATH}")
    message(FATAL_ERROR "The directory for Azure Kinect Body Tracking SDK is not valid. Please specify k4abt_DIR_PATH.")
endif()

add_library(k4a::k4abt SHARED IMPORTED)

set_target_properties(k4a::k4abt PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${k4abt_DIR_PATH}/sdk/include/"
)

set_property(TARGET k4a::k4abt APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(k4a::k4abt PROPERTIES 
    IMPORTED_IMPLIB_RELEASE "${k4abt_DIR_PATH}/sdk/windows-desktop/amd64/release/lib/k4abt.lib"
    IMPORTED_LOCATION_RELEASE "${k4abt_DIR_PATH}/sdk/windows-desktop/amd64/release/bin/k4abt.dll"
)

set_property(TARGET k4a::k4abt APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(k4a::k4abt PROPERTIES 
    IMPORTED_IMPLIB_DEBUG "${k4abt_DIR_PATH}/sdk/windows-desktop/amd64/release/lib/k4abt.lib"
    IMPORTED_LOCATION_DEBUG "${k4abt_DIR_PATH}/sdk/windows-desktop/amd64/release/bin/k4abt.dll"
)