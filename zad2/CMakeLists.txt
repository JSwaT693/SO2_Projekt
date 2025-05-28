cmake_minimum_required(VERSION 3.27)
project(Chat)

set(CMAKE_CXX_STANDARD 17)

add_executable(server
        server/Server.cpp
)
target_link_libraries(server ws2_32)

add_executable(client
        client/Client.cpp
)
target_link_libraries(client ws2_32)
