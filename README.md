# SimpleSocket
Simple socket implementation for C++. Supports Windows and Linux.

### Example
``` c++
#include "simple_socket/simple_socket.hpp"
#include <vector>

int main()
{
    sockets::UDPServer socket("127.0.0.1", 10000);
    socket_bind();

    std::vector<char> buf(65535);

    socket.receive(buf.data(), buf.size());
    
    return 0;
}
```
