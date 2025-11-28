#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <type_traits>
#include <algorithm>
#include <cctype>
#include <optional>
#include <cstdlib>
#include <cmath>

class FormatUtils {
public:
    /*
    The toString method can be extended to support custom data types.
    By providing a template specialization, FormatUtils::toString can
    handle additional user-defined types.

    The Log class internally relies on FormatUtils::toString to convert
    its input parameters, so custom specializations are automatically
    used there as well.

    Example:
    template<>
    static inline std::string FormatUtils::toString<Vector2>(Vector2 vec) {
        return vec.ToString();
    }
    */

    template<typename T>
    static std::optional<T> stringToNumber(const std::string& str) {
        static_assert(std::is_arithmetic<T>::value, "stringToNumber requires arithmetic types");
        try {
            if constexpr (std::is_integral<T>::value) {
                return static_cast<T>(std::stoll(str));
            }
            else if constexpr (std::is_floating_point<T>::value) {
                return static_cast<T>(std::stold(str));
            }
        }
        catch (const std::exception&) {
            return std::nullopt;
        }
    }

    static std::string removeSpaces(std::string str) {
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        return str;
    }

    static std::string replaceChar(std::string str, char cToReplace, char c) {
        std::replace(str.begin(), str.end(), cToReplace, c);
        return str;
    }

    template<typename T>
    static std::string trimTrailingZeros(T value) {
        static_assert(std::is_arithmetic<T>::value, "trimTrailingZeros requires arithmetic types");
        std::string str = std::to_string(value);
        if (str.find('.') == std::string::npos)
            return str;

        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (!str.empty() && str.back() == '.') {
            str.pop_back();
        }
        return str;
    }

    static std::string toUpperCase(std::string string) {
        std::transform(string.begin(), string.end(), string.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return string;
    }

    static std::string toLowerCase(std::string string) {
        std::transform(string.begin(), string.end(), string.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return string;
    }

    // Array overload
    template<typename T, std::size_t N>
    static std::string arrayToString(const T(&arr)[N]) {
        std::ostringstream result;
        result << "[";
        for (std::size_t i = 0; i < N; ++i) {
            result << toString(arr[i]);
            if (i != N - 1)
                result << ", ";
        }
        result << "]";
        return result.str();
    }

    // Pair overload
    template<typename T1, typename T2>
    static std::string toString(const std::pair<T1, T2>& p) {
        return "{" + toString(p.first) + ": " + toString(p.second) + "}";
    }

    // C-style string
    template<std::size_t N>
    static std::string toString(const char(&arr)[N]) {
        return std::string(arr);
    }

    // Base toString for generic types
    template<typename T>
    static std::string toString(T value) {
        if constexpr (std::is_same_v<T, bool>) {
            return (value) ? "true" : "false";
        }
        else if constexpr (std::is_arithmetic_v<T>) {
            return trimTrailingZeros(value);
        }
        else if constexpr (std::is_same_v<T, nullptr_t>) {
            return "nullptr";
        }
        else if constexpr (std::is_same_v<T, const char*>) {
            return value ? std::string(value) : "nullptr";
        }
        else if constexpr (std::is_pointer_v<T>) {
            if (!value) return "nullptr";
            std::ostringstream oss;
            oss << static_cast<const void*>(value);
            return oss.str();
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return value;
        }
        else if constexpr (is_iterable<T>::value) { // Generic container support
            std::ostringstream oss;
            oss << "[";
            bool first = true;
            for (const auto& item : value) {
                if (!first) oss << ", ";
                oss << toString(item);
                first = false;
            }
            oss << "]";
            return oss.str();
        }
        else {
            static_assert(always_false<T>::value, "Unsupported type for toString");
        }
    }

    template<typename... Args>
    static std::string joinArgs(Args&&... args) {
        return joinArgsImpl(", ", std::forward<Args>(args)...);
    }

    template<typename... Args>
    static std::string joinArgsSeperator(const std::string& separator, Args&&... args) {
        return joinArgsImpl(separator, std::forward<Args>(args)...);
    }

    static std::string formatString(const std::string& format) {
        return format;
    }

    template<typename T, typename... Args>
    static std::string formatString(const std::string& format, T&& value, Args&&... args) {
        return formatStringImpl(0, format, std::forward<T>(value), std::forward<Args>(args)...);
    }

private:
    FormatUtils() {}

    template<typename T>
    struct always_false : std::false_type {};

    // Trait to detect iterable containers
    template<typename T, typename = void>
    struct is_iterable : std::false_type {};

    template<typename T>
    struct is_iterable<T, std::void_t<
        decltype(std::begin(std::declval<T>())),
        decltype(std::end(std::declval<T>()))
        >> : std::true_type {};

    template<typename... Args>
    static std::string joinArgsImpl(const std::string& separator, Args&&... args) {
        std::vector<std::string> strings;
        (strings.push_back(toString(std::forward<Args>(args))), ...);

        std::ostringstream result;
        for (size_t i = 0; i < strings.size(); ++i) {
            result << strings[i];
            if (i != strings.size() - 1)
                result << separator;
        }
        return result.str();
    }

    /// Internal implementation of formatString with recursion depth.
    template<typename T, typename... Args>
    static std::string formatStringImpl(int depth, const std::string& format, T&& value, Args&&... args) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) {
            if (depth == 0) {
                // No placeholders found: fallback to concatenated args.
                return FormatUtils::joinArgs(format, std::forward<T>(value), std::forward<Args>(args)...);
            }
            return format;
        }

        std::string result = format.substr(0, pos)
            + FormatUtils::toString(std::forward<T>(value))
            + formatStringImpl(1, format.substr(pos + 2), std::forward<Args>(args)...);
        return result;
    }

    static std::string formatStringImpl(int, const std::string& format) {
        return format;
    }
};