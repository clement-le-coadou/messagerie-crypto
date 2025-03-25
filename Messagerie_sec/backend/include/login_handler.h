#ifndef LOGIN_HANDLER_H
#define LOGIN_HANDLER_H

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <jwt-cpp/jwt.h>
#include "database.h"
#include "response_sender.h"
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace http = boost::beast::http;
namespace json = boost::json;

class LoginHandler {
public:
    static void handle_login(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket);
};

#endif // LOGIN_HANDLER_H
