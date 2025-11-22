#pragma once
#include <string>
#include "Types/Types.h"

namespace SDLCore {


    /**
    * @brief Convert a result code into a human-readable message.
    *
    * Returns an explanatory message associated with the given SDLResult.
    *
    * @param result The result value to translate.
    * @return A descriptive message for the specified result.
    */
    std::string GetError(SDLResult result);

    /**
    * @brief Retrieve the most recent error message.
    *
    * Returns the last error message recorded by the system.
    * Should only be called after an operation that explicitly
    * indicates an error condition.
    *
    * @return The last stored error message.
    */
    std::string GetError();

    /**
    * @brief Replace the current error message.
    *
    * Overwrites any previously stored error with the given message.
    *
    * @param msg The new error message.
    */
    void SetError(const std::string& msg);

    /**
    * @brief Replace the current error message using a formatted string. (formatting like Log)
    *
    * Constructs a formatted message from the provided arguments and
    * replaces any previously stored error text.
    *
    * @param args Variadic arguments forwarded to the formatting function.
    */
    template<typename... Args>
    void SetErrorF(Args&&... args) {
        SetError(Log::GetFormattedString(std::forward<Args>(args)...));
    }

    /**
    * @brief Append an additional error message.
    *
    * Appends the given message to the current error text,
    * preserving previously collected error information.
    *
    * @param msg The message to append.
    */
    void AddError(const std::string& msg);

    /**
    * @brief Append an additional formatted error message. (formatting like Log)
    *
    * Constructs a formatted message from the provided arguments and
    * appends it to the currently stored error text.
    *
    * @param args Variadic arguments forwarded to the formatting function.
    */
    template<typename... Args>
    void AddErrorF(Args&&... args) {
        AddError(Log::GetFormattedString(std::forward<Args>(args)...));
    }

}