#!/usr/bin/env bash

emcc cpp/main.cpp \
    cpp/libraries/libraw/LibRaw-0.21.1/lib/.libs/libraw.a \
    -I/./cpp/libraries/libraw/LibRaw-0.21.1/libraw \
    -o out/libraw.js \
    -s EXPORT_ES6=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="createLibRawModule" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ALLOW_TABLE_GROWTH=1 \
    -s INITIAL_MEMORY=8388608 \
    -s STACK_SIZE=4194304 \
    -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','addFunction','removeFunction']" \
    -s "EXPORTED_FUNCTIONS=['_malloc', '_free', '_processData']" \
    --bind