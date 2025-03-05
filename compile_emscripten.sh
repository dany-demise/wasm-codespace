#!/usr/bin/env bash

emcc -O2 cpp/main.cpp -o out/ptr_test.js \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="createTestPtrModule" \
    -s EXPORT_ES6=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ALLOW_TABLE_GROWTH=1 \
    -s INITIAL_MEMORY=131072 \
    -s STACK_SIZE = 4194304 \
    -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" \
    -s "EXPORTED_FUNCTIONS=['_malloc', '_free']" \
    --bind