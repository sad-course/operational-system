//
// Created by mirla on 17/10/2025.
//

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

struct chatMessage {
    char username[32];
    char text[1024];
    int type;
};

#endif //SOCKET_UTILS_H