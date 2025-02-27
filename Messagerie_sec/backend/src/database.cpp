#include "database.h"
#include <iostream>

Database::Database(const std::string& connStr) : dbConn(connStr) {
    if (!dbConn.is_open()) {
        throw std::runtime_error("Impossible de se connecter à la base de données");
    }
    std::cout << "Connexion à PostgreSQL réussie !" << std::endl;
}

bool Database::addUser(const std::string& username, const std::string& passwordHash, const std::string& publicKey) {
    try {
        pqxx::work txn(dbConn);
        txn.exec_params(
            "INSERT INTO users (username, password_hash, public_key) VALUES ($1, $2, $3)",
            username, passwordHash, publicKey
        );
        txn.commit();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Erreur lors de l'ajout d'un utilisateur : " << e.what() << std::endl;
        return false;
    }
}

bool Database::userExists(const std::string& username) {
    try {
        pqxx::work txn(dbConn);
        pqxx::result result = txn.exec_params(
            "SELECT id FROM users WHERE username = $1",
            username
        );
        return !result.empty();
    } catch (const std::exception &e) {
        std::cerr << "Erreur lors de la vérification de l'utilisateur : " << e.what() << std::endl;
        return false;
    }
}

bool Database::addMessage(int senderId, int receiverId, const std::string& content) {
    try {
        pqxx::work txn(dbConn);
        txn.exec_params(
            "INSERT INTO messages (sender_id, receiver_id, content) VALUES ($1, $2, $3)",
            senderId, receiverId, content
        );
        txn.commit();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Erreur lors de l'envoi du message : " << e.what() << std::endl;
        return false;
    }
}
