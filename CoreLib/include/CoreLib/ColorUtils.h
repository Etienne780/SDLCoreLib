#pragma once
#include <string>

class Vector3;
class Vector4;

/**
 * @brief Utility class for color space conversions and operations.
 */
class ColorUtils {
public:
    /**
    * @brief Converts HSV (Hue, Saturation, Value) to RGB color space.
    * @param color HSV color as Vector3: (Hue in [0.0, 360.0], Saturation and Value in [0.0, 1.0])
    * @return RGB color as Vector3 in [0, 1]
    */
    static Vector3 HSVToRGB(const Vector3& color);

    /**
     * @brief Converts HSV (Hue, Saturation, Value) to RGB color space.
     * @param h Hue angle in degrees [0, 360]
     * @param s Saturation [0, 1]
     * @param v Value (brightness) [0, 1]
     * @return RGB color as Vector3 in [0, 1]
     */
    static Vector3 HSVToRGB(float h, float s, float v);

    /**
    * @brief Converts RGB to HSV color space.
    * @param color RGB color as Vector3, where each component is expected to be in [0.0, 1.0].
    * @return HSV color as Vector3: (H in degrees [0,360], S and V in [0,1])
    */
    static Vector3 RGBToHSV(const Vector3& color);

    /**
     * @brief Converts RGB to HSV color space.
     * @param r Red component [0, 1]
     * @param g Green component [0, 1]
     * @param b Blue component [0, 1]
     * @return HSV color as Vector3: (H in degrees [0,360], S and V in [0,1])
     */
    static Vector3 RGBToHSV(float r, float g, float b);

    /**
     * @brief Linearly interpolates between two RGB colors.
     * @param a Start color
     * @param b End color
     * @param t Interpolation factor [0, 1]
     * @return Interpolated RGB color
     */
    static Vector3 LerpRGB(const Vector3& a, const Vector3& b, float t);

    /**
     * @brief Clamps a color to the range [0, 1] per component.
     * @param color Input color
     * @return Clamped color
     */
    static Vector3 ClampColor(const Vector3& color);

    /**
     * @brief Converts RGB values (in range [0, 1]) to a hexadecimal color string (e.g., "#FFAABB").
     *
     * @param color RGB color as Vector3, where each component is expected to be in [0.0, 1.0].
     * @param withPrefix If true, prepends a '#' character to the resulting string.
     * @return Hexadecimal color string representing the RGB input.
     */
    static std::string RGBToHex(const Vector3& color, bool withPrefix = true);

    /**
    * @brief Converts RGB values (in range [0, 1]) to a hexadecimal color string (e.g., "#FFAABB").
    *
    * @param r Red component [0, 1]
    * @param g Green component [0, 1]
    * @param b Blue component [0, 1]
    * @param withPrefix If true, prepends a '#' character to the resulting string.
    * @return Hexadecimal color string representing the RGB input.
    */
    static std::string RGBToHex(float r, float g, float b, bool withPrefix = true);

    /**
    * @brief Converts HSV values (Hue in [0.0, 360.0), Saturation and Value in [0.0, 1.0]) to a hexadecimal color string (e.g., "#FFAABB").
    *
    * @param color HSV color as Vector3: (Hue in [0.0, 360.0], Saturation and Value in [0.0, 1.0])
    * @param withPrefix If true, prepends a '#' character to the resulting string.
    * @return Hexadecimal color string representing the RGB input.
    */
    static std::string HSVToHex(const Vector3& color, bool withPrefix = true);

    /**
    * @brief Converts HSV values (Hue in [0.0, 360.0], Saturation and Value in [0.0, 1.0]) to a hexadecimal color string (e.g., "#FFAABB").
    *
    * @param h Hue angle in degrees [0, 360)
    * @param s Saturation [0, 1]
    * @param v Value (brightness) [0, 1]
    * @param withPrefix If true, prepends a '#' character to the resulting string.
    * @return Hexadecimal color string representing the RGB input.
    */
    static std::string HSVToHex(float r, float g, float b, bool withPrefix = true);

    /**
    * @brief Converts a hexadecimal color string to an RGB color.
    *
    * Accepts strings in the format "#RRGGBB", where RR, GG, and BB are two-digit
    * hexadecimal values representing red, green, and blue components.
    *
    * @param hex Hexadecimal color string (must start with '#' and be exactly 7 characters long)
    * @return RGB color as Vector3, with each component in the range [0.0, 1.0]
    */
    static Vector3 HexToRGB(const std::string& hex);

    /**
    * @brief Converts a hexadecimal color string to an RGBA color.
    *
    * Accepts strings in the format "#RRGGBBAA", where RR, GG, BB, and AA are
    * two-digit hexadecimal values representing red, green, blue, and alpha components.
    *
    * @param hex Hexadecimal color string (must start with '#' and be 9 characters long)
    * @return RGBA color as Vector4, with each component normalized to [0.0, 1.0]
    */
    static Vector4 HexToRGBA(const std::string& hex);

private:
    ColorUtils();

    static std::string RGBToHexImpl(int r, int g, int b, bool withPrefix = true);
};