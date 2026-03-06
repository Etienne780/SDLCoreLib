#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
* @brief A simple binary serializer for trivially copyable types and vectors.
*
* Supports Little-Endian storage, primitive types, vectors, and complex/nested structures.
*/
class BinarySerializer {
public:
    BinarySerializer() = default;
    ~BinarySerializer() = default;

    /**
    * @brief Adds multiple fields to the serializer in a single call.
    *
    * This is a variadic overload that forwards each argument to the appropriate
    * `AddField` overload.
    * 
    * It can handle:
    * 
    * - Primitive trivially copyable types
    * 
    * - std::vector<T> of trivially copyable types
    *
    * Note: Nested or complex structures must be added explicitly using
    * `AddComplexField` and are not handled automatically by this function.
    *
    * The fields are serialized in the order they are passed, so the same
    * order must be used during deserialization.
    *
    * code:
    * 
    * BinarySerializer s;
    * 
    * s.AddFields(width, height, pieces); // primitives and vectors
    * 
    * s.AddComplexField(boardCmds, [](BinarySerializer& s, const BoardCommand& cmd){
    * 
    *     s.AddField(cmd.columnIndex, cmd.rowIndex, cmd.fill, cmd.startRight);
    * 
    *     s.AddField(cmd.pieces);
    * 
    * });
    *
    * @tparam T Parameter pack of types to serialize.
    * @param value Variadic arguments, each forwarded to `AddField`.
    */
    template<typename ...T>
    void AddFields(const T& ...value) {
        (this->AddField(value), ...);
    }

    /**
    * @brief Adds a primitive value to the buffer in Little-Endian order.
    *
    * @tparam T Trivially copyable type to serialize.
    * @param value The value to serialize.
    */
    template<typename T>
    void AddField(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>,
            "AddField requires trivially copyable type");

        ForEachByteLE(value, [this](uint8_t byte, size_t /*index*/) {
            m_buffer.push_back(byte);
        });
    }

    /**
    * @brief Serializes a std::string in length-prefixed format.
    *
    * The string is stored as:
    * 
    * 1. uint32_t length (Little-Endian via AddField)
    * 
    * 2. raw character bytes (no null terminator)
    *
    * @param value The string to serialize.
    */
    template<>
    void AddField(const std::string& value) {
        AddField(static_cast<uint32_t>(value.size()));
        m_buffer.insert(
            m_buffer.end(),
            reinterpret_cast<const uint8_t*>(value.data()),
            reinterpret_cast<const uint8_t*>(value.data()) + value.size()
        );
    }

    /**
    * @brief Adds a vector of trivially copyable elements.
    *
    * Stores the size first as uint32_t, followed by all elements in order.
    *
    * @tparam T Element type of the vector.
    * @param vec The vector to serialize.
    */
    template<typename T>
    void AddField(const std::vector<T>& vec) {
        static_assert(std::is_trivially_copyable_v<T>,
            "AddField requires trivially copyable type");

        AddField(static_cast<uint32_t>(vec.size()));
        for (const auto& e : vec)
            AddField(e);
    }

    /**
    * @brief Adds a vector of complex elements using a custom serializer lambda.
    *
    * Stores the size first as uint32_t, followed by all elements in order.
    * 
    * Example for nested structures like Vector2:
    * 
    * code:
    * 
    * serializer.AddComplexField(vectors, [](BinarySerializer& s, const Vector2& vec){
    * 
    *     s.AddField(vec.x);
    * 
    *     s.AddField(vec.y);
    * 
    * });
    *
    * @tparam T Element type of the vector.
    * @tparam Serializer Lambda type that accepts (BinarySerializer&, const T&)
    * @param vec The vector to serialize.
    * @param ser Lambda to serialize each element.
    */
    template<typename T, typename Serializer>
    void AddComplexField(const std::vector<T>& vec, Serializer ser) {
        AddField(static_cast<uint32_t>(vec.size()));
        static_assert(std::is_invocable_v<Serializer, BinarySerializer&, const T&>,
            "Serializer lambda must accept (BinarySerializer&, const T&)");
        
        for (const auto& e : vec) {
            ser(*this, e);
        }
    }

    /**
    * @brief Returns the raw serialized byte buffer.
    * @return std::vector<uint8_t> Serialized data buffer.
    */
    std::vector<uint8_t> ToBuffer() const;

    /**
    * @brief Returns the serialized buffer as a Base64 string.
    * @return std::string Base64 encoded string of the serialized data.
    */
    std::string ToBase64() const;

private:
    std::vector<uint8_t> m_buffer;

    /**
    * @brief Calls a lambda for each byte of a trivially copyable value in Little-Endian order.
    *
    * Automatically reverses byte order on Big-Endian systems.
    *
    * @tparam T Type of the value.
    * @tparam Func Lambda type: void(uint8_t byte, size_t index)
    * @param value Value to iterate over bytes.
    * @param f Lambda called for each byte.
    */
    template<typename T, typename Func>
    inline void ForEachByteLE(const T& value, Func&& f)
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "ForEachByte requires trivially copyable type");

        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        for (size_t i = 0; i < sizeof(T); ++i)
            f(ptr[i], i);
#else
        // reverse order on big-endian CPU
        for (size_t i = 0; i < sizeof(T); ++i)
            f(ptr[sizeof(T) - 1 - i], i);
#endif
    }

};