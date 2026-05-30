#include "webc.h"
#include "routes.h"

int main() {

    Server server;
    server.port = 8080;

    startServer(&server);
    addGlobalHeader(&server, "Server" , "WebC");

    addRoute(&server, "/accueil", accueil, GET);

    server.staticRoute = "/static/";
    server.staticPath = "static";

    server.defautlMethod = accueil;

    run(&server);
    stop(&server);

    return 0;
}
