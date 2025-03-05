// wasmWorker.js
import createLibRawModule from './out/libraw.js';

let Module = null;

self.onmessage = async (event) => {
  const { type, fileBuffer } = event.data;

  if (type === 'process') {
    try {
      // Initialize Wasm module once
      if (!Module) {
        Module = await createLibRawModule();
      }

      // Convert incoming data to Uint8Array
      const rawImageArray = new Uint8Array(fileBuffer);

      // Allocate memory in the Emscripten heap
      const buf = Module._malloc(rawImageArray.byteLength);
      Module.HEAPU8.set(rawImageArray, buf);

      // Allocate memory for metadata
      const widthPtr = Module._malloc(4);
      const heightPtr = Module._malloc(4);
      const colorsPtr = Module._malloc(4);
      const imageSizePtr = Module._malloc(4);

      Module._setParams({
        "use_camera_wb" : false,
        "use_auto_wb" : true,
        "bright" : 1,
        "dcb_iterations" : 1,
      });

      // Call into Wasm
      const imagePtr = Module._processImageData(
        buf,
        rawImageArray.byteLength,
        widthPtr,
        heightPtr,
        colorsPtr,
        imageSizePtr
      );
      if (!imagePtr) {
        throw new Error('Image processing failed (NULL pointer)');
      }

      let pp = Module._getParams();
      console.log(pp);

      // Extract metadata
      const width = Module.HEAP32[widthPtr >> 2];
      const height = Module.HEAP32[heightPtr >> 2];
      const colors = Module.HEAP32[colorsPtr >> 2];
      const imageSize = Module.HEAP32[imageSizePtr >> 2];

      // Convert the region of Wasm memory to a temporary typed array
      const tempImageData = new Uint16Array(
        Module.HEAPU16.buffer,
        imagePtr,
        imageSize / 2
      );

      // CREATE A NEW BUFFER & COPY so that we have a standalone ArrayBuffer
      // which can be transferred back to the main thread
      const processedImageData = new Uint16Array(imageSize / 2);
      processedImageData.set(tempImageData);

      // Free the memory in Wasm
      Module._free(buf);
      Module._free(widthPtr);
      Module._free(heightPtr);
      Module._free(colorsPtr);
      Module._free(imageSizePtr);
      Module._free(imagePtr);

      // Post the data back, transferring the buffer
      self.postMessage(
        {
          type: 'processed',
          width,
          height,
          colors,
          imageSize,
          buffer: processedImageData.buffer
        },
        [processedImageData.buffer] // Transferable
      );
    } catch (err) {
      self.postMessage({ type: 'error', message: err.message });
    }
  }
};