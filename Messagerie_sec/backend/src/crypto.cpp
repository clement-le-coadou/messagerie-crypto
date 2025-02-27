#include "crypto.h"
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <iostream>

void Crypto::generateECCKey() {
    EC_KEY *key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!key || !EC_KEY_generate_key(key)) {
        std::cerr << "Erreur lors de la génération de la clé ECC" << std::endl;
        return;
    }

    // Sauvegarde de la clé privée
    FILE *privFile = fopen("private.pem", "wb");
    PEM_write_ECPrivateKey(privFile, key, nullptr, nullptr, 0, nullptr, nullptr);
    fclose(privFile);

    // Sauvegarde de la clé publique
    FILE *pubFile = fopen("public.pem", "wb");
    PEM_write_EC_PUBKEY(pubFile, key);
    fclose(pubFile);

    EC_KEY_free(key);
    std::cout << "Clés ECC générées !" << std::endl;
}
