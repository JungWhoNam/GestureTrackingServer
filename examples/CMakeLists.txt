cmake_minimum_required(VERSION 3.15)

project(tutorial_socket)

set (CMAKE_CXX_STANDARD 14)
message (STATUS "This build is for C++${CMAKE_CXX_STANDARD}")

add_executable("tcp-client" tcp-client.cpp)
add_executable("tcp-server" tcp-server.cpp)

# # Add -lpthread
# target_link_libraries(tcp-client pthread)
# target_link_libraries(tcp-server pthread)

# # # Add -I../async-sockets
# target_include_directories(tcp-client PUBLIC ../async-sockets)
# target_include_directories(tcp-server PUBLIC ../async-sockets)

# add_executable("simple-tcp-server" simple-server.cpp)
# add_executable("simple-tcp-client" simple-client.cpp)