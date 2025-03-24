#include "get_contact_requests_handler.h"
#include <iostream>

void GetContactRequestsHandler::handle_get_contact_requests(const json::value& parsed_body, http::response<http::string_body>& res, std::shared_ptr<Database> db_, boost::asio::ip::tcp::socket& socket) {
    std::cout << "Fetching contact requests..." << std::endl;

    if (!parsed_body.as_object().contains("user_id")) {
        res.result(http::status::bad_request);
        res.body() = "Missing user_id";
        ResponseSender::send_response(res, socket);
        return;
    }

    int user_id = json::value_to<int>(parsed_body.at("user_id"));
    auto requests = db_->getContactRequests(user_id);
    json::array json_requests;

    for (const auto& request : requests) {
        json::object request_obj;
        request_obj["id"] = request.id;
        request_obj["username"] = request.username;
        json_requests.push_back(request_obj);
    }

    std::cout << "Contact requests: " << json::serialize(json_requests) << std::endl;

    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    res.body() = json::serialize(json_requests);
    
    ResponseSender::send_response(res, socket);
}
