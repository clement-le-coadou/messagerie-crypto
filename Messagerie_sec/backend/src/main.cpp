#include "server.h"
#include "database.h"
#include <iostream>
#include <stdexcept>  // Pour la gestion des exceptions standard

// Exception personnalisée, si nécessaire
class MyException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Une erreur personnalisée est survenue";
    }
};

int main() {
    try {
        // Connexion à la base de données
        Database db("dbname=secure_messenger user=postgres password=cytech0001 host=localhost port=5432");
        
        // Ajouter un utilisateur pour tester
        db.addUser("Alice", "hashed_password", "public_key_ECC");

        // Démarrer le serveur
        Server server(8080);
        server.run();
    } catch (const std::exception& e) {  // Capture les exceptions standard
        std::cerr << "Erreur détectée: " << e.what() << std::endl;
        return 1;  // Retourne une erreur
    } catch (const MyException& e) {  // Si tu as une exception personnalisée
        std::cerr << "Erreur personnalisée: " << e.what() << std::endl;
        return 2;  // Retourne une erreur spécifique
    } catch (...) {  // Capture toutes les autres exceptions non spécifiées
        std::cerr << "Erreur inconnue." << std::endl;
        return 3;  // Retourne une erreur générique
    }
    
    return 0;  // Tout s'est bien passé
}
