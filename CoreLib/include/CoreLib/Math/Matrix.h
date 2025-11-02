#pragma once
#include <string>
#include <vector>
#include "../FormatUtils.h"

class Vector2;
class Vector3;
class Vector4;

/**
 * @brief A row-major matrix class.
 *
 * This matrix stores its elements in row-major order, meaning all elements of
 * the first row are stored first in memory, followed by all elements of the second row, etc.
 *
 * Example:
 * Matrix m = {
 *     {1, 2, 3},
 *     {4, 5, 6}
 * };
 * Storage layout in m_data:
 * [1, 2, 3, 4, 5, 6]
 *
 * Note:
 * OpenGL uses column-major layout for matrices by default.
 * When passing this matrix to OpenGL, it must be converted to column-major
 * order (see ToColMajorData()).
 */
class Matrix {
public:
    Matrix();
    // Constructs a matrix with the specified number of rows and columns, initialized to zero.
    Matrix(int rows, int cols);

    // Constructs a matrix with the specified dimensions and initializes it with the given raw float array (row-major order).
    Matrix(int rows, int cols, const float* values);

    // Constructs a matrix from a nested initializer list (e.g., {{1, 2}, {3, 4}}).
    Matrix(std::initializer_list<std::initializer_list<float>> values);

    // OPTIMIZED: Copy constructor
    Matrix(const Matrix& other);

    // OPTIMIZED: Move constructor
    Matrix(Matrix&& other) noexcept;

    // OPTIMIZED: Copy assignment operator
    Matrix& operator=(const Matrix& other);

    // OPTIMIZED: Move assignment operator  
    Matrix& operator=(Matrix&& other) noexcept;

    /**
    * @brief Returns the number of rows in the matrix.
    * @return Integer representing the row count.
    */
    int GetRowCount() const;

    /**
    * @brief Returns the number of columns in the matrix.
    * @return Integer representing the column count.
    */
    int GetColCount() const;

    /**
    * @brief Provides mutable access to the raw matrix data (row-major order).
    * @return Pointer to the internal float array representing the matrix data.
    */
    float* GetData();

    /**
    * @brief Provides read-only access to the raw matrix data (row-major order).
    * @return Const pointer to the internal float array representing the matrix data.
    */
    const float* GetData() const;

    /*
     * @brief Gets the translation component from a 4x4 transformation matrix.
     * @return Vector3 containing the X, Y, Z translation values.
     */
    Vector3 GetTranslation() const;

    /*
     * @brief Extracts the rotation component from a 4x4 transformation matrix.
     * @details The returned vector contains Euler angles (in radians)
     * @return Vector3 containing rotation angles around X, Y, Z axes.
     */
    Vector3 GetRotation() const;

    /*
     * @brief Extracts the scale component from a 4x4 transformation matrix.
     * @return Vector3 containing the scale factors along X, Y, Z axes.
     */
    Vector3 GetScale() const;

    Matrix& SetData(float value);

    Matrix& SetDataDirty();

    #pragma region to_conversion

    /**
     * @brief Converts the matrix to a flat float array in column-major order.
     * @return A std::vector<float> containing the matrix elements in column-major layout.
     */
    std::vector<float> ToColMajorData() const;

    /**
     * @brief Converts the matrix to a flat float array suitable for OpenGL.
     *
     * Internally returns the same data as ToColMajorData().data(), since OpenGL expects
     * column-major layout for uniform matrices.
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
    * @brief Converts the matrix to a readable string
    * @param prefix is a prefix that gets writen before every row
    * @return A string representing the matrix content.
    */
    std::string ToString(const std::string& prefix) const;

    /**
    * @brief Converts this matrix to a Vector2 if dimensions are compatible (2x1 or 1x2).
    * @return A Vector2 containing the corresponding matrix elements.
    * @throws std::runtime_error if dimensions are incompatible.
    */
    Vector2 ToVector2() const;

    /**
    * @brief Converts this matrix to a Vector3 if dimensions are compatible (3x1 or 1x3).
    * @return A Vector3 containing the corresponding matrix elements.
    * @throws std::runtime_error if dimensions are incompatible.
    */
    Vector3 ToVector3() const;

    /**
    * @brief Converts this matrix to a Vector4 if dimensions are compatible (4x1 or 1x4).
    * @return A Vector4 containing the corresponding matrix elements.
    * @throws std::runtime_error if dimensions are incompatible.
    */
    Vector4 ToVector4() const;

    #pragma endregion

    // Accesses or modifies an element at the specified row and column
    float& operator()(int row, int col);

    // Returns the value at the specified row and column
    const float& operator()(int row, int col) const;

    #pragma region operation=

    // Adds another matrix element-wise
    Matrix& operator+=(const Matrix& other);
    // Subtracts another matrix element-wise
    Matrix& operator-=(const Matrix& other);
    // Matrix multiplication with another matrix
    Matrix& operator*=(const Matrix& other);
    // Adds a scalar to all matrix elements
    Matrix& operator+=(float scalar);
    // Subtracts a scalar from all matrix elements
    Matrix& operator-=(float scalar);
    // Multiplies all elements by a scalar
    Matrix& operator*=(float scalar);
    // Divides all elements by a scalar
    Matrix& operator/=(float scalar);

    #pragma endregion

    #pragma region operation

    // Matrix addition
    Matrix operator+(const Matrix& other) const;
    // Matrix subtraction
    Matrix operator-(const Matrix& other) const;
    // Matrix-scalar addition
    Matrix operator+(float scalar) const;
    // Matrix-scalar subtraction
    Matrix operator-(float scalar) const;
    // Matrix multiplication with another matrix
    Matrix operator*(const Matrix& other) const;
    // Matrix-vector multiplication (Vector2)
    Vector2 operator*(const Vector2& other) const;
    // Matrix-vector multiplication (Vector3)
    Vector3 operator*(const Vector3& other) const;
    // Matrix-vector multiplication (Vector4)
    Vector4 operator*(const Vector4& other) const;
    // Matrix-scalar multiplication
    Matrix operator*(float scalar) const;
    // Matrix-scalar division
    Matrix operator/(float scalar) const;

    #pragma endregion

private:
    int m_rows = 0;
    int m_cols = 0;
    std::vector<float> m_data;
    mutable bool m_isDataDirty = true;
    mutable std::vector<float> m_cachedColMajorData;
    
    // row-major layout
    int ToIndex(int row, int col) const;

    void UpdateColMajorCache() const;
};

#pragma region non_member_operations

Matrix operator+(float scalar, const Matrix& matrix);
Matrix operator-(float scalar, const Matrix& matrix);
Matrix operator*(float scalar, const Matrix& matrix);
Matrix operator/(float scalar, const Matrix& matrix);

#pragma endregion

template<>
static inline std::string FormatUtils::toString<Matrix>(Matrix value) {
    return value.ToString();
}
