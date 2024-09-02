#ifndef DATATRANSMISSION_PASSWORD_H
#define DATATRANSMISSION_PASSWORD_H

#include <string>
#include <sodium.h>

std::string encryptPassword(const std::string &password, unsigned char key[crypto_secretbox_KEYBYTES]);
std::string decryptPassword(const std::string &encrypted_password, unsigned char key[crypto_secretbox_KEYBYTES]);

#endif //DATATRANSMISSION_PASSWORD_H
