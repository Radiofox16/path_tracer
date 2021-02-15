#pragma once

#include "Ray.hpp"
#include "configuration.hpp"

struct Material
{
    //! how light color parts absorbs  by material
    //! for air or ideal transparent glass it is  1.,1.,1.,0.
    //! 0.,0.,0.,1. absorbs all light
    Color color;

    //! how many color self emission of light
    Color emission;

    //! how big diffusion probability cone
    float diffusion;

    //! probability light transfer into material
    float transparency;

    //! refractive index
    float refractive_idx;

    Vector3f reflect_ideal(Vector3f failing_dir, Vector3f normal) const
    {
        auto n = normal.normalized();
        auto f = failing_dir.normalized();

        n *= std::abs(n.dot(f));

        return (n * 2) + f;
    }

    // Reflect direction
    Vector3f reflect(Vector3f failing_dir, Vector3f normal) const
    {
        auto reflection = reflect_ideal(failing_dir, normal);

        auto rnd = []() {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        };

        if (diffusion > EPSILON)
        {
            float diffs = diffusion;
            if (diffs > (1. - EPSILON))
                diffs = (1. - EPSILON);

            diffs *= M_PI_2;

            Vector3f part1 = ((rnd() > 0.5) ? 1 : -1) * (reflection.cross(normal).normalized() * std::tan(diffs) * rnd());
            Vector3f part2 = ((rnd() > 0.5) ? 1 : -1) * (part1.cross(normal).normalized() * std::tan(diffs) * rnd());

            reflection += part1;
            reflection += part2;
        }

        return reflection;
    }

    // Refract direction
    Vector3f refract(Vector3f failing_dir, Vector3f normal, const Material &ray_env) const
    {
        const Vector3f n = normal.normalized();
        const Vector3f f = failing_dir.normalized();
        const double r = ray_env.refractive_idx / refractive_idx;
        const double cos_n_f = std::abs(n.dot(f));

        const auto sinT2 = r * r * (1. - (cos_n_f * cos_n_f));

        if (sinT2 > 1.0f)
        {
            return reflect_ideal(failing_dir, normal);
        }

        Vector3f offs_part = (f + cos_n_f * n).normalized() * std::sqrt(sinT2);
        Vector3f norm_part = -n * std::sqrt(1. - sinT2);

        return offs_part + norm_part;
    }

    Ray interact(Ray falling_ray, Vector3f interaction_point, Vector3f normal, const Material &ray_env) const
    {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        Ray result_ray(interaction_point, {});

        if (transparency > r)
        {
            result_ray.dir = refract(falling_ray.dir, normal, ray_env);
        }
        else
        {
            result_ray.dir = reflect(falling_ray.dir, normal);
        }

        return result_ray;
    }
};

static const Material AIR({1, 1, 1}, {0, 0, 0}, 0., 1., 1.);
