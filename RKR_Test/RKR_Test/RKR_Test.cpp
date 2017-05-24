#include <RgbKR.h>

#include <vector>
#include <iostream>


int main()
{
    const uint8_t imgWidth = rkrGetWidth(),
                  imgHeight = rkrGetHeight();
    
    std::vector<RGBKey> colors;
    colors.resize(imgWidth * imgHeight);

    for (size_t y = 0; y < imgHeight; ++y)
    {
        for (size_t x = 0; x < imgWidth; ++x)
        {
            auto& rgb = colors[x + (y * imgWidth)].RGB;
            rgb[0] = x * 10;
            rgb[1] = y * 40;
            rgb[2] = 0;
        }
    }

    std::cout << (size_t)rkrRenderToKeyboardSimple(colors.data()) << "\n";

    char d;
    std::cin >> d;

    return 0;
}