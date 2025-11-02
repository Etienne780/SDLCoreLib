#pragma once
#include <string>
#include <cstring>
#include <cmath>
#include "../FormatUtils.h"

class Vector2;
class Vector3;
class Vector4;

/**
 * @brief A high-performance 4x4 matrix class optimized for game engines.
 *
 * This matrix stores its elements in row-major order using a fixed-size array
 * for maximum performance. No dynamic allocations, cache-friendly, and optimized
 * for transformations commonly used in 3D graphics.
 */
class Matrix4x4 {
public:
    // Constructors
    Matrix4x4();
    Matrix4x4(const float* values);
    Matrix4x4(std::initializer_list<std::initializer_list<float>> values);

    // Copy constructor - optimized with memcpy
    Matrix4x4(const Matrix4x4& other);

    // Move constructor - optimized
    Matrix4x4(Matrix4x4&& other) noexcept;

    // Assignment operators
    Matrix4x4& operator=(const Matrix4x4& other);
    Matrix4x4& operator=(Matrix4x4&& other) noexcept;

    /**
     * @brief Provides mutable access to the raw matrix data (row-major order).
     * @return Pointer to the internal float array representing the matrix data.
     */
    float* GetData() {
        m_isColMajorCacheDirty = true;
        return m_data;
    }

    /**
     * @brief Provides read-only access to the raw matrix data (row-major order).
     * @return Const pointer to the internal float array representing the matrix data.
     */
    const float* GetData() const { return m_data; }

    /**
     * @brief Gets the translation component from the transformation matrix.
     * @return Vector3 containing the X, Y, Z translation values.
     */
    Vector3 GetTranslation() const;

    /**
     * @brief Extracts the rotation component from the transformation matrix.
     * @return Vector3 containing rotation angles around X, Y, Z axes (Euler angles in radians).
     */
    Vector3 GetRotation() const;

    /**
     * @brief Extracts the scale component from the transformation matrix.
     * @return Vector3 containing the scale factors along X, Y, Z axes.
     */
    Vector3 GetScale() const;

    /**
     * @brief Sets all matrix elements to the specified value.
     * @param value The value to set all elements to.
     * @return Reference to this matrix for chaining.
     */
    Matrix4x4& SetData(float value);

    /**
     * @brief Marks the column-major cache as dirty (for internal use).
     * @return Reference to this matrix for chaining.
     */
    Matrix4x4& SetDataDirty() {
        m_isColMajorCacheDirty = true;
        return *this;
    }

    /**
     * @brief Converts the matrix to a flat float array suitable for OpenGL.
     *
     * Returns a pointer to column-major data that OpenGL expects.
     * Uses caching for performance - only recalculates when matrix changes.
     *
     * @return A const float* containing the matrix elements in column-major layout.
     */
    const float* ToOpenGLData() const;

    /**
     * @brief Converts the matrix to a readable string.
     * @return A string representing the matrix content.
     */
    std::string ToString() const;

    /**
     * @brief Converts the matrix to a readable string with prefix.
     * @param prefix Prefix that gets written before every row.
     * @return A string representing the matrix content.
     */
    std::string ToString(const std::string& prefix) const;

    // Element access operators
    float& operator()(int row, int col);
    const float& operator()(int row, int col) const;

    // Arithmetic assignment operators
    Matrix4x4& operator+=(const Matrix4x4& other);
    Matrix4x4& operator-=(const Matrix4x4& other);
    Matrix4x4& operator*=(const Matrix4x4& other);
    Matrix4x4& operator+=(float scalar);
    Matrix4x4& operator-=(float scalar);
    Matrix4x4& operator*=(float scalar);
    Matrix4x4& operator/=(float scalar);

    // Arithmetic operators
    Matrix4x4 operator+(const Matrix4x4& other) const;
    Matrix4x4 operator-(const Matrix4x4& other) const;
    Matrix4x4 operator+(float scalar) const;
    Matrix4x4 operator-(float scalar) const;
    Matrix4x4 operator*(const Matrix4x4& other) const;
    Vector4 operator*(const Vector4& other) const;
    Matrix4x4 operator*(float scalar) const;
    Matrix4x4 operator/(float scalar) const;

private:
    // Fixed-size array for maximum performance - no heap allocation
    alignas(16) float m_data[16];

