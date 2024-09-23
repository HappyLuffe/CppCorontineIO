#include "co_async/debug.hpp"
#include "co_async/task.hpp"
#include "co_async/filesystem.hpp"
#include "co_async/socket.hpp"
#include "co_async/stream.hpp"

using namespace co_async;
using namespace std::literals;

EpollLoop loop;

Task<> handle_connection(AsyncFile conn) {
    debug(), "发来的消息头如下：";
    FileStream s(loop, std::move(conn));
    while (true) {
        auto l = co_await s.getchar();
        debug(), l;
        if (l == '\n') {
            break;
        }
    }
    debug(), "发来泥码被";
    co_await s.puts("HTTP/1.1 200 OK\r\n");
    co_await s.puts("Content-Type: text/plain\r\n");
    co_await s.puts("Content-Length: 12\r\n");
    co_await s.puts("\r\n");
    co_await s.puts("Hello, world");
    co_await s.flush();
    co_return;
}

Task<> amain() {
    debug(), "开始监听";
    auto serv = co_await create_tcp_server(loop, socket_address(ip_address("127.0.0.1"), 8081));
    socket_listen(serv);

    while (true) {
        auto [conn, addr] = co_await socket_accept<IpAddress>(loop, serv);

        debug(), "收到了来自", addr, "的连接";
        spawn_task(handle_connection(std::move(conn)));
    }
}

int main() {
    run_task(loop, amain());
    return 0;
}
