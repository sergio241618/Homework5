cmake_minimum_required(VERSION 3.10)
project(LinuxCaesarTCP C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add the caesar cipher source file to a library
add_library(caesar STATIC caesar.c)

# Add the server executable and link it with the caesar library
add_executable(tcp_server server.c)
target_link_libraries(tcp_server PRIVATE caesar)

# Add the client executable and link it with the caesar library
add_executable(tcp_client client.c)
target_link_libraries(tcp_client PRIVATE caesar)