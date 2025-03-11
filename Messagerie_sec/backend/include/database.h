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

struct ContactRequest {
    int id;
    std::string username;
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
    
    std::vector<std::pair<int, std::string>> getUserContacts(int user_id);
    std::vector<Message> getMessages(int user_id, int contact_id);
    

    bool updateMessageStatus(int message_id, const std::string& status);

    int getUserId(const std::string& username, const std::string& password_hash);

    std::string getUserPublicKey(int user_id);

    bool acceptContactRequest(int user_id, int contact_id);

    bool sendContactRequest(int user_id, const std::string& public_key);

    std::vector<ContactRequest> getContactRequests(int user_id);

    bool rejectContactRequest(int user_id, int contact_id);
    
private:
    pqxx::connection conn_;
};

#endif // DATABASE_H
