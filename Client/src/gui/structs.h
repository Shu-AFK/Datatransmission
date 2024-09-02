#ifndef DATATRANSMISSION_STRUCTS_H
#define DATATRANSMISSION_STRUCTS_H

#include <sodium.h>

#include "../client.h"

enum SmartButtonState {
    None = 0,
    Hovered,
    Pressed,
    Released,
};

struct Connection {
    std::shared_ptr<Client> client;
    char buttonName[MAX_CONNECTION_BUTTON_LENGTH];
    ImVec4 col;
    unsigned char key[crypto_secretbox_KEYBYTES];

    explicit Connection(std::shared_ptr<Client> iclient) : client(std::move(iclient)), col(buttonNotActiveCol){
        buttonName[0] = '\0';

        randombytes_buf(key, sizeof(key));
    }
};

#endif //DATATRANSMISSION_STRUCTS_H
