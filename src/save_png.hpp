#pragma once

#include "configuration.hpp"
#include <iostream>
#include <png++/png.hpp>

void save_png(const Canvas &cvs, std::string_view path)
{
    auto max_float = 0.0f;
    for (const auto &arr : cvs)
    {
        for (auto &&i : arr)
        {
            auto mx = i.maxCoeff();
            if (max_float < mx)
                max_float = mx;
        }
    }

    png::image<png::rgb_pixel> image(IMAGE_WIDTH, IMAGE_HEIGHT);

    for (auto y = 0; y < IMAGE_HEIGHT; ++y)
    {
        auto &img_string = cvs[y];

        for (auto x = 0; x < IMAGE_WIDTH; ++x)
        {
            Vector3f pix = (img_string[x] * 255.f) / max_float;
            image[y][x] = png::rgb_pixel(pix[0], pix[1], pix[2]);
        }
    }

    image.write(path.data());
}