    // Cached column-major data for OpenGL - only calculated when needed
    mutable bool m_isColMajorCacheDirty = true;
    mutable alignas(16) float m_cachedColMajorData[16];

    // Inline helper for index calculation (row-major)
    inline int ToIndex(int row, int col) const {
        return row * 4 + col;
    }

    // Internal method to update column-major cache
    void UpdateColMajorCache() const;
};

// Non-member operators
Matrix4x4 operator+(float scalar, const Matrix4x4& matrix);
Matrix4x4 operator-(float scalar, const Matrix4x4& matrix);
Matrix4x4 operator*(float scalar, const Matrix4x4& matrix);
Matrix4x4 operator/(float scalar, const Matrix4x4& matrix);

/**
* @brief GLTransform provides functions to create and manipulate 4x4 matrices, primarily for OpenGL use.
*/
namespace GLTransform4x4 {
    Matrix4x4 Identity();

    Matrix4x4 Scale(float x, float y, float z);
    Matrix4x4 Scale(const Vector3& scale);
    Matrix4x4 Scale(float uniform);

    Matrix4x4 Translate(float x, float y, float z);
    Matrix4x4 Translate(const Vector3& translation);

    Matrix4x4 RotateX(float radians);
    Matrix4x4 RotateY(float radians);
    Matrix4x4 RotateZ(float radians);
    Matrix4x4 RotateXYZ(float rx, float ry, float rz);
    Matrix4x4 RotateXYZ(const Vector3& radians);

    /**
    * @brief Creates a perspective projection matrix (right-handed coordinate system).
    *
    * Simulates a realistic perspective where distant objects appear smaller.
    * This matrix is commonly used for 3D scenes with depth perception.
    *
    * @param fovy    Field of view in the y-direction, in radians.
    * @param aspect  Aspect ratio of the viewport (width / height).
    * @param zNear   The near clipping plane distance (must be > 0).
    * @param zFar    The far clipping plane distance (must be > zNear).
    * @return        A 4x4 perspective projection matrix.
    */
    Matrix4x4 Perspective(float fovy, float aspect, float zNear, float zFar);
    /**
    * @brief Creates an orthographic projection matrix (right-handed coordinate system).
    *
    * Maps a 3D volume defined by the left, right, bottom, top, near, and far planes into normalized device coordinates.
    * This is typically used for 2D rendering or UI systems, where perspective distortion is not desired.
    *
    * @param left   The left plane of the view volume.
    * @param right  The right plane of the view volume.
    * @param bottom The bottom plane of the view volume.
    * @param top    The top plane of the view volume.
    * @param zNear  The near clipping plane distance.
    * @param zFar   The far clipping plane distance.
    * @return       A 4x4 orthographic projection matrix.
    */
    Matrix4x4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
    Matrix4x4 LookAt(const Vector3& position, const Vector3& target, const Vector3& up);
    Matrix4x4 LookRotation(const Vector3& forward, const Vector3& up);

    void MakeIdentity(Matrix4x4& out);
    void MakeScale(Matrix4x4& out, float x, float y, float z);
    void MakeScale(Matrix4x4& out, const Vector3& scale);
    void MakeTranslate(Matrix4x4& out, float x, float y, float z);
    void MakeTranslate(Matrix4x4& out, const Vector3& tranlation);
    void MakeRotateX(Matrix4x4& out, float radians);
    void MakeRotateY(Matrix4x4& out, float radians);
    void MakeRotateZ(Matrix4x4& out, float radians);
    void MakeRotateXYZ(Matrix4x4& out, float rx, float ry, float rz);
    void MakeRotateXYZ(Matrix4x4& out, const Vector3& radians);

    Vector3 MatrixToEuler(const Matrix4x4& m);
}

template<>
static inline std::string FormatUtils::toString<Matrix4x4>(Matrix4x4 value) {
    return value.ToString();
}
