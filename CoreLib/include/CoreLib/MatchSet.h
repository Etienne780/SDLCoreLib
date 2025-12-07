#pragma once
#include <vector>
#include <type_traits>
#include <initializer_list>
#include "FormatUtils.h"

/**
* @brief Enum representing logical operations for evaluating sets of values.
*/
enum Operation {
    AND = 0,  /**< All comparisons must be true for a positive evaluation */
    OR        /**< Any single comparison being true is sufficient */
};

/**
* @brief Template class to represent a set of values and compare them to a target value using a logical operation.
*
* This class allows evaluating whether a given value matches a set of values according to the specified
* operation (AND / OR). Supports all standard comparison operators via operator overloading.
*
* @tparam T Type of the values stored in the set.
*/
template<typename T>
class MatchSet {
public:
    /**
    * @brief Constructs a MatchSet with a variadic list of values.
    *
    * @tparam Args Types of the arguments, should be convertible to T
    * @param op Logical operation to use when evaluating the set (AND / OR)
    * @param args Variadic list of values to store in the set
    */
    template<typename... Args>
    MatchSet(Operation op, Args&&... args)
        : m_operation(op), m_values{ std::forward<Args>(args)... } {
    }

    /**
    * @brief Constructs a MatchSet from an initializer list of values.
    *
    * @param op Logical operation to use when evaluating the set (AND / OR)
    * @param values Initializer list of values to store in the set
    */
    MatchSet(Operation op, std::initializer_list<T> values)
        : m_operation(op), m_values(values) {
    }

    /**
    * @brief Greater-than comparison against the set.
    */
    bool operator>(const T& other) const { return Evaluate(other, std::less<T>()); }

    /**
    * @brief Less-than comparison against the set.
    */
    bool operator<(const T& other) const { return Evaluate(other, std::greater<T>()); }

    /**
    * @brief Greater-than-or-equal comparison against the set.
    */
    bool operator>=(const T& other) const { return Evaluate(other, std::less_equal<T>()); }

    /**
    * @brief Less-than-or-equal comparison against the set.
    */
    bool operator<=(const T& other) const { return Evaluate(other, std::greater_equal<T>()); }

    /**
    * @brief Equality comparison against the set.
    */
    bool operator==(const T& other) const { return Evaluate(other, std::equal_to<T>()); }

    /**
    * @brief Inequality comparison against the set.
    */
    bool operator!=(const T& other) const { return Evaluate(other, std::not_equal_to<T>()); }

    /**
    * @brief Evaluates a single value against the stored set using a provided comparison functor.
    *
    * @tparam Compare Type of comparison functor (e.g., std::less<T>, std::equal_to<T>)
    * @param other The value to compare
    * @param cmp Comparison functor to use
    * @return True if the value satisfies the set according to the logical operation, false otherwise
    */
    template<typename Compare>
    bool Evaluate(const T& other, Compare cmp) const {
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
    Operation m_operation;   /**< Logical operation used for evaluation */
    std::vector<T> m_values; /**< Stored set of values */
};

/** Free operator overloads to allow standard comparisons with MatchSet on the left-hand side */
template<typename T>
bool operator<(const T& lhs, const MatchSet<T>& rhs) { return rhs.Evaluate(lhs, std::less<T>()); }

template<typename T>
bool operator>(const T& lhs, const MatchSet<T>& rhs) { return rhs.Evaluate(lhs, std::greater<T>()); }

template<typename T>
bool operator<=(const T& lhs, const MatchSet<T>& rhs) { return rhs.Evaluate(lhs, std::less_equal<T>()); }

template<typename T>
bool operator>=(const T& lhs, const MatchSet<T>& rhs) { return rhs.Evaluate(lhs, std::greater_equal<T>()); }

template<typename T>
bool operator==(const T& lhs, const MatchSet<T>& rhs) { return rhs.Evaluate(lhs, std::equal_to<T>()); }

template<typename T>
bool operator!=(const T& lhs, const MatchSet<T>& rhs) { return rhs.Evaluate(lhs, std::not_equal_to<T>()); }
