#include "Material.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "Timer.hpp"
#include "configuration.hpp"
#include "random.hpp"
#include "save_png.hpp"
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <vector>

// TODO FIX MULTIMATERIAL AND MANY OTHER STUFF
static const Material WHT_RDNT({0., 0., 0.}, {12, 12, 12}, 0.0, 0.0, 1.0);
// static const Material RED_DIFF({0.8, 0.1, 0.1}, {0, 0, 0}, 0.1, 0.0, 1.0);
static const Material GRN_MIRR({0.1, 0.9, 0.1}, {0, 0, 0}, 0.03, 0.0, 1.0);
// static const Material BLU_DIFF({0.1, 0.1, 0.8}, {0, 0, 0}, 0.67, 0.0, 1.0);
static const Material WHT_GLAS({1.0, 1.0, 1.0}, {0, 0, 0}, 0.10, 1.0, 1.55);

static const Material UWHT_DIFF({0.999, 0.999, 0.999}, {0., 0., 0.}, 0.92, 0.0, 1.0);
static const Material UWHT_GLAS({0.999, 0.999, 0.999}, {0., 0., 0.}, 0., 1.0, 1.6);
static const Material UWHT_MIRR({0.999, 0.999, 0.999}, {0., 0., 0.}, 0.05, 0.0, 1.0);

static const Material BLU_DIFF({0.25, 0.25, 0.75}, {}, 1.0, 0.0, 1.0);
static const Material GRN_DIFF({0.25, 0.75, 0.25}, {}, 1.0, 0.0, 1.0);
static const Material RED_DIFF({0.75, 0.25, 0.25}, {}, 1.0, 0.0, 1.0);

static const Material MGT_DIFF({0.75, 0.10, 0.75}, {}, 0.0, 0.0, 1.0);

static const Material WHT_DIFF({0.75, 0.75, 0.75}, {0., 0., 0.}, 0.99, 0.0, 1.0);

static const Material BLK_DIFF({0., 0., 0.}, {0., 0., 0.}, 0.0, 0.0, 1.0);
static const Material BLK_RDNT({0., 0., 0.}, {12, 12, 12}, 0.0, 0.0, 1.0);

// AIR
static const Material ENV({1, 1, 1}, {0, 0, 0}, 0., 1., 1.);

// static const Sphere spheres[] = {
//     Sphere(WHT_RDNT, {0, 0, -500}, 450),
//     Sphere(WHT_RDNT, {25, 25, 200}, 10),
//     Sphere(RED_DIFF, {-25, -25, 130}, 20),
//     Sphere(GRN_MIRR, {-25, 25, 130}, 20),
//     Sphere(BLU_DIFF, {25, -25, 130}, 20),
//     Sphere(WHT_GLAS, {25, 25, 100}, 20)};
// Sphere(WHT_GLAS, {0, 0, 130}, 20)};

static const Sphere spheres[] = {
    Sphere(BLK_RDNT, {0, 4000, 6000}, 1000),
    // Sphere(WHT_GLAS, {0, 0, -22000}, 21000),
    Sphere(BLU_DIFF, {0, -4000 - 1e5, 0}, 1e5),  // BOTTOM
    Sphere(BLU_DIFF, {0, +4000 + 1e5, 0}, 1e5),  // TOP
    Sphere(RED_DIFF, {+4000 + 1e5, 0, 0}, 1e5),  // RIGHT
    Sphere(RED_DIFF, {-4000 - 1e5, 0, 0}, 1e5),  // LEFT
    Sphere(GRN_DIFF, {0, 0, 10000 + 1e5}, 1e5),  // BACK
    Sphere(MGT_DIFF, {-2000, -2000, 8000}, 2000) // BACK
    // Sphere(RED_DIFF, {0, 0, 80}, 20)
};

// static const Sphere spheres[] = {
// Sphere(RED_DIFF, Vec3(1e5 + 1, 40.8, 81.6), 1e5),   // LEFT
// Sphere(BLU_DIFF, Vec3(-1e5 + 99, 40.8, 81.6), 1e5), // RGHT
// Sphere(WHT_DIFF, Vec3(50, 40.8, 1e5), 1e5),         // BACK
// Sphere(BLK_DIFF, Vec3(50, 40.8, -1e5 + 170), 1e5),  // FRNT
// Sphere(WHT_DIFF, Vec3(50, 1e5, 81.6), 1e5),         // BOTM
// Sphere(WHT_DIFF, Vec3(50, -1e5 + 81.6, 81.6), 1e5), // TOP
// Sphere(UWHT_MIRR, Vec3(27, 16.5, 47), 16.5),        //
// Sphere(UWHT_GLAS, Vec3(73, 16.5, 78), 16.5),
// Sphere(BLK_RDNT, Vec3(50, 681.6 - .27, 81.6), 600)};

