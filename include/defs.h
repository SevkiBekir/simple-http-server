/*************************************************************************
   defs.h
   Author: Sevki Kocadag, skocadag@juniper.net

   (C) 2021, Juniper Networks

   Description: Definitios of the project
*************************************************************************/

#ifndef SIMPLE_HTTP_SERVER_DEFS_H
#define SIMPLE_HTTP_SERVER_DEFS_H

#include <cstdint>
#include <sys/socket.h>

static const uint16_t MAX_REQUEST_SIZE = 2047;
static const uint16_t BUFFER_SIZE = 1024;

static const char *CONTENT_400 = "HTTP/1.1 400 Bad Request\r\n"
                   "Connection: close\r\n"
                   "Content-Length: 11\r\n\r\nBad Request";

static const char *CONTENT_404 = "HTTP/1.1 404 Not Found\r\n"
                   "Connection: close\r\n"
                   "Content-Length: 9\r\n\r\nNot Found";

struct ClientInfo {
    socklen_t addressLength;
    struct sockaddr_storage address;
    int socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;

    bool operator==(const ClientInfo &rhs) const {
        return addressLength == rhs.addressLength &&
               &address == &rhs.address &&
               socket == rhs.socket &&
               request == rhs.request &&
               received == rhs.received;
    }

    bool operator!=(const ClientInfo &rhs) const {
        return !(rhs == *this);
    }
};

#endif //SIMPLE_HTTP_SERVER_DEFS_H
