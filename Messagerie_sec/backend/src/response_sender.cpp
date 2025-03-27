#include "response_sender.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include "response_sender.h"

void ResponseSender::send_response(const http::response<http::string_body>& res,
                                   std::shared_ptr<ssl::stream<asio::ip::tcp::socket>> ssl_socket) {
    http::async_write(*ssl_socket, res, [ssl_socket](beast::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "Write error: " << ec.message() << std::endl;
        }
        ssl_socket->async_shutdown([](beast::error_code ec) {
            if (ec) {
                std::cerr << "SSL Shutdown error: " << ec.message() << std::endl;
            }
        });
    });
}
