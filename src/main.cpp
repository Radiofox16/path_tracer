#include "Material.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "Timer.hpp"
#include "configuration.hpp"
#include "save_png.hpp"
#include <boost/container/static_vector.hpp>
#include <vector>

// TODO FIX MULTIMATERIAL AND MANY OTHER STUFF

static const Material WHT_RDNT({0.1, 0.1, 0.1}, {1, 1, 1}, 0.10, 0.0, 1.0);
static const Material RED_DIFF({0.8, 0.1, 0.1}, {0, 0, 0}, 0.67, 0.0, 1.0);
static const Material GRN_MIRR({0.1, 0.9, 0.1}, {0, 0, 0}, 0.03, 0.0, 1.0);
static const Material BLU_DIFF({0.1, 0.1, 0.8}, {0, 0, 0}, 0.67, 0.0, 1.0);
static const Material WHT_GLAS({1.0, 1.0, 1.0}, {0, 0, 0}, 0.10, 1.0, 1.55);

static const Sphere spheres[] = {
    Sphere(WHT_RDNT, {0, 0, -500}, 450),
    Sphere(WHT_RDNT, {25, 25, 200}, 10),
    Sphere(RED_DIFF, {-25, -25, 130}, 20),
    Sphere(GRN_MIRR, {-25, 25, 130}, 20),
    Sphere(BLU_DIFF, {25, -25, 130}, 20),
    Sphere(WHT_GLAS, {25, 25, 100}, 20)};
// Sphere(WHT_GLAS, {0, 0, 130}, 20)};

// static const Sphere spheres[] = {
//     Sphere(WHT_RDNT, {0, 0, -5000}, 4900),
//     Sphere(WHT_GLAS, {0, 0, 100}, 50),
// };

constexpr auto spheres_count = sizeof(spheres) / sizeof(spheres[0]);

auto find_nearest_hitting_sphere(Ray ray)
{
    int i = 0;
    ray.origin += ray.dir * EPSILON;

    constexpr auto max_float = std::numeric_limits<float>::infinity();
    Vector3f hit{max_float, max_float, max_float};
    for (; i < spheres_count; i++)
    {
        hit = spheres[i].intersect(ray);
        if (hit.allFinite())
            break;
    }

    std::pair result{hit, i};

    for (; i < spheres_count; i++)
    {
        hit = spheres[i].intersect(ray);
        if (hit.allFinite())
        {
            if ((hit - ray.origin).squaredNorm() < (result.first - ray.origin).squaredNorm())
            {
                result = std::pair{hit, i};
            }
        }
    }

    return result;
}

using Trace = boost::container::static_vector<Material, RAY_MAX_DEPTH>;
Trace trace_ray(Ray cam_ray)
{
    Trace trace;
    // Push beginning for refraction
    trace.push_back(AIR);

    int prev_sphere_num = -1;

    Ray current_ray = cam_ray;
    for (size_t depth = 1; depth < RAY_MAX_DEPTH; depth++)
    {
        auto [hit, sphere_num] = find_nearest_hitting_sphere(current_ray);
        if (sphere_num == spheres_count)
            break;

        if (prev_sphere_num != sphere_num)
        {
            current_ray = spheres[sphere_num].interact(current_ray, hit, trace.back());
            trace.push_back(spheres[sphere_num].material());
        }
        else
        {
            Vector3f norm = spheres[sphere_num].normInPoint(hit);
            current_ray = AIR.interact(current_ray, hit, -norm, spheres[sphere_num].material());
            trace.push_back(AIR);
        }

        prev_sphere_num = sphere_num;
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
    }

    return pix_color;
}

struct Camera
{
    //< Position in space
    Vector3f pos;

    //< Front direction
    Vector3f dir;

    //< Vector from center to screen up
    Vector3f up;

    Vector3f horisonalStepPerPixel() const
    {
        Vector3f cam_right = up.cross(dir);
        cam_right.normalize();
        return std::tan(degree2rad(FOV_DEG * 0.5)) * cam_right / (IMAGE_WIDTH / 2);
    }

    Vector3f verticalStepPerPixel() const
    {
        auto up_norm = -up.normalized();
        return std::tan(degree2rad(ASPECT_RATIO * FOV_DEG * 0.5)) * up / (IMAGE_HEIGHT / 2);
    }
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
    Timer algo_time("Full");
    algo_time.start();

    const auto h_step = cam.horisonalStepPerPixel();
    const auto v_step = cam.verticalStepPerPixel();

    float persentage = 0.0f;

    auto canvas = createCanvas();

    Vector3f camera_dir = cam.dir - h_step * (IMAGE_WIDTH / 2) - v_step * (IMAGE_HEIGHT / 2);
    for (size_t y = 0; y < IMAGE_HEIGHT; y++, camera_dir += v_step)
    {
        persentage += 100.f / IMAGE_HEIGHT;
        // Timer h_string_timer("h_string");
        // h_string_timer.start();
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

        std::cout << "persentage: " << persentage << std::endl;
        // h_string_timer.stop();
        // h_string_timer.print();
    }

    algo_time.print();

    save_png(canvas, "test.png");
    return 0;
}