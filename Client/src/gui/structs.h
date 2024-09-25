#ifndef DATATRANSMISSION_STRUCTS_H
#define DATATRANSMISSION_STRUCTS_H

#include <memory>

#include <sodium.h>

#include "../client.h"
#include "config.h"
#include "style.h"
#include "../../../include/imgui/imgui.h"

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
    unsigned char *key = nullptr;

    explicit Connection(std::shared_ptr<Client> iclient) : client(std::move(iclient)), col(buttonNotActiveCol){
        buttonName[0] = '\0';
        key = static_cast<unsigned char *>(malloc(crypto_secretbox_KEYBYTES));
        randombytes_buf(key, sizeof(key));
    }

    Connection() : client(nullptr), col(buttonNotActiveCol) {
        buttonName[0] = '\0';
        key = static_cast<unsigned char *>(malloc(crypto_secretbox_KEYBYTES));
        randombytes_buf(key, sizeof(key));
    }

    ~Connection() {
        if(key != nullptr)
            free(key);
    }
};

#endif //DATATRANSMISSION_STRUCTS_H
