#include "Profiler.h"
#include "SDLCoreTime.h"
#include <iostream>
#include <algorithm>

namespace SDLCore::Debug {

    void Profiler::Begin(const char* name) {
        RegisterOrderIfNeeded(name);
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
        RegisterOrderIfNeeded(name);

        ProfileStats& stats = s_stats[name];
        stats.callCount++;
        stats.totalMs += ms;
        stats.minMs = std::min(stats.minMs, ms);
        stats.maxMs = std::max(stats.maxMs, ms);
    }

    void Profiler::PrintAndReset() {
        std::cout << "==== Profiler Report ====\n";
        std::cout << "FrameRate: " << Time::GetFrameRate() << " FPS\n\n";

        // Collect entries into a vector for sorting
        std::vector<std::pair<const char*, ProfileStats>> entries;
        entries.reserve(s_stats.size());

        for (const auto& it : s_stats) {
            entries.push_back(it);
        }

        // Sort by insertion order
        std::sort(entries.begin(), entries.end(),
            [](const auto& a, const auto& b) {
                return s_order[a.first] < s_order[b.first];
            });

        for (const auto& [name, stats] : entries) {
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
        s_order.clear();
        s_nextOrder = 0;
    }

    void Profiler::RegisterOrderIfNeeded(const char* name) {
        if (s_order.find(name) == s_order.end()) {
            s_order[name] = s_nextOrder++;
        }
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
