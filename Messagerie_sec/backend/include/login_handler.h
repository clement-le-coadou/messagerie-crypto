#ifndef LOGIN_HANDLER_H
#define LOGIN_HANDLER_H

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <jwt-cpp/jwt.h>
#include "database.h"
#include "response_sender.h"

namespace http = boost::beast::http;
namespace json = boost::json;

class LoginHandler {
public:
    static void handle_login(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, boost::asio::ip::tcp::socket& socket);
};

#endif // LOGIN_HANDLER_H
