#pragma once

#include "Material.hpp"
#include "Ray.hpp"
#include "configuration.hpp"
#include <cmath>

class Sphere
{
private:
    Material mat_;

    // Center
    Vec3 cnr_;
    float radius_;

public:
    Sphere(Material mat, Vec3 orig, float radius) : mat_(mat), cnr_(orig), radius_(radius)
    {
    }

    Sphere(const Sphere &) = default;
    Sphere(Sphere &&) = default;

    Vec3::value_type intersect(const Ray &ray) const
    {
        auto A = ray.dir.dot(ray.dir);
        auto B = 2 * (ray.orn - cnr_).dot(ray.dir);
        auto C = cnr_.dot(cnr_) - radius_ * radius_ - 2 * (ray.orn.dot(cnr_)) + ray.orn.dot(ray.orn);

        auto D = B * B - 4 * A * C;
        if (D < EPSILON)
            return false;

        B = -B / (2 * A);
        D = std::sqrt(D) / (2 * A);

        Vec3::value_type t;

        return (t = B - D) > EPSILON ? t : ((t = B + D) > EPSILON ? t : 0);
    }

    constexpr auto &material() const
    {
        return mat_;
    }

    const Vec3 normInPoint(const Vec3 &pnt) const
    {
        return pnt - cnr_;
    }
};