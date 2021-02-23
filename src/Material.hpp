#pragma once

#include "Ray.hpp"
#include "configuration.hpp"
#include "random.hpp"

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

    Vec3 reflect_ideal(Vec3 failing_dir, Vec3 normal) const
    {
        auto n = normal.normalized();
        auto f = failing_dir.normalized();

        n *= std::abs(n.dot(f));

        return (n * 2) + f;
    }

    // Reflect direction
    Vec3 reflect(Vec3 failing_dir, Vec3 normal) const
    {
        auto reflection = reflect_ideal(failing_dir, normal);

        double r1 = 2 * M_PI * erand48(SEED), r2 = erand48(SEED) * diffusion, r2s = sqrt(r2);

        reflection.normalize();
        reflection *= std::sqrt(1 - r2);
        Vec3 part1 = reflection.cross(normal).normalized() * std::cos(r1) * r2s;
        Vec3 part2 = part1.cross(normal).normalized() * std::sin(r1) * r2s;

        reflection += part1;
        reflection += part2;

        return reflection;
    }

    // Refract direction
    Vec3 refract(Vec3 failing_dir, Vec3 normal, const Material &ray_env) const
    {
        const Vec3 n = normal.normalized();
        const Vec3 f = failing_dir.normalized();
        const double r = ray_env.refractive_idx / refractive_idx;
        const double cos_n_f = std::abs(n.dot(f));

        const auto sinT2 = r * r * (1. - (cos_n_f * cos_n_f));

        if (sinT2 > 1.0f)
        {
            return reflect_ideal(failing_dir, normal);
        }

        Vec3 offs_part = (f + cos_n_f * n).normalized() * std::sqrt(sinT2);
        Vec3 norm_part = -n * std::sqrt(1. - sinT2);

        return offs_part + norm_part;
    }
};
