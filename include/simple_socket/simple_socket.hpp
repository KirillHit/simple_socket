#ifndef SIMPLE_SOCKET_HPP_
#define SIMPLE_SOCKET_HPP_

#include "simple_socket_exception.hpp"
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <memory>


namespace sockets
{


enum class SocketType {
    TYPE_STREAM = SOCK_STREAM,
    TYPE_DGRAM = SOCK_DGRAM
};


enum class SocketErrors {
    RECEIVE_ERROR = -1,
    SEND_ERROR = -2,
    BIND_ERROR = -3,
    ACCEPT_ERROR = -4,
    CONNECT_ERROR = -5,
    LISTEN_ERROR = -6
};


class Socket
{
public:
    explicit Socket(SocketType socket_type);
    ~Socket();
    void set_socket(uint16_t port = 8000, const std::string& ip_address = "127.0.0.1");
protected:
    void set_port(uint16_t port);
    void set_address(const std::string& ip_address);
protected:
    int sockfd_;
    sockaddr_in address_;
};


class UDPClient : public Socket
{
public:
    UDPClient(uint16_t port = 8000, const std::string& ip_address = "127.0.0.1");
    int send_mes(const uint8_t* mes, const size_t mes_size);
};


class UDPServer : public Socket
{
public:
    UDPServer(uint16_t port = 8000, const std::string& ip_address = "127.0.0.1");
    int socket_bind();
    ssize_t receive(uint8_t* recv_buf, const size_t recv_buf_size);
private:
    sockaddr_in client_;
    socklen_t client_size_ = sizeof(sockaddr_in);
};


class TCPClient : public Socket
{
public:
    TCPClient(uint16_t port = 8000, const std::string& ip_address = "127.0.0.1");
    int make_connection();
    ssize_t receive(uint8_t* recv_buf, const size_t recv_buf_size);
    int send_mes(const uint8_t* mes, const size_t mes_size);
};


class TCPServer : public Socket
{
public:
    TCPServer(uint16_t port = 8000, const std::string& ip_address = "127.0.0.1");
    int socket_bind();
    int socket_listen();
    ssize_t receive(uint8_t* recv_buf, const size_t recv_buf_size);
    int send_mes(const uint8_t* mes, const size_t mes_size);
private:
    int client_sock_;
    sockaddr_in client_;
    socklen_t client_size_ = sizeof(sockaddr_in);
};


}  // namespace sockets

#endif  // SIMPLE_SOCKET_HPP_