constexpr auto spheres_count = sizeof(spheres) / sizeof(spheres[0]);

auto find_nearest_hitting_sphere(const Ray &ray)
{
    using flt = Vec3::value_type;
    flt tmp, dir_multiplier = std::numeric_limits<flt>::max();
    int id = -1;

    for (size_t i = 0; i < spheres_count; i++)
    {
        tmp = spheres[i].intersect(ray);
        if (tmp != 0 && tmp < dir_multiplier)
        {
            dir_multiplier = tmp;
            id = i;
        }
    }

    return std::pair{id, dir_multiplier};
}

using Trace = boost::container::static_vector<Material, RAY_MAX_DEPTH>;
Trace trace_ray(const Ray &cam_ray)
{
    Trace trace;
    Ray ray = cam_ray;

    trace.push_back(ENV);

    boost::container::small_vector<int, RAY_MAX_DEPTH> spheres_stack;

    while (trace.size() < RAY_MAX_DEPTH - 1)
    {
        auto [id, dir_multiplier] = find_nearest_hitting_sphere(ray);
        if (id == -1)
            break;

        const auto &material = spheres[id].material();
        const auto interact_pnt = ray.positionAlong(dir_multiplier);
        const auto normal = spheres[id].normInPoint(interact_pnt);

        if (erand48(SEED) < material.transparency)
        {
            ray.dir = material.refract(ray.dir, normal, trace.back());
            if (spheres_stack.size() && spheres_stack.back() == id)
            {
                spheres_stack.pop_back();
                if (spheres_stack.size())
                    trace.push_back(spheres[spheres_stack.back()].material());
                else
                    trace.push_back(trace.front());
            }
            else
            {
                spheres_stack.push_back(id);
            }
        }
        else
        {
            ray.dir = material.reflect(ray.dir, normal);
            trace.push_back(material);

            if (spheres_stack.size())
                trace.push_back(spheres[spheres_stack.back()].material());
            else
                trace.push_back(ENV);
        }

        ray.orn = interact_pnt;

        if (material.color.isZero())
            break;
    }

    return trace;
}

Color calculate_color(Trace trace)
{
    Color pix_color{0, 0, 0};

    for (int i = trace.size() - 1; i >= 0; i--)
    {
        pix_color = pix_color.cwiseProduct(trace[i].color);
        pix_color += trace[i].emission;

        // std::cout << pix_color.transpose() << std::endl;
    }
    // std::cout << "--------------------------------------------" << std::endl;

    // return trace[1].color;

    return pix_color;
}

struct Camera
{
    //< Position in space
    Vec3 pos;

    //< Front direction
    Vec3 dir;

    //< Vector from center to screen up
    Vec3 up;

    Vec3 horisonalStepPerPixel() const
    {
        Vec3 cam_right = up.cross(dir);
        cam_right.normalize();
        return std::tan(degree2rad(FOV_DEG * 0.5)) * cam_right / (IMAGE_WIDTH / 2);
    }

    Vec3 verticalStepPerPixel() const
    {
        Vec3 up_norm = -up.normalized();
        return std::tan(degree2rad(ASPECT_RATIO * FOV_DEG * 0.5)) * up_norm / (IMAGE_HEIGHT / 2);
    }
};

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
    // Pre-defined camera
    Camera cam{{0., 0., 0.}, {0., 0., 1.}, {0., 1., 0.}};

    // INIT
    /////////////////////////////////////////////
    std::cout.precision(2);
    std::cout << std::fixed;

    Timer algo_time("Full");
    algo_time.start();

    const auto h_step = cam.horisonalStepPerPixel();
    const auto v_step = cam.verticalStepPerPixel();

    float persentage = 0.0f;

    auto canvas = createCanvas();
    /////////////////////////////////////////////
    // END INIT

    Vec3 camera_dir = cam.dir - h_step * (IMAGE_WIDTH / 2) - v_step * (IMAGE_HEIGHT / 2);
    for (size_t y = 0; y < IMAGE_HEIGHT; y++, camera_dir += v_step)
    {
        SEED[2] = y * y * 12345;
        persentage += 100.f / IMAGE_HEIGHT;

        for (size_t x = 0; x < IMAGE_WIDTH; x++, camera_dir += h_step)
        {
            Color summary_color{0, 0, 0};
            for (size_t i = 0; i < RAYS_PER_PIXEL; i++)
            {
                auto trace = trace_ray({cam.pos, camera_dir});
                summary_color += calculate_color(std::move(trace));
            }
            canvas[y][x] = summary_color / RAYS_PER_PIXEL;
        }
        camera_dir -= h_step * IMAGE_WIDTH;

        std::cout << "\rPersentage: " << persentage << '%';
        std::cout.flush();
        // h_string_timer.stop();
        // h_string_timer.print();
    }

    std::cout << std::endl;
    algo_time.print();

    save_png(canvas, "test.png");
    return 0;
}