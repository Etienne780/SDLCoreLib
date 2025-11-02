#pragma once
#include <stdexcept>
#include "../FormatUtils.h"

class Matrix;
class Vector3;
class Vector4;

/**
 * @class Vector2
 * @brief Represents a 2D vector with common vector operations.
 */
class Vector2 {
public:
    // Predefined direction vectors
    static const Vector2 up;     ///< Unit vector pointing upwards (0, 1)
    static const Vector2 down;   ///< Unit vector pointing downwards (0, -1)
    static const Vector2 left;   ///< Unit vector pointing left (-1, 0)
    static const Vector2 right;  ///< Unit vector pointing right (1, 0)
    static const Vector2 one;    ///< Vector with both components set to one (1, 1)
    static const Vector2 zero;    ///< Vector with both components set to zero (0, 0)

    float x = 0; ///< X component of the vector
    float y = 0; ///< Y component of the vector

    Vector2();
    Vector2(float value);
    Vector2(float x, float y);
    Vector2(const Vector3& vec);
    Vector2(const Vector4& vec);

    /**
    * @brief Converts the vector to a string representation.
    * @return String in the format "(x, y)".
    */
    std::string ToString() const;
    /**
    * @brief Converts this vector to a 2x1 column matrix.
    * @return A Matrix with 2 rows and 1 column representing this vector.
    */
    Matrix ToMatrix2x1() const;
    /**
    * @brief Converts this vector to a 1x2 row matrix.
    * @return A Matrix with 1 row and 2 columns representing this vector.
    */
    Matrix ToMatrix1x2() const;

    /**
    * @brief Sets each component of a vector
    * 
    * @param x component of the vector
    * @param y component of the vector
    * 
    * @return this vector
    */
    Vector2& Set(float x, float y);

    /**
    * @brief Sets each component of a vector to the given value
    * @param fill fill value
    * @return this vector
    */
    Vector2& Set(float fill);

    /**
    * @brief Normalizes the vector to have length 1.
    * Modifies the vector in place.
    * 
    * @return this vector
    */
    Vector2& Normalize();

    /**
    * @brief Normalizes the vector to have length 1.
    * @return Normalized copy of the vector
    */
    Vector2 Normalized() const;

    /**
    * @brief Normalizes a vector to have length 1.
    * @param vec the vector that gets Normalized.
    * @return Normalized vector
    */
    static Vector2 Normalize(const Vector2& vec);

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
    float Dot(const Vector2& other) const;

    /**
    * @brief Static dot product between two vectors.
    * @param a First vector.
    * @param b Second vector.
    * @return Scalar dot product result.
    */
    static float Dot(const Vector2& a, const Vector2& b);

    /**
    * @brief Computes the 2D cross product with another vector.
    * Note: In 2D, cross product returns a scalar (z-component of 3D cross).
    * @param other The other vector.
    * @return Scalar cross product result.
    */
    float Cross(const Vector2& other) const;

    /**
    * @brief Static 2D cross product between two vectors.
    * @param a First vector.
    * @param b Second vector.
    * @return Scalar cross product result.
    */
    static float Cross(const Vector2& a, const Vector2& b);

    /**
    * @brief Linearly interpolates between two vectors.
    * @param a Start vector.
    * @param b End vector.
    * @param t Interpolation factor in [0, 1].
    * @return Interpolated vector.
    */
    static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);

    /**
    * @brief Computes the Euclidean distance between two vectors.
    * @param a First vector.
    * @param b Second vector.
    * @return Distance as a float.
    */
    static float Distance(const Vector2& a, const Vector2& b);

    /**
    * @brief Computes the Euclidean distance between two vectors Squared.
    * @param a First vector.
    * @param b Second vector.
    * @return Distance as a float Squared.
    */
    static float SquaredDistance(const Vector2& a, const Vector2& b);

    // Compound assignment operators modifying this vector

    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(const Vector2& other);
    Vector2& operator/=(const Vector2& other);
    Vector2& operator+=(float scalar);
    Vector2& operator-=(float scalar);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);

    // Binary operators returning new vectors

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(const Vector2& other) const;
    Vector2 operator/(const Vector2& other) const;
    Vector2 operator+(float scalar) const;
    Vector2 operator-(float scalar) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;

    /**
     * @brief Index operator for accessing vector components.
     * @param index Component index (0 = x, 1 = y).
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

Vector2 operator+(float scalar, const Vector2& other);
Vector2 operator-(float scalar, const Vector2& other);
Vector2 operator*(float scalar, const Vector2& other);
Vector2 operator/(float scalar, const Vector2& other);

#pragma endregion

template<>
static inline std::string FormatUtils::toString<Vector2>(Vector2 value) {
    return value.ToString();
}
