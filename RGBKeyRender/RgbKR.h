#pragma once

#include "Platform.h"
#include <stdint.h>

extern "C"
{
    //Gets the exact width of the keyboard.
    //Using a texture of exactly the right width/height makes things more accurate.
    //Returns 0 if there is a problem with the RGB keyboard library.
    RKR_API uint8_t rkrGetWidth();
    //Gets the exact height of the keyboard.
    //Using a texture of exactly the right width/height makes things more accurate.
    //Returns 0 if there is a problem with the RGB keyboard library.
    RKR_API uint8_t rkrGetHeight();


    typedef RKR_API struct
    {
        uint8_t RGB[3];
    } RGBKey;

    //The "rgbaTexture" should be a set of pixels, left to right then top to bottom,
    //    where each pixel contains three one-byte components: Red, Green, and Blue.
    //The pixels are applied to the keyboard using bilinear filtering.
    //Returns 0 if it succeeds, and a Razer Chroma SDK error code if it fails.
    RKR_API long rkrRenderToKeyboard(uint32_t imgWidth, uint32_t imgHeight, const RGBKey* texture);
    //The "rgbaTexture" should be a set of rkrGetWidth() * rkrGetHeight() pixels,
    //    left to right then top to bottom,
    //    where each pixel contains three one-byte components: Red, Green, and Blue.
    //Returns 0 if it succeeds, and a Razer Chroma SDK error code if it fails.
    RKR_API long rkrRenderToKeyboardSimple(const RGBKey* texture);


    //Cleans up the library behind this interface.
    //Returns 0 if it succeeds, and a Razer Chroma SDK error code if it fails.
    RKR_API long rkrCleanUp();
}