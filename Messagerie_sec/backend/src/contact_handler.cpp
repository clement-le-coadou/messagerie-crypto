#include "contact_handler.h"
#include <iostream>

using boost::asio::ip::tcp;

void ContactHandler::handle_get_contacts(const json::value& parsed_body, http::response<http::string_body>& res, Database* db_, tcp::socket& socket) {
    std::cout << "getContacts appelé" << std::endl;

    if (!parsed_body.as_object().contains("user_id")) {
        res.result(http::status::bad_request);
        res.body() = "Missing user_id";
        ResponseSender::send_response(res, socket);
        return;
    }

    int user_id = std::stoi(json::value_to<std::string>(parsed_body.at("user_id")));
    auto contacts = db_->getUserContacts(user_id);
    json::array json_contacts;

    for (const auto& contact : contacts) {
        json::object obj;
        obj["id"] = contact.first;
        obj["username"] = contact.second;
        json_contacts.push_back(obj);
    }

    std::cout << "Contacts retrieved: " << json::serialize(json_contacts) << std::endl;

    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    res.body() = json::serialize(json_contacts);
    ResponseSender::send_response(res, socket);
}
