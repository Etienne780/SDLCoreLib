#pragma once
#include <xhash>

template<typename ID_TYPE, ID_TYPE INVALID_ID, typename TAG>
class CoreID {
public:
	ID_TYPE value = INVALID_ID;

	/*
	* @brief The result of value == INVALID_ID
	* @return true on invalid value
	*/
	bool IsInvalid() const {
		return value == INVALID_ID;
	}

	/*
	* @brief Sets the id to an invalid value
	*/
	void SetInvalid() {
		value = INVALID_ID;
	}

	/*
	* @brief Converts the ID to a string
	*/
	std::string ToString() const {
		return IsInvalid() ? "InvalidID" : FormatUtils::toString(value);
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

	explicit constexpr operator bool() const { return value != 0; }
};

namespace std {
	template<typename ID_TYPE, ID_TYPE INVALID_ID, typename TAG>
	struct hash<CoreID<ID_TYPE, INVALID_ID, TAG>> {
		size_t operator()(const CoreID<ID_TYPE, INVALID_ID, TAG>& id) const noexcept {
			return std::hash<uint32_t>{}(id.value);
		}
	};
}