#include "database.h"
#include <pqxx/pqxx>
#include <iostream>
#include <stdexcept>

Database::Database(const std::string& connection_str)
    : conn_(connection_str) {}

    bool Database::addUser(const std::string& username, const std::string& password, const std::string& publicKey) {
        try {
            // Vérifiez si l'utilisateur existe déjà
            pqxx::work txn(conn_);
            pqxx::result result = txn.exec("SELECT 1 FROM users WHERE username = " + txn.quote(username));
            
            if (result.size() > 0) {
                // L'utilisateur existe déjà
                std::cerr << "Username already exists!" << std::endl;
                return false;
            }
    
            // Si l'utilisateur n'existe pas, insérez-le
            txn.exec("INSERT INTO users (username, password_hash, public_key) VALUES (" 
                     + txn.quote(username) + ", "
                     + txn.quote(password) + ", "
                     + txn.quote(publicKey) + ")");
            txn.commit();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error adding user: " << e.what() << std::endl;
            return false;
        }
    }
    

bool Database::validateUser(const std::string& username, const std::string& password_hash) {
    try {
        pqxx::work txn(conn_);
        pqxx::result r = txn.exec("SELECT * FROM users WHERE username = " + txn.quote(username) + " AND password_hash = " + txn.quote(password_hash));
        return !r.empty();
    } catch (const std::exception& e) {
        std::cerr << "Error validating user: " << e.what() << std::endl;
        return false;
    }
}

bool Database::sendMessage(int sender_id, int receiver_id, const std::string& content) {
    try {
        pqxx::work txn(conn_);
        txn.exec0("INSERT INTO messages (sender_id, receiver_id, content) VALUES (" + txn.quote(sender_id) + ", " + txn.quote(receiver_id) + ", " + txn.quote(content) + ")");
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Message> Database::getMessages(int user_id) {
    std::vector<Message> messages;
    try {
        pqxx::work txn(conn_);

        pqxx::result r = txn.exec_params(
            "SELECT id, sender_id, receiver_id, content, status, timestamp, signature "
            "FROM messages WHERE receiver_id = $1 ORDER BY timestamp ASC",
            user_id
        );

        for (auto row : r) {
            messages.push_back({
                row["id"].as<int>(),
                row["sender_id"].as<int>(),
                row["receiver_id"].as<int>(),
                row["content"].as<std::string>(),
                row["status"].as<std::string>(),
                row["timestamp"].as<std::string>(),
                row["signature"].as<std::string>()
            });
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving messages: " << e.what() << std::endl;
    }
    return messages;
}


bool Database::updateMessageStatus(int message_id, const std::string& status) {
    try {
        pqxx::work txn(conn_);
        txn.exec_params("UPDATE messages SET status = $1 WHERE id = $2", status, message_id);
        txn.commit();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error updating message status: " << e.what() << std::endl;
        return false;
    }
}

