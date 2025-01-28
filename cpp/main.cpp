#include <emscripten/bind.h>
#include <iostream>

 int processBuffer() {
    return 0;
}


extern "C" {
  EMSCRIPTEN_KEEPALIVE
  void processData(uint8_t* dataPtr, size_t dataSize) {
    // Example: Print the data size
    printf("Received data of size: %zu bytes\n", dataSize);

    // Example: Access the data
    for (size_t i = 0; i < 1000; ++i) {
        // Process each byte as needed
        uint8_t byte = dataPtr[i];
        // For demonstration, let's just print the first few bytes
        printf("Byte %zu: %u\n", i, byte);
    }
  }
}

// void processData(uint8_t* dataPtr, size_t dataSize) {
    
// }

// Main function as an entry point for the WebAssembly module
int main() {
    std::cout << "WebAssembly module loaded successfully!" << std::endl;
    return 0;
}

// Bind the function so it's accessible from JavaScript
EMSCRIPTEN_BINDINGS(testModule)
{
   // emscripten::function( "processData", &processData, emscripten::allow_raw_pointer<emscripten::arg<1>>() );
}

//     emscripten::function( "processBuffer", &processBuffer, emscripten::allow_raw_pointer<emscripten::arg<1>>() );
