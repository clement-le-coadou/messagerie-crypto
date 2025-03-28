#include <iostream>
#include "server.h"

int main() {
    try {
        // Correct : création via shared_ptr
        auto server = std::make_shared<Server>(8080);
        server->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
