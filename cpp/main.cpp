#include <emscripten/bind.h>
#include <iostream>
#include "libraries/libraw/LibRaw-0.21.1/libraw/libraw.h"
#include <stdlib.h>
#include <malloc.h>

 int processBuffer() {
    return 0;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    uint8_t* processImageData(uint8_t* dataPtr, size_t dataSize, int* width, int* height, int* colors, size_t* imageSize) {
        printf("Received data of size: %zu bytes\n", dataSize);

        LibRaw RawProcessor;
        int ret = RawProcessor.open_buffer(dataPtr, dataSize);
        if (ret != LIBRAW_SUCCESS) {
            throw std::runtime_error("LibRaw: open_buffer() failed, code = " + std::to_string(ret));
        }
        
        ret = RawProcessor.unpack();
        if (ret != LIBRAW_SUCCESS) {
            throw std::runtime_error("LibRaw: unpack() failed, code = " + std::to_string(ret));
        }
        
        RawProcessor.imgdata.params.output_bps = 8;
        ret = RawProcessor.dcraw_process();
        if (ret != LIBRAW_SUCCESS) {
            throw std::runtime_error("LibRaw: dcraw_process() failed, code = " + std::to_string(ret));
        }

        libraw_processed_image_t* image = RawProcessor.dcraw_make_mem_image(&ret);
        if (!image || ret != LIBRAW_SUCCESS) {
            throw std::runtime_error("LibRaw: dcraw_make_mem_image() failed, code = " + std::to_string(ret));
        }

        printf("Processing successful\n");
        printf("Image width: %d\n", image->width);
        printf("Image height: %d\n", image->height);
        printf("Image colors: %d\n", image->colors);

        *width = image->width;
        *height = image->height;
        *colors = image->colors;
        *imageSize = image->data_size;

        uint8_t* outputData = (uint8_t*)malloc(image->data_size);
        if (!outputData) {
            throw std::runtime_error("Memory allocation failed");
        }
        memcpy(outputData, image->data, image->data_size);

        LibRaw::dcraw_clear_mem(image);
        return outputData;
    }
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

    // 2) Open buffer with LibRaw
    LibRaw RawProcessor;
    int ret = RawProcessor.open_buffer(dataPtr, dataSize);
    if (ret != LIBRAW_SUCCESS) {
        throw std::runtime_error("LibRaw: open_buffer() failed, code = " + std::to_string(ret));
    }
    // 3) Unpack the RAW data
    ret = RawProcessor.unpack();
    if (ret != LIBRAW_SUCCESS) {
        throw std::runtime_error("LibRaw: unpack() failed, code = " + std::to_string(ret));
    }
    // 4) Perform default processing (demosaicing, etc.)
    RawProcessor.imgdata.params.output_bps = 8;
    ret = RawProcessor.dcraw_process();
    if (ret != LIBRAW_SUCCESS) {
        throw std::runtime_error("LibRaw: dcraw_process() failed, code = " + std::to_string(ret));
    }

    // 5) Retrieve processed image data in memory
    libraw_processed_image_t* image = RawProcessor.dcraw_make_mem_image(&ret);
    if (!image || ret != LIBRAW_SUCCESS) {
        throw std::runtime_error("LibRaw: dcraw_make_mem_image() failed, code = " + std::to_string(ret));
    }
    else {
        printf("Great success\n");
        
        // Print image dimensions
        printf("Image width: %d\n", image->width);
        printf("Image height: %d\n", image->height);
        printf("Image colors: %d\n", image->colors);

        // Remember to free the memory allocated for the image
        LibRaw::dcraw_clear_mem(image);
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
