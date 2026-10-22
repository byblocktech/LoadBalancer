/*
 * Connection of client to byteStreamer (client->byteStreamer)  (client->"server":8080)
 * Connection of byteStreamer to server  (byteStreamer->server) ("client"->server)
 * Requested, accepted google return to byteStreamer
 * byteStreamer return to client
 * server.google <-> byteStreamer <-> client
 *
 *
 *
 * class client; class byteStreamer; class server?
 */
#include <boost/asio.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <memory>
#include <ctime>

using namespace boost::asio::ip;

class tcp_server {
public:
    tcp_server(boost::asio::io_context& io_ctx, const uint16_t port)
    : io_context_(io_ctx), acceptor_(io_ctx,tcp::endpoint(tcp::v4(), port)) {

        start_accept();
    }
    ~tcp_server() {}

private:
    void start_accept() {
        auto socket = std::make_shared<tcp::socket>(io_context_);

        acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
            if (!ec) {
                std::cout << "Connected" << std::endl;
                //here the byteStream
            } else {
                std::cerr << ec.message() << std::endl;
            }

            start_accept();
        });
    }

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
};

class connByteStream : std::enable_shared_from_this<connByteStream>{
public:
    //ok, we need std::move because we cant copy the socket, that works directly with o.s
    connByteStream(tcp::socket socket) : socket_(std::move(socket)) {}

    void init() {
        read_async_bytes();
    }

private:
    void read_async_bytes() {
        auto self(shared_from_this());

        socket_.async_read_some(boost::asio::buffer(buff),
            [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                    std::cout << bytes_transferred << std::endl;
                    read_async_bytes();
                }else {
                    std::cerr << error.message() << std::endl;
                }
        });
    }
    tcp::socket socket_;
    std::array<char, 1024> buff;
};

int main() {
    for (;;) {
        try {
            boost::asio::io_context io_context;

            tcp_server tcp_server(io_context, 8080);

            io_context.run();
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}











