#include "Profiler.h"
#include "SDLCoreTime.h"
#include <iostream>

namespace SDLCore::Debug {

    void Profiler::Begin(const char* name) {
        s_active[name] = std::chrono::high_resolution_clock::now();
    }

    void Profiler::End(const char* name) {
        auto it = s_active.find(name);
        if (it == s_active.end())
            return;

        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - it->second).count();

        ProfileStats& stats = s_stats[name];
        stats.callCount++;
        stats.totalMs += ms;
        stats.minMs = std::min(stats.minMs, ms);
        stats.maxMs = std::max(stats.maxMs, ms);

        s_active.erase(it);
    }

    void Profiler::Record(const char* name, double ms) {
        ProfileStats& stats = s_stats[name];
        stats.callCount++;
        stats.totalMs += ms;
        stats.minMs = std::min(stats.minMs, ms);
        stats.maxMs = std::max(stats.maxMs, ms);
    }

    void Profiler::PrintAndReset() {
        std::cout << "==== Profiler Report ====\n";
        std::cout << "FrameRate: " << Time::GetFrameRate() << " FPS\n\n";

        for (const auto& [name, stats] : s_stats) {
            double avg = stats.totalMs / static_cast<double>(stats.callCount);

            std::cout
                << name << ":\n"
                << "  Calls: " << stats.callCount << "\n"
                << "  Total: " << stats.totalMs << " ms\n"
                << "  Avg:   " << avg << " ms\n"
                << "  Min:   " << stats.minMs << " ms\n"
                << "  Max:   " << stats.maxMs << " ms\n\n";
        }

        Reset();
    }

    void Profiler::Reset() {
        s_stats.clear();
        s_active.clear();
    }

    ProfilerScope::ProfilerScope(const char* name)
        : m_name(name),
        m_start(std::chrono::high_resolution_clock::now()) {
    }

    ProfilerScope::~ProfilerScope() {
        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - m_start).count();

        Profiler::Record(m_name, ms);
    }

}
