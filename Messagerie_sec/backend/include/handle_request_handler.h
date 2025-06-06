#ifndef HANDLE_REQUEST_HANDLER_H
#define HANDLE_REQUEST_HANDLER_H

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>
#include "database.h"
#include "response_sender.h"

namespace http = boost::beast::http;
namespace json = boost::json;

class HandleRequestHandler {
public:
    static void handle_contact_request(const json::value& parsed_body, http::response<http::string_body>& res, 
        std::shared_ptr<Database> db_, 
        std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket);
};

#endif // HANDLE_REQUEST_HANDLER_H
