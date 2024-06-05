#include "simple_socket/simple_socket.hpp"

namespace sockets
{

Socket::Socket(const SocketType socket_type) : socket_type_{socket_type}
{
#ifdef _WIN32
    if (!socket_count)
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            throw SimpleSocketException(__FILE__, __LINE__,
                                        "Error initializing Winsock " +
                                            WSAGetLastError());
        }
        ++socket_count;
    }
#endif

    if (socket_init() < 0)
    {
        throw SimpleSocketException(__FILE__, __LINE__,
                                    "Could not create socket");
    }

    address_.sin_family = AF_INET;
}

int Socket::set_socket(const std::string &ip_address, uint16_t port)
{
    set_port(port);
    return set_address(ip_address);
}

/**
 * @brief Sets timeouts for the connect and receive functions
 * 
 * @param ms timeout in ms. Default 1000 ms. 0 for infinite wait.
 */
void Socket::set_timeout(const unsigned int ms)
{
    timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); 
}

void Socket::set_port(uint16_t port)
{
    address_.sin_port = htons(port);
}

int Socket::set_address(const std::string &ip_address)
{
    if (ip_address.empty())
    {
        address_.sin_addr.s_addr = htonl(INADDR_ANY);
        return 0;
    }
    if (!inet_pton(AF_INET, ip_address.c_str(), &address_.sin_addr))
    {
        return static_cast<int>(SocketErrors::INCORRECT_ADDRESS);
    }
    return 0;
}

int Socket::socket_init()
{
    sockfd_ = socket(AF_INET, static_cast<int>(socket_type_), 0);
    if (sockfd_ < 0)
    {
        return -1;
    }

    set_timeout();

    int optval = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) <
        0)
    {
        return -1;
    }

    return 0;
}

void Socket::socket_close()
{
#ifdef _WIN32
    ::closesocket(sockfd_);
    --socket_count;
#else
    ::close(sockfd_);
#endif
}

Socket::~Socket()
{
    socket_close();

#ifdef _WIN32
    if (!socket_count)
    {
        WSACleanup();
    }
#endif
}

UDPClient::UDPClient(const std::string &ip_address, uint16_t port)
    : Socket(SocketType::TYPE_DGRAM)
{
    set_address(ip_address);
    set_port(port);
}

int UDPClient::send_mes(const char *mes, const int mes_size)
{
    if (sendto(sockfd_, mes, mes_size, 0,
               reinterpret_cast<sockaddr *>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}

UDPServer::UDPServer(const std::string &ip_address, uint16_t port)
    : Socket(SocketType::TYPE_DGRAM)
{
    set_port(port);
    set_address(ip_address);
}

int UDPServer::socket_bind()
{
    if (bind(sockfd_, reinterpret_cast<sockaddr *>(&address_),
             sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::BIND_ERROR);
    return 0;
}

int UDPServer::receive(char *recv_buf, const int recv_buf_size)
{
    return recvfrom(sockfd_, recv_buf, recv_buf_size, 0,
                    reinterpret_cast<sockaddr *>(&client_), &client_size_);
}

TCPSocket::TCPSocket(const std::string &ip_address, uint16_t port)
    : Socket(SocketType::TYPE_STREAM)
{
    set_address(ip_address);
    set_port(port);
}

int TCPSocket::receive(char *recv_buf, const int recv_buf_size)
{
    if (!is_connected_ && make_connection())
    {
        return static_cast<int>(SocketErrors::CONNECT_ERROR);
    }

    int res = recv(dest_sock_, recv_buf, recv_buf_size, 0);

    if (res < 0)
    {
        make_connection();
    }

    return res;
}

int TCPSocket::send_mes(const char *mes, const int mes_size)
{
    if (!is_connected_ && make_connection())
    {
        return static_cast<int>(SocketErrors::CONNECT_ERROR);
    }

#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif

    if (send(dest_sock_, mes, mes_size, flags) < 0)
    {
        make_connection();
        return static_cast<int>(SocketErrors::SEND_ERROR);
    }

    return 0;
}

bool TCPSocket::is_connected()
{
    return is_connected_;
}

TCPClient::TCPClient(const std::string &ip_address, uint16_t port)
    : TCPSocket(ip_address, port)
{
    dest_sock_ = sockfd_;
}

int TCPClient::make_connection()
{
    // you cannot reuse the connect function for the same socket
    if (is_connected_)
    {
        socket_close();
        socket_init();
        dest_sock_ = sockfd_;
        is_connected_ = false;
    }

    if (connect(sockfd_, reinterpret_cast<sockaddr *>(&address_),
                sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::CONNECT_ERROR);
    is_connected_ = true;

    return 0;
}

TCPServer::TCPServer(const std::string &ip_address, uint16_t port)
    : TCPSocket(ip_address, port)
{
}

TCPServer::~TCPServer()
{
    close_connection();
}

int TCPServer::socket_bind()
{
    if (bind(sockfd_, reinterpret_cast<sockaddr *>(&address_),
             sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::BIND_ERROR);
    return 0;
}

int TCPServer::make_connection()
{
    if (listen(sockfd_, 5) < 0)
        return static_cast<int>(SocketErrors::LISTEN_ERROR);

    close_connection();

    dest_sock_ =
        accept(sockfd_, reinterpret_cast<sockaddr *>(&client_), &client_size_);

    if (dest_sock_ < 0)
        return static_cast<int>(SocketErrors::ACCEPT_ERROR);

    is_connected_ = true;

    return 0;
}

void TCPServer::close_connection()
{
    if (is_connected_)
    {
#ifdef _WIN32
        ::closesocket(dest_sock_);
#else
        ::close(dest_sock_);
#endif
        is_connected_ = false;
    }
}

/**
 * @brief Set the keepalive options on the socket
 *
 * @param keepidle The time (in seconds) the connection needs to remain
 * idle before TCP starts sending keepalive probes (TCP_KEEPIDLE socket option)
 * (>=1)
 * @param keepcnt The maximum number of keepalive probes TCP should
 * send before dropping the connection. (TCP_KEEPCNT socket option) (>=1)
 * @param keepintvl The time (in seconds) between individual keepalive probes.
 * (TCP_KEEPINTVL socket option) (>=1)
 * @return int
 */
int TCPServer::set_keepalive(const int &keepidle, const int &keepcnt,
                             const int &keepintvl)
{
    if (keepidle < 1 || keepcnt < 1 || keepintvl < 1)
    {
        throw SimpleSocketException(__FILE__, __LINE__,
                                    "set_keepalive parameters are incorrect. "
                                    "Values cannot be less than one");
    }

    int optval = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval,
                   sizeof(optval)) < 0)
    {
        return -1;
    }

    // set the keepalive options
    if (setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPCNT, (char *)&keepcnt,
                   sizeof(keepcnt)) < 0)
    {
        return -1;
    }

    if (setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPIDLE, (char *)&keepidle,
                   sizeof(keepidle)) < 0)
    {
        return -1;
    }

    if (setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPINTVL, (char *)&keepintvl,
                   sizeof(keepintvl)) < 0)
    {
        return -1;
    }

    return 0;
}

} // namespace sockets
