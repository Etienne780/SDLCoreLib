#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

/**
* @brief A simple binary reader for trivially copyable types and vectors.
*
* Supports Little-Endian deserialization, primitive types, vectors, and complex/nested structures.
*/
class BinaryDeserializer {
public:
    /**
    * @brief Constructs a BinaryDeserializer for a given buffer.
    * @param buffer Reference to the serialized byte buffer.
    */
    BinaryDeserializer(const std::vector<uint8_t>& buffer)
        : m_buffer(buffer), m_offset(0){
    }

    /**
    * @brief Reads a trivially copyable value from the buffer in Little-Endian order.
    *
    * Automatically reverses bytes on Big-Endian systems.
    *
    * @tparam T Type to read.
    * @return T The read value.
    * @throws std::runtime_error If reading would overflow the buffer.
    */
    template<typename T>
    T Read() {
        static_assert(std::is_trivially_copyable_v<T>,
            "Read requires trivially copyable type");

        if (sizeof(T) > m_buffer.size() - m_offset)
            throw std::runtime_error("BinaryDeserializer: buffer overflow, current offset + T size is larger than the buffer");

        T value{};

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        // Little-endian: copy bytes directly
        std::memcpy(&value, m_buffer.data() + m_offset, sizeof(T));
#else
        // Big-endian: reverse bytes
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i)
            ptr[i] = m_buffer[m_offset + sizeof(T) - 1 - i];
#endif
        m_offset += sizeof(T);
        return value;
    }

    /**
    * @brief Reads a vector of trivially copyable elements.
    *
    * Assumes a uint32_t size prefix was written before the vector data.
    *
    * @tparam T Element type.
    * @return std::vector<T> Deserialized vector.
    */
    template<typename T>
    std::vector<T> ReadVector() {
        uint32_t size = Read<uint32_t>();
        std::vector<T> vec;
        vec.reserve(size);

        for (uint32_t i = 0; i < size; ++i)
            vec.push_back(Read<T>());

        return vec;
    }

    /**
    * @brief Reads a length-prefixed std::string from the buffer.
    *
    * The method expects the following layout:
    *
    * 1. uint32_t length (Little-Endian)
    * 
    * 2. Raw character bytes (no null terminator)
    *
    * @return std::string The reconstructed string.
    * @throws std::runtime_error If the buffer does not contain enough bytes.
    */
    std::string ReadString() {
        uint32_t length = Read<uint32_t>();

        if (length > m_buffer.size() - m_offset) {
            throw std::runtime_error(
                "BinaryDeserializer: buffer overflow while reading string"
            );
        }

        std::string value(
            reinterpret_cast<const char*>(m_buffer.data() + m_offset),
            length
        );

        m_offset += length;
        return value;
    }

    /**
    * @brief Reads a vector of complex elements using a lambda deserializer.
    *
    * Assumes a uint32_t size prefix was written before the vector data.
    * 
    * @tparam T Element type.
    * @tparam ReaderFunc Lambda type: T(BinaryDeserializer&)
    * @param readerFunc Lambda to deserialize each element.
    * @return std::vector<T> Deserialized vector.
   */
    template<typename T, typename ReaderFunc>
    std::vector<T> ReadVector(ReaderFunc readerFunc) {
        uint32_t size = Read<uint32_t>();
        std::vector<T> vec;
        vec.reserve(size);

        for (uint32_t i = 0; i < size; ++i)
            vec.push_back(readerFunc(*this));

        return vec;
    }

    /**
    * @brief Checks whether the read cursor has reached the end of the buffer.
    *
    * Returns true if no more bytes are available for reading.
    * This can be used after deserialization to ensure that the
    * entire buffer has been consumed.
    *
    * @return true  If the current offset is at or beyond the buffer size.
    * @return false If there are still unread bytes remaining.
    */
    inline bool IsAtEnd() const noexcept {
        return m_offset >= m_buffer.size();
    }

    /**
    * @brief Returns the number of unread bytes remaining in the buffer.
    *
    * Useful for validation, debugging, or checking whether enough
    * data is available before attempting further reads.
    *
    * @return size_t Number of bytes remaining from the current offset
    *                to the end of the buffer.
    */
    inline size_t Remaining() const noexcept {
        return m_buffer.size() - m_offset;
    }

private:
    const std::vector<uint8_t>& m_buffer;
    size_t m_offset = 0;
};