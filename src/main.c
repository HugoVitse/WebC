#include "../include/webc.h"

Response* connard(Response* reponse){
    reponse->status = 404;
    addHeaderToResponse( (Header){"Content-Type", "text/html"} , reponse);
    return( addFileContentToResponseBody("static/accueil.html", reponse) );
}

Response* testGET(Response* reponse){
    reponse->status = 200;
    addHeaderToResponse( (Header){"Content-Type", "text/html"} , reponse);
    return( addFileContentToResponseBody("static/get.html", reponse) );
}

Response* testPOST(Response* reponse){
    reponse->status = 200;
    addHeaderToResponse( (Header){"Content-Type", "application/json"} , reponse);
    return( addFileContentToResponseBody("static/post.html", reponse) );
}

Response* jsonTEST(Response* reponse){
    reponse->status = 200;
    addHeaderToResponse( (Header){"Content-Type", "application/json"} , reponse);
    return( addContentToResponseBody("{\"a\":\"b\"}", reponse) );

}



int main() {

    Server server;
    server.port = 8080;

    startServer(&server);
    addGlobalHeader(&server, "Server" , "WebC");
    addGlobalHeader(&server, "Cache-Control" , "no-store, no-cache, must-revalidate, max-age=0");
    addGlobalHeader(&server, "Pragma" , "no-cache");
    addGlobalHeader(&server, "Expires" , "0");

    addRoute(&server, "/test", connard, GET);
    addRoute(&server, "/verb", testGET, GET);
    addRoute(&server, "/verb", testPOST, POST);
    addRoute(&server, "/json", jsonTEST, GET);


    server.staticRoute = "/static/";
    server.staticPath = "static";

    server.defautlMethod = connard;

    run(&server);

    stop(&server);

    return 0;
}
