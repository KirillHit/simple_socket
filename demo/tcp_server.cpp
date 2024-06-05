#include "simple_socket/simple_socket.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace std::chrono_literals;

#pragma pack(push, 1)
struct Msg
{
    char str[13] = "Hello client";
    uint16_t mes_num = 0;
    int64_t time = 0;
};
#pragma pack(pop)
const size_t MSG_SIZE = sizeof(Msg);

int main()
{
    Msg rx_msg;
    Msg tx_msg;

    // Server initialization
    sockets::TCPServer server;
    server.set_socket("127.0.0.1", 10000);
    server.set_keepalive(1, 1, 1);
    server.set_timeout(50);
    server.socket_bind();

    int idx = 0;
    while (true)
    {
        std::this_thread::sleep_for(500ms);
        ++idx;
        std::cout << "try: " << idx << "; " << std::endl;

        // Receive message
        int res =
            server.receive(reinterpret_cast<char *>(&rx_msg), MSG_SIZE);

        // Processing the result
        switch (res)
        {
        case static_cast<int>(sockets::SocketErrors::RECEIVE_ERROR):
            std::cerr << "Receive error" << std::endl;
            continue;
            break;
        case static_cast<int>(sockets::SocketErrors::CONNECT_ERROR):
            std::cerr << "Connect error" << std::endl;
            continue;
            break;
        case 0:
            std::cerr << "Empty message" << std::endl;
            break;
            continue;
        default:
            std::cout << "Recv: " << rx_msg.mes_num << " " << rx_msg.time << " "
                      << rx_msg.str << std::endl;
            break;
        }

        tx_msg.mes_num = idx;
        tx_msg.time =
            std::chrono::system_clock::now().time_since_epoch().count();

        // Send message
        res = server.send_mes(reinterpret_cast<char *>(&tx_msg), MSG_SIZE);

        // Processing the result
        switch (res)
        {
        case static_cast<int>(sockets::SocketErrors::CONNECT_ERROR):
            std::cerr << "Connect error" << std::endl;
            break;
        case static_cast<int>(sockets::SocketErrors::SEND_ERROR):
            std::cerr << "Send error" << std::endl;
            break;
        default:
            std::cout << "Send success; " << std::endl;
            break;
        }
    }

    return 0;
}