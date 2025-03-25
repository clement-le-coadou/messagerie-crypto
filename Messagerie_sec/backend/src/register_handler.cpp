#include "register_handler.h"
#include <iostream>

void RegisterHandler::handle_register(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket) {
    std::cout << "Processing registration..." << std::endl;

    // Vérifier la présence des champs nécessaires
    if (!parsed_body.as_object().contains("username") ||
        !parsed_body.as_object().contains("password") ||
        !parsed_body.as_object().contains("publicKey")) {
        
        res.result(http::status::bad_request);
        res.body() = "Missing username, password, or publicKey";
        ResponseSender::send_response(res, ssl_socket);
        return;
    }

    // Récupérer les valeurs du JSON
    std::string username = json::value_to<std::string>(parsed_body.at("username"));
    std::string password = json::value_to<std::string>(parsed_body.at("password"));
    std::string publicKey = json::value_to<std::string>(parsed_body.at("publicKey"));

    // Ajouter l'utilisateur à la base de données
    if (db_->addUser(username, password, publicKey)) {
        res.result(http::status::ok);
        res.body() = "User registered successfully";
    } else {
        res.result(http::status::internal_server_error);
        res.body() = "Failed to register user";
    }

    ResponseSender::send_response(res, ssl_socket);
}
