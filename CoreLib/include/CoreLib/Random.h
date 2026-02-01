#pragma once

#include <random>
#include <unordered_map>

#include "IDManager.h"
#include "CoreID.h"
#include "FormatUtils.h"

/**
 * @brief Invalid ID value for random devices
 */
const uint32_t RANDOM_DEVICE_INVALID_ID = std::numeric_limits<uint32_t>::max();

/**
 * @brief Tag type used to create a strongly typed RandomDeviceID
 */
struct RandomDeviceTag {};

/**
 * @brief Strongly typed ID for identifying random number generator devices
 */
using RandomDeviceID = CoreID<uint32_t, RANDOM_DEVICE_INVALID_ID, RandomDeviceTag>;

/**
 * @brief Static random number utility class
 *
 * Provides a default random device as well as support for multiple
 * independent random devices identified by RandomDeviceID.
 *
 * The class supports integral and floating-point number generation,
 * including ranged and positive-only values.
 */
class Random {
public:
    Random() = delete;

    /**
    * @brief Set the seed of the default random device
    * @param seed Seed value
    */
    static void SetSeed(uint32_t seed);

    /**
    * @brief Get the seed of the default random device
    * @return Current seed value
    */
    static uint32_t GetSeed();

    /**
    * @brief Set the seed of a specific random device
    * @param id Random device identifier
    * @param seed Seed value
    */
    static void SetSeed(RandomDeviceID id, uint32_t seed);

    /**
    * @brief Get the seed of a specific random device
    * @param id Random device identifier
    * @return Seed value, or 0 if device does not exist
    */
    static uint32_t GetSeed(RandomDeviceID id);

    /**
    * @brief Create a new random device
    *
    * If seed is 0, a random seed generated via std::random_device is used.
    *
    * @param seed Optional seed value
    * @return Identifier of the newly created random device
    */
    static RandomDeviceID CreateRandomDevice(uint32_t seed = 0);

    /**
    * @brief Delete an existing random device
    * @param id Random device identifier
    */
    static void DeleteRandomDevice(RandomDeviceID id);

    /**
    * @brief Generate a random number using the default device
    *
    * Integral types: full numeric range
    * Floating point types: range [0, 1)
    *
    * @tparam T Numeric type
    * @return Random value
    */
    template<typename T>
    static T GetNumber();

    /**
    * @brief Generate a positive random number using the default device
    *
    * Integral types: range [1, max]
    * Floating point types: range [0, 1)
    *
    * @tparam T Numeric type
    * @return Positive random value
    */
    template<typename T>
    static T GetPositiveNumber();

    /**
    * @brief Generate a random number within a specific range using the default device
    *
    * @tparam T Numeric type
    * @param start Range start (inclusive)
    * @param end Range end (inclusive for integers)
    * @return Random value within the specified range
    */
    template<typename T>
    static T GetRangeNumber(T start, T end);

    /**
    * @brief Generate a random number using a specific random device
    *
    * Falls back to the default device if the ID is invalid.
    *
    * @tparam T Numeric type
    * @param id Random device identifier
    * @return Random value
    */
    template<typename T>
    static T GetNumber(RandomDeviceID id);

    /**
    * @brief Generate a positive random number using a specific random device
    *
    * Falls back to the default device if the ID is invalid.
    *
    * @tparam T Numeric type
    * @param id Random device identifier
    * @return Positive random value
    */
    template<typename T>
    static T GetPositiveNumber(RandomDeviceID id);

    /**
    * @brief Generate a random number within a specific range using a specific device
    *
    * Falls back to the default device if the ID is invalid.
    *
    * @tparam T Numeric type
    * @param id Random device identifier
    * @param start Range start (inclusive)
    * @param end Range end (inclusive for integers)
    * @return Random value within the specified range
    */
    template<typename T>
    static T GetRangeNumber(RandomDeviceID id, T start, T end);

private:
    /**
    * @brief Internal structure representing a random device
    */
    struct Device {
        std::mt19937 generator; /**< Mersenne Twister generator */
        uint32_t seed = 0;      /**< Stored seed value */
    };

    /**
    * @brief ID manager for random device identifiers
    */
    static inline IDManager<uint32_t, RANDOM_DEVICE_INVALID_ID> m_idManager;

    /**
    * @brief Map of active random devices
    */
    static inline std::unordered_map<RandomDeviceID, Device> m_devices;

    /**
    * @brief Default random device used when no ID is specified
    */
    static inline Device m_defaultDevice{
        std::mt19937(std::random_device{}()),
        0
    };
};

/**
* @brief String conversion for RandomDeviceID
* @param id Random device identifier
* @return String representation of the ID
*/
template<>
static inline std::string FormatUtils::toString<RandomDeviceID>(RandomDeviceID id) {
    return id.ToString();
}