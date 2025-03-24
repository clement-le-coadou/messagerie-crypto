#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include "database.h"
#include "message_handler.h"
#include "message_getter.h"
#include "response_sender.h"
#include "contact_handler.h"
#include "send_contact_request_handler.h"
#include "handle_request_handler.h"
#include "get_contact_requests_handler.h"
#include "login_handler.h"
#include "register_handler.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace websocket = boost::beast::websocket;

class Server {
public:
    explicit Server(unsigned short port);
    void run();

private:
    void do_accept();
    int validate_jwt_token(const std::string& token);
    void handle_request(beast::http::request<beast::http::string_body> req, asio::ip::tcp::socket& socket);
    void handle_websocket(asio::ip::tcp::socket socket);
    void broadcast_message(const std::string& message);
    void send_response(const beast::http::response<beast::http::string_body>& res, asio::ip::tcp::socket& socket);

    asio::io_context ioc;
    asio::ip::tcp::acceptor acceptor_;
    std::shared_ptr<Database> db_;
    std::set<std::shared_ptr<websocket::stream<asio::ip::tcp::socket>>> clients_;
};

#endif // SERVER_H
