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
    * Log::Subscribe([](const std::string& msg) {
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
    static bool IsLevelSelected(Level level);

    /**
     * @brief Enables specific log levels.
     * All previously set levels will be cleared.
     * Usage: Log::SetLevel(Log::levelError, Log::levelDebug);
     */
    template<typename... Args>
    static void SetLevel(Args&&... args) {
        static_assert((std::is_same_v<std::decay_t<Args>, Log::Level> && ...), "All arguments must be of type Log::Level.");

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
     * @brief Logs an error message with format string and arguments.
     * Requires levelError to be enabled.
     */
    template<typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>
        >>
        static void Error(T&& format, Args&&... args) {
        if (!m_levelError) return;
        m_print(levelError, "[ERROR]: " + FormatUtils::formatString(std::forward<T>(format), std::forward<Args>(args)...));
    }

    /// Logs a raw error message composed from arguments, without format string.
    template<typename... Args>
    static void Error(Args&&... args) {
        if (!m_levelError) return;
        m_print(levelError, "[ERROR]: " + FormatUtils::joinArgs(std::forward<Args>(args)...));
    }

    /// Logs a formatted warning message.
    template<typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>
        >>
        static void Warn(T&& format, Args&&... args) {
        if (!m_levelWarning) return;
        m_print(levelWarning, "[WARNING]: " + FormatUtils::formatString(std::forward<T>(format), std::forward<Args>(args)...));
    }

    /// Logs a raw warning message.
    template<typename... Args>
    static void Warn(Args&&... args) {
        if (!m_levelWarning) return;
        m_print(levelWarning, "[WARNING]: " + FormatUtils::joinArgs(std::forward<Args>(args)...));
    }

    /// Logs an informational message using format string.
    template<typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>
        >>
        static void Info(T&& format, Args&&... args) {
        if (!m_levelInfo) return;
        m_print(levelInfo, "[INFO]: " + FormatUtils::formatString(std::forward<T>(format), std::forward<Args>(args)...));
    }

    /// Logs an informational message with argument joining.
    template<typename... Args>
    static void Info(Args&&... args) {
        if (!m_levelInfo) return;
        m_print(levelInfo, "[INFO]: " + FormatUtils::joinArgs(std::forward<Args>(args)...));
    }

    /// Logs a debug message with formatting.
    template<typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>
        >>
        static void Debug(T&& format, Args&&... args) {
        if (!m_levelDebug) return;
        m_print(levelDebug, "[Debug]: " + FormatUtils::formatString(std::forward<T>(format), std::forward<Args>(args)...));
    }

    /// Logs a debug message without formatting.
    template<typename... Args>
    static void Debug(Args&&... args) {
        if (!m_levelDebug) return;
        m_print(levelDebug, "[Debug]: " + FormatUtils::joinArgs(std::forward<Args>(args)...));
    }

    /// Prints a formatted string without log level prefix.
    template<typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>
        >>
        static void Print(T&& format, Args&&... args) {
        m_print(levelInfo, FormatUtils::formatString(std::forward<T>(format), std::forward<Args>(args)...));
    }

    /// Prints joined arguments without log level prefix.
    template<typename... Args>
    static void Print(Args&&... args) {
        m_print(levelInfo, FormatUtils::joinArgs(std::forward<Args>(args)...));
    }

    /// Conditionally prints a formatted string based on log level.
    template<typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>
        >>
        static void Print(Level level, T&& format, Args&&... args) {
        if (!IsLevelSelected(level)) return;
        m_print(level, FormatUtils::formatString(std::forward<T>(format), std::forward<Args>(args)...));
    }

    /// Conditionally prints joined arguments based on log level.
    template<typename... Args>
    static void Print(Level level, Args&&... args) {
        if (!IsLevelSelected(level)) return;
        m_print(level, FormatUtils::joinArgs(std::forward<Args>(args)...));
    }

    /// Returns a formatted string for reuse (without printing).
    template<typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char*>
        >>
        static std::string GetFormattedString(T&& format, Args&&... args) {
        return FormatUtils::formatString(std::forward<T>(format), std::forward<Args>(args)...);
    }

    /// Returns joined arguments as string.
    template<typename... Args>
    static std::string GetFormattedString(Args&&... args) {
        return FormatUtils::joinArgs(std::forward<Args>(args)...);
    }

private:
    Log() = delete;

    // Flags to track active log levels.
    static bool m_levelError;
    static bool m_levelWarning;
    static bool m_levelInfo;
    static bool m_levelDebug;

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

    /// Helper for formatted print with extra arguments.
    template<typename... Args>
    static void m_print(const Level& logLevel, const std::string& message, Args&&... args) {
        m_print(logLevel, FormatUtils::formatString(0, message, std::forward<Args>(args)...));
    }

    class AsyncLogger {
    public:
        AsyncLogger(const std::string& filename);
        ~AsyncLogger();
        void Log(const std::string& message);

    private:
        //void ProcessQueue();

        class Impl;
        std::unique_ptr<Impl> pImpl;
    };
};