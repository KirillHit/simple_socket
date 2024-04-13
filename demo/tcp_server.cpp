#include "simple_socket/simple_socket.hpp"
#include <iostream>
#include <string>
#include <vector>


int main()
{
    std::string send_data = "Hello client";
    std::vector<char> recv_data(13);
    bool is_connected = false;
    
    sockets::TCPServer server;
    server.set_socket("127.0.0.1", 10000);
    if(server.socket_bind() < 0) {
        std::cout << "Bind error" << std::endl;
        return 1;
    }

    while (true) 
    {
        if (!is_connected) {
            if(server.socket_listen() < 0) {
                std::cout << "Connect error" << std::endl;
                continue;
            }
            is_connected = true;
        }
        
        if(server.receive(recv_data.data(), recv_data.size()) < 0) {
            std::cout << "Receive error" << std::endl;
            is_connected = false;
            continue;
        }
        std::cout << "Receive: " << std::string(recv_data.begin(), recv_data.end()) << std::endl;
        
        if(server.send_mes(send_data.c_str(), send_data.size()) < 0) {
            std::cout << "Send error" << std::endl;
            is_connected = false;
            continue;
        }
        std::cout << "Send: " << send_data << std::endl;

    }

    std::cout << "Stop server" << std::endl;

    return 0;
}