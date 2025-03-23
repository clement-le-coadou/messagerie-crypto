cmake_minimum_required(VERSION 3.10)

# Utiliser vcpkg
set(CMAKE_TOOLCHAIN_FILE "H:/Run/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file" FORCE)

# Nom du projet
project(SecureMessenger)

# Utiliser le standard C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "PostgreSQL Include Dir: ${PostgreSQL_INCLUDE_DIRS}")
message(STATUS "pqxx Include Dir: ${libpqxx_INCLUDE_DIRS}")
message(STATUS "pqxx Library: ${pqxx_LIBRARIES}")

# Trouver les bibliothèques installées via vcpkg
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system asio beast json)
find_package(PostgreSQL REQUIRED)
find_package(libpqxx REQUIRED)

# Création de l'exécutable
add_executable(backend
    src/main.cpp
    src/server.cpp
    src/crypto.cpp
    src/database.cpp
    src/utils.cpp
    src/message_handler.cpp
)

# Ajouter les répertoires d'inclusion
target_include_directories(backend PRIVATE 
    include 
    ${OPENSSL_INCLUDE_DIR}   
    ${Boost_INCLUDE_DIRS} 
    ${PostgreSQL_INCLUDE_DIRS} 
    ${libpqxx_INCLUDE_DIRS}
    "H:/Run/vcpkg/installed/x64-windows/include"  # 📌 Ajout des headers vcpkg
)

# Lier les bibliothèques nécessaires
target_link_libraries(backend 
    PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    Boost::system 
    Boost::asio
    Boost::beast
    Boost::json
    PostgreSQL::PostgreSQL
    libpqxx::pqxx
)

# Ajout de flags pour éviter des erreurs sous MSVC
if (MSVC)
    target_compile_options(backend PRIVATE /W4 /MP)
    target_compile_definitions(backend PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
