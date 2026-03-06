#include <stdexcept>

#include "Random.h"

void Random::SetSeed(uint32_t seed) {
    m_defaultDevice.seed = seed;
    m_defaultDevice.generator.seed(seed);
}

uint32_t Random::GetSeed() {
    return m_defaultDevice.seed;
}

RandomDeviceID Random::CreateRandomDevice(uint32_t seed) {
    RandomDeviceID newID = RandomDeviceID(m_idManager.GetNewUniqueIdentifier());
    std::mt19937 gen(seed ? seed : std::random_device{}());
    m_devices[newID] = Device{ gen, seed };
    return newID;
}

void Random::DeleteRandomDevice(RandomDeviceID id) {
    m_idManager.FreeUniqueIdentifier(id.value);
    m_devices.erase(id);
}

void Random::SetSeed(RandomDeviceID id, uint32_t seed) {
    auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        Log::Warn("Random::SetSeed: Could not set seed {} for id {}, id not found!", seed, id);
        return;
    }

    it->second.seed = seed;
    it->second.generator.seed(seed);
}

uint32_t Random::GetSeed(RandomDeviceID id) {
    auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        Log::Warn("Random::GetSeed: Could not get seed for id {}, id not found!", id);
        return 0;
    }

    return it->second.seed;
}

// --- Random number generation ---
template<typename T>
T Random::GetNumber() {
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        return dist(m_defaultDevice.generator);
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(0.0, 1.0);
        return dist(m_defaultDevice.generator);
    }
    else {
        static_assert(false, "Unsupported type for GetNumber");
    }
}

template<typename T>
T Random::GetPositiveNumber() {
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(1, std::numeric_limits<T>::max());
        return dist(m_defaultDevice.generator);
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(0.0, 1.0);
        return dist(m_defaultDevice.generator);
    }
    else {
        static_assert(false, "Unsupported type for GetPositiveNumber");
    }
}

template<typename T>
T Random::GetRangeNumber(T start, T end) {
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(start, end);
        return dist(m_defaultDevice.generator);
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(start, end);
        return dist(m_defaultDevice.generator);
    }
    else {
        static_assert(false, "Unsupported type for GetRangeNumber");
    }
}

template<typename T>
T Random::GetNumber(RandomDeviceID id) {
    auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        Log::Warn("Random::GetNumber: Could not get number for id {}, id not found! Used default device", id);
        return GetNumber<T>();
    }

    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        return dist(it->second.generator);
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(0.0, 1.0);
        return dist(it->second.generator);
    }
    else {
        static_assert(false, "Unsupported type for GetNumber");
    }
}

template<typename T>
T Random::GetPositiveNumber(RandomDeviceID id) {
    auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        Log::Warn("Random::GetPositiveNumber: id {} not found, using default device", id);
        return GetPositiveNumber<T>();
    }

    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(1, std::numeric_limits<T>::max());
        return dist(it->second.generator);
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(0.0, 1.0);
        return dist(it->second.generator);
    }
    else {
        static_assert(false, "Unsupported type for GetPositiveNumber");
    }
}

template<typename T>
T Random::GetRangeNumber(RandomDeviceID id, T start, T end) {
    auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        Log::Warn("Random::GetRangeNumber: Could not get range number for id {}, id not found! Used default device", id);
        return GetRangeNumber<T>(start, end);
    }

    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(start, end);
        return dist(it->second.generator);
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(start, end);
        return dist(it->second.generator);
    }
    else {
        static_assert(false, "Unsupported type for GetRangeNumber");
    }
}

#pragma region Explicit instantiations for common types

// ============================================================
// GetNumber (no device)
// ============================================================
template int16_t  Random::GetNumber<int16_t>();
template uint16_t Random::GetNumber<uint16_t>();
template int32_t  Random::GetNumber<int32_t>();
template uint32_t Random::GetNumber<uint32_t>();
template int64_t  Random::GetNumber<int64_t>();
template uint64_t Random::GetNumber<uint64_t>();
template float       Random::GetNumber<float>();
template double      Random::GetNumber<double>();
template long double Random::GetNumber<long double>();

