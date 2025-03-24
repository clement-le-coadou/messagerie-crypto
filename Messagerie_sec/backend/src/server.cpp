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
            std::cout << "Envoi d'une demande de contact..." << std::endl;
        
            if (!parsed_body.as_object().contains("user_id") || !parsed_body.as_object().contains("public_key")) {
                res.result(http::status::bad_request);
                res.body() = "Missing user_id or public_key";
                ResponseSender::send_response(res, socket);
                return;
            }
        
            int user_id = json::value_to<int>(parsed_body.at("user_id"));
            std::string public_key = json::value_to<std::string>(parsed_body.at("public_key"));
        
            if (db_->sendContactRequest(user_id, public_key)) {
                res.result(http::status::ok);
                res.body() = "Contact request sent successfully";
            } else {
                res.result(http::status::internal_server_error);
                res.body() = "Failed to send contact request";
            }
        }
        else if (req.target() == "/handle_request") {
            std::cout << "Handling contact request..." << std::endl;
            
            if (!parsed_body.as_object().contains("user_id") || !parsed_body.as_object().contains("request_id") || !parsed_body.as_object().contains("accept")) {
                res.result(http::status::bad_request);
                res.body() = "Missing user_id, request_id, or accept field";
                ResponseSender::send_response(res, socket);
                return;
            }
            
            int user_id = json::value_to<int>(parsed_body.at("user_id"));
            int request_id = json::value_to<int>(parsed_body.at("request_id"));
            bool accept = json::value_to<bool>(parsed_body.at("accept"));
            
            // Logique pour accepter ou refuser la demande de contact
            if (accept) {
                if (db_->acceptContactRequest(user_id, request_id)) {
                    res.result(http::status::ok);
                    res.body() = "Contact request accepted";
                } else {
                    res.result(http::status::internal_server_error);
                    res.body() = "Failed to accept contact request";
                }
            } else {
                if (db_->rejectContactRequest(user_id, request_id)) {
                    res.result(http::status::ok);
                    res.body() = "Contact request rejected";
                } else {
                    res.result(http::status::internal_server_error);
                    res.body() = "Failed to reject contact request";
                }
            }
            
            ResponseSender::send_response(res, socket);
        }
        else if (req.target() == "/get_contact_requests") {
            std::cout << "Fetching contact requests..." << std::endl;
        
            // Vérifier la présence de user_id dans la requête JSON
            if (!parsed_body.as_object().contains("user_id")) {
                res.result(http::status::bad_request);
                res.body() = "Missing user_id";
                ResponseSender::send_response(res, socket);
                return;
            }
        
            // Récupérer le user_id depuis la requête
            int user_id = json::value_to<int>(parsed_body.at("user_id"));
        
            // Récupérer les demandes de contact en attente depuis la base de données
            auto requests = db_->getContactRequests(user_id);
            json::array json_requests;
        
            for (const auto& request : requests) {
                json::object request_obj;
                request_obj["id"] = request.id;
                request_obj["username"] = request.username;
                json_requests.push_back(request_obj);
            }
        
            std::cout << "Contact requests: " << json::serialize(json_requests) << std::endl;
        
            // Répondre avec les demandes de contact
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = json::serialize(json_requests);
        }
        else if (req.target() == "/login") {
            std::string username = json::value_to<std::string>(parsed_body.at("username"));
            std::string password = json::value_to<std::string>(parsed_body.at("password"));
            int user_id = db_->getUserId(username, password);
            std::string public_key = db_->getUserPublicKey(user_id);

            if (db_->validateUser(username, password)) {
                // Create JWT token on successful login
                auto token = jwt::create()
                    .set_issuer("SecureMessenger")
                    .set_type("JWT")
                    .set_payload_claim("sender_id", jwt::claim(std::to_string(user_id))) // Sender's user ID here
                    .set_payload_claim("public_key", jwt::claim(public_key))
                    .sign(jwt::algorithm::hs256{"your_secret_key"});

            // Création de la réponse JSON
            json::object response_json;
            response_json["token"] = token;
            response_json["user_id"] = user_id; // On renvoie user_id
            response_json["public_key"] = public_key;

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
        }
        else if (req.target() == "/register") {
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
