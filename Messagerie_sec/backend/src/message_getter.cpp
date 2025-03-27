// message_getter.cpp
#include "message_getter.h"
#include <boost/json.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

void handle_get_messages(
    const boost::json::value& parsed_body,
    boost::beast::http::response<boost::beast::http::string_body>& res,
    std::shared_ptr<Database> db_,
    std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket) 
{
    std::cout << "getMessages appelé" << std::endl;

    // Vérifier si les identifiants nécessaires sont présents
    if (!parsed_body.as_object().contains("user_id") || !parsed_body.as_object().contains("contact_id")) {
        res.result(boost::beast::http::status::bad_request);
        res.body() = "Missing user_id or contact_id";
        ResponseSender::send_response(res, ssl_socket);
        return;
    }

    // Extraire les identifiants à partir du corps JSON
    int user_id = boost::json::value_to<int>(parsed_body.at("user_id"));
    int contact_id = boost::json::value_to<int>(parsed_body.at("contact_id"));

    // Récupérer les messages de la base de données
    auto messages = db_->getMessages(user_id, contact_id); // Nouvelle version de getMessages
    boost::json::array json_messages;

    // Construire la réponse JSON
    for (const auto& msg : messages) {
        boost::json::object message;
        message["sender"] = msg.sender_id;
        message["content"] = msg.content;
        json_messages.push_back(message);
    }

    std::cout << "Messages retrieved: " << boost::json::serialize(json_messages) << std::endl;

    // Préparer la réponse HTTP
    res.result(boost::beast::http::status::ok);
    res.set(boost::beast::http::field::content_type, "application/json");
    res.body() = boost::json::serialize(json_messages);

    // Envoyer la réponse au client
    ResponseSender::send_response(res, ssl_socket);
}
