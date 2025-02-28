#include <iostream>
#include "server.h"

int main() {
    try {
        // Démarrage du serveur sur le port 8080
        Server server(8080);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
