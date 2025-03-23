#ifndef RESPONSE_SENDER_H
#define RESPONSE_SENDER_H

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace http = boost::beast::http;
namespace asio = boost::asio;

class ResponseSender {
public:
    static void send_response(const http::response<http::string_body>& res, asio::ip::tcp::socket& socket);
};

#endif // RESPONSE_SENDER_H
