#ifndef GET_CONTACT_REQUESTS_HANDLER_H
#define GET_CONTACT_REQUESTS_HANDLER_H

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include "database.h"
#include "response_sender.h"

namespace http = boost::beast::http;
namespace json = boost::json;

class GetContactRequestsHandler {
public:
    static void handle_get_contact_requests(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, boost::asio::ip::tcp::socket& socket);
};

#endif // GET_CONTACT_REQUESTS_HANDLER_H
