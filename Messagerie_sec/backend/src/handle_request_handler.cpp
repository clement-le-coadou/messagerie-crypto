#include "handle_request_handler.h"
#include <iostream>

void HandleRequestHandler::handle_contact_request(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, boost::asio::ip::tcp::socket& socket) {
    std::cout << "Handling contact request..." << std::endl;

    if (!parsed_body.as_object().contains("user_id") || 
        !parsed_body.as_object().contains("request_id") || 
        !parsed_body.as_object().contains("accept")) {
        
        res.result(http::status::bad_request);
        res.body() = "Missing user_id, request_id, or accept field";
        ResponseSender::send_response(res, socket);
        return;
    }

    int user_id = json::value_to<int>(parsed_body.at("user_id"));
    int request_id = json::value_to<int>(parsed_body.at("request_id"));
    bool accept = json::value_to<bool>(parsed_body.at("accept"));

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
