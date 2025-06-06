#ifndef CONTACT_HANDLER_H
#define CONTACT_HANDLER_H

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <boost/asio.hpp>
#include "database.h"
#include "response_sender.h"
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace http = boost::beast::http;
namespace json = boost::json;
using boost::asio::ip::tcp;

class ContactHandler {
public:
    static void handle_get_contacts(const json::value& parsed_body, http::response<http::string_body>& res, Database* db_, std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket);
};

#endif // CONTACT_HANDLER_H
