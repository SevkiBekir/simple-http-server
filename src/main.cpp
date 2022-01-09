#include <iostream>
#include "http_server.h"
#include <signal.h>

int signIntSignalCallbackHandler(int signal, void* httpServer) {

    static HttpServer* savedServer = NULL;

    if(savedServer == NULL){
        savedServer = static_cast<HttpServer*>(httpServer);
    }

    if(signal == SIGINT || signal == SIGTERM){
        savedServer->closeServer();
        exit(signal);
    }

    return 0;
}

int main() {
    std::cout << "====== SIMPLE HTTP SERVER ======" << std::endl;
    HttpServer httpServer(8080);
    signal(SIGINT, (void (*)(int))signIntSignalCallbackHandler);
    signal(SIGTERM, (void (*)(int))signIntSignalCallbackHandler);
    signIntSignalCallbackHandler(0, &httpServer);
    int result = httpServer.run();

    std::cout << "================================" << std::endl;
    httpServer.closeServer();
    return result;
}
