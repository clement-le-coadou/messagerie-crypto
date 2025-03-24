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
            try {
                if (ec) {
                    throw std::runtime_error("Error: " + ec.message());
                }

                std::cout << "Client connected: " << socket.remote_endpoint() << std::endl;

                beast::flat_buffer buffer;
                http::request<http::string_body> req;

                http::read(socket, buffer, req);
                std::cout << "Received HTTP request:\n" << req << std::endl;

                handle_request(req, socket);
            } catch (const std::exception& e) {
                std::cerr << "Exception occurred: " << e.what() << std::endl;
            }
            do_accept();
        });
}

// Fonction pour valider le JWT et extraire l'ID utilisateur
int Server::validate_jwt_token(const std::string& token) {
    try {
        // Remplacez "your_secret_key" par la clé utilisée pour signer les tokens
        auto decoded_token = jwt::decode(token);

        // Vérifiez la signature et les revendications du token ici (par exemple l'émetteur et la clé)
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{"your_secret_key"})
            .with_issuer("SecureMessenger");

        verifier.verify(decoded_token);

        // Extraire l'ID utilisateur depuis les revendications du token
        std::string user_id_str = decoded_token.get_payload_claim("sender_id").as_string();
        int user_id = std::stoi(user_id_str);

        return user_id;  // Retourne l'ID utilisateur
    } catch (const std::exception& e) {
        std::cerr << "JWT validation failed: " << e.what() << std::endl;
        return -1;  // Si le token est invalide ou expiré, retournez -1
    }
}


void Server::handle_request(http::request<http::string_body> req, asio::ip::tcp::socket& socket) {
    http::response<http::string_body> res;
    json::value parsed_body;
    std::string token;
    int user_id = -1;

    try {
        if (!req.body().empty()) {
            parsed_body = json::parse(req.body());
        }
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.body() = "Invalid JSON format";
        ResponseSender::send_response(res, socket);
        return;
    }

    // Check if the request contains a token for authentication
    if (req.target() != "/login" && req.find(http::field::authorization) == req.end() && req.target() != "/register") {
        res.result(http::status::unauthorized);
        res.body() = "Authorization required";
        ResponseSender::send_response(res, socket);
        return;
    }

    // Extract the token if available
    if (req.find(http::field::authorization) != req.end()) {
        token = req[http::field::authorization].substr(7); // Retirer "Bearer "

        // Valider le token et récupérer l'ID utilisateur
        user_id = validate_jwt_token(token);
        if (user_id == -1) {
            res.result(http::status::unauthorized);
            res.body() = "Invalid or expired token";
            ResponseSender::send_response(res, socket);
            return;
        }
    }

    if (req.method() == http::verb::post) {
        if (req.target() == "/send_message") {
            MessageHandler::handle_send_message(parsed_body, res, db_);
        }
        else if (req.target() == "/get_messages") {
            handle_get_messages(parsed_body, res, db_, socket);
        }
        else if (req.target() == "/get_contacts") {
            ContactHandler::handle_get_contacts(parsed_body, res, db_.get(), socket);
        }
        else if (req.target() == "/send_contact_request") {
            SendContactRequestHandler::handle_send_contact_request(parsed_body, res, db_, socket);
        }
        else if (req.target() == "/handle_request") {
            HandleRequestHandler::handle_contact_request(parsed_body, res, db_, socket);
        }
        else if (req.target() == "/get_contact_requests") {
            GetContactRequestsHandler::handle_get_contact_requests(parsed_body, res, db_, socket);
        }
        else if (req.target() == "/login") {
            LoginHandler::handle_login(parsed_body, res, db_, socket);
        }
        else if (req.target() == "/register") {
            RegisterHandler::handle_register(parsed_body, res, db_, socket);
        }
        else {
            res.result(http::status::not_found);
            res.body() = "Route not found";
        }
    }
    else {
        res.result(http::status::method_not_allowed);
        res.body() = "Method not allowed";
    }
    ResponseSender::send_response(res, socket);
}
