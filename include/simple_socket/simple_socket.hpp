#ifndef SIMPLE_SOCKET_HPP_
#define SIMPLE_SOCKET_HPP_

#include "simple_socket_exception.hpp"
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <memory>
#include <string>
#include <thread>


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
protected:
    void set_port(uint16_t port);
    void set_address(const std::string& ip_address);
protected:
    int sockfd_;
    sockaddr_in address_;
};


class SocketBuf : public Socket
{
public:
    SocketBuf(SocketType socket_type ,const size_t buf_size = 65535);
    std::shared_ptr<uint8_t> data();
protected:
    size_t buf_size_;
    std::shared_ptr<uint8_t> buf_;
};


class UDPClient : public Socket
{
public:
    UDPClient(uint16_t port = 8000, const std::string& ip_address = "127.0.0.1");
    int send_message(const std::string& message);
    int send_message(const uint8_t* message, const size_t len);
};


class UDPServer : public SocketBuf
{
public:
    UDPServer(uint16_t port = 8000, const std::string& ip_address = "0.0.0.0", const size_t buf_size = 65535);
    int socket_bind();
    ssize_t receive();
private:
    sockaddr_in client_;
    socklen_t client_size_ = sizeof(sockaddr_in);
};


class TCPClient : public SocketBuf
{
public:
    TCPClient(uint16_t port = 8000, const std::string& ip_address = "127.0.0.1", const size_t buf_size = 65535);
    int make_connection();
    int send_message(const std::string& message);
    int send_message(const uint8_t* message, const size_t len);
};


class TCPServer : public SocketBuf
{
public:
    TCPServer(u_short port, const std::string& ip_address = "0.0.0.0", const size_t buf_size = 65535);
    int socket_bind();
    int spin(void (*func)(uint8_t* const, const size_t, uint8_t** const, size_t* const));
private:
    int client_sock_;
    sockaddr_in client_;
    socklen_t client_size_ = sizeof(sockaddr_in);
};


}  // namespace sockets

#endif  // SIMPLE_SOCKET_HPP_
