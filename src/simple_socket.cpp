#include "simple_socket/simple_socket.hpp"


namespace sockets
{

Socket::Socket(const SocketType socket_type) : socket_type_{socket_type}
{
#ifdef _WIN32
    if (!socket_count) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            throw SimpleSocketException(__FILE__, __LINE__, "Error initializing Winsock " + WSAGetLastError());
        }
        ++socket_count;
    }
#endif
    
    sockfd_ = socket(AF_INET, static_cast<int>(socket_type_), 0);
    if (sockfd_ < 0) {
        throw SimpleSocketException(__FILE__, __LINE__, "Could not create socket");
    }

    address_.sin_family = AF_INET;
}


int Socket::set_socket(const std::string& ip_address, uint16_t port)
{    
    set_port(port);
    return set_address(ip_address);
}


void Socket::set_port(uint16_t port)
{
    address_.sin_port = htons(port);
}


int Socket::set_address(const std::string& ip_address)
{
    if(!inet_pton(AF_INET, ip_address.c_str(), &address_.sin_addr)) {
        return static_cast<int>(SocketErrors::INCORRECT_ADDRESS);
    }
    return 0;
}


void Socket::socket_update()
{
    socket_close();
    sockfd_ = socket(AF_INET, static_cast<int>(socket_type_), 0);
    if (sockfd_ < 0) {
        throw SimpleSocketException(__FILE__, __LINE__, "Could not create socket");
    }
}


void Socket::socket_close()
{
#ifdef _WIN32
    ::closesocket(sockfd_);
    if (!--socket_count){
        WSACleanup();
    }
#else
    ::close(sockfd_);
#endif
}


Socket::~Socket()
{
    socket_close();
}


UDPClient::UDPClient(const std::string& ip_address, uint16_t port) : Socket(SocketType::TYPE_DGRAM)
{
    set_address(ip_address);
    set_port(port);
}


int UDPClient::send_mes(const char* mes, const int mes_size)
{
    if(sendto(sockfd_, mes, mes_size, 0, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}


UDPServer::UDPServer(const std::string& ip_address, uint16_t port) 
: Socket(SocketType::TYPE_DGRAM)
{
    set_port(port);
    set_address(ip_address);
}


int UDPServer::socket_bind()
{
    if (bind(sockfd_, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::BIND_ERROR);
    return 0;
}


int UDPServer::receive(char* recv_buf, const int recv_buf_size)
{
    return recvfrom(sockfd_, recv_buf, recv_buf_size, 0, reinterpret_cast<sockaddr*>(&client_), &client_size_);
}


TCPSocket::TCPSocket() 
: Socket(SocketType::TYPE_STREAM)
{}


/**
 * @brief Set the keepalive options on the socket
 * 
 * @param keepidle The time (in seconds) the connection needs to remain 
 * idle before TCP starts sending keepalive probes (TCP_KEEPIDLE socket option)
 * @param keepcnt The maximum number of keepalive probes TCP should 
 * send before dropping the connection. (TCP_KEEPCNT socket option)
 * @param keepintvl The time (in seconds) between individual keepalive probes.
 * (TCP_KEEPINTVL socket option)
 * @return int 
 */
int TCPSocket::set_keepalive(const int& keepidle, const int& keepcnt, const int& keepintvl)
{
    int optval = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0) {
        return -1;
    }

    //set the keepalive options
    if (setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)) < 0) {
        return -1;
    }

    if (setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle)) < 0) {
        return -1;
    }

    if (setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl)) < 0) {
        return -1;
    }

    return 0;
}


TCPClient::TCPClient(const std::string& ip_address, uint16_t port) 
{
    set_address(ip_address);
    set_port(port);
}


int TCPClient::make_connection()
{
    // you cannot reuse the connect function for the same socket
    if (is_connected) {
        socket_update();
        is_connected = false;
    }

    if (connect(sockfd_, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::CONNECT_ERROR);
    is_connected = true;

    return 0;
}


int TCPClient::receive(char* recv_buf, const int recv_buf_size)
{
    return recv(sockfd_, recv_buf, recv_buf_size, 0);
}


int TCPClient::send_mes(const char* mes, const int mes_size)
{
    if(send(sockfd_, mes, mes_size, MSG_NOSIGNAL) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}


TCPServer::TCPServer(const std::string& ip_address, uint16_t port)
{
    set_port(port);
    set_address(ip_address);
};


TCPServer::~TCPServer()
{
    close_connection();
}


int TCPServer::socket_bind()
{
    if (bind(sockfd_, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::BIND_ERROR);
    return 0;
}


int TCPServer::socket_listen()
{
    if (listen(sockfd_, 5) < 0)
        return static_cast<int>(SocketErrors::LISTEN_ERROR);

    close_connection();

    client_sock_ = accept(sockfd_, reinterpret_cast<sockaddr*>(&client_), &client_size_);

    if (client_sock_ < 0)
        return static_cast<int>(SocketErrors::ACCEPT_ERROR);

    is_open = true;

    return 0;
}


void TCPServer::close_connection()
{
    if(is_open) {
#ifdef _WIN32
        ::closesocket(client_sock_);
#else
        ::close(client_sock_);
#endif
        is_open = false;
    }
}


int TCPServer::receive(char* recv_buf, const int recv_buf_size)
{
    return recv(client_sock_, recv_buf, recv_buf_size, 0);
}


int TCPServer::send_mes(const char* mes, const int mes_size)
{
    if(send(client_sock_, mes, mes_size, MSG_NOSIGNAL) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}


} // namespace sockets
