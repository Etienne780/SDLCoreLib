#pragma once

inline constexpr unsigned int SDLCORE_INVALID_ID = std::numeric_limits<unsigned int>::max();

namespace SDLCore {

	typedef int SDLResult;

	template<typename Tag>
	struct SDLCoreID {
		unsigned int value = SDLCORE_INVALID_ID;

		constexpr SDLCoreID() = default;
		explicit constexpr SDLCoreID(unsigned int v) : value(v) {}

		constexpr bool operator==(const SDLCoreID& other) const { return value == other.value; }
		constexpr bool operator!=(const SDLCoreID& other) const { return value != other.value; }
		constexpr bool operator<(const SDLCoreID& other) const { return value < other.value; }
		constexpr bool operator>(const SDLCoreID& other) const { return value > other.value; }
		constexpr bool operator<=(const SDLCoreID& other) const { return value <= other.value; }
		constexpr bool operator>=(const SDLCoreID& other) const { return value >= other.value; }

		explicit constexpr operator bool() const { return value != 0; }
	};

	struct WindowTag {};
	using WindowID = SDLCoreID<WindowTag>;

}