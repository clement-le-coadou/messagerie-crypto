#include "server.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <jwt-cpp/jwt.h> // Include JWT-cpp
#include <boost/asio/ssl.hpp>

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace json = boost::json;

Server::Server(unsigned short port)
    : acceptor_(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
    ssl_context_(asio::ssl::context::tlsv12), // Ajout du contexte SSL
    db_(std::make_shared<Database>("dbname=secure_messenger user=postgres password=cytech0001 host=localhost port=5432")) {

    // Charger les certificats et clés
    ssl_context_.set_options(
    asio::ssl::context::default_workarounds |
    asio::ssl::context::no_sslv2 |
    asio::ssl::context::no_sslv3 |
    asio::ssl::context::single_dh_use);

    ssl_context_.use_certificate_chain_file("H:/Pascal/VisualCodeProjects/JavaScriptWorkspace/SecureMessagingApp/messagerie-crypto/Messagerie_sec/backend/certs/server.crt");
    ssl_context_.use_private_key_file("H:/Pascal/VisualCodeProjects/JavaScriptWorkspace/SecureMessagingApp/messagerie-crypto/Messagerie_sec/backend/certs/server.key", asio::ssl::context::pem);
    ssl_context_.use_tmp_dh_file("H:/Pascal/VisualCodeProjects/JavaScriptWorkspace/SecureMessagingApp/messagerie-crypto/Messagerie_sec/backend/certs/dh2048.pem");

    std::cout << "Secure server started on port " << port << std::endl;
}

void Server::run() {
    do_accept();
    ioc.run();
}

void Server::handle_session(std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> ssl_socket) {
    auto self = shared_from_this(); // Si Server est un `std::enable_shared_from_this<Server>`

    beast::flat_buffer buffer;
    auto req = std::make_shared<http::request<http::string_body>>();

    // Lire la requête HTTP sur le socket SSL
    http::async_read(*ssl_socket, buffer, *req,
        [this, self, ssl_socket, req](beast::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::cout << "Received HTTPS request:\n" << *req << std::endl;
                handle_request(*req, ssl_socket);
            } else {
                std::cerr << "Read error: " << ec.message() << std::endl;
            }
        });
}


void Server::do_accept() {
    acceptor_.async_accept(
        [this](beast::error_code ec, asio::ip::tcp::socket socket) {
            if (ec) {
                std::cerr << "Accept failed: " << ec.message() << std::endl;
                return;
            }

            // Création d'un stream SSL
            auto ssl_socket = std::make_shared<asio::ssl::stream<asio::ip::tcp::socket>>(std::move(socket), ssl_context_);
            
            // Lancer la négociation TLS
            ssl_socket->async_handshake(asio::ssl::stream_base::server,
                [this, ssl_socket](beast::error_code ec) {
                    if (!ec) {
                        handle_session(ssl_socket);
                    } else {
                        std::cerr << "TLS handshake failed: " << ec.message() << std::endl;
                    }
                });
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


void Server::handle_request(http::request<http::string_body> req, std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket) {
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
        ResponseSender::send_response(res, ssl_socket);
        return;
    }

    // Check if the request contains a token for authentication
    if (req.target() != "/login" && req.find(http::field::authorization) == req.end() && req.target() != "/register") {
        res.result(http::status::unauthorized);
        res.body() = "Authorization required";
        ResponseSender::send_response(res, ssl_socket);
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
            ResponseSender::send_response(res, ssl_socket);
            return;
        }
    }

    if (req.method() == http::verb::post) {
        if (req.target() == "/send_message") {
            MessageHandler::handle_send_message(parsed_body, res, db_);
        }
        else if (req.target() == "/get_messages") {
            handle_get_messages(parsed_body, res, db_, ssl_socket);
        }
        else if (req.target() == "/get_contacts") {
            ContactHandler::handle_get_contacts(parsed_body, res, db_.get(), ssl_socket);
        }
        else if (req.target() == "/send_contact_request") {
            SendContactRequestHandler::handle_send_contact_request(parsed_body, res, db_, ssl_socket);
        }
        else if (req.target() == "/handle_request") {
            HandleRequestHandler::handle_contact_request(parsed_body, res, db_, ssl_socket);
        }
        else if (req.target() == "/get_contact_requests") {
            GetContactRequestsHandler::handle_get_contact_requests(parsed_body, res, db_, ssl_socket);
        }
        else if (req.target() == "/login") {
            LoginHandler::handle_login(parsed_body, res, db_, ssl_socket);
        }
        else if (req.target() == "/register") {
            RegisterHandler::handle_register(parsed_body, res, db_, ssl_socket);
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
    ResponseSender::send_response(res, ssl_socket);
}
