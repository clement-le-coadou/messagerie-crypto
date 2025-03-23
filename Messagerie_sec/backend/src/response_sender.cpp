#include "response_sender.h"

void ResponseSender::send_response(const http::response<http::string_body>& res, asio::ip::tcp::socket& socket) {
    std::cout << "Sending response: " << res.body() << std::endl;
    boost::beast::error_code ec;
    http::write(socket, res, ec);
    
    if (ec) {
        std::cerr << "Error writing to socket: " << ec.message() << std::endl;
    }

    socket.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
    if (ec) {
        std::cerr << "Error shutting down socket: " << ec.message() << std::endl;
    }
}
