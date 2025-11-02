#pragma once
#include <string>

class TimeUtils {
public:
    /**
    * @brief Gets the current date as a string. y-m-d
    */
    static std::string GetCurrentDateString();

    /**
    * @brief Gets the current time as a string. h-m-s
    */
    static std::string GetCurrentTimeString();

private:
    TimeUtils();
};

