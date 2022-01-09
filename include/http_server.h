/*************************************************************************
   HttpServer.h
   Author: Sevki Kocadag,

   Description: Http Server Class
*************************************************************************/

#ifndef SIMPLE_HTTP_SERVER_HTTP_SERVER_H
#define SIMPLE_HTTP_SERVER_HTTP_SERVER_H


#include <map>
#include <string>
#include <vector>

struct ClientInfo;

class HttpServer {
public:
    HttpServer(int port);
    virtual ~HttpServer() = default;
    HttpServer(const HttpServer& httpServer) = default;
    HttpServer& operator=(const HttpServer& httpServer) = default;
    HttpServer(HttpServer&& httpServer) = default;
    HttpServer& operator=(HttpServer&& httpServer) = default;

protected:

private:
    void init();
    int run();
    void initContentTypeMap();
    int createSocket(const std::string& host, const std::string& port);
    std::string getContentType(const std::string& path);
    ClientInfo& getClient(int socket);
    void dropClient(struct ClientInfo& clientInfo);
    const char* getClientAddress(const struct ClientInfo& clientInfo);
    fd_set waitOnClients(int serverSocket);
    void send400ErrorMessage(struct ClientInfo& clientInfo);
    void send404ErrorMessage(struct ClientInfo& clientInfo);
    void serveResource(struct ClientInfo& clientInfo, std::string& path);

    std::map<std::string, std::string> contentTypeMap;
    std::vector<struct ClientInfo> clients;
    int port;



};


#endif //SIMPLE_HTTP_SERVER_HTTP_SERVER_H
