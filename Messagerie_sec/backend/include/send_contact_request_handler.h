#ifndef SEND_CONTACT_REQUEST_HANDLER_H
#define SEND_CONTACT_REQUEST_HANDLER_H

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include "database.h"
#include "response_sender.h"
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace http = boost::beast::http;
namespace json = boost::json;

class SendContactRequestHandler {
public:
    static void handle_send_contact_request(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket);
};

#endif // SEND_CONTACT_REQUEST_HANDLER_H
