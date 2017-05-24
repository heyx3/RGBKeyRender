#include "RgbKR.h"

#include <RzChromaSDKDefines.h>
#include <RzChromaSDKTypes.h>
#include <RzErrors.h>

#include <vector>


//Chroma SDK definitions:
namespace
{
    #define ALL_DEVICES         0
    #define KEYBOARD_DEVICES    1
    #define MOUSEMAT_DEVICES    2
    #define MOUSE_DEVICES       3
    #define HEADSET_DEVICES     4
    #define KEYPAD_DEVICES      5

    #ifdef _WIN64
        #define CHROMASDKDLL        L"RzChromaSDK64.dll"
    #else
        #define CHROMASDKDLL        L"RzChromaSDK.dll"
    #endif

    using namespace ChromaSDK;
    using namespace ChromaSDK::Keyboard;
    using namespace ChromaSDK::Keypad;
    using namespace ChromaSDK::Mouse;
    using namespace ChromaSDK::Mousepad;
    using namespace ChromaSDK::Headset;

    using Init =                        RZRESULT (*) ();
    using UnInit =                      RZRESULT (*) ();
    using CreateEffect =                RZRESULT (*) (RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
    using CreateKeyboardEffect =        RZRESULT (*) (ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
    using CreateKeypadEffect =          RZRESULT (*) (ChromaSDK::Keypad::EFFECT_TYPE Effect,   PRZPARAM pParam, RZEFFECTID *pEffectId);
    using CreateMouseEffect =           RZRESULT (*) (ChromaSDK::Mouse::EFFECT_TYPE Effect,    PRZPARAM pParam, RZEFFECTID *pEffectId);
    using CreateMousepadEffect =        RZRESULT (*) (ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
    using CreateHeadsetEffect =         RZRESULT (*) (ChromaSDK::Headset::EFFECT_TYPE Effect,  PRZPARAM pParam, RZEFFECTID *pEffectId);
    using SetEffect =                   RZRESULT (*) (RZEFFECTID EffectId);
    using DeleteEffect =                RZRESULT (*) (RZEFFECTID EffectId);
    using RegisterEventNotification =   RZRESULT (*) (HWND hWnd);
    using UnregisterEventNotification = RZRESULT (*) ();
    using QueryDevice =                 RZRESULT (*) (RZDEVICEID DeviceId, ChromaSDK::DEVICE_INFO_TYPE &DeviceInfo);
    
    Init chr_Init = NULL;
    UnInit chr_UnInit = NULL;
    CreateEffect chr_CreateEffect = NULL;
    CreateKeyboardEffect chr_CreateKeyboardEffect = NULL;
    CreateKeypadEffect chr_CreateKeypadEffect = NULL;
    CreateMouseEffect chr_CreateMouseEffect = NULL;
    CreateMousepadEffect chr_CreateMousepadEffect = NULL;
    CreateHeadsetEffect chr_CreateHeadsetEffect = NULL;
    SetEffect chr_SetEffect = NULL;
    DeleteEffect chr_DeleteEffect = NULL;
    QueryDevice chr_QueryDevice = NULL;
    
    HMODULE ChromaSDKModule = NULL;
    RZRESULT CheckModule()
    {
        if (ChromaSDKModule != NULL)
            return RZRESULT_SUCCESS;

        ChromaSDKModule = LoadLibrary(CHROMASDKDLL);
        if (ChromaSDKModule == NULL)
            return RZRESULT_INVALID;


        //Load the functions.

#define LOAD_FUNC(funcType) \
    chr_##funcType = (funcType)GetProcAddress(ChromaSDKModule, #funcType); \
    if (chr_##funcType == NULL) \
        return RZRESULT_INVALID;

        LOAD_FUNC(Init);
        auto initResult = chr_Init();
        if (initResult != RZRESULT_SUCCESS)
            return initResult;

        LOAD_FUNC(UnInit);
        LOAD_FUNC(CreateEffect);
        LOAD_FUNC(CreateKeyboardEffect);
        LOAD_FUNC(CreateKeypadEffect);
        LOAD_FUNC(CreateMouseEffect);
        LOAD_FUNC(CreateMousepadEffect);
        LOAD_FUNC(CreateHeadsetEffect);
        LOAD_FUNC(SetEffect);
        LOAD_FUNC(DeleteEffect);
        LOAD_FUNC(QueryDevice);

#undef LOAD_FUNC

        return RZRESULT_SUCCESS;
    }
}

//My custom effect stuff:
namespace
{
    ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE theEffect = {};

    template<typename T>
    inline T Min(T a, T b) { return (a < b ? a : b); }
    template<typename T>
    inline T Max(T a, T b) { return (a > b ? a : b); }

    inline float ColorByteToFloat(uint8_t byte)
    {
        return byte / 255.0f;
    }
    inline uint8_t FloatToColorByte(float f)
    {
        return (uint8_t)Min(255.01f, Max(0.01f, f * 255.0f));
    }

    inline float Lerp(float a, float b, float t) { return a + (t * (b - a)); }

    //Interpolates between the four given values in a square, given the 2D coordinate from 0 to 1.
    inline float Lerp(float value_minXY, float value_maxXminY,
                      float value_minXmaxY, float value_maxXY,
                      float tX, float tY)
    {
        return Lerp(Lerp(value_minXY, value_maxXminY, tX),
                    Lerp(value_minXmaxY, value_maxXY, tX),
                    tY);
    }

    std::vector<float> colorBuffers[3] = { {}, {}, {} };
}


//The DLL code:

float rkrGetAspectRatio()
{
    RZRESULT result = CheckModule();
    if (result != RZRESULT_SUCCESS)
        return result;

    return (float)ChromaSDK::Keyboard::MAX_ROW / ChromaSDK::Keyboard::MAX_COLUMN;
}
uint8_t rkrGetWidth()
{
    RZRESULT result = CheckModule();
    if (result != RZRESULT_SUCCESS)
        return result;

    return ChromaSDK::Keyboard::MAX_COLUMN;
}
uint8_t rkrGetHeight()
{
    RZRESULT result = CheckModule();
    if (result != RZRESULT_SUCCESS)
        return result;

    return ChromaSDK::Keyboard::MAX_ROW;
}

int rkrRenderToKeyboard(uint32_t imgWidth, uint32_t imgHeight, const RGBKey* texData)
{
    RZRESULT result = CheckModule();
    if (result != RZRESULT_SUCCESS)
        return result;

    //Convert color data to floats.
    for (size_t i = 0; i < 3; ++i)
        colorBuffers[i].resize(imgWidth * imgHeight);
    for (size_t x = 0; x < imgWidth; ++x)
    {
        for (size_t y = 0; y < imgHeight; ++y)
        {
            size_t index = x + (y * imgWidth);
            for (size_t i = 0; i < 3; ++i)
                colorBuffers[i][index] = ColorByteToFloat(texData[index].RGB[i]);
        }
    }

    //Interpolate between the float data.
    const size_t max_row = ChromaSDK::Keyboard::MAX_ROW,
                 max_col = ChromaSDK::Keyboard::MAX_COLUMN;
    for (size_t row = 0; row < max_row; row++)
    {
        float posY = row / (float)(max_row - 1),
              floorY = std::floor(posY * imgHeight),
              tY = posY - floorY;

        size_t minPixelY = Min<size_t>(imgHeight - 1, (size_t)floorY),
               maxPixelY = Min<size_t>(imgHeight - 1, minPixelY + 1);

        for (size_t col = 0; col < max_col; col++)
        {
            float posX = col / (float)(max_col - 1),
                  floorX = std::floor(posX * imgWidth),
                  tX = posX - floorX;

            size_t minPixelX = Min<size_t>(imgWidth - 1, (size_t)floorX),
                   maxPixelX = Min<size_t>(imgWidth - 1, minPixelX + 1);

            //Sample each color component with bilinear filtering.
            size_t pixelMinXY = minPixelX + (minPixelY * imgWidth),
                   pixelMaxXMinY = maxPixelX + (minPixelY * imgWidth),
                   pixelMinXMaxY = minPixelX + (maxPixelY * imgWidth),
                   pixelMaxXY = maxPixelX + (maxPixelY * imgWidth);
            uint8_t rgb[3];
            for (size_t i = 0; i < 3; ++i)
            {
                float componentF = Lerp(colorBuffers[i][pixelMinXY], colorBuffers[i][pixelMaxXMinY],
                                        colorBuffers[i][pixelMinXMaxY], colorBuffers[i][pixelMaxXY],
                                        tX, tY);
                rgb[i] = FloatToColorByte(componentF);
            }

            //Set this key's color to the sampled color.
            theEffect.Color[row][col] = RGB(rgb[0], rgb[1], rgb[2]);
        }
    }
    
    //Apply the effect.
    return chr_CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &theEffect, nullptr);
}
int rkrRenderToKeyboardSimple(const RGBKey* texData)
{
    RZRESULT result = CheckModule();
    if (result != RZRESULT_SUCCESS)
        return result;

    for (size_t row = 0; row < ChromaSDK::Keyboard::MAX_ROW; row++)
    {
        for (size_t col = 0; col < ChromaSDK::Keyboard::MAX_COLUMN; col++)
        {
            auto rgb = texData[col + (row * ChromaSDK::Keyboard::MAX_COLUMN)];
            theEffect.Color[row][col] = RGB(rgb.RGB[0], rgb.RGB[1], rgb.RGB[2]);
        }
    }

    //Apply the effect.
    return chr_CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &theEffect, nullptr);
}

int rkrCleanUp()
{
    if (ChromaSDKModule == NULL)
        return RZRESULT_SUCCESS;

    auto result = chr_UnInit();

    if (result == RZRESULT_SUCCESS)
    {
        FreeLibrary(ChromaSDKModule);
        ChromaSDKModule = NULL;
    }

    return result;
}