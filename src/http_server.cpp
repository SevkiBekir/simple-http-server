/*************************************************************************
   HttpServer.h
   Author: Sevki Kocadag,

   Description: Http Server Class Implementations
*************************************************************************/

#include <iostream>
#include "http_server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "defs.h"

HttpServer::HttpServer() {
    init();
}

void HttpServer::init() {
    initContentTypeMap();
}

void HttpServer::initContentTypeMap() {
    contentTypeMap.insert({".css", "text/csv"});
    contentTypeMap.insert({".csv", "text/csv"});
    contentTypeMap.insert({".gif", "image/gif"});
    contentTypeMap.insert({".htm", "text/html"});
    contentTypeMap.insert({".html", "text/html"});
    contentTypeMap.insert({".ico", "image/x-ico"});
    contentTypeMap.insert({".jpeg", "image/jpeg"});
    contentTypeMap.insert({".jpg", "image/jpeg"});
    contentTypeMap.insert({".js", "application/javascript"});
    contentTypeMap.insert({".json", "application/json"});
    contentTypeMap.insert({".png", "image/png"});
    contentTypeMap.insert({".pdf", "application/pdf"});
    contentTypeMap.insert({".svg", "image/svg+xml"});
    contentTypeMap.insert({".txt", "text/plain"});
}

std::string HttpServer::getContentType(const std::string &path) {
    std::string extension = path.substr(path.find_last_of('.'));
    auto found = contentTypeMap.find(extension);
    if(found != contentTypeMap.end()){
        return "application/octet-stream";
    }
    return found->second;
}

int HttpServer::createSocket(const std::string &host, const std::string &port) {
    std::cout << "Configuring local address..." << std::endl;
    struct addrinfo hints{};
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bindAddress;
    auto result = getaddrinfo(host.c_str(), port.c_str(), &hints, &bindAddress);
    if(result != 0){
        std::cerr << "Failed to get binding address info" << std::endl;
        return result;
    }

    std::cout << "Creating socket..." << std::endl;
    int listenSocket;
    listenSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
    if (listenSocket < 0) {
        std::cerr << "Failed to create listen socket: " << errno << std::endl;
        exit(1);
    }

    std::cout << "Binding socket to local address..." << std::endl;
    if (bind(listenSocket, bindAddress->ai_addr, bindAddress->ai_addrlen)) {
        std::cerr << "Failed to bind listen socket: " << errno << std::endl;
        exit(1);
    }
    freeaddrinfo(bindAddress);

    std::cout << "Listening..." << std::endl;
    if (listen(listenSocket, 10) < 0) {
        std::cerr << "Failed to listen socket: " << errno << std::endl;
        exit(1);
    }


    return listenSocket;
}

ClientInfo &HttpServer::getClient(int socket) {
    auto clientItr = std::find_if(clients.begin(), clients.end(),
                                  [socket](const struct ClientInfo& client)
                                          {return socket == client.socket;}
                                          );

    if (clientItr != clients.end()){
        return *clientItr;
    }

    // insert it
    struct ClientInfo clientInfo;
    clientInfo.socket = socket;
    clientInfo.addressLength = sizeof (clientInfo.address);

    clients.push_back(clientInfo);
    return clients.back();
}

void HttpServer::dropClient(struct ClientInfo &clientInfo) {
    close(clientInfo.socket);
    auto itr = std::find(clients.begin(), clients.end(), clientInfo);
    if (itr == clients.end()){
        std::cerr << "Failed to drop client " << std::endl;
    }

    clients.erase(itr);
}

const char *HttpServer::getClientAddress(const ClientInfo &clientInfo) {
    char* addressBuffer = new char[100];
    getnameinfo((struct sockaddr*)&clientInfo.address, clientInfo.addressLength,
                addressBuffer, sizeof(addressBuffer), 0, 0, NI_NUMERICHOST);
    return addressBuffer;
}

fd_set HttpServer::waitOnClients(int serverSocket) {
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(serverSocket, &reads);
    int maxSocket = serverSocket;

    for(auto& client : clients){
        FD_SET(client.socket, &reads);
        if (client.socket > maxSocket){
            maxSocket = client.socket;
        }
    }


    if (select(maxSocket+1, &reads, 0, 0, 0) < 0) {
        std::cerr << "Failed to select socket: " << errno << std::endl;
        exit(1);
    }

    return reads;
}

void HttpServer::send400ErrorMessage(ClientInfo &clientInfo) {
    send(clientInfo.socket, content_400, strlen(content_400),0);
    dropClient(clientInfo);
}

void HttpServer::send404ErrorMessage(ClientInfo &clientInfo) {
    send(clientInfo.socket, content_404, strlen(content_400),0);
    dropClient(clientInfo);
}

void HttpServer::serveResource(ClientInfo &clientInfo, std::string &path) {
    std::string clientAddress = std::string(getClientAddress(clientInfo));
    std::cout << "serve_resource..." << std::endl;
    std::cout << "Client Address: " << clientAddress << std::endl;
    std::cout << "Requested Path: " << path << std::endl;

    if(path == "/"){
        path = "/index.html";
    }

    if(path.length() > 100){
        send400ErrorMessage(clientInfo);
        return;
    }

    if(path.find("..") != std::string::npos){
        send404ErrorMessage(clientInfo);
        return;
    }

    std::string fullPath = "public" + path;

    FILE *filePtr = fopen(fullPath.c_str(),"rb");

    if(filePtr == NULL){
        send404ErrorMessage(clientInfo);
        return;
    }

    fseek(filePtr, 0L, SEEK_END);
    size_t contentLength = ftell(filePtr);
    rewind(filePtr);

    std::string contentType = getContentType(fullPath);

    char buffer[BUFFER_SIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(clientInfo.socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Connection: close\r\n");
    send(clientInfo.socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %zu\r\n", contentLength);
    send(clientInfo.socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n", contentType.c_str());
    send(clientInfo.socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(clientInfo.socket, buffer, strlen(buffer), 0);

    int r;

    do {
        send(clientInfo.socket, buffer, r, 0);
    } while ((r = fread(buffer, 1, BUFFER_SIZE, filePtr)));

    fclose(filePtr);
    dropClient(clientInfo);
}
