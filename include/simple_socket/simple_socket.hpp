#ifndef SIMPLE_SOCKET_HPP_
#define SIMPLE_SOCKET_HPP_

#include "simple_socket_exception.hpp"
#include <cstdint>
#include <memory>
#include <string>

#ifdef _WIN32
#include <Ws2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace sockets
{

enum class SocketType
{
    TYPE_STREAM = SOCK_STREAM,
    TYPE_DGRAM = SOCK_DGRAM
};

enum class SocketErrors
{
    RECEIVE_ERROR = -1,
    SEND_ERROR = -2,
    BIND_ERROR = -3,
    ACCEPT_ERROR = -4,
    CONNECT_ERROR = -5,
    LISTEN_ERROR = -6,
    INCORRECT_ADDRESS = -7
};

#ifdef _WIN32
static int socket_count = 0;
typedef int SockaddrSize;
#else
typedef unsigned int SockaddrSize;
#endif

class Socket
{
  public:
    explicit Socket(SocketType socket_type);
    ~Socket();
    int set_socket(const std::string &ip_address, uint16_t port);
    void set_timeout(const unsigned int ms = 1000);

  protected:
    void set_port(uint16_t port);
    int set_address(const std::string &ip_address);
    int socket_init();
    void socket_close();

  protected:
    int sockfd_;
    sockaddr_in address_;
    SocketType socket_type_;
};

class UDPClient : public Socket
{
  public:
    UDPClient(const std::string &ip_address = "127.0.0.1",
              uint16_t port = 8000);
    int send_mes(const char *mes, const int mes_size);
};

class UDPServer : public Socket
{
  public:
    UDPServer(const std::string &ip_address = "127.0.0.1",
              uint16_t port = 8000);
    int socket_bind();
    int receive(char *recv_buf, const int recv_buf_size);

  private:
    sockaddr_in client_;
    SockaddrSize client_size_ = sizeof(sockaddr_in);
};

class TCPSocket : public Socket
{
  public:
    TCPSocket(const std::string &ip_address, uint16_t port);
    int receive(char *recv_buf, const int recv_buf_size);
    int send_mes(const char *mes, const int mes_size);
    virtual int make_connection() = 0;
    bool is_connected();

  protected:
    bool is_connected_ = false;
    int dest_sock_;
};

class TCPClient : public TCPSocket
{
  public:
    TCPClient(const std::string &ip_address = "127.0.0.1",
              uint16_t port = 8000);
    int make_connection() override;
};

class TCPServer : public TCPSocket
{
  public:
    TCPServer(const std::string &ip_address = "127.0.0.1",
              uint16_t port = 8000);
    ~TCPServer();
    int socket_bind();
    int make_connection() override;
    void close_connection();
    int set_keepalive(const int &keepidle, const int &keepcnt,
                      const int &keepintvl);

  private:
    sockaddr_in client_;
    SockaddrSize client_size_ = sizeof(sockaddr_in);
};

} // namespace sockets

#endif // SIMPLE_SOCKET_HPP_
