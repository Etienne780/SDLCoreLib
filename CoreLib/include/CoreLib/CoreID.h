#pragma once
#include <functional>
#include <type_traits>
#include <string>

template<typename ID_TYPE, ID_TYPE INVALID_ID, typename TAG>
class CoreID {
    static_assert(std::is_integral_v<ID_TYPE>, "ID_TYPE must be an integral type");

public:
    ID_TYPE value = INVALID_ID;

    bool IsInvalid() const {
        return value == INVALID_ID;
    }

    void SetInvalid() {
        value = INVALID_ID;
    }

    std::string ToString() const {
        return IsInvalid() ? "InvalidID" : std::to_string(value);
    }

    constexpr CoreID() = default;
    explicit constexpr CoreID(ID_TYPE v) : value(v) {}

    constexpr bool operator==(ID_TYPE other) const { return value == other; }
    constexpr bool operator!=(ID_TYPE other) const { return value != other; }
    constexpr bool operator<(ID_TYPE other) const { return value < other; }
    constexpr bool operator>(ID_TYPE other) const { return value > other; }
    constexpr bool operator<=(ID_TYPE other) const { return value <= other; }
    constexpr bool operator>=(ID_TYPE other) const { return value >= other; }

    constexpr bool operator==(const CoreID& other) const { return value == other.value; }
    constexpr bool operator!=(const CoreID& other) const { return value != other.value; }
    constexpr bool operator<(const CoreID& other) const { return value < other.value; }
    constexpr bool operator>(const CoreID& other) const { return value > other.value; }
    constexpr bool operator<=(const CoreID& other) const { return value <= other.value; }
    constexpr bool operator>=(const CoreID& other) const { return value >= other.value; }

    explicit constexpr operator bool() const { return !IsInvalid(); }
};

namespace std {
    template<typename ID_TYPE, ID_TYPE INVALID_ID, typename TAG>
    struct hash<CoreID<ID_TYPE, INVALID_ID, TAG>> {
        size_t operator()(const CoreID<ID_TYPE, INVALID_ID, TAG>& id) const noexcept {
            return std::hash<ID_TYPE>{}(id.value);
        }
    };
}
