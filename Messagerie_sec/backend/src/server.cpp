#include "server.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <jwt-cpp/jwt.h> // Include JWT-cpp
#include <boost/asio/ssl.hpp>
#include <fstream>
#include <filesystem>

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace json = boost::json;
namespace fs = std::filesystem;

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

    // Déterminer le chemin du dossier des certificats
    fs::path certs_path = fs::current_path() / "certs";

    ssl_context_.use_certificate_chain_file((certs_path / "server.crt").string());
    ssl_context_.use_private_key_file((certs_path / "server.key").string(), asio::ssl::context::pem);
    ssl_context_.use_tmp_dh_file((certs_path / "dh2048.pem").string());

    std::cout << "Secure server started on port " << port << std::endl;
}

void Server::run() {
    do_accept();
    ioc.run();
}

void Server::handle_session(std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> ssl_socket) {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;

    try {
        std::cout << "[Server] Starting to read HTTPS request..." << std::endl;
        std::cout << "Buffer size in the beginning : " << buffer.size() << " bytes" << std::endl;

        // Lire la requête de manière synchrone
        http::read(*ssl_socket, buffer, req);

        // Affichage de la taille du buffer après la lecture
        std::cout << "Buffer size after read: " << buffer.size() << " bytes" << std::endl;

        std::cout << "[Server] Received HTTPS request:" << std::endl;
        std::cout << "Method: " << req.method_string() << std::endl;
        std::cout << "Target: " << req.target() << std::endl;
        std::cout << "Headers: " << req.base() << std::endl;

        // Log the content of the buffer
        std::string buffer_content = beast::buffers_to_string(buffer.data());
        std::cout << "[Server] Buffer Content: " << buffer_content << std::endl;

        std::cout << "Body: " << req.body() << std::endl;

        handle_request(req, ssl_socket);

    } catch (const std::exception& e) {
        std::cerr << "[Server] Exception caught: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[Server] Unknown exception caught!" << std::endl;
    }
}




void Server::do_accept() {
    std::cout << "[Server] Waiting for new connection..." << std::endl;

    acceptor_.async_accept(
        [this](beast::error_code ec, asio::ip::tcp::socket socket) {
            if (ec) {
                std::cerr << "[Server] Accept failed: " << ec.message() << std::endl;
            } else {
                std::cout << "[Server] New connection accepted" << std::endl;
                auto ssl_socket = std::make_shared<asio::ssl::stream<asio::ip::tcp::socket>>(std::move(socket), ssl_context_);
                ssl_socket->async_handshake(asio::ssl::stream_base::server,
                    [this, ssl_socket](beast::error_code ec) {
                        if (!ec) {
                            std::cout << "[Server] TLS handshake successful" << std::endl;
                            handle_session(ssl_socket);
                        } else {
                            std::cerr << "[Server] TLS handshake failed: " << ec.message() << std::endl;
                        }
                    });
            }
            do_accept();
        }
    );
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
