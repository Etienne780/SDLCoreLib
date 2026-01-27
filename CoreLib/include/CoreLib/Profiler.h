#pragma once
#include <unordered_map>
#include <string>
#include <chrono>
#include <cstdint>
#include <limits>
#include <functional>


struct ProfileStats {
    uint64_t callCount = 0;
    double totalMs = 0.0;
    double minMs = std::numeric_limits<double>::max();
    double maxMs = 0.0;
};

class Profiler {
public:

    /**
    * @brief Begin a manual profiling section.
    *
    * Marks the start time of a named profiling section. Multiple sections can be active simultaneously.
    * @param name Name of the profiling section.
    */
    static void Begin(const char* name);

    /**
    * @brief End a manual profiling section.
    *
    * Marks the end time of a previously started profiling section and records the elapsed time.
    * Updates call count, total time, minimum, and maximum durations for the section.
    * @param name Name of the profiling section that was started with Begin().
    */
    static void End(const char* name);

    /**
    * @brief Record a finished profiling sample.
    *
    * Adds a single timing measurement (in milliseconds) to a named section without starting/stopping.
    * Useful for cases where timing is measured externally or manually calculated.
    * @param name Name of the profiling section.
    * @param ms Elapsed time in milliseconds for this sample.
    */
    static void Record(const char* name, double ms);

    /**
    * @brief Print all profiling results and reset accumulated data.
    *
    * Outputs a summary of all recorded profiling sections, including:
    *  - Number of calls
    * 
    *  - Total time
    * 
    *  - Average time
    * 
    *  - Minimum time
    * 
    *  - Maximum time
    *
    * Optionally allows a callback to append extra information to the report.
    *
    * After printing, all stored profiling data is cleared.
    *
    * @param printExtra Optional function to append extra information to the output.
    */
    static void PrintAndReset(std::function<void(std::string& outMsg)> printExtra = nullptr);

    /**
    * @brief Reset all profiler data.
    *
    * Clears all stored statistics, active sections, and insertion order.
    * After calling this, the profiler is in a clean state with no recorded data.
    */
    static void Reset();

private:
    struct ActiveSection {
        const char* name;
        std::chrono::high_resolution_clock::time_point start;
    };

    static inline std::unordered_map<const char*, ProfileStats> s_stats;
    static inline std::vector<ActiveSection> s_activeStack;

    static inline std::unordered_map<const char*, uint32_t> s_order;
    static inline uint32_t s_nextOrder = 0;

    // Store nesting level for printing
    static inline std::unordered_map<const char*, uint32_t> s_maxNesting;

    static void RegisterOrderIfNeeded(const char* name);
};

// RAII-based scope profiler
class ProfilerScope {
public:
    explicit ProfilerScope(const char* name);
    ~ProfilerScope();

private:
    const char* m_name;
    std::chrono::high_resolution_clock::time_point m_start;
};