#include "login_handler.h"
#include <iostream>

void LoginHandler::handle_login(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket) {
    std::cout << "Processing login..." << std::endl;

    // Vérifier la présence des champs nécessaires
    if (!parsed_body.as_object().contains("username") || !parsed_body.as_object().contains("password")) {
        res.result(http::status::bad_request);
        res.body() = "Missing username or password";
        ResponseSender::send_response(res, ssl_socket);
        return;
    }

    // Récupérer les valeurs du JSON
    std::string username = json::value_to<std::string>(parsed_body.at("username"));
    std::string password = json::value_to<std::string>(parsed_body.at("password"));

    int user_id = db_->getUserId(username, password);
    std::string public_key = db_->getUserPublicKey(user_id);

    if (db_->validateUser(username, password)) {
        // Générer le token JWT
        auto token = jwt::create()
            .set_issuer("SecureMessenger")
            .set_type("JWT")
            .set_payload_claim("sender_id", jwt::claim(std::to_string(user_id)))
            .set_payload_claim("public_key", jwt::claim(public_key))
            .sign(jwt::algorithm::hs256{"your_secret_key"});

        // Construire la réponse JSON
        json::object response_json;
        response_json["token"] = token;
        response_json["user_id"] = user_id;
        response_json["public_key"] = public_key;

        std::string response_body = json::serialize(response_json);

        // Envoyer la réponse
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = response_body;
        res.prepare_payload();

        std::cout << "Generated Token: " << token << std::endl;
    } else {
        res.result(http::status::unauthorized);
        res.body() = "Invalid username or password";
    }

    ResponseSender::send_response(res, ssl_socket);
}
