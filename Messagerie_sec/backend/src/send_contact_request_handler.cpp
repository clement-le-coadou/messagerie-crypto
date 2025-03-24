#include "send_contact_request_handler.h"
#include <iostream>

void SendContactRequestHandler::handle_send_contact_request(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, boost::asio::ip::tcp::socket& socket) {
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

    ResponseSender::send_response(res, socket);
}
