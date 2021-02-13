#include "configuration.hpp"
#include "save_png.hpp"
#include <vector>

struct Camera
{
    //< Position in space
    Vector3f pos;

    //< Front direction
    Vector3f dir;

    //< Vector from center to screen up
    Vector3f up;
};

// Pre-defined camera
Camera cam{{0., 0., 0.}, {0., 0., 1.}, {0., 1., 0.}};

auto createCanvas()
{
    Canvas cvs(IMAGE_HEIGHT);

    for (auto &i : cvs)
    {
        i.fill(BACKGROUND_COLOR);
    }

    return cvs;
}

int main()
{
    auto canvas = createCanvas();

    save_png(canvas, "test.png");
    return 0;
}