#pragma once

#include <chrono>
#include <string>

namespace DEBUGGING
{
    class Timer
    {
    public:
        explicit Timer(const std::string& name) :
            Name(name),
            StartTime(std::chrono::high_resolution_clock::now())
        {}

        std::string GetElapsedTimeText() const
        {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto elapsed_time = current_time - StartTime;
            std::string elapsed_time_text = Name + ": " + std::to_string(elapsed_time.count());
            return elapsed_time_text;
        }

        std::string Name = {};
        std::chrono::time_point<std::chrono::high_resolution_clock> StartTime = {};
    };
}
