#pragma once
#include <vector>
#include <type_traits>
#include <initializer_list>

#include "FormatUtils.h"

enum Operation {
    AND = 0,
    OR
};

template<typename T>
class MatchSet {
public:
    template<typename... Args>
    MatchSet(Operation op, Args&&... args)
        : m_operation(op), m_values{ std::forward<Args>(args)... } {
    }

    MatchSet(Operation op, std::initializer_list<T> values)
        : m_operation(op), m_values(values) {
    }

    bool operator>(const T& other) const { return evaluate(other, std::less<T>()); }
    bool operator<(const T& other) const { return evaluate(other, std::greater<T>()); }
    bool operator>=(const T& other) const { return evaluate(other, std::less_equal<T>()); }
    bool operator<=(const T& other) const { return evaluate(other, std::greater_equal<T>()); }
    bool operator==(const T& other) const { return evaluate(other, std::equal_to<T>()); }
    bool operator!=(const T& other) const { return evaluate(other, std::not_equal_to<T>()); }

    // Ermöglicht Zugriff von freien Operatoren
    template<typename Compare>
    bool evaluate(const T& other, Compare cmp) const {
        if (m_values.empty())
            return false;

        switch (m_operation) {
        case AND: {
            for (const auto& v : m_values) {
                if (!cmp(other, v))
                    return false;
            }
            return true;
        }
        case OR: {
            for (const auto& v : m_values) {
                if (cmp(other, v))
                    return true;
            }
            return false;
        }
        }
    }

private:
    Operation m_operation;
    std::vector<T> m_values;
};

template<typename T>
bool operator<(const T& lhs, const MatchSet<T>& rhs) {
    return rhs.evaluate(lhs, std::less<T>());
}

template<typename T>
bool operator>(const T& lhs, const MatchSet<T>& rhs) {
    return rhs.evaluate(lhs, std::greater<T>());
}

template<typename T>
bool operator<=(const T& lhs, const MatchSet<T>& rhs) {
    return rhs.evaluate(lhs, std::less_equal<T>());
}

template<typename T>
bool operator>=(const T& lhs, const MatchSet<T>& rhs) {
    return rhs.evaluate(lhs, std::greater_equal<T>());
}

template<typename T>
bool operator==(const T& lhs, const MatchSet<T>& rhs) {
    return rhs.evaluate(lhs, std::equal_to<T>());
}

template<typename T>
bool operator!=(const T& lhs, const MatchSet<T>& rhs) {
    return rhs.evaluate(lhs, std::not_equal_to<T>());
}