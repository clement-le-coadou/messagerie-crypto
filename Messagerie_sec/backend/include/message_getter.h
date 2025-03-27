// message_getter.h
#ifndef MESSAGE_GETTER_H
#define MESSAGE_GETTER_H

#include <boost/json.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <memory>
#include "database.h"  // Inclure la classe Database
#include "response_sender.h"
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>

void handle_get_messages(
    const boost::json::value& parsed_body,
    boost::beast::http::response<boost::beast::http::string_body>& res,
    std::shared_ptr<Database> db_,
    std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket);

#endif // MESSAGE_GETTER_H
