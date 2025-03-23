// message_handler.h
#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <boost/json.hpp>
#include <boost/beast/http.hpp>
#include "database.h"  // Inclure votre classe de gestion de la base de données

class MessageHandler {
    public:
        static void handle_send_message(
            const boost::json::value& parsed_body, 
            boost::beast::http::response<boost::beast::http::string_body>& res,
            std::shared_ptr<Database> db_);
    };
    

#endif // MESSAGE_HANDLER_H
