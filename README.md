# SimpleSocket
C++ библиотека неблокирующих сокетов (UDP/TCP) для Windows и Linux. Проектировалась для решения задач телеуправления и предполагает постоянное соединение двух устройств для передачи данных. Устойчива к разрывам соединения. Примеры использования: [udp_streamer](https://github.com/KirillHit/udp_streamer.git), [youbot_rc](https://github.com/KirillHit/youbot_rc.git).

## Demo
В папке «demo» содержатся примеры кода сервера и клиента (UDP и TCP). 

Сборка и запуск:

``` bash
mkdir build
cd build
cmake -DDemo:=ON ..
cmake --build .
cd demo
# First terminal
.\tcp_server
# second terminal
.\tcp_client
```

## Examples
``` c++
// TCP server
#include "simple_socket/simple_socket.hpp"

int main()
{
    sockets::TCPServer server;
    server.set_socket("127.0.0.1", 10000);
    server.set_keepalive(1, 1, 1);
    server.set_timeout(500);
    server.socket_bind();

    server.receive(reinterpret_cast<char *>(&rx_msg), MSG_SIZE);
    server.send_mes(reinterpret_cast<char *>(&tx_msg), MSG_SIZE);
    
    return 0;
}
```

``` c++
// TCP client
#include "simple_socket/simple_socket.hpp"

int main()
{
    sockets::TCPClient client;
    client.set_socket("127.0.0.1", 10000);

    client.send_mes(reinterpret_cast<char *>(&tx_msg), MSG_SIZE);
    client.receive(reinterpret_cast<char *>(&rx_msg), MSG_SIZE);
    
    return 0;
}
```
