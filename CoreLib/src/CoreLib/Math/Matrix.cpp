#include "CoreLib\Math\Matrix.h"

#include "CoreLib\Math\Vector2.h"
#include "CoreLib\Math\Vector3.h"
#include "CoreLib\Math\Vector4.h"

#include "CoreLib\FormatUtils.h"

Matrix::Matrix() {
}

Matrix::Matrix(int rows, int cols)
    : m_rows(rows), m_cols(cols), m_data(rows* cols, 0.0f) {
    m_cachedColMajorData.reserve(rows * cols);
}

Matrix::Matrix(int rows, int cols, const float* values)
    : m_rows(rows), m_cols(cols), m_data(values, values + (rows * cols)) {
    m_cachedColMajorData.reserve(rows * cols);
}

Matrix::Matrix(std::initializer_list<std::initializer_list<float>> values) {
    m_rows = static_cast<int>(values.size());
    m_cols = static_cast<int>(values.begin()->size());
    m_data.reserve(m_rows * m_cols);
    m_cachedColMajorData.reserve(m_rows * m_cols);

    for (const auto& row : values) {
        if (row.size() != static_cast<size_t>(m_cols))
            throw std::runtime_error("All rows must have the same number of elements.");
        m_data.insert(m_data.end(), row.begin(), row.end());
    }
}

Matrix::Matrix(const Matrix & other)
    : m_rows(other.m_rows), m_cols(other.m_cols), m_data(other.m_data),
    m_isDataDirty(other.m_isDataDirty) {

    if (!m_isDataDirty && !other.m_cachedColMajorData.empty()) {
        m_cachedColMajorData = other.m_cachedColMajorData;
    }
    else {
        m_cachedColMajorData.reserve(m_rows * m_cols);
    }
}

Matrix::Matrix(Matrix&& other) noexcept
    : m_rows(other.m_rows), m_cols(other.m_cols),
    m_data(std::move(other.m_data)),
    m_isDataDirty(other.m_isDataDirty),
    m_cachedColMajorData(std::move(other.m_cachedColMajorData)) {

    other.m_rows = 0;
    other.m_cols = 0;
    other.m_isDataDirty = true;
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        m_rows = other.m_rows;
        m_cols = other.m_cols;
        m_data = other.m_data;
        m_isDataDirty = other.m_isDataDirty;

        if (!m_isDataDirty && !other.m_cachedColMajorData.empty()) {
            m_cachedColMajorData = other.m_cachedColMajorData;
        }
        else {
            m_cachedColMajorData.clear();
            m_cachedColMajorData.reserve(m_rows * m_cols);
        }
    }
    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept {
    if (this != &other) {
        m_rows = other.m_rows;
        m_cols = other.m_cols;
        m_data = std::move(other.m_data);
        m_isDataDirty = other.m_isDataDirty;
        m_cachedColMajorData = std::move(other.m_cachedColMajorData);

        other.m_rows = 0;
        other.m_cols = 0;
        other.m_isDataDirty = true;
    }
    return *this;
}

int Matrix::GetRowCount() const { 
    return m_rows; 
}

int Matrix::GetColCount() const { 
    return m_cols; 
}

float* Matrix::GetData() {
    m_isDataDirty = true;
    return m_data.data(); 
}

const float* Matrix::GetData() const {
    m_isDataDirty = true;
    return m_data.data();
}

Vector3 Matrix::GetTranslation() const {
#ifndef NDEBUG
    if (GetRowCount() != 4 || GetColCount() != 4) {
        throw std::runtime_error("Matrix must be 4x4 to extract Translation");
    }
#endif
    return Vector3((*this)(0, 3), (*this)(1, 3), (*this)(2, 3));
}

