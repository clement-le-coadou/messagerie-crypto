#include "server.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <jwt-cpp/jwt.h> // Include JWT-cpp

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace json = boost::json;

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
                std::cout << "Received HTTP request:\n" << req << std::endl;

                handle_request(req, socket);
            } else {
                std::cerr << "Error: " << ec.message() << std::endl;
            }
            do_accept();
        });
}

void Server::handle_request(http::request<http::string_body> req, asio::ip::tcp::socket& socket) {
    http::response<http::string_body> res;
    json::value parsed_body;
    std::string token;

    try {
        if (!req.body().empty()) {
            parsed_body = json::parse(req.body());
        }
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.body() = "Invalid JSON format";
        send_response(res, socket);
        return;
    }

    // Check if the request contains a token for authentication
    if (req.target() != "/login" && req.find(http::field::authorization) == req.end()) {
        res.result(http::status::unauthorized);
        res.body() = "Authorization required";
        send_response(res, socket);
        return;
    }

    // Extract the token if available
    if (req.find(http::field::authorization) != req.end()) {
        std::string token = std::string(req[http::field::authorization]);

    }

    if (req.method() == http::verb::post) {
if (req.target() == "/send_message") {
    std::cout << "-------------------- Envoi de message en cours --------------------" << std::endl;

    if (!parsed_body.as_object().contains("recipient") || !parsed_body.as_object().contains("content") || !parsed_body.as_object().contains("sender_id")) {
        res.result(http::status::bad_request);
        res.body() = "Missing sender_id, recipient, or content";
        send_response(res, socket);
        return;
    }

    // Récupérer sender_id directement depuis la requête JSON
    int sender_id = json::value_to<int>(parsed_body.at("sender_id"));
    int recipient_id = json::value_to<int>(parsed_body.at("recipient"));
    std::string content = json::value_to<std::string>(parsed_body.at("content"));

    if (db_->sendMessage(sender_id, recipient_id, content)) {
        res.result(http::status::ok);
        res.body() = "Message sent successfully";
    } else {
        res.result(http::status::internal_server_error);
        res.body() = "Failed to send message";
    }

    send_response(res, socket);
    std::cout << "-------------------- Fin d'envoi de message --------------------" << std::endl;
}
else if (req.target() == "/get_messages") {
            std::cout << "getMessages appelé" << std::endl;
        
            if (!parsed_body.as_object().contains("user_id") || !parsed_body.as_object().contains("contact_id")) {
                res.result(http::status::bad_request);
                res.body() = "Missing user_id or contact_id";
                send_response(res, socket);
                return;
            }
        
            int user_id = json::value_to<int>(parsed_body.at("user_id"));
            int contact_id = json::value_to<int>(parsed_body.at("contact_id"));
        
            auto messages = db_->getMessages(user_id, contact_id); // Nouvelle version de getMessages
            json::array json_messages;
        
            for (const auto& msg : messages) {
                json::object message;
                message["sender"] = msg.sender_id;
                message["content"] = msg.content;
                json_messages.push_back(message);
            }
        
            std::cout << "Messages retrieved: " << json::serialize(json_messages) << std::endl;
        
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = json::serialize(json_messages);
        }else if (req.target() == "/get_contacts") {
            std::cout << "getContacts appelé" << std::endl;
        
            if (!parsed_body.as_object().contains("user_id")) {
                res.result(http::status::bad_request);
                res.body() = "Missing user_id";
                send_response(res, socket);
                return;
            }
        
            int user_id = std::stoi(json::value_to<std::string>(parsed_body.at("user_id")));
            auto contacts = db_->getUserContacts(user_id);
            json::array json_contacts;
        
            for (const auto& contact : contacts) {
                json::object obj;
                obj["id"] = contact.first;
                obj["username"] = contact.second;
                json_contacts.push_back(obj);
            }
        
            std::cout << "Contacts retrieved: " << json::serialize(json_contacts) << std::endl;
        
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = json::serialize(json_contacts);
        }   
         else if (req.target() == "/login") {
            std::string username = json::value_to<std::string>(parsed_body.at("username"));
            std::string password = json::value_to<std::string>(parsed_body.at("password"));
            int user_id = db_->getUserId(username, password);

            if (db_->validateUser(username, password)) {
                // Create JWT token on successful login
                auto token = jwt::create()
                    .set_issuer("SecureMessenger")
                    .set_type("JWT")
                    .set_payload_claim("sender_id", jwt::claim(std::to_string(user_id))) // Sender's user ID here
                    .sign(jwt::algorithm::hs256{"your_secret_key"});

            // Création de la réponse JSON
            json::object response_json;
            response_json["token"] = token;
            response_json["user_id"] = user_id; // On renvoie user_id

        std::string response_body = json::serialize(response_json);

        // Envoi de la réponse
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = response_body;
        res.prepare_payload();
                std::cout << "Generated Token: " << token << std::endl;

            } else {
                res.result(http::status::unauthorized);
                res.body() = "Invalid username or password";
            }
        } else if (req.target() == "/register") {
            std::string username = json::value_to<std::string>(parsed_body.at("username"));
            std::string password = json::value_to<std::string>(parsed_body.at("password"));
            std::string publicKey = json::value_to<std::string>(parsed_body.at("publicKey"));

            if (db_->addUser(username, password, publicKey)) {
                res.result(http::status::ok);
                res.body() = "User registered successfully";
            } else {
                res.result(http::status::internal_server_error);
                res.body() = "Failed to register user";
            }
        } else {
            res.result(http::status::not_found);
            res.body() = "Route not found";
        }
    } else {
        res.result(http::status::method_not_allowed);
        res.body() = "Method not allowed";
    }

    send_response(res, socket);
}


void Server::send_response(const http::response<http::string_body>& res, asio::ip::tcp::socket& socket) {
    std::cout << "Sending response: " << res.body() << std::endl;
    beast::error_code ec;
    http::write(socket, res, ec);
    
    if (ec) {
        std::cerr << "Error writing to socket: " << ec.message() << std::endl;
    }

    socket.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
    if (ec) {
        std::cerr << "Error shutting down socket: " << ec.message() << std::endl;
    }
}
