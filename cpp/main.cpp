#include <emscripten/bind.h>
#include <iostream>
#include "libraries/libraw/LibRaw-0.21.1/libraw/libraw.h"
#include <stdlib.h>
#include <malloc.h>

LibRaw RawProcessor;

extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    uint8_t *processImageData(uint8_t *dataPtr, size_t dataSize, int *width, int *height, int *colors, size_t *imageSize)
    {
        // printf("Received data of size: %zu bytes\n", dataSize);

        int ret = RawProcessor.open_buffer(dataPtr, dataSize);
        if (ret != LIBRAW_SUCCESS)
        {
            throw std::runtime_error("LibRaw: open_buffer() failed, code = " + std::to_string(ret));
        }

        ret = RawProcessor.unpack();
        if (ret != LIBRAW_SUCCESS)
        {
            throw std::runtime_error("LibRaw: unpack() failed, code = " + std::to_string(ret));
        }

        RawProcessor.imgdata.params.output_bps = 16;
        ret = RawProcessor.dcraw_process();
        if (ret != LIBRAW_SUCCESS)
        {
            throw std::runtime_error("LibRaw: dcraw_process() failed, code = " + std::to_string(ret));
        }

        libraw_processed_image_t *image = RawProcessor.dcraw_make_mem_image(&ret);
        if (!image || ret != LIBRAW_SUCCESS)
        {
            throw std::runtime_error("LibRaw: dcraw_make_mem_image() failed, code = " + std::to_string(ret));
        }

        // printf("Processing successful\n");
        // printf("Image width: %d\n", image->width);
        // printf("Image height: %d\n", image->height);
        // printf("Image colors: %d\n", image->colors);

        *width = image->width;
        *height = image->height;
        *colors = image->colors;
        *imageSize = image->data_size;

        uint8_t *outputData = (uint8_t *)malloc(image->data_size);
        if (!outputData)
        {
            throw std::runtime_error("Memory allocation failed");
        }
        memcpy(outputData, image->data, image->data_size);

        LibRaw::dcraw_clear_mem(image);
        return outputData;
    }
}

// Sets parameters from a JavaScript object
EMSCRIPTEN_KEEPALIVE
void setParams(emscripten::val jsObj)
{
    // Update only if the property exists in the passed object.
    if (jsObj.hasOwnProperty("user_flip"))
        RawProcessor.imgdata.params.user_flip = jsObj["user_flip"].as<int>();
    if (jsObj.hasOwnProperty("half_size"))
        RawProcessor.imgdata.params.half_size = jsObj["half_size"].as<bool>();
    if (jsObj.hasOwnProperty("four_color_rgb"))
        RawProcessor.imgdata.params.four_color_rgb = jsObj["four_color_rgb"].as<bool>();
    if (jsObj.hasOwnProperty("dcb_iterations"))
        RawProcessor.imgdata.params.dcb_iterations = jsObj["dcb_iterations"].as<int>();
    if (jsObj.hasOwnProperty("fbdd_noiserd"))
        RawProcessor.imgdata.params.fbdd_noiserd = jsObj["fbdd_noiserd"].as<int>();
    if (jsObj.hasOwnProperty("use_camera_wb"))
        RawProcessor.imgdata.params.use_camera_wb = jsObj["use_camera_wb"].as<bool>();
    if (jsObj.hasOwnProperty("use_auto_wb"))
        RawProcessor.imgdata.params.use_auto_wb = jsObj["use_auto_wb"].as<bool>();
    // if(jsObj.hasOwnProperty("output_bps"))
    //     RawProcessor.imgdata.params.output_bps = jsObj["output_bps"].as<int>();
    if (jsObj.hasOwnProperty("no_auto_bright"))
        RawProcessor.imgdata.params.no_auto_bright = jsObj["no_auto_bright"].as<bool>();
    if (jsObj.hasOwnProperty("auto_bright_thr"))
        RawProcessor.imgdata.params.auto_bright_thr = jsObj["auto_bright_thr"].as<float>();
    if (jsObj.hasOwnProperty("adjust_maximum_thr"))
        RawProcessor.imgdata.params.adjust_maximum_thr = jsObj["adjust_maximum_thr"].as<float>();
    if (jsObj.hasOwnProperty("bright"))
        RawProcessor.imgdata.params.bright = jsObj["bright"].as<float>();
    if (jsObj.hasOwnProperty("highlight"))
        RawProcessor.imgdata.params.highlight = jsObj["highlight"].as<int>();
    if (jsObj.hasOwnProperty("exp_shift"))
        RawProcessor.imgdata.params.exp_shift = jsObj["exp_shift"].as<float>();
    if (jsObj.hasOwnProperty("exp_preserve_highlights"))
        RawProcessor.imgdata.params.adjust_maximum_thr = jsObj["exp_preserve_highlights"].as<float>();
    if (jsObj.hasOwnProperty("no_auto_scale"))
        RawProcessor.imgdata.params.no_auto_scale = jsObj["no_auto_scale"].as<bool>();

    // Handle gamma array if provided (expecting at least 2 values)
    if (jsObj.hasOwnProperty("gamm"))
    {
        emscripten::val gamm = jsObj["gamm"];
        if (gamm.hasOwnProperty("length") && gamm["length"].as<unsigned>() >= 2)
        {
            RawProcessor.imgdata.params.gamm[0] = gamm[0].as<float>();
            RawProcessor.imgdata.params.gamm[1] = gamm[1].as<float>();
        }
    }
}

