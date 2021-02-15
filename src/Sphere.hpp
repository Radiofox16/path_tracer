#pragma once

#include "Material.hpp"
#include "Ray.hpp"
#include "configuration.hpp"
#include <cmath>

class Sphere
{
private:
    Material mat_;
    Vector3f orig_;
    float radius_;

public:
    Sphere(Material mat, Vector3f orig, float radius) : mat_(mat), orig_(orig), radius_(radius)
    {
    }

    Sphere(const Sphere &) = default;
    Sphere(Sphere &&) = default;

    Vector3f intersect(const Ray &ray) const
    {
        auto A = ray.dir.dot(ray.dir);
        auto B = 2 * (ray.origin - orig_).dot(ray.dir);
        auto C = orig_.dot(orig_) - radius_ * radius_ - 2 * (ray.origin.dot(orig_)) + ray.origin.dot(ray.origin);

        auto D = B * B - 4 * A * C;
        if (D < EPSILON)
        {
            auto tmp = std::numeric_limits<float>::infinity();
            return {tmp, tmp, tmp};
        }

        auto const_part = -B / (2 * A);
        auto discr_part = std::sqrt(D) / (2 * A);
        if (const_part > 0.f)
        {
            if (const_part > discr_part)
            {
                return ray.origin + ray.dir * (const_part - discr_part);
            }
            else
            {
                return ray.origin + ray.dir * (const_part + discr_part);
            }
        }

        if (const_part + discr_part < EPSILON)
        {
            auto tmp = std::numeric_limits<float>::infinity();
            return {tmp, tmp, tmp};
        }

        return ray.origin + ray.dir * (const_part + discr_part);
    }

    bool intersects(const Ray &ray) const
    {
        auto A = ray.dir.dot(ray.dir);
        auto B = 2 * (ray.origin - orig_).dot(ray.dir);
        auto C = orig_.dot(orig_) - radius_ * radius_ - 2 * (ray.origin.dot(orig_)) + ray.origin.dot(ray.origin);

        auto D = B * B - 4 * A * C;
        return !(D < EPSILON && (std::sqrt(D) - B) < EPSILON);
    }

    constexpr auto &material() const
    {
        return mat_;
    }

    const Vector3f normInPoint(const Vector3f &pnt) const
    {
        return pnt - orig_;
    }

    Ray interact(Ray ray, Vector3f interaction_point, const Material &ray_env) const
    {
        auto normal = normInPoint(interaction_point);
        return mat_.interact(ray, interaction_point, normal, ray_env);
    }
};