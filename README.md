# WebC

Small HTTP server in C with simple routing, global headers, and static file serving.

## Build

### Prerequisites
- GCC
- make
- python

Build the standalone server binary:
```sh
make webc
```

Build the static library for embedding:
```sh
make libwebc
```

Generate the single-header release file (also builds a release binary):
```sh
make webc-release
```

### Using WebC in your own app 

### Prerequisites

- GCC

To use Webc in your own app, you can use this minimal structure :

```sh
├── build
│   └── app
├── defaultPages
│   ├── badrequest.html
│   ├── index.html
│   └── notfound.html
├── include
│   └── 𝘄𝗲𝗯𝗰.𝗵
├── lib
│   └── 𝗹𝗶𝗯𝘄𝗲𝗯𝗰.𝗮
├── Makefile
└── src
    └── main.c
```

Minimal app setup steps:
1. Copy the library and header into your app:
	- build/release/libwebc.a -> your_app/lib/libwebc.a
	- build/release/webc.h -> your_app/include/webc.h
2. Compile your app against the library:
```sh
gcc src/*.c -o build/app -I./include -L./lib -lwebc
```
3. Write code, minimal example :

```c
#include "../include/webc.h"

Response* accueil(Response* reponse, Request* request){
    reponse->status = 200;
    addHeaderToResponse( (Header){"Content-Type", "text/html"} , reponse);
    return( addFileContentToResponseBody("static/accueil.html", reponse) );
}

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

```

## Run
```sh
./build/webc
```
The server listens on port 8080 by default.

If you built your own app, run its binary (for example):
```sh
./build/app
```

## Example routes
These are just examples of route usage in the current code:
- GET /accueil -> HTML (static/accueil.html)
- GET /verb -> HTML (static/get.html)
- POST /verb -> JSON (static/post.html)
- GET /json -> inline JSON

## Static files
Files in static/ are served under the prefix:
- /static/

## Structure
- include/: C headers
- src/: source code
- static/: pages and assets
- build/: generated binary

## Notes
Default global headers:
- Server: WebC
- Cache-Control: no-store, no-cache, must-revalidate, max-age=0
- Pragma: no-cache
- Expires: 0
