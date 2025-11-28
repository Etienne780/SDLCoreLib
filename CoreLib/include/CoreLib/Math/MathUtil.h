#pragma once
#include <type_traits>

class MathUtil {
public:
	template<typename A, typename B, typename T>
	static auto Lerp(A a, B b, T t) {
		static_assert(
			std::is_arithmetic<A>::value &&
			std::is_arithmetic<B>::value &&
			std::is_arithmetic<T>::value,
			"Lerp requires arithmetic types"
			);

		using Result = std::common_type_t<A, B, T>;
		return static_cast<Result>(a) + (static_cast<Result>(b) - static_cast<Result>(a)) * static_cast<Result>(t);
	}

	template<typename A, typename B>
	static auto Min(A a, B b) {
		static_assert(std::is_arithmetic<A>::value && std::is_arithmetic<B>::value,
			"Min requires arithmetic types");

		using Result = std::common_type_t<A, B>;
		return (a > b) ? static_cast<Result>(b) : static_cast<Result>(a);
	}

	template<typename A, typename B>
	static auto Max(A a, B b) {
		static_assert(std::is_arithmetic<A>::value && std::is_arithmetic<B>::value,
			"Max requires arithmetic types");

		using Result = std::common_type_t<A, B>;
		return (a > b) ? static_cast<Result>(a) : static_cast<Result>(b);
	}

	template<typename A, typename B, typename C>
	static void Clamp(A& a, B min, C max) {
		static_assert(std::is_arithmetic<A>::value &&
			std::is_arithmetic<B>::value &&
			std::is_arithmetic<C>::value,
			"Clamp requires arithmetic types");

		using Result = std::common_type_t<A, B, C>;
		Result minVal = static_cast<Result>(min);
		Result maxVal = static_cast<Result>(max);

		if (minVal > maxVal)
			std::swap(minVal, maxVal);

		if (a < minVal) a = static_cast<A>(minVal);
		if (a > maxVal) a = static_cast<A>(maxVal);
	}

	template<typename A, typename B, typename C>
	static auto ClampValue(A a, B min, C max) {
		static_assert(std::is_arithmetic_v<A> &&
			std::is_arithmetic_v<B> &&
			std::is_arithmetic_v<C>,
			"Clamp requires arithmetic types");

		using Result = std::common_type_t<A, B, C>;
		Result minVal = static_cast<Result>(min);
		Result maxVal = static_cast<Result>(max);

		if (minVal > maxVal)
			std::swap(minVal, maxVal);

		if (a < minVal) return static_cast<A>(minVal);
		if (a > maxVal) return static_cast<A>(maxVal);
		return a;
	}

private:
	MathUtil();
};