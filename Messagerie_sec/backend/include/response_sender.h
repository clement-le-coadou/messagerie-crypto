#ifndef RESPONSE_SENDER_H
#define RESPONSE_SENDER_H

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <iostream>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;

class ResponseSender {
public:
    static void send_response(const http::response<http::string_body>& res,
                              std::shared_ptr<ssl::stream<asio::ip::tcp::socket>> ssl_socket);
};

#endif // RESPONSE_SENDER_H
