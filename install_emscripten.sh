#!/usr/bin/env bash

git clone https://github.com/emscripten-core/emsdk.git

./emsdk/emsdk install latest

./emsdk/emsdk activate latest

source ./emsdk/emsdk_env.sh