#pragma once

#include "configuration.hpp"
#include <iostream>
#include <png++/png.hpp>

inline double clamp(double x)
{
    return x < 0. ? 0 : x > 1. ? 1.
                               : x;
}

inline png::rgb_pixel to_rgb_pixel(const Color &c)
{
    // Gamma correction
    auto gm_cr = [](double x) {
        return static_cast<int>(pow(clamp(x), 1 / 2.2) * 255 + 0.5);
    };

    return png::rgb_pixel(gm_cr(c[0]), gm_cr(c[1]), gm_cr(c[2]));
}

void save_png(const Canvas &cvs, std::string_view path)
{
    png::image<png::rgb_pixel> image(IMAGE_WIDTH, IMAGE_HEIGHT);

    for (auto y = 0; y < IMAGE_HEIGHT; ++y)
    {
        auto &img_string = cvs[y];

        for (auto x = 0; x < IMAGE_WIDTH; ++x)
        {
            image[y][x] = to_rgb_pixel(img_string[x]);
        }
    }

    image.write(path.data());
}
