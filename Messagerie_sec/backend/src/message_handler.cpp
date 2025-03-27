#include "message_handler.h"
#include <iostream>
#include <boost/json.hpp>
#include <boost/beast/http.hpp>


void MessageHandler::handle_send_message(
    const boost::json::value& parsed_body, 
    boost::beast::http::response<boost::beast::http::string_body>& res,
    std::shared_ptr<Database> db_) 
{
    std::cout << "-------------------- Envoi de message en cours --------------------" << std::endl;

    // Conversion de parsed_body en un objet JSON
    boost::json::object obj = parsed_body.as_object();

    // Vérification des champs nécessaires
    if (obj.find("recipient") == obj.end() || 
        obj.find("content") == obj.end() || 
        obj.find("sender_id") == obj.end()) {
        res.result(boost::beast::http::status::bad_request);
        res.body() = "Missing sender_id, recipient, or content";
        return;
    }

    // Récupération des valeurs
    int sender_id = boost::json::value_to<int>(obj["sender_id"]);
    int recipient_id = boost::json::value_to<int>(obj["recipient"]);
    std::string content = boost::json::value_to<std::string>(obj["content"]);

    // Traitement de l'envoi du message dans la base de données
    if (db_->sendMessage(sender_id, recipient_id, content)) {
        res.result(boost::beast::http::status::ok);
        res.body() = "Message sent successfully";
    } else {
        res.result(boost::beast::http::status::internal_server_error);
        res.body() = "Failed to send message";
    }

    std::cout << "-------------------- Fin d'envoi de message --------------------" << std::endl;
}
