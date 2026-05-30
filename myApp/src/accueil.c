#include "webc.h"

Response* accueil(Response* reponse, Request* request){
    reponse->status = 200;
    addHeaderToResponse( (Header){"Content-Type", "text/html"} , reponse);
    return( addFileContentToResponseBody("static/accueil.html", reponse) );
}
