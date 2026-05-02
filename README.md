# WebC

Small HTTP server in C with simple routing, global headers, and static file serving.

## Prerequisites
- GCC
- make

## Build
```sh
make webc
```

## Run
```sh
./build/webc
```
The server listens on port 8080.

## Example routes
These are just examples of route usage in the current code:
- GET /acceil -> HTML (static/accueil.html)
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
