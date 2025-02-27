#include "utils.h"
#include <iostream>

void Utils::logMessage(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}
