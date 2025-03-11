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

std::vector<Message> Database::getMessages(int user_id, int contact_id) {
    std::vector<Message> messages;
    try {
        pqxx::work txn(conn_);

        pqxx::result r = txn.exec_params(
            "SELECT id, sender_id, receiver_id, content, status, timestamp, signature "
            "FROM messages "
            "WHERE (sender_id = $1 AND receiver_id = $2) "
            "   OR (sender_id = $2 AND receiver_id = $1) "
            "ORDER BY timestamp ASC",
            user_id, contact_id
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

int Database::getUserId(const std::string& username, const std::string& password_hash) {
    try {
        pqxx::work txn(conn_);

        // Vérification de l'utilisateur avec son nom d'utilisateur et son mot de passe hashé
        pqxx::result r = txn.exec("SELECT id FROM users WHERE username = " + txn.quote(username) + " AND password_hash = " + txn.quote(password_hash));

        // Si un utilisateur est trouvé, on renvoie son id
        if (!r.empty()) {
            return r[0]["id"].as<int>();
        } else {
            // Si l'utilisateur n'existe pas, on renvoie -1 pour indiquer un échec
            return -1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting user ID: " << e.what() << std::endl;
        return -1;  // Renvoi de -1 en cas d'erreur
    }
}

// std::vector<std::pair<int, std::string>> Database::getUserContacts(int user_id) {
//     std::vector<std::pair<int, std::string>> contacts;
//     try {
//         pqxx::work txn(conn_);

//         pqxx::result r = txn.exec_params(
//             "SELECT DISTINCT u.id, u.username "
//             "FROM users u "
//             "JOIN messages m ON (u.id = m.sender_id OR u.id = m.receiver_id) "
//             "WHERE u.id != $1 AND (m.sender_id = $1 OR m.receiver_id = $1)",
//             user_id
//         );

//         for (auto row : r) {
//             contacts.emplace_back(row["id"].as<int>(), row["username"].as<std::string>());
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "Error retrieving contacts: " << e.what() << std::endl;
//     }
//     return contacts;
// }

std::vector<std::pair<int, std::string>> Database::getUserContacts(int user_id) {
    std::vector<std::pair<int, std::string>> contacts;
    try {
        pqxx::work txn(conn_);

        // On sélectionne les contacts associés à l'utilisateur à partir de la table contacts
        pqxx::result r = txn.exec_params(
            "SELECT u.id, u.username "
            "FROM users u "
            "JOIN contacts c ON u.id = c.contact_id "
            "WHERE c.user_id = $1",
            user_id
        );

        // On parcourt les résultats pour récupérer les id et noms des contacts
        for (auto row : r) {
            contacts.emplace_back(row["id"].as<int>(), row["username"].as<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving contacts: " << e.what() << std::endl;
    }
    return contacts;
}


bool Database::sendContactRequest(int user_id, const std::string& public_key) {
    try {
        pqxx::work txn(conn_);

        // Vérifier si la requête préparée existe, sinon la créer
        static bool prepared = false;
        if (!prepared) {
            conn_.prepare("find_user_by_key", "SELECT id FROM users WHERE public_key = $1");
            conn_.prepare("check_request", "SELECT 1 FROM contact_requests WHERE sender_id = $1 AND receiver_id = $2 AND status = 'pending'");
            conn_.prepare("insert_request", "INSERT INTO contact_requests (sender_id, receiver_id, status) VALUES ($1, $2, 'pending')");
            prepared = true;
        }

        // Vérifier si l'utilisateur avec cette clé publique existe
        pqxx::result result = txn.exec_prepared("find_user_by_key", public_key);
        if (result.empty()) {
            return false; // Clé publique non trouvée
        }

        int contact_id = result[0][0].as<int>();

        // Vérifier si une demande existe déjà
        pqxx::result check_existing = txn.exec_prepared("check_request", user_id, contact_id);
        if (!check_existing.empty()) {
            return false; // Demande déjà envoyée
        }

        // Insérer la demande de contact
        txn.exec_prepared("insert_request", user_id, contact_id);
        txn.commit();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'envoi de la demande de contact : " << e.what() << std::endl;
        return false;
    }
}


std::string Database::getUserPublicKey(int user_id) {
    try {
        pqxx::work txn(conn_);

        // Préparer la requête (si elle n'a pas déjà été préparée)
        conn_.prepare("get_user_public_key", "SELECT public_key FROM users WHERE id = $1");

        // Exécuter la requête préparée
        pqxx::result result = txn.exec_prepared("get_user_public_key", user_id);

        if (result.empty()) {
            return "";
        }

        return result[0]["public_key"].as<std::string>();

    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de la récupération de la clé publique : " << e.what() << std::endl;
        return "";
    }
}

std::vector<ContactRequest> Database::getContactRequests(int user_id) {
    std::vector<ContactRequest> requests;
    
    pqxx::work txn(conn_);
    pqxx::result result = txn.exec("SELECT sender_id, username FROM contact_requests "
                                   "JOIN users ON contact_requests.sender_id = users.id "
                                   "WHERE receiver_id = " + txn.quote(user_id) + " AND status = 'pending'");

    for (const auto& row : result) {
        ContactRequest request;
        request.id = row["sender_id"].as<int>();
        request.username = row["username"].as<std::string>();
        requests.push_back(request);
    }
    
    txn.commit();
    return requests;
}

bool Database::acceptContactRequest(int user_id, int contact_id) {
    try {
        pqxx::work txn(conn_);

        // Vérifier si la requête préparée existe, sinon la créer
        static bool prepared = false;
        if (!prepared) {
            conn_.prepare("update_request", "UPDATE contact_requests SET status = 'accepted' WHERE receiver_id = $1 AND sender_id = $2 AND status = 'pending'");
            conn_.prepare("insert_contact", "INSERT INTO contacts (user_id, contact_id) VALUES ($1, $2), ($2, $1)");
            prepared = true;
        }

        // Mettre à jour la demande de contact
        pqxx::result res = txn.exec_prepared("update_request", user_id, contact_id);
        if (res.affected_rows() == 0) {
            return false; // Aucune demande en attente
        }

        // Ajouter les contacts mutuels
        txn.exec_prepared("insert_contact", user_id, contact_id);
        txn.commit();  // Assurez-vous que le commit a bien lieu

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'acceptation du contact : " << e.what() << std::endl;
        return false;
    }
}


bool Database::rejectContactRequest(int user_id, int contact_id) {
    try {
        pqxx::work txn(conn_);

        // Vérifier si la requête préparée existe, sinon la créer
        static bool prepared = false;
        if (!prepared) {
            conn_.prepare("delete_request", "DELETE FROM contact_requests WHERE receiver_id = $1 AND sender_id = $2 AND status = 'pending'");
            prepared = true;
        }

        // Supprimer la demande de contact
        pqxx::result res = txn.exec_prepared("delete_request", user_id, contact_id);
        if (res.affected_rows() == 0) {
            return false; // Aucune demande à rejeter
        }

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors du rejet de la demande de contact : " << e.what() << std::endl;
        return false;
    }
}




