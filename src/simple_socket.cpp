#include "simple_socket/simple_socket.hpp"


namespace sockets
{


Socket::Socket(const SocketType socket_type)
{
    sockfd_ = socket(AF_INET, static_cast<int>(socket_type), 0);
    if (sockfd_ < 0)
        throw SimpleSocketException(__FILE__, __LINE__, "Could not create socket");

    address_.sin_family = AF_INET;
}


void Socket::set_socket(uint16_t port, const std::string& ip_address)
{
    if (sockfd_ > 0) {
        close(sockfd_);
        sockfd_ = -1;
    }
    
    set_address(ip_address);
    set_port(port);
}


void Socket::set_port(uint16_t port)
{
    address_.sin_port = htons(port);
}


void Socket::set_address(const std::string& ip_address)
{
    if(!inet_pton(AF_INET, ip_address.c_str(), &address_.sin_addr))
        throw SimpleSocketException(__FILE__, __LINE__, "Bad address");
}


Socket::~Socket()
{
    close(sockfd_);
}


UDPClient::UDPClient(uint16_t port, const std::string& ip_address) : Socket(SocketType::TYPE_DGRAM)
{
    set_address(ip_address);
    set_port(port);
}


int UDPClient::send_mes(const uint8_t* mes, const size_t mes_size)
{
    if(sendto(sockfd_, mes, mes_size, 0, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}


UDPServer::UDPServer(uint16_t port, const std::string& ip_address) 
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


ssize_t UDPServer::receive(uint8_t* recv_buf, const size_t recv_buf_size)
{
    return recvfrom(sockfd_, recv_buf, recv_buf_size, 0, reinterpret_cast<sockaddr*>(&client_), &client_size_);
}


TCPClient::TCPClient(uint16_t port, const std::string& ip_address) 
: Socket(SocketType::TYPE_STREAM)
{
    set_address(ip_address);
    set_port(port);
}


int TCPClient::make_connection()
{
    if (connect(sockfd_, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::CONNECT_ERROR);
    return 0;
}


ssize_t TCPClient::receive(uint8_t* recv_buf, const size_t recv_buf_size)
{
    return recv(sockfd_, recv_buf, recv_buf_size, 0);
}


int TCPClient::send_mes(const uint8_t* mes, const size_t mes_size)
{
    if(send(sockfd_, mes, mes_size, 0) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}



TCPServer::TCPServer(uint16_t port, const std::string& ip_address) 
: Socket(SocketType::TYPE_STREAM)
{
    set_port(port);
    set_address(ip_address);
};


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

    client_sock_ = accept(sockfd_, reinterpret_cast<sockaddr*>(&client_), &client_size_);

    if (client_sock_ < 0)
        return static_cast<int>(SocketErrors::ACCEPT_ERROR);

    return 0;
}


ssize_t TCPServer::receive(uint8_t* recv_buf, const size_t recv_buf_size)
{
    return recvfrom(sockfd_, recv_buf, recv_buf_size, 0, reinterpret_cast<sockaddr*>(&client_), &client_size_);
}


int TCPServer::send_mes(const uint8_t* mes, const size_t mes_size)
{
    if(sendto(sockfd_, mes, mes_size, 0, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}


} // namespace sockets
