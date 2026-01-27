#include "Profiler.h"
#include <iostream>
#include <algorithm>

void Profiler::Begin(const char* name) {
    RegisterOrderIfNeeded(name);
    s_activeStack.push_back({ name, std::chrono::high_resolution_clock::now() });
}

void Profiler::End(const char* name) {
    if (s_activeStack.empty()) return;

    ActiveSection top = s_activeStack.back();
    s_activeStack.pop_back();

    if (top.name != name) {
        std::cerr << "[Profiler] Warning: mismatched End() call. Expected " << top.name
            << " but got " << name << "\n";
        return;
    }

    double ms = std::chrono::duration<double, std::milli>(
        std::chrono::high_resolution_clock::now() - top.start
    ).count();

    ProfileStats& stats = s_stats[name];
    stats.callCount++;
    stats.totalMs += ms;
    stats.minMs = std::min(stats.minMs, ms);
    stats.maxMs = std::max(stats.maxMs, ms);

    // save max nesting for indenting
    s_maxNesting[name] = std::max(s_maxNesting[name], static_cast<uint32_t>(s_activeStack.size()));
}

void Profiler::Record(const char* name, double ms) {
    RegisterOrderIfNeeded(name);

    ProfileStats& stats = s_stats[name];
    stats.callCount++;
    stats.totalMs += ms;
    stats.minMs = std::min(stats.minMs, ms);
    stats.maxMs = std::max(stats.maxMs, ms);
}

void Profiler::PrintAndReset(std::function<void(std::string& outMsg)> printExtra) {
    std::cout << "==== Profiler Report ====\n";

    if (printExtra) {
        std::string msg;
        printExtra(msg);
        std::cout << msg << "\n";
    }

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
        uint32_t indent = s_maxNesting[name];
        std::string indentStr(indent * 2, ' '); // 2 spaces per nesting level;
        std::string indentHeaderStr = indentStr;
        if (indent > 0) {
            indentHeaderStr += "|-";
            indentStr += '|';
        }

        std::cout
            << indentStr << "\n"
            << indentHeaderStr << name << ":\n"
            << indentStr << "  Calls: " << stats.callCount << "\n"
            << indentStr << "  Total: " << stats.totalMs << " ms\n"
            << indentStr << "  Avg:   " << avg << " ms\n"
            << indentStr << "  Min:   " << stats.minMs << " ms\n"
            << indentStr << "  Max:   " << stats.maxMs << " ms\n";
    }

    Reset();
}

void Profiler::Reset() {
    s_stats.clear();
    s_activeStack.clear();
    s_order.clear();
    s_maxNesting.clear();
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