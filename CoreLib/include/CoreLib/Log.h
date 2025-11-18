#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include "FormatUtils.h"

/**
 * @brief Provides basic logging functionality with support for multiple log levels.
 *
 * This class allows logging messages to the console with various log levels
 * (Error, Warning, Info, Debug) and formatting support similar to fmt or Python-style
 * placeholders. Logging levels can be enabled or disabled at runtime.
 */
class Log {
private:
    class AsyncLogger;
    
public:
    // Logging severity levels.
    enum Level : short {
        levelError,
        levelWarning,
        levelInfo,
        levelDebug
    };

    using LogCallback = std::function<void(Level LogLevel, const std::string&)>;
    using SubscriberID = size_t;
    /*
    * Log::Subscribe([](Level LogLevel, const std::string& msg) {
    *   std::cout << "(FILE) " << msg << std::endl;
    * });
    */
    
    /*
    * @brief Subscribes a callback function to the logging system.
    *        The callback will be invoked whenever a new log message is generated.
    * @note  The returned SubscriberID must be used to unsubscribe later.
    *        It is the caller's responsibility to unsubscribe, typically in the destructor
    *        of the subscribing class, to avoid dangling callbacks.
    * @param callback The function to be called with each log message.
    * @return A unique SubscriberID that can be used to unsubscribe.
    */
    static SubscriberID Subscribe(LogCallback callback);

    /*
    * @brief Removes a previously registered log callback from the logging system.
    * @param id The SubscriberID returned by Subscribe.
    * @note  Must be called before the subscriber is destroyed to prevent invalid access.
    */
    static void Unsubscribe(SubscriberID id);

    /// Clears the current output log (console).
    static void ClearLog();

    /// Checks whether a specific log level is currently enabled.
    static bool IsLogLevelEnabled(Level level);

    /**
    * @brief Enables specific log levels.
    * All previously set levels will be cleared.
    * Usage: Log::SetLevel(Log::levelError, Log::levelDebug);
    */
    template<typename... Args>
    static void SetLevel(Args&&... args) {
        static_assert((std::is_same_v<std::decay_t<Args>, Log::Level> && ...), 
            "All arguments must be of type Log::Level.");

        m_levelError = false;
        m_levelWarning = false;
        m_levelInfo = false;
        m_levelDebug = false;

        for (auto a : { args... }) {
            switch (a) {
            case Log::levelError:   m_levelError = true;   break;
            case Log::levelWarning: m_levelWarning = true; break;
            case Log::levelInfo:    m_levelInfo = true;    break;
            case Log::levelDebug:   m_levelDebug = true;   break;
            }
        }
    }

    /**
    * @brief Saves the logs to a given path
    * @param path needs to be so path/name. no extension
    */
    static void SaveLogs(const std::string& path);

    // ------------------------- Logging Methods -------------------------

    /**
    * @brief Logs an error message with optional formatting.
    * @tparam T Type of the format string or first argument (can be string, or const char*).
    * @tparam Args Additional arguments to format into the message.
    * @param format Format string or first argument.
    * @param args Variadic arguments for formatting.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static void Error(T&& format, Args&&... args) {
        m_printWithPrefix(m_prefixError, levelError, std::forward<T>(format), std::forward<Args>(args)...);
    }

    /**
    * @brief Logs an error message without formatting.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param args Arguments to print.
    */
    template<typename... Args>
    static void Error(Args&&... args) {
        m_printWithPrefix(m_prefixError, levelError, std::forward<Args>(args)...);
    }

    /**
    * @brief Logs a warning message with optional formatting.
    * @tparam T Type of the format string or first argument (can be string, or const char*).
    * @tparam Args Additional arguments to format into the message.
    * @param format Format string or first argument.
    * @param args Variadic arguments for formatting.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static void Warn(T&& format, Args&&... args) {
        m_printWithPrefix(m_prefixWarn, levelWarning, std::forward<T>(format), std::forward<Args>(args)...);
    }

    /**
    * @brief Logs a warning message without formatting.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param args Arguments to print.
    */
    template<typename... Args>
    static void Warn(Args&&... args) {
        m_printWithPrefix(m_prefixWarn, levelWarning, std::forward<Args>(args)...);
    }

    /**
    * @brief Logs an informational message with optional formatting.
    * @tparam T Type of the format string or first argument (can be string, or const char*).
    * @tparam Args Additional arguments to format into the message.
    * @param format Format string or first argument.
    * @param args Variadic arguments for formatting.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static void Info(T&& format, Args&&... args) {
        m_printWithPrefix(m_prefixInfo, levelInfo, std::forward<T>(format), std::forward<Args>(args)...);
    }

    /**
    * @brief Logs an informational message without formatting.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param args Arguments to print.
    */
    template<typename... Args>
    static void Info(Args&&... args) {
        m_printWithPrefix(m_prefixInfo, levelInfo, std::forward<Args>(args)...);
    }

    /**
    * @brief Logs a debug message with optional formatting.
    * @tparam T Type of the format string or first argument (can be string, or const char*).
    * @tparam Args Additional arguments to format into the message.
    * @param format Format string or first argument.
    * @param args Variadic arguments for formatting.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static void Debug(T&& format, Args&&... args) {
        m_printWithPrefix(m_prefixDebug, levelDebug, std::forward<T>(format), std::forward<Args>(args)...);
    }

    /**
    * @brief Logs a debug message without formatting.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param args Arguments to print.
    */
    template<typename... Args>
    static void Debug(Args&&... args) {
        m_printWithPrefix(m_prefixDebug, levelDebug, std::forward<Args>(args)...);
    }
    
