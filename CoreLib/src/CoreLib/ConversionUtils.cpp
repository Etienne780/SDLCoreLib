#include <charconv>
#include <type_traits>

#include "CoreLib\ConversionUtils.h"
#include "CoreLib\CoreMath.h"

int ConversionUtils::HexToIntegral(const std::string& hex) {
    size_t start = 0;
    if (hex.rfind("0x", 0) == 0 || hex.rfind("0X", 0) == 0) start = 2;
    else if (hex.rfind("#", 0) == 0) start = 1;

    int value = 0;
    auto [ptr, ec] = std::from_chars(hex.data() + start, hex.data() + hex.size(), value, 16);
    if (ec != std::errc()) return -1;
    return value;
}

float ConversionUtils::ToDegrees(float radians) {
    return radians * (180.0f / static_cast<float>(CORE_PI));
}

float ConversionUtils::ToRadians(float degrees) {
    return degrees * (static_cast<float>(CORE_PI) / 180.0f);
}

float ConversionUtils::ToDegrees(double radians) {
    return static_cast<float>(radians * (180.0 / CORE_PI));
}

float ConversionUtils::ToRadians(double degrees) {
    return static_cast<float>(degrees * (CORE_PI / 180.0));
}

double ConversionUtils::ToDegreesDouble(float radians) {
    return static_cast<double>(radians) * (180.0 / CORE_PI);
}

double ConversionUtils::ToRadiansDouble(float degrees) {
    return static_cast<double>(degrees) * (CORE_PI / 180.0);
}

double ConversionUtils::ToDegreesDouble(double radians) {
    return radians * (180.0 / CORE_PI);
}

double ConversionUtils::ToRadiansDouble(double degrees) {
    return degrees * (CORE_PI / 180.0);
}
