add_executable(tcp_client tcp_client.cpp)
target_link_libraries(tcp_client PRIVATE simple_socket)

add_executable(tcp_server tcp_server.cpp)
target_link_libraries(tcp_server PRIVATE simple_socket)

add_executable(udp_client udp_client.cpp)
target_link_libraries(udp_client PRIVATE simple_socket)

add_executable(udp_server udp_server.cpp)
target_link_libraries(udp_server PRIVATE simple_socket)