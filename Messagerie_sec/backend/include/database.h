#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>
#include <string>
#include <vector>

struct Message {
    int id;
    int sender_id;
    int receiver_id;
    std::string content;
    std::string status;
    std::string timestamp;
    std::string signature;
};


class Database {
public:
    explicit Database(const std::string& connection_str);
    
    // Fonction pour ajouter un utilisateur
    bool addUser(const std::string& username, const std::string& password_hash, const std::string& public_key);
    
    // Fonction pour valider un utilisateur
    bool validateUser(const std::string& username, const std::string& password_hash);
    
    // Fonction pour envoyer un message
    bool sendMessage(int sender_id, int receiver_id, const std::string& content);
    
    // Fonction pour obtenir les messages d'un utilisateur
    std::vector<Message> getMessages(int user_id);

    bool updateMessageStatus(int message_id, const std::string& status);
    
    
private:
    pqxx::connection conn_;
};

#endif // DATABASE_H
