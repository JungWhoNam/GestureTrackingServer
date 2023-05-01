add_library (GestureTracker INTERFACE)

target_include_directories(GestureTracker INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)