Vector3 Matrix::GetRotation() const {
#ifndef NDEBUG
    if (GetRowCount() != 4 || GetColCount() != 4) {
        throw std::runtime_error("Matrix must be 4x4 to extract Rotation");
    }
#endif
    // First remove scale
    Vector3 scale = GetScale();
    Matrix rotMat = *this;
    rotMat(0, 0) /= scale.x; rotMat(0, 1) /= scale.x; rotMat(0, 2) /= scale.x;
    rotMat(1, 0) /= scale.y; rotMat(1, 1) /= scale.y; rotMat(1, 2) /= scale.y;
    rotMat(2, 0) /= scale.z; rotMat(2, 1) /= scale.z; rotMat(2, 2) /= scale.z;

    // Extract Euler angles from rotation matrix (row-major, YXZ order)
    float sy = -rotMat(0, 2);
    float cy = std::sqrt(1 - sy * sy);

    float x, y, z;
    if (cy > 1e-6f) {
        x = std::atan2(rotMat(1, 2), rotMat(2, 2));
        y = std::asin(sy);
        z = std::atan2(rotMat(0, 1), rotMat(0, 0));
    }
    else {
        // Gimbal lock
        x = std::atan2(-rotMat(2, 1), rotMat(1, 1));
        y = std::asin(sy);
        z = 0.0f;
    }

    return Vector3(x, y, z);
}

Vector3 Matrix::GetScale() const {
#ifndef NDEBUG
    if (GetRowCount() != 4 || GetColCount() != 4) {
        throw std::runtime_error("Matrix must be 4x4 to extract Scale");
    }
#endif
    float scaleX = std::sqrt((*this)(0, 0) * (*this)(0, 0) + (*this)(0, 1) * (*this)(0, 1) + (*this)(0, 2) * (*this)(0, 2));
    float scaleY = std::sqrt((*this)(1, 0) * (*this)(1, 0) + (*this)(1, 1) * (*this)(1, 1) + (*this)(1, 2) * (*this)(1, 2));
    float scaleZ = std::sqrt((*this)(2, 0) * (*this)(2, 0) + (*this)(2, 1) * (*this)(2, 1) + (*this)(2, 2) * (*this)(2, 2));
    return Vector3(scaleX, scaleY, scaleZ);
}

Matrix& Matrix::SetData(float value) {
    m_isDataDirty = true;
    std::fill(m_data.begin(), m_data.end(), value);
    return *this;
}

Matrix& Matrix::SetDataDirty() {
    m_isDataDirty = true;
    return *this;
}

void Matrix::UpdateColMajorCache() const {
    if (m_isDataDirty) {
        const size_t totalElements = m_rows * m_cols;

        if (m_cachedColMajorData.size() != totalElements) {
            m_cachedColMajorData.resize(totalElements);
        }

        for (int col = 0; col < m_cols; ++col) {
            for (int row = 0; row < m_rows; ++row) {
                m_cachedColMajorData[col * m_rows + row] = m_data[row * m_cols + col];
            }
        }

        m_isDataDirty = false;
    }
}

#pragma region to_conversion
std::vector<float> Matrix::ToColMajorData() const {
    UpdateColMajorCache();
    return m_cachedColMajorData;
}

const float* Matrix::ToOpenGLData() const {
    UpdateColMajorCache();
    return m_cachedColMajorData.data();
}

