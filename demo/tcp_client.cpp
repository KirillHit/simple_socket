#include "simple_socket/simple_socket.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <vector>

using namespace std::chrono_literals;


int main()
{
    std::string send_data = "Hello server";
    std::vector<char> recv_data(13);
    bool is_connected = false;
    
    sockets::TCPClient client;
    client.set_socket("127.0.0.1", 10000);

    while (true) 
    {
        std::this_thread::sleep_for(500ms);
        
        if (!is_connected) {
            if(client.make_connection() < 0) {
                std::cout << "Connect error" << std::endl;
                continue;
            } 
            is_connected = true;
        }
        
        if(client.send_mes(send_data.c_str(), send_data.size()) < 0) {
            std::cout << "Send error" << std::endl;
            is_connected = false;
            continue;
        }
        std::cout << "Send: " << send_data << std::endl;
        
        if(client.receive(recv_data.data(), recv_data.size()) < 0) {
            std::cout << "Receive error" << std::endl;
            is_connected = false;
            continue;
        }
        std::cout << "Receive: " << std::string(recv_data.begin(), recv_data.end()) << std::endl;
    }

    std::cout << "Stop client" << std::endl;

    return 0;
}