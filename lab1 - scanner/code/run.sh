#!/bin/zsh

gcc ./server/server.c ./server/warp.c -o ./server/server
gcc ./client/client.c ./client/warp.c -o ./client/client

# ld server.o warp.o -macosx_version_min 10.13 -lc -o server
# ld client.o warp.o -macosx_version_min 10.13 -lc -o client

gnome-terminal -x bash -c "cd ./client;exec bash"

cd server;./server