    /**
    * @brief Generic print function with formatting.
    * @tparam T Type of the format string or first argument (can be string, or const char*).
    * @tparam Args Additional arguments to format into the message.
    * @param format Format string or first argument.
    * @param args Variadic arguments for formatting.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static void Print(T&& format, Args&&... args) {
        m_printWithPrefix("", levelInfo, std::forward<T>(format), std::forward<Args>(args)...);
    }

    /**
    * @brief Generic print function without formatting.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param args Arguments to print.
    */
    template<typename... Args>
    static void Print(Args&&... args) {
        m_printWithPrefix("", levelInfo, std::forward<Args>(args)...);
    }

    /**
    * @brief Generic print function with formatting to a specific log level.
    * @tparam T Type of the format string or first argument (can be string, or const char*).
    * @tparam Args Additional arguments to format into the message.
    * @param level Log level to print the message to.
    * @param format Format string or first argument.
    * @param args Variadic arguments for formatting.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static void Print(Level level, T&& format, Args&&... args) {
        m_printWithPrefix("", level, std::forward<T>(format), std::forward<Args>(args)...);
    }

    /**
    * @brief Generic print function to a specific log level without formatting.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param level Log level to print the message to.
    * @param args Arguments to print.
    */
    template<typename... Args>
    static void Print(Level level, Args&&... args) {
        m_printWithPrefix("", level, std::forward<Args>(args)...);
    }

    /**
    * @brief Returns a formatted string for reuse (without printing) using a format string and arguments.
    * @tparam T Type of the format string or first argument (can be string, or const char*).
    * @tparam Args Additional arguments to format into the string.
    * @param format Format string or first argument.
    * @param args Variadic arguments for formatting.
    * @return A string containing the formatted result.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static std::string GetFormattedString(T&& format, Args&&... args) {
        // check if first arg is nullptr
        auto safe_format = [&]() -> std::string {
            if constexpr (std::is_same_v<std::decay_t<T>, std::nullptr_t>) {
                return "nullptr";
            }
            else if constexpr (std::is_convertible_v<T, const char*>) {
                return format ? std::string(format) : "nullptr";
            }
            else {
                return FormatUtils::toString(std::forward<T>(format));
            }
        }();

        return FormatUtils::formatString(safe_format, std::forward<Args>(args)...);
    }

    /**
    * @brief Returns a joined string from multiple arguments without any formatting.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param args Arguments to convert and join.
    * @return A string with all arguments joined (similar to comma-separated or concatenated values).
    */
    template<typename... Args>
    static std::string GetFormattedString(Args&&... args) {
        return FormatUtils::joinArgs(std::forward<Args>(args)...);
    }

private:
    Log() = delete;

    // Flags to track active log levels.
    static inline bool m_levelError = true;
    static inline bool m_levelWarning = true;
    static inline bool m_levelInfo = true;
    static inline bool m_levelDebug = true;

    static constexpr const char* m_prefixError =    "[ERROR]: ";
    static constexpr const char* m_prefixWarn =     "[WARNING]: ";
    static constexpr const char* m_prefixInfo =     "[INFO]: ";
    static constexpr const char* m_prefixDebug =    "[DEBUG]: ";

    static bool m_saveLogs;
    static std::unique_ptr<AsyncLogger> m_asyncLogger;
    struct Subscriber {
        SubscriberID id;
        LogCallback callback;
    };

    static std::vector<Subscriber> m_subscribers;
    static SubscriberID m_nextId;

    /// Low-level printer implementation (console output).
    static void m_print(const Level& logLevel, const std::string& message);

    /**
    * @brief Internal helper to print a message with a prefix and formatting, handling nullptr as first argument safely.
    * @tparam T Type of the first argument (format string or other type).
    * @tparam Args Additional arguments for formatting.
    * @param prefix Prefix string (e.g., "[INFO]: ").
    * @param level Log level of the message.
    * @param format First argument or format string.
    * @param args Variadic arguments for formatting.
    */
    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>>>
    static void m_printWithPrefix(const char* prefix, Level level, T&& format, Args&&... args) {
        if (!IsLogLevelEnabled(level)) return;
        m_print(level, std::string(prefix) + GetFormattedString(format, std::forward<Args>(args)...));
    }

    /**
    * @brief Internal helper to print a message with a prefix, joining all arguments.
    * @tparam Args Variadic arguments of any type, converted to string and joined as a single message separated by ','.
    * @param prefix Prefix string (e.g., "[INFO]: ").
    * @param level Log level of the message.
    * @param args Arguments to join into a single message string.
    */
    template<typename... Args>
    static void m_printWithPrefix(const char* prefix, Level level, Args&&... args) {
        if (!IsLogLevelEnabled(level)) return;
        m_print(level, std::string(prefix) + GetFormattedString(std::forward<Args>(args)...));
    }

    class AsyncLogger {
    public:
        AsyncLogger(const std::string& filename);
        ~AsyncLogger();
        void Log(const std::string& message);

    private:

        class Impl;
        std::unique_ptr<Impl> pImpl;
    };
};

template<>
static inline std::string FormatUtils::toString<Log::Level>(Log::Level level) {
    switch (level)
    {
    case Log::levelError:   return "Error";
    case Log::levelWarning: return "Warning";
    case Log::levelInfo:    return "Info";
    case Log::levelDebug:   return "Debug";
    default:                return "UNKNOWN";
    }
}