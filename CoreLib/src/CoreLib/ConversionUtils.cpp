#include <charconv>
#include <stdexcept>
#include <type_traits>

#include "CoreLib/ConversionUtils.h"
#include "CoreLib/CoreMath.h"

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

std::string ConversionUtils::ToBase64(const std::vector<uint8_t>& data) {
    std::string result;
    result.reserve(((data.size() + 2) / 3) * 4);

    size_t i = 0;
    while (i + 3 <= data.size()) {
        // 3 Bytes -> 24 Bits
        uint32_t triple =
            (static_cast<uint32_t>(data[i]) << 16) |
            (static_cast<uint32_t>(data[i + 1]) << 8) |
            (static_cast<uint32_t>(data[i + 2]));

        result.push_back(kBase64Table[(triple >> 18) & 0x3F]);
        result.push_back(kBase64Table[(triple >> 12) & 0x3F]);
        result.push_back(kBase64Table[(triple >> 6) & 0x3F]);
        result.push_back(kBase64Table[triple & 0x3F]);

        i += 3;
    }

    size_t remaining = data.size() - i;
    if (remaining == 1) {
        uint32_t triple = data[i] << 16;
        result.push_back(kBase64Table[(triple >> 18) & 0x3F]);
        result.push_back(kBase64Table[(triple >> 12) & 0x3F]);
        result.push_back('=');
        result.push_back('=');
    }
    else if (remaining == 2) {
        uint32_t triple = (data[i] << 16) | (data[i + 1] << 8);
        result.push_back(kBase64Table[(triple >> 18) & 0x3F]);
        result.push_back(kBase64Table[(triple >> 12) & 0x3F]);
        result.push_back(kBase64Table[(triple >> 6) & 0x3F]);
        result.push_back('=');
    }

    return result;
}

std::vector<uint8_t> ConversionUtils::FromBase64(const std::string& base64Str) {
    if (base64Str.size() % 4 != 0)
        throw std::runtime_error("Invalid Base64 length");

    std::vector<uint8_t> result;
    result.reserve((base64Str.size() / 4) * 3);

    for (size_t i = 0; i < base64Str.size(); i += 4) {
        int8_t c0 = kBase64ReverseTable[static_cast<uint8_t>(base64Str[i])];
        int8_t c1 = kBase64ReverseTable[static_cast<uint8_t>(base64Str[i + 1])];
        int8_t c2 = kBase64ReverseTable[static_cast<uint8_t>(base64Str[i + 2])];
        int8_t c3 = kBase64ReverseTable[static_cast<uint8_t>(base64Str[i + 3])];

        if (c0 < 0 || c1 < 0 || c2 < 0 || c3 < 0)
            throw std::runtime_error("Invalid Base64 character");

        uint32_t triple =
            (static_cast<uint32_t>(c0) << 18) |
            (static_cast<uint32_t>(c1) << 12) |
            (static_cast<uint32_t>(c2) << 6) |
            static_cast<uint32_t>(c3);

        result.push_back((triple >> 16) & 0xFF);
        if (base64Str[i + 2] != '=')
            result.push_back((triple >> 8) & 0xFF);
        if (base64Str[i + 3] != '=')
            result.push_back(triple & 0xFF);
    }

    return result;
}