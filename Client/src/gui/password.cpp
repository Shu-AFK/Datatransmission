#include "password.h"

#include <vector>
#include <cstring>
#include <stdexcept>

std::string encryptPassword(const std::string &password, unsigned char key[crypto_secretbox_KEYBYTES]) {
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    std::vector<unsigned char> ciphertext(crypto_secretbox_MACBYTES + password.size());
    crypto_secretbox_easy(ciphertext.data(), reinterpret_cast<const unsigned char*>(password.data()), password.size(), nonce, key);

    // Prepend nonce to the ciphertext for later use in decryption
    std::string encrypted_password(reinterpret_cast<char*>(nonce), sizeof(nonce));
    encrypted_password.append(reinterpret_cast<char*>(ciphertext.data()), ciphertext.size());

    return encrypted_password;
}

std::string decryptPassword(const std::string &encrypted_password, unsigned char key[crypto_secretbox_KEYBYTES]) {
    if (encrypted_password.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
        throw std::runtime_error("Invalid encrypted password length");
    }

    // Extract the nonce from the beginning of the encrypted password
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    std::memcpy(nonce, encrypted_password.data(), sizeof(nonce));

    // Extract the ciphertext
    std::vector<unsigned char> decrypted(encrypted_password.size() - sizeof(nonce) - crypto_secretbox_MACBYTES);
    if (crypto_secretbox_open_easy(decrypted.data(), reinterpret_cast<const unsigned char*>(encrypted_password.data()) + sizeof(nonce),
                                   encrypted_password.size() - sizeof(nonce), nonce, key) != 0) {
        throw std::runtime_error("Decryption failed");
    }

    return std::string(reinterpret_cast<char*>(decrypted.data()), decrypted.size());
}