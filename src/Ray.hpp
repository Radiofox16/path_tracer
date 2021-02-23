#pragma once

#include "configuration.hpp"

struct Ray
{
    // Origin
    Vec3 orn;

    // Direction
    Vec3 dir;

    /**
     * @brief 
     * origin + mult * dir
     * @param mult - multiplier for dir to calculate position on ray 
     * @return Vec3 - position on ray
     */
    Vec3 positionAlong(auto mult) const
    {
        return orn + mult * dir;
    }
};