// Returns the current parameters as a JavaScript object
EMSCRIPTEN_KEEPALIVE
emscripten::val getParams()
{
    emscripten::val obj = emscripten::val::object();
    obj.set("user_flip", RawProcessor.imgdata.params.user_flip);
    obj.set("half_size", RawProcessor.imgdata.params.half_size);
    obj.set("four_color_rgb", RawProcessor.imgdata.params.four_color_rgb);
    obj.set("dcb_iterations", RawProcessor.imgdata.params.dcb_iterations);
    obj.set("dcb_iterations", RawProcessor.imgdata.params.dcb_iterations);
    obj.set("fbdd_noiserd", RawProcessor.imgdata.params.fbdd_noiserd);
    obj.set("use_camera_wb", RawProcessor.imgdata.params.use_camera_wb);
    obj.set("use_auto_wb", RawProcessor.imgdata.params.use_auto_wb);
    // obj.set("output_bps", RawProcessor.imgdata.params.output_bps);
    obj.set("no_auto_bright", RawProcessor.imgdata.params.no_auto_bright);
    obj.set("auto_bright_thr", RawProcessor.imgdata.params.auto_bright_thr);
    obj.set("adjust_maximum_thr", RawProcessor.imgdata.params.adjust_maximum_thr);
    obj.set("bright", RawProcessor.imgdata.params.bright);
    obj.set("highlight", RawProcessor.imgdata.params.highlight);
    obj.set("exp_shift", RawProcessor.imgdata.params.exp_shift);
    obj.set("exp_preserve_highlights", RawProcessor.imgdata.params.adjust_maximum_thr);
    obj.set("no_auto_scale", RawProcessor.imgdata.params.no_auto_scale);

    // Create a JavaScript array for gamma values.
    emscripten::val gamm = emscripten::val::array();
    gamm.set(emscripten::val(0), RawProcessor.imgdata.params.gamm[0]);
    gamm.set(emscripten::val(1), RawProcessor.imgdata.params.gamm[1]);
    obj.set("gamm", gamm);

    return obj;
}

EMSCRIPTEN_KEEPALIVE
emscripten::val getCameraInfo()
{
    emscripten::val obj = emscripten::val::object();
    // Concatenate camera make and model.
    std::string cameraModel = std::string(RawProcessor.imgdata.idata.make) + " " + std::string(RawProcessor.imgdata.idata.model);
    obj.set("cameraModel", cameraModel);
    obj.set("aperture", RawProcessor.imgdata.other.aperture);
    obj.set("iso", RawProcessor.imgdata.other.iso_speed);
    obj.set("shutterSpeed", RawProcessor.imgdata.other.shutter);
    return obj;
}
EMSCRIPTEN_BINDINGS(LibRawModule)
{
    emscripten::function("_setParams", &setParams);
    emscripten::function("_getParams", &getParams);
    emscripten::function("_getCameraInfo", &getCameraInfo);
}
