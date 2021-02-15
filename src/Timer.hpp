#pragma once

#include "configuration.hpp"
#include <chrono>
#include <iostream>
#include <string>

class Timer
{
private:
    std::string timer_name_;
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point finish_;

public:
    explicit inline Timer(std::string &&timer_name) noexcept : timer_name_(timer_name) {}
    inline Timer(Timer &&) = default;

    inline void start()
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

    inline void stop()
    {
        finish_ = std::chrono::high_resolution_clock::now();
    }

    inline void print()
    {
        if (finish_.time_since_epoch().count() == 0)
            finish_ = std::chrono::high_resolution_clock::now();

        auto ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish_ - start_);
        std::cout << timer_name_ << ' ' << ms_duration.count() << "ms" << std::endl;
    }
};