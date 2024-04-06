#ifndef SIMPLE_SOCKET_EXCEPTION_HPP_
#define SIMPLE_SOCKET_EXCEPTION_HPP_

#include <stdexcept>
#include <string>


namespace sockets
{

class SimpleSocketException : public std::runtime_error
{
public:
    SimpleSocketException(const char *file, int line, const std::string &arg)
    : std::runtime_error(arg)
    {
    msg_ = std::string(file) + ":" + std::to_string(line) + ": " + arg;
    }

    const char *what() const noexcept override
    {
    return msg_.c_str();
    }

private:
    std::string msg_;
};

}  // namespace sockets

#endif  // SIMPLE_SOCKET_EXCEPTION_HPP_