std::string Matrix::ToString() const {
    std::ostringstream oss;
    for (int i = 0; i < GetRowCount(); ++i) {
        for (int j = 0; j < GetColCount(); ++j) {
            oss << FormatUtils::toString((*this)(i, j)) << " ";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Matrix::ToString(const std::string& prefix) const {
    std::ostringstream oss;
    for (int i = 0; i < GetRowCount(); ++i) {
        oss << prefix;
        for (int j = 0; j < GetColCount(); ++j) {
            oss << FormatUtils::toString((*this)(i, j)) << " ";
        }
        oss << "\n";
    }
    return oss.str();
}

Vector2 Matrix::ToVector2() const {
    #ifndef NDEBUG
    if (!((GetRowCount() == 2 && GetColCount() == 1) || (GetRowCount() == 1 && GetColCount() == 2))) {
        throw std::runtime_error("Matrix cannot be converted to Vector2 due to incompatible dimensions");
    }
    #endif
    return GetColCount() == 1 ?
        Vector2((*this)(0, 0), (*this)(1, 0)) :
        Vector2((*this)(0, 0), (*this)(0, 1));
}

Vector3 Matrix::ToVector3() const {
    #ifndef NDEBUG
    if (!((GetRowCount() == 3 && GetColCount() == 1) || (GetRowCount() == 1 && GetColCount() == 3))) {
        throw std::runtime_error("Matrix cannot be converted to Vector3 due to incompatible dimensions");
    }
    #endif
    return GetColCount() == 1 ?
        Vector3((*this)(0, 0), (*this)(1, 0), (*this)(2, 0)) :
        Vector3((*this)(0, 0), (*this)(0, 1), (*this)(0, 2));
}

Vector4 Matrix::ToVector4() const {
    #ifndef NDEBUG
    if (!((GetRowCount() == 4 && GetColCount() == 1) || (GetRowCount() == 1 && GetColCount() == 4))) {
        throw std::runtime_error("Matrix cannot be converted to Vector4 due to incompatible dimensions");
    }
    #endif
    return GetColCount() == 1 ?
        Vector4((*this)(0, 0), (*this)(1, 0), (*this)(2, 0), (*this)(3, 0)) :
        Vector4((*this)(0, 0), (*this)(0, 1), (*this)(0, 2), (*this)(0, 3));
}

int Matrix::ToIndex(int row, int col) const {
    #ifndef NDEBUG
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols) {
        throw std::runtime_error("Matrix index out of bounds");
    }
    #endif
    return row * m_cols + col; // row-major layout
}

#pragma endregion

float& Matrix::operator()(int row, int col) {
    #ifndef NDEBUG
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols) {
        throw std::runtime_error("Matrix index out of bounds");
    }
    #endif
    m_isDataDirty = true;
    return m_data[ToIndex(row, col)];
}

const float& Matrix::operator()(int row, int col) const {
    #ifndef NDEBUG
    if (row < 0 || col < 0 || row >= m_rows || col >= m_cols) {
        throw std::runtime_error("Matrix index out of bounds");
    }
    #endif
    return m_data[ToIndex(row, col)];
}

#pragma region operation=

Matrix & Matrix::operator+=(const Matrix& other) {
    if (m_rows != other.m_rows || m_cols != other.m_cols) {
        throw std::runtime_error("Matrix dimensions do not match for addition.");
    }

    m_isDataDirty = true;
    const float* b = other.GetData();

    const size_t totalElements = m_rows * m_cols;
    for (size_t i = 0; i < totalElements; ++i) {
        m_data[i] += b[i];
    }
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& other) {
#ifndef NDEBUG
    if (m_rows != other.m_rows || m_cols != other.m_cols) {
        throw std::runtime_error("Matrix dimensions do not match for subtraction.");
    }
#endif

    m_isDataDirty = true;
    const float* b = other.GetData();

    const size_t totalElements = m_rows * m_cols;
    for (size_t i = 0; i < totalElements; ++i) {
        m_data[i] -= b[i];
    }
    return *this;
}

Matrix& Matrix::operator*=(const Matrix& other) {
    #ifndef NDEBUG
    if (m_cols != other.m_rows) {
        throw std::runtime_error("Matrix dimensions invalid for multiplication.");
    }
    #endif

    m_isDataDirty = true;

    *this = (*this) * other;
    return *this;
}

Matrix& Matrix::operator+=(float scalar) {
    m_isDataDirty = true;

    for (float& element : m_data) {
        element += scalar;
    }
    return *this;
}

Matrix& Matrix::operator-=(float scalar) {
    m_isDataDirty = true;

    for (float& element : m_data) {
        element -= scalar;
    }
    return *this;
}

Matrix& Matrix::operator*=(float scalar) {
    m_isDataDirty = true;

    for (float& element : m_data) {
        element *= scalar;
    }
    return *this;
}

Matrix& Matrix::operator/=(float scalar) {
#ifndef NDEBUG
    if (scalar == 0)
        throw std::runtime_error("Matrix division by zero is not allowed");
#endif
    m_isDataDirty = true;

    for (float& element : m_data) {
        element /= scalar;
    }
    return *this;
}

#pragma endregion

#pragma region operation

Matrix Matrix::operator+(const Matrix& other) const {
    Matrix result = *this;
    result += other;
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    Matrix result = *this;
    result -= other;
    return result;
}

Matrix Matrix::operator+(float scalar) const {
    Matrix result = *this;
    result += scalar;
    return result;
}

Matrix  Matrix::operator-(float scalar) const {
    Matrix result = *this;
    result -= scalar;
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (m_cols != other.m_rows)
        throw std::runtime_error("Matrix multiplication not allowed");

    Matrix result(m_rows, other.m_cols);
    const float* a = GetData();
    const float* b = other.GetData();
    float* r = result.GetData();

    // Clear result matrix
    std::fill(result.m_data.begin(), result.m_data.end(), 0.0f);

    for (int i = 0; i < m_rows; ++i) {
        for (int k = 0; k < m_cols; ++k) {
            const float aik = a[i * m_cols + k];
            for (int j = 0; j < other.m_cols; ++j) {
                r[i * other.m_cols + j] += aik * b[k * other.m_cols + j];
            }
        }
    }

    return result;
}

Vector2 Matrix::operator*(const Vector2& other) const {
    if (m_cols != 2)
        throw std::runtime_error("Matrix column count must match Vector2 size (2)");

    Vector2 result{};
    const float* a = GetData();

    result.x = a[0] * other.x + a[1] * other.y;
    if (m_rows > 1) {
        result.y = a[m_cols] * other.x + a[m_cols + 1] * other.y;
    }

    return result;
}

Vector3 Matrix::operator*(const Vector3& other) const {
    if (m_cols != 3)
        throw std::runtime_error("Matrix column count must match Vector3 size (3)");

    Vector3 result{};
    const float* a = GetData();

    result.x = a[0] * other.x + a[1] * other.y + a[2] * other.z;
    if (m_rows > 1) {
        result.y = a[m_cols] * other.x + a[m_cols + 1] * other.y + a[m_cols + 2] * other.z;
    }
    if (m_rows > 2) {
        result.z = a[2 * m_cols] * other.x + a[2 * m_cols + 1] * other.y + a[2 * m_cols + 2] * other.z;
    }

    return result;
}

Vector4 Matrix::operator*(const Vector4& other) const {
    if (m_cols != 4)
        throw std::runtime_error("Matrix column count must match Vector4 size (4)");

    Vector4 result;
    const float* a = GetData();

    result.x = a[0] * other.x + a[1] * other.y + a[2] * other.z + a[3] * other.w;
    result.y = a[4] * other.x + a[5] * other.y + a[6] * other.z + a[7] * other.w;
    result.z = a[8] * other.x + a[9] * other.y + a[10] * other.z + a[11] * other.w;
    result.w = a[12] * other.x + a[13] * other.y + a[14] * other.z + a[15] * other.w;

    return result;
}

Matrix Matrix::operator*(float scalar) const {
    Matrix result = *this;
    result *= scalar;
    return result;
}

Matrix Matrix::operator/(float scalar) const {
    Matrix result = *this;
    result /= scalar;
    return result;
}

#pragma endregion

#pragma region non_member_operations

Matrix operator+(float scalar, const Matrix& matrix) {
    return (matrix + scalar);
}

Matrix operator-(float scalar, const Matrix& matrix) {
    int rows = matrix.GetRowCount();
    int cols = matrix.GetColCount();

    Matrix result(rows, cols);
    const float* a = matrix.GetData();
    float* r = result.GetData();

    const size_t totalElements = rows * cols;
    for (size_t i = 0; i < totalElements; ++i) {
        r[i] = scalar - a[i];
    }

    return result;
}

Matrix operator*(float scalar, const Matrix& matrix) {
    return (matrix * scalar);
}

Matrix operator/(float scalar, const Matrix& matrix) {
    int rows = matrix.GetRowCount();
    int cols = matrix.GetColCount();

    Matrix result(rows, cols);
    const float* a = matrix.GetData();
    float* r = result.GetData();

    const size_t totalElements = rows * cols;
    for (size_t i = 0; i < totalElements; ++i) {
        #ifndef NDEBUG
        if (a[i] == 0.0f) {
            throw std::runtime_error("Division by zero in matrix element");
        }
        #endif
        r[i] = scalar / a[i];
    }

    return result;
}

#pragma endregion