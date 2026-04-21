#include "../include/webc.h"

Response* connard(Response* reponse){
    reponse->status = 404;
    addHeaderToResponse( (Header){"Content-Type", "text/html"} , reponse);
    return( addContentToResponse("static/accueil.html", reponse) );
}

int main() {

    Server server;
    server.port = 8080;

    startServer(&server);
    addGlobalHeader(&server, "Server" , "WebC");
    addGlobalHeader(&server, "Cache-Control" , "no-store, no-cache, must-revalidate, max-age=0");
    addGlobalHeader(&server, "Pragma" , "no-cache");
    addGlobalHeader(&server, "Expires" , "0");

    addRoute(&server, "/test", connard);

    server.staticRoute = "/static/";
    server.staticPath = "static";

    server.defautlMethod = connard;

    run(&server);

    stop(&server);

    return 0;
}
