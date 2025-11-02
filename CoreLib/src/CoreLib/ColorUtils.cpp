#include <cmath>
#include <sstream>
#include <algorithm>

#include "CoreLib\ColorUtils.h"

#include "CoreLib\Math\Vector3.h"
#include "CoreLib\Math\Vector4.h"
#include "CoreLib\ConversionUtils.h"
#include "CoreLib\Log.h"

Vector3 ColorUtils::HSVToRGB(const Vector3& color) {
    return HSVToRGB(color.x, color.y, color.z);
}

Vector3 ColorUtils::HSVToRGB(float h, float s, float v) {
    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r = 0, g = 0, b = 0;
    if (0 <= h && h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else if (h < 360) { r = c; g = 0; b = x; }

    return Vector3(r + m, g + m, b + m);
}

Vector3 ColorUtils::RGBToHSV(const Vector3& color) {
    return RGBToHSV(color.x, color.y, color.z);
}

Vector3 ColorUtils::RGBToHSV(float r, float g, float b) {
    float max = std::fmax(r, std::fmax(g, b));
    float min = std::fmin(r, std::fmin(g, b));
    float delta = max - min;

    float h = 0;
    if (delta != 0) {
        if (max == r) {
            h = 60.0f * std::fmod(((g - b) / delta), 6.0f);
        }
        else if (max == g) {
            h = 60.0f * (((b - r) / delta) + 2.0f);
        }
        else {
            h = 60.0f * (((r - g) / delta) + 4.0f);
        }
    }
    if (h < 0) h += 360.0f;

    float s = max == 0 ? 0 : delta / max;
    float v = max;

    return Vector3(h, s, v);
}

Vector3 ColorUtils::LerpRGB(const Vector3& a, const Vector3& b, float t) {
    return Vector3(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    );
}

Vector3 ColorUtils::ClampColor(const Vector3& color) {
    float r = std::fmax(0.0f, std::fmin(1.0f, color.x));
    float g = std::fmax(0.0f, std::fmin(1.0f, color.y));
    float b = std::fmax(0.0f, std::fmin(1.0f, color.z));
    return Vector3(r, g, b);
}

std::string ColorUtils::RGBToHex(const Vector3& color, bool withPrefix) {
    return RGBToHex(color.x, color.y, color.z, withPrefix);
}

std::string ColorUtils::RGBToHex(float r, float g, float b, bool withPrefix) {
    int newR = static_cast<int>(r * 255.0f);
    int newG = static_cast<int>(g * 255.0f);
    int newB = static_cast<int>(b * 255.0f);

    newR = std::clamp(newR, 0, 255);
    newG = std::clamp(newG, 0, 255);
    newB = std::clamp(newB, 0, 255);

    return RGBToHexImpl(newR, newG, newB, withPrefix);
}

std::string ColorUtils::HSVToHex(const Vector3& color, bool withPrefix) {
    return HSVToHex(color.x, color.y, color.z, withPrefix);
}

std::string ColorUtils::HSVToHex(float h, float s, float v, bool withPrefix) {
    Vector3 rgb = HSVToRGB(h, s, v);

    int r = static_cast<int>(rgb.x * 255.0f);
    int g = static_cast<int>(rgb.y * 255.0f);
    int b = static_cast<int>(rgb.z * 255.0f);

    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);

    return RGBToHexImpl(r, g, b, withPrefix);
}

std::string ColorUtils::RGBToHexImpl(int r, int g, int b, bool withPrefix) {
    std::ostringstream stream;
    stream << std::hex << std::uppercase
        << (r < 16 ? "0" : "") << r
        << (g < 16 ? "0" : "") << g
        << (b < 16 ? "0" : "") << b;
    return withPrefix ? "#" + stream.str() : stream.str();
}

Vector3 ColorUtils::HexToRGB(const std::string& hex) {
    Vector3 result;

    if (hex.empty() || hex[0] != '#') {
        Log::Warn("HexToRGB: Hex string must start with '#'.");
        return result;
    }

    if (hex.length() != 7) {
        Log::Warn("HexToRGB: Hex string must be in format '#RRGGBB'.");
        return result;
    }

    try {
        int r = ConversionUtils::HexToIntegral(hex.substr(1, 2)); // R (at index 1 and 2)
        int g = ConversionUtils::HexToIntegral(hex.substr(3, 2)); // G (at index 3 and 4)
        int b = ConversionUtils::HexToIntegral(hex.substr(5, 2)); // B (at index 5 and 6)

        result = Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
    }
    catch (const std::exception& e) {
        Log::Error("HexToRGB: Conversion failed - {}", e.what());
    }

    return result;
}

Vector4 ColorUtils::HexToRGBA(const std::string& hex) {
    Vector4 result;

    if (hex.empty() || hex[0] != '#') {
        Log::Warn("HexToRGBA: Hex string must start with '#'.");
        return result;
    }

    if (hex.length() != 9) {
        Log::Warn("HexToRGBA: Hex string must be in format '#RRGGBBAA'.");
        return result;
    }

    try {
        int r = ConversionUtils::HexToIntegral(hex.substr(1, 2)); // R (at index 1 and 2)
        int g = ConversionUtils::HexToIntegral(hex.substr(3, 2)); // G (at index 3 and 4)
        int b = ConversionUtils::HexToIntegral(hex.substr(5, 2)); // B (at index 5 and 6)
        int a = ConversionUtils::HexToIntegral(hex.substr(7, 2)); // B (at index 7 and 8)

        result = Vector4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
    catch (const std::exception& e) {
        Log::Error("HexToRGBA: Conversion failed - {}", e.what());
    }

    return result;
}