#pragma once
#include <unordered_map>
#include <string>
#include <chrono>
#include <cstdint>
#include <limits>

namespace SDLCore::Debug {

    struct ProfileStats {
        uint64_t callCount = 0;
        double totalMs = 0.0;
        double minMs = std::numeric_limits<double>::max();
        double maxMs = 0.0;
    };

    class Profiler {
    public:
        // Begin a manual profiling section
        static void Begin(const char* name);

        // End a manual profiling section
        static void End(const char* name);

        // Record a finished profiling sample
        static void Record(const char* name, double ms);

        // Print all profiling results and clear accumulated data
        static void PrintAndReset();

        static void Reset();

    private:
        static inline std::unordered_map<const char*, ProfileStats> s_stats;
        static inline std::unordered_map<const char*, std::chrono::high_resolution_clock::time_point> s_active;

        static inline std::unordered_map<const char*, uint32_t> s_order;
        static inline uint32_t s_nextOrder = 0;

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

}