// ============================================================
// GetPositiveNumber (no device)
// ============================================================
template int16_t  Random::GetPositiveNumber<int16_t>();
template uint16_t Random::GetPositiveNumber<uint16_t>();
template int32_t  Random::GetPositiveNumber<int32_t>();
template uint32_t Random::GetPositiveNumber<uint32_t>();
template int64_t  Random::GetPositiveNumber<int64_t>();
template uint64_t Random::GetPositiveNumber<uint64_t>();
template float       Random::GetPositiveNumber<float>();
template double      Random::GetPositiveNumber<double>();
template long double Random::GetPositiveNumber<long double>();

// ============================================================
// GetRangeNumber (no device)
// ============================================================
template int16_t  Random::GetRangeNumber<int16_t>(int16_t, int16_t);
template uint16_t Random::GetRangeNumber<uint16_t>(uint16_t, uint16_t);
template int32_t  Random::GetRangeNumber<int32_t>(int32_t, int32_t);
template uint32_t Random::GetRangeNumber<uint32_t>(uint32_t, uint32_t);
template int64_t  Random::GetRangeNumber<int64_t>(int64_t, int64_t);
template uint64_t Random::GetRangeNumber<uint64_t>(uint64_t, uint64_t);
template float       Random::GetRangeNumber<float>(float, float);
template double      Random::GetRangeNumber<double>(double, double);
template long double Random::GetRangeNumber<long double>(long double, long double);

// ============================================================
// GetNumber (with device)
// ============================================================
template int16_t  Random::GetNumber<int16_t>(RandomDeviceID);
template uint16_t Random::GetNumber<uint16_t>(RandomDeviceID);
template int32_t  Random::GetNumber<int32_t>(RandomDeviceID);
template uint32_t Random::GetNumber<uint32_t>(RandomDeviceID);
template int64_t  Random::GetNumber<int64_t>(RandomDeviceID);
template uint64_t Random::GetNumber<uint64_t>(RandomDeviceID);
template float       Random::GetNumber<float>(RandomDeviceID);
template double      Random::GetNumber<double>(RandomDeviceID);
template long double Random::GetNumber<long double>(RandomDeviceID);

// ============================================================
// GetPositiveNumber (with device)
// ============================================================
template int16_t  Random::GetPositiveNumber<int16_t>(RandomDeviceID);
template uint16_t Random::GetPositiveNumber<uint16_t>(RandomDeviceID);
template int32_t  Random::GetPositiveNumber<int32_t>(RandomDeviceID);
template uint32_t Random::GetPositiveNumber<uint32_t>(RandomDeviceID);
template int64_t  Random::GetPositiveNumber<int64_t>(RandomDeviceID);
template uint64_t Random::GetPositiveNumber<uint64_t>(RandomDeviceID);
template float       Random::GetPositiveNumber<float>(RandomDeviceID);
template double      Random::GetPositiveNumber<double>(RandomDeviceID);
template long double Random::GetPositiveNumber<long double>(RandomDeviceID);

// ============================================================
// GetRangeNumber (with device)
// ============================================================
template int16_t  Random::GetRangeNumber<int16_t>(RandomDeviceID, int16_t, int16_t);
template uint16_t Random::GetRangeNumber<uint16_t>(RandomDeviceID, uint16_t, uint16_t);
template int32_t  Random::GetRangeNumber<int32_t>(RandomDeviceID, int32_t, int32_t);
template uint32_t Random::GetRangeNumber<uint32_t>(RandomDeviceID, uint32_t, uint32_t);
template int64_t  Random::GetRangeNumber<int64_t>(RandomDeviceID, int64_t, int64_t);
template uint64_t Random::GetRangeNumber<uint64_t>(RandomDeviceID, uint64_t, uint64_t);
template float       Random::GetRangeNumber<float>(RandomDeviceID, float, float);
template double      Random::GetRangeNumber<double>(RandomDeviceID, double, double);
template long double Random::GetRangeNumber<long double>(RandomDeviceID, long double, long double);

#pragma endregion