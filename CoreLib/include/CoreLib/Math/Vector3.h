#pragma once
#include <stdexcept>
#include "../FormatUtils.h"

class Matrix;
class Vector2;
class Vector4;

/**
 * @class Vector3
 * @brief Represents a 3D vector with common vector operations.
 */
class Vector3 {
public:
    // Predefined direction vectors
    static const Vector3 forward;  ///< Unit vector pointing forward (0, 0, 1)
    static const Vector3 back;     ///< Unit vector pointing backward (0, 0, -1)
    static const Vector3 up;       ///< Unit vector pointing upwards (0, 1, 0)
    static const Vector3 down;     ///< Unit vector pointing downwards (0, -1, 0)
    static const Vector3 left;     ///< Unit vector pointing left (-1, 0, 0)
    static const Vector3 right;    ///< Unit vector pointing right (1, 0, 0)
    static const Vector3 one;      ///< Vector with all components set to one (1, 1, 1)
    static const Vector3 zero;      ///< Vector with all components set to zero (0, 0, 0)

    float x = 0; ///< X component of the vector
    float y = 0; ///< Y component of the vector
    float z = 0; ///< Z component of the vector

    Vector3();
    Vector3(float value);
    Vector3(float x, float y, float z);
    Vector3(const Vector2& vec, float z);
    Vector3(const Vector4& vec);

    /**
     * @brief Converts the vector to a string representation.
     * @return String in the format "(x, y, z)".
     */
    std::string ToString() const;
    /**
    * @brief Converts this vector to a 3x1 column matrix.
    * @return A Matrix with 3 rows and 1 column representing this vector.
    */
    Matrix ToMatrix3x1() const;
    /**
     * @brief Converts this vector to a 1x3 row matrix.
     * @return A Matrix with 1 row and 3 columns representing this vector.
     */
    Matrix ToMatrix1x3() const;

    /**
    * @brief Sets each component of a vector
    *
    * @param x component of the vector
    * @param y component of the vector
    * @param z component of the vector
    *
    * @return this vector
    */
    Vector3& Set(float x, float y, float z);

    /**
    * @brief Sets each component of a vector to the given value
    * @param fill fill value
    * @return this vector
    */
    Vector3& Set(float fill);

    /**
    * @brief Normalizes the vector to have length 1.
    * Modifies the vector in place.
    *
    * @return this vector
    */
    Vector3& Normalize();

    /**
    * @brief Normalizes the vector to have length 1.
    * @return Normalized copy of the vector
    */
    Vector3 Normalized() const;

    /**
    * @brief Normalizes a vector to have length 1.
    * @param vec the vector that gets Normalized.
    * @return Normalized vector
    */
    static Vector3 Normalize(const Vector3& vec);

    /**
     * @brief Calculates the magnitude (length) of the vector.
     * @return Length as a float.
     */
    float Magnitude() const;

    /**
     * @brief Calculates the squared magnitude of the vector.
     * Avoids costly square root operation.
     * @return Squared length as a float.
     */
    float SquaredMagnitude() const;

    /**
     * @brief Computes the dot product with another vector.
     * @param other The other vector.
     * @return Scalar dot product result.
     */
    float Dot(const Vector3& other) const;

    /**
    * @brief Static dot product between two vectors.
    * @param a First vector.
    * @param b Second vector.
    * @return Scalar dot product result.
    */
    static float Dot(const Vector3& a, const Vector3& b);

    /**
    * @brief Computes the cross product with another vector.
    * @param other The other vector.
    * @return New vector orthogonal to this and other.
    */
    Vector3 Cross(const Vector3& other) const;

    /**
    * @brief Static cross product between two vectors.
    * @param a First vector.
    * @param b Second vector.
    * @return Vector orthogonal to a and b.
    */
    static Vector3 Cross(const Vector3& a, const Vector3& b);

    /**
    * @brief Linearly interpolates between two vectors.
    * @param a Start vector.
    * @param b End vector.
    * @param t Interpolation factor in [0, 1].
    * @return Interpolated vector.
    */
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

    /**
    * @brief Computes the Euclidean distance between two vectors.
    * @param a First vector.
    * @param b Second vector.
    * @return Distance as a float.
    */
    static float Distance(const Vector3& a, const Vector3& b);

    /**
    * @brief Computes the Euclidean distance between two vectors Squared.
    * @param a First vector.
    * @param b Second vector.
    * @return Distance as a float Squared.
    */
    static float SquaredDistance(const Vector3& a, const Vector3& b);

    // Compound assignment operators modifying this vector

    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(const Vector3& other);
    Vector3& operator/=(const Vector3& other);
    Vector3& operator+=(float scalar);
    Vector3& operator-=(float scalar);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    // Binary operators returning new vectors

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(const Vector3& other) const;
    Vector3 operator/(const Vector3& other) const;
    Vector3 operator+(float scalar) const;
    Vector3 operator-(float scalar) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;

    /**
     * @brief Index operator for accessing vector components.
     * @param index Component index (0 = x, 1 = y, 2 = z).
     * @return Reference to the component.
     * @throws std::out_of_range if index is invalid.
     */
    float& operator[](int index);

    /**
     * @brief Const index operator for accessing components.
     * @param index Component index.
     * @return Const reference to component.
     * @throws std::out_of_range if index is invalid.
     */
    const float& operator[](int index) const;
};

#pragma region non_member_operations

// Non-member operators to allow scalar op vector with scalar first

Vector3 operator+(float scalar, const Vector3& other);
Vector3 operator-(float scalar, const Vector3& other);
Vector3 operator*(float scalar, const Vector3& other);
Vector3 operator/(float scalar, const Vector3& other);

#pragma endregion

template<>
static inline std::string FormatUtils::toString<Vector3>(Vector3 value) {
    return value.ToString();
}
