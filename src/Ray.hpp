#pragma once

#include "configuration.hpp"

struct Ray
{
    Vector3f origin;
    Vector3f dir;

    /**
     * @brief 
     * origin + mult * dir
     * @param mult - multiplier for dir to calculate position on ray 
     * @return Vector3f - position on ray
     */
    Vector3f positionAlong(float mult)
    {
        return origin + mult * dir;
    }
};
