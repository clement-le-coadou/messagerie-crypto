#include "server.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <pqxx/pqxx>

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace json = boost::json;
namespace websocket = boost::beast::websocket;

Server::Server(unsigned short port)
    : acceptor_(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      db_(std::make_shared<Database>("dbname=secure_messenger user=postgres password=cytech0001 host=localhost port=5432")) {
    std::cout << "Server started on port " << port << std::endl;
}

void Server::run() {
    do_accept();
    ioc.run();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](beast::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "Client connected: " << socket.remote_endpoint() << std::endl;

                beast::flat_buffer buffer;
                http::request<http::string_body> req;

                http::read(socket, buffer, req);
                std::cout << "Received HTTP request:\n" << req << std::endl;  // Log the request

                if (websocket::is_upgrade(req)) {
                    std::cout << "WebSocket upgrade detected" << std::endl;
                    handle_websocket(std::move(socket));
                } else {
                    std::cout << "HTTP request received instead of WebSocket" << std::endl;
                    handle_request(req, socket);
                }
            } else {
                std::cerr << "Error: " << ec.message() << std::endl;
            }
            do_accept();
        });
}





void Server::handle_websocket(asio::ip::tcp::socket socket) {
    try {
        auto ws = std::make_shared<websocket::stream<asio::ip::tcp::socket>>(std::move(socket));
        
        // Log when the WebSocket is accepting a connection
        std::cout << "Attempting to accept WebSocket connection..." << std::endl;
        ws->accept();
        
        // Log successful WebSocket connection
        std::cout << "WebSocket connection established" << std::endl;
        
        clients_.insert(ws);

        for (;;) {
            beast::flat_buffer buffer;
            ws->read(buffer);
            std::string message = beast::buffers_to_string(buffer.data());
            broadcast_message(message);
        }
    } catch (const std::exception& e) {
        std::cerr << "WebSocket error: " << e.what() << std::endl;
    }
}


void Server::broadcast_message(const std::string& message) {
    for (auto& client : clients_) {
        client->write(asio::buffer(message));
    }
}

void Server::handle_request(http::request<http::string_body> req, asio::ip::tcp::socket& socket) {
    http::response<http::string_body> res;
    json::value parsed_body;

    try {
        parsed_body = json::parse(req.body());
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.body() = "Invalid JSON format";
        send_response(res, socket);
        return;
    }

    // Check if the method is POST
    if (req.method() == http::verb::post) {

        if (req.target() == "/send_message") {
            // Handle message sending (you already have this)
            int sender_id = 1;
            int recipient_id = json::value_to<int>(parsed_body.at("recipient"));
            std::string content = json::value_to<std::string>(parsed_body.at("content"));

            if (db_->sendMessage(sender_id, recipient_id, content)) {
                res.result(http::status::ok);
                res.body() = "Message sent successfully";
            } else {
                res.result(http::status::internal_server_error);
                res.body() = "Failed to send message";
            }

        } else if (req.target() == "/login") {
            // Handle login request
            std::string username = json::value_to<std::string>(parsed_body.at("username"));
            std::string password = json::value_to<std::string>(parsed_body.at("password"));

            // Validate login credentials
            if (db_->validateUser(username, password)) {
                res.result(http::status::ok);
                res.body() = "Login successful!";
            } else {
                res.result(http::status::unauthorized);
                res.body() = "Invalid username or password";
            }

        } else if (req.target() == "/register") {
            // Handle registration request
            std::string username = json::value_to<std::string>(parsed_body.at("username"));
            std::string password = json::value_to<std::string>(parsed_body.at("password"));
            std::string publicKey = json::value_to<std::string>(parsed_body.at("publicKey"));

            // Register the user
            if (db_->addUser(username, password, publicKey)) {
                res.result(http::status::ok);
                res.body() = "User registered successfully";
            } else {
                res.result(http::status::internal_server_error);
                res.body() = "Failed to register user";
            }

        } else {
            // Handle other invalid routes
            res.result(http::status::method_not_allowed);
            res.body() = "Method not allowed";
        }

    } else {
        // Handle unsupported HTTP methods
        res.result(http::status::method_not_allowed);
        res.body() = "Method not allowed";
    }

    send_response(res, socket);
}


void Server::send_response(const http::response<http::string_body>& res, asio::ip::tcp::socket& socket) {
    http::write(socket, res);
    socket.shutdown(asio::ip::tcp::socket::shutdown_send);
}
