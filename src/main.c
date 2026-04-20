#include "../include/webc.h"

char* connard(char* reponse){

    return( addContentToResponse("static/accueil.html", reponse) );


}

int main() {

    Server server;
    server.port = 8080;

    startServer(&server);
    addHeader(&server, "Server" , "WebC");
    addHeader(&server, "Cache-Control" , "no-store, no-cache, must-revalidate, max-age=0");
    addHeader(&server, "Pragma" , "no-cache");
    addHeader(&server, "Expires" , "0");


    addRoute(&server, "/test", connard);
    server.staticRoute = "/static/";
    server.staticPath = "static";

    while(1) {
        handleConnection(&server);
    }

    freeServer(&server);
    close(server.server_fd);
    return 0;
}
