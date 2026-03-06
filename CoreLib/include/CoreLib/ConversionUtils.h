#pragma once
#include <string>
#include <type_traits>
#include <bitset>
#include <sstream>
#include <vector>
#include <cstdint>

/**
* @brief Provides utility functions for common data format conversions.
*/
class ConversionUtils {
public:
    ConversionUtils(const ConversionUtils&) = delete;
    ConversionUtils(ConversionUtils&&) = delete;

    ConversionUtils& operator=(const ConversionUtils&) = delete;
    ConversionUtils& operator=(ConversionUtils&&) = delete;

    /**
    * @brief Converts a hexadecimal string to an integer.
    *
    * Supports optional "0x" or "0X" prefix.
    *
    * @param hex The hex string to convert.
    * @return The corresponding integer value or -1 if failed.
    */
    static int HexToIntegral(const std::string& hex);

    /**
    * @brief Converts an integral value to a hexadecimal string.
    *
    * @tparam T An integral type (e.g., int, uint32_t).
    * @param num The value to convert.
    * @param withPrefix If true, includes the "0x" prefix.
    * @return A string representing the value in hexadecimal format.
    */
    template<typename T>
    static std::string IntegralToHex(T num, bool withPrefix) {
        static_assert(std::is_integral<T>::value, "IntegralToHex requires integral types");

        std::ostringstream stream;
        stream << std::hex << std::uppercase << static_cast<uint64_t>(num);

        return withPrefix ? "0x" + stream.str() : stream.str();
    }

    /**
    * @brief Converts an integral value to a binary string.
    *
    * @tparam T An integral type (e.g., int, uint32_t).
    * @param num The value to convert.
    * @param withPrefix If true, includes the "0b" prefix.
    * @return A string representing the value in binary format.
    */
    template<typename T>
    static std::string IntegralToBinary(T num, bool withPrefix) {
        static_assert(std::is_integral<T>::value, "IntegralToBinary requires integral types");

        constexpr size_t bits = sizeof(T) * 8;
        std::bitset<bits> binary(static_cast<uint64_t>(num));

        // Remove leading zeroes
        std::string binStr = binary.to_string();
        size_t firstOne = binStr.find('1');
        if (firstOne != std::string::npos) {
            binStr = binStr.substr(firstOne);
        }
        else {
            binStr = "0";
        }

        return withPrefix ? "0b" + binStr : binStr;
    }

    // Converts an angle in radians to degrees.
    // Example: π radians ≈ 180 degrees.
    static float ToDegrees(float radians);

    // Converts an angle in degrees to radians.
    // Example: 180 degrees ≈ π radians.
    static float ToRadians(float degrees);

    // Converts an angle in radians to degrees.
    // Example: π radians ≈ 180 degrees.
    static float ToDegrees(double radians);

    // Converts an angle in degrees to radians.
    // Example: 180 degrees ≈ π radians.
    static float ToRadians(double degrees);

    // Converts an angle in radians to degrees.
    // Example: π radians ≈ 180 degrees.
    static double ToDegreesDouble(float radians);

    // Converts an angle in degrees to radians.
    // Example: 180 degrees ≈ π radians.
    static double ToRadiansDouble(float degrees);

    // Converts an angle in radians to degrees.
    // Example: π radians ≈ 180 degrees.
    static double ToDegreesDouble(double radians);

    // Converts an angle in degrees to radians.
    // Example: 180 degrees ≈ π radians.
    static double ToRadiansDouble(double degrees);

    /**
    * @brief Encodes raw bytes into a Base64 string.
    *
    * @param data Vector of bytes to encode.
    * @return Base64-encoded string.
    */
    static std::string ToBase64(const std::vector<uint8_t>& data);

    /**
    * @brief Decodes a Base64 string back into bytes.
    *
    * @param base64Str Base64-encoded input string.
    * @return Vector of decoded bytes.
    * @throws std::runtime_error if input is invalid.
    */
    static std::vector<uint8_t> FromBase64(const std::string& base64Str);

private:
    static inline const char kBase64Table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    static inline const int8_t kBase64ReverseTable[256] = {
        /* -1 = invalid character */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1, 0,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        // rest default -1
    };

    ConversionUtils();
};