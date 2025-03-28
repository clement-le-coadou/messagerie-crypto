#include "response_sender.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

void ResponseSender::send_response(const http::response<http::string_body>& res,
                                   std::shared_ptr<ssl::stream<asio::ip::tcp::socket>> ssl_socket) {
    try {
        // Écrire la réponse de manière synchrone
        http::write(*ssl_socket, res);

        // Fermer la connexion SSL de manière synchrone
        ssl_socket->shutdown();
    } catch (const std::exception& e) {
        std::cerr << "Error during response sending or SSL shutdown: " << e.what() << std::endl;
    }
}
