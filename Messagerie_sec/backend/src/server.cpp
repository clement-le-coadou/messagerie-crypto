#include "server.h"
#include <iostream>

Server::Server(int port) : port(port) {}

void Server::run() {
    std::cout << "Serveur WebSocket démarré sur le port " << port << std::endl;
    // Implémentation du serveur ici...
}
