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


SocketBuf::SocketBuf(SocketType socket_type ,const size_t buf_size) 
: Socket(socket_type), buf_size_{buf_size}, buf_(new uint8_t[buf_size]) 
{}


std::shared_ptr<uint8_t> SocketBuf::data()
{
    return buf_;
}


UDPClient::UDPClient(uint16_t port, const std::string& ip_address) : Socket(SocketType::TYPE_DGRAM)
{
    set_address(ip_address);
    set_port(port);
}


int UDPClient::send_message(const std::string& message)
{
    return send_message((uint8_t*) message.c_str(), message.length());
}


int UDPClient::send_message(const uint8_t* message, const size_t len)
{
    if(sendto(sockfd_, message, len, 0, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return 0;
}


UDPServer::UDPServer(uint16_t port, const std::string& ip_address, const size_t buf_size) 
: SocketBuf(SocketType::TYPE_DGRAM, buf_size)
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


ssize_t UDPServer::receive()
{
    return recvfrom(sockfd_, buf_.get(), buf_size_, 0, reinterpret_cast<sockaddr*>(&client_), &client_size_);
}


TCPClient::TCPClient(uint16_t port, const std::string& ip_address, const size_t buf_size) 
: SocketBuf(SocketType::TYPE_STREAM, buf_size)
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


int TCPClient::send_message(const std::string& message)
{
    return send_message((uint8_t*) message.c_str(), message.length());
}


int TCPClient::send_message(const uint8_t* message, const size_t len)
{
    if (send(sockfd_, message, len, 0) < 0)
        return static_cast<int>(SocketErrors::SEND_ERROR);
    return recv(sockfd_, buf_.get(), buf_size_, 0);
}


TCPServer::TCPServer(uint16_t port, const std::string& ip_address, const size_t buf_size) 
: SocketBuf(SocketType::TYPE_STREAM, buf_size)
{
    set_port(port);
    set_address(ip_address);
};


int TCPServer::socket_bind()
{
    if (bind(sockfd_, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0)
        return static_cast<int>(SocketErrors::BIND_ERROR);

    if (listen(sockfd_, 5) < 0)
        return static_cast<int>(SocketErrors::LISTEN_ERROR);

    client_sock_ = accept(sockfd_, reinterpret_cast<sockaddr*>(&client_), &client_size_);

    if (client_sock_ < 0)
        return static_cast<int>(SocketErrors::ACCEPT_ERROR);

    return 0;
}


int TCPServer::spin(void (*func)(uint8_t* const, const size_t, uint8_t** const, size_t* const))
{
    if (recv(client_sock_, buf_.get(), buf_size_, 0) < 0)
        return static_cast<int>(SocketErrors::RECEIVE_ERROR);

    uint8_t* ans_buf;
    size_t ans_size;

    func(buf_.get(), buf_size_, &ans_buf, &ans_size);
        
    if (send(client_sock_, ans_buf, ans_size, 0))
        return static_cast<int>(SocketErrors::SEND_ERROR);
    
    return 0;
}


} // namespace sockets
