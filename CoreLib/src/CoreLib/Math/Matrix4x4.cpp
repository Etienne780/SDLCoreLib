#include <sstream>
#include <stdexcept>

#include "CoreLib\Math\Vector2.h"
#include "CoreLib\Math\Vector3.h"
#include "CoreLib\Math\Vector4.h"
#include "CoreLib\FormatUtils.h"
#include "CoreLib\Math\Matrix4x4.h"

Matrix4x4::Matrix4x4() {
    std::memset(m_data, 0, sizeof(m_data));
    m_data[0] = m_data[5] = m_data[10] = m_data[15] = 1.0f;
}

Matrix4x4::Matrix4x4(const float* values) {
    std::memcpy(m_data, values, sizeof(m_data));
}

Matrix4x4::Matrix4x4(std::initializer_list<std::initializer_list<float>> values) {
    if (values.size() != 4) {
        throw std::runtime_error("Matrix4x4 must have exactly 4 rows");
    }

    int row = 0;
    for (const auto& rowData : values) {
        if (rowData.size() != 4) {
            throw std::runtime_error("Matrix4x4 must have exactly 4 columns per row");
        }

        int col = 0;
        for (float val : rowData) {
            m_data[row * 4 + col] = val;
            ++col;
        }
        ++row;
    }
}

Matrix4x4::Matrix4x4(const Matrix4x4& other) {
    std::memcpy(m_data, other.m_data, sizeof(m_data));
    m_isColMajorCacheDirty = other.m_isColMajorCacheDirty;
    if (!m_isColMajorCacheDirty) {
        std::memcpy(m_cachedColMajorData, other.m_cachedColMajorData, sizeof(m_cachedColMajorData));
    }
}

Matrix4x4::Matrix4x4(Matrix4x4&& other) noexcept {
    std::memcpy(m_data, other.m_data, sizeof(m_data));
    m_isColMajorCacheDirty = other.m_isColMajorCacheDirty;
    if (!m_isColMajorCacheDirty) {
        std::memcpy(m_cachedColMajorData, other.m_cachedColMajorData, sizeof(m_cachedColMajorData));
    }
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other) {
    if (this != &other) {
        std::memcpy(m_data, other.m_data, sizeof(m_data));
        m_isColMajorCacheDirty = other.m_isColMajorCacheDirty;
        if (!m_isColMajorCacheDirty) {
            std::memcpy(m_cachedColMajorData, other.m_cachedColMajorData, sizeof(m_cachedColMajorData));
        }
    }
    return *this;
}

Matrix4x4& Matrix4x4::operator=(Matrix4x4&& other) noexcept {
    if (this != &other) {
        std::memcpy(m_data, other.m_data, sizeof(m_data));
        m_isColMajorCacheDirty = other.m_isColMajorCacheDirty;
        if (!m_isColMajorCacheDirty) {
            std::memcpy(m_cachedColMajorData, other.m_cachedColMajorData, sizeof(m_cachedColMajorData));
        }
    }
    return *this;
}

Vector3 Matrix4x4::GetTranslation() const {
    return Vector3(m_data[3], m_data[7], m_data[11]);
}

Vector3 Matrix4x4::GetRotation() const {
    Vector3 scale = GetScale();

    // Create normalized rotation matrix (remove scale)
    float m00 = m_data[0] / scale.x, m01 = m_data[1] / scale.x, m02 = m_data[2] / scale.x;
    float m10 = m_data[4] / scale.y, m11 = m_data[5] / scale.y, m12 = m_data[6] / scale.y;
    float m20 = m_data[8] / scale.z, m21 = m_data[9] / scale.z, m22 = m_data[10] / scale.z;

    // Extract Euler angles from rotation matrix (YXZ order)
    float sy = -m02;
    float cy = std::sqrt(1 - sy * sy);

    float x, y, z;
    if (cy > 1e-6f) {
        x = std::atan2(m12, m22);
        y = std::asin(sy);
        z = std::atan2(m01, m00);
    }
    else {
        // Gimbal lock
        x = std::atan2(-m21, m11);
        y = std::asin(sy);
        z = 0.0f;
    }

    return Vector3(x, y, z);
}

Vector3 Matrix4x4::GetScale() const {
    // Extract scale as length of the first 3 column vectors
    float scaleX = std::sqrt(m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2]);
    float scaleY = std::sqrt(m_data[4] * m_data[4] + m_data[5] * m_data[5] + m_data[6] * m_data[6]);
    float scaleZ = std::sqrt(m_data[8] * m_data[8] + m_data[9] * m_data[9] + m_data[10] * m_data[10]);
    return Vector3(scaleX, scaleY, scaleZ);
}

Matrix4x4& Matrix4x4::SetData(float value) {
    for (int i = 0; i < 16; ++i) {
        m_data[i] = value;
    }
    m_isColMajorCacheDirty = true;
    return *this;
}

void Matrix4x4::UpdateColMajorCache() const {
    if (m_isColMajorCacheDirty) {
        // Convert from row-major to column-major
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                m_cachedColMajorData[col * 4 + row] = m_data[row * 4 + col];
            }
        }
        m_isColMajorCacheDirty = false;
    }
}

const float* Matrix4x4::ToOpenGLData() const {
    UpdateColMajorCache();
    return m_cachedColMajorData;
}

std::string Matrix4x4::ToString() const {
    std::ostringstream oss;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            oss << FormatUtils::toString(m_data[i * 4 + j]) << " ";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Matrix4x4::ToString(const std::string& prefix) const {
    std::ostringstream oss;
    for (int i = 0; i < 4; ++i) {
        oss << prefix;
        for (int j = 0; j < 4; ++j) {
            oss << FormatUtils::toString(m_data[i * 4 + j]) << " ";
        }
        oss << "\n";
    }
    return oss.str();
}

float& Matrix4x4::operator()(int row, int col) {
#ifndef NDEBUG
    if (row < 0 || row >= 4 || col < 0 || col >= 4) {
        throw std::runtime_error("Matrix4x4 index out of bounds");
    }
#endif
    m_isColMajorCacheDirty = true;
    return m_data[ToIndex(row, col)];
}

const float& Matrix4x4::operator()(int row, int col) const {
#ifndef NDEBUG
    if (row < 0 || row >= 4 || col < 0 || col >= 4) {
        throw std::runtime_error("Matrix4x4 index out of bounds");
    }
#endif
    return m_data[ToIndex(row, col)];
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other) {
    for (int i = 0; i < 16; ++i) {
        m_data[i] += other.m_data[i];
    }
    m_isColMajorCacheDirty = true;
    return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other) {
    for (int i = 0; i < 16; ++i) {
        m_data[i] -= other.m_data[i];
    }
    m_isColMajorCacheDirty = true;
    return *this;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) {
    *this = *this * other;
    return *this;
}

Matrix4x4& Matrix4x4::operator+=(float scalar) {
    for (int i = 0; i < 16; ++i) {
        m_data[i] += scalar;
    }
    m_isColMajorCacheDirty = true;
    return *this;
}

Matrix4x4& Matrix4x4::operator-=(float scalar) {
    for (int i = 0; i < 16; ++i) {
        m_data[i] -= scalar;
    }
    m_isColMajorCacheDirty = true;
    return *this;
}

Matrix4x4& Matrix4x4::operator*=(float scalar) {
    for (int i = 0; i < 16; ++i) {
        m_data[i] *= scalar;
    }
    m_isColMajorCacheDirty = true;
    return *this;
}

Matrix4x4& Matrix4x4::operator/=(float scalar) {
#ifndef NDEBUG
    if (scalar == 0.0f) {
        throw std::runtime_error("Division by zero");
    }
#endif
    for (int i = 0; i < 16; ++i) {
        m_data[i] /= scalar;
    }
    m_isColMajorCacheDirty = true;
    return *this;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const {
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i) {
        result.m_data[i] = m_data[i] + other.m_data[i];
    }
    return result;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const {
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i) {
        result.m_data[i] = m_data[i] - other.m_data[i];
    }
    return result;
}

Matrix4x4 Matrix4x4::operator+(float scalar) const {
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i) {
        result.m_data[i] = m_data[i] + scalar;
    }
    return result;
}

Matrix4x4 Matrix4x4::operator-(float scalar) const {
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i) {
        result.m_data[i] = m_data[i] - scalar;
    }
    return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
    Matrix4x4 result;
    result.SetData(0.0f); // Initialize to zero

    // Optimized matrix multiplication
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.m_data[i * 4 + j] += m_data[i * 4 + k] * other.m_data[k * 4 + j];
            }
        }
    }

    return result;
}

Vector4 Matrix4x4::operator*(const Vector4& other) const {
    return Vector4(
        m_data[0] * other.x + m_data[1] * other.y + m_data[2] * other.z + m_data[3] * other.w,
        m_data[4] * other.x + m_data[5] * other.y + m_data[6] * other.z + m_data[7] * other.w,
        m_data[8] * other.x + m_data[9] * other.y + m_data[10] * other.z + m_data[11] * other.w,
        m_data[12] * other.x + m_data[13] * other.y + m_data[14] * other.z + m_data[15] * other.w
    );
}

Matrix4x4 Matrix4x4::operator*(float scalar) const {
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i) {
        result.m_data[i] = m_data[i] * scalar;
    }
    return result;
}

Matrix4x4 Matrix4x4::operator/(float scalar) const {
#ifndef NDEBUG
    if (scalar == 0.0f) {
        throw std::runtime_error("Division by zero");
    }
#endif
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i) {
        result.m_data[i] = m_data[i] / scalar;
    }
    return result;
}

// Non-member operators
Matrix4x4 operator+(float scalar, const Matrix4x4& matrix) {
    return matrix + scalar;
}

Matrix4x4 operator-(float scalar, const Matrix4x4& matrix) {
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i) {
        result.GetData()[i] = scalar - matrix.GetData()[i];
    }
    return result;
}

Matrix4x4 operator*(float scalar, const Matrix4x4& matrix) {
    return matrix * scalar;
}

Matrix4x4 operator/(float scalar, const Matrix4x4& matrix) {
    Matrix4x4 result;
    const float* data = matrix.GetData();
    float* resultData = result.GetData();

    for (int i = 0; i < 16; ++i) {
#ifndef NDEBUG
        if (data[i] == 0.0f) {
            throw std::runtime_error("Division by zero in matrix element");
        }
#endif
        resultData[i] = scalar / data[i];
    }
    return result;
}

namespace GLTransform4x4 {

    Matrix4x4 Identity() {
        return Matrix4x4();
    }

    Matrix4x4 Scale(float x, float y, float z) {
        float data[16] = {
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    Matrix4x4 Scale(const Vector3& scale) {
        return Scale(scale.x, scale.y, scale.z);
    }

    Matrix4x4 Scale(float uniform) {
        return Scale(uniform, uniform, uniform);
    }

    Matrix4x4 Translate(float x, float y, float z) {
        float data[16] = {
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    Matrix4x4 Translate(const Vector3& translation) {
        return Translate(translation.x, translation.y, translation.z);
    }

    Matrix4x4 RotateX(float radians) {
        float c = std::cos(radians);
        float s = std::sin(radians);
        float data[16] = {
            1, 0,  0, 0,
            0, c, -s, 0,
            0, s,  c, 0,
            0, 0,  0, 1
        };
        return Matrix4x4(data);
    }

    Matrix4x4 RotateY(float radians) {
        float c = std::cos(radians);
        float s = std::sin(radians);
        float data[16] = {
             c, 0, s, 0,
             0, 1, 0, 0,
            -s, 0, c, 0,
             0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    Matrix4x4 RotateZ(float radians) {
        float c = std::cos(radians);
        float s = std::sin(radians);
        float data[16] = {
            c, -s, 0, 0,
            s,  c, 0, 0,
            0,  0, 1, 0,
            0,  0, 0, 1
        };
        return Matrix4x4(data);
    }

    Matrix4x4 RotateXYZ(float rx, float ry, float rz) {
        return RotateZ(rz) * RotateY(ry) * RotateX(rx);
    }

    Matrix4x4 RotateXYZ(const Vector3& radians) {
        return RotateXYZ(radians.x, radians.y, radians.z);
    }

    Matrix4x4 Perspective(float fovy, float aspect, float zNear, float zFar) {
        Matrix4x4 result;
        result.SetData(0.0f);

        float* data = result.GetData();
        float const tanHalfFovy = std::tan(fovy / 2.0f);

        data[0] = 1.0f / (aspect * tanHalfFovy);
        data[5] = 1.0f / tanHalfFovy;
        data[10] = -(zFar + zNear) / (zFar - zNear);
        data[11] = -(2.0f * zFar * zNear) / (zFar - zNear);
        data[14] = -1.0f;

        return result;
    }

    Matrix4x4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar) {
        Matrix4x4 result;
        result.SetData(0.0f);

        float* data = result.GetData();
        data[0] = 2.0f / (right - left);
        data[5] = 2.0f / (top - bottom);
        data[10] = -2.0f / (zFar - zNear);
        data[15] = 1.0f;

        data[3] = -(right + left) / (right - left);
        data[7] = -(top + bottom) / (top - bottom);
        data[11] = -(zFar + zNear) / (zFar - zNear);

        return result;
    }

    Matrix4x4 LookAt(const Vector3& position, const Vector3& target, const Vector3& up) {
        Vector3 f = (target - position).Normalize();        // forward
        Vector3 r = up.Cross(f).Normalize();               // right
        Vector3 u = f.Cross(r);                            // true up

        float data[16] = {
            r.x,  r.y,  r.z, -r.Dot(position),
            u.x,  u.y,  u.z, -u.Dot(position),
           -f.x, -f.y, -f.z,  f.Dot(position),
            0,    0,    0,    1
        };

        return Matrix4x4(data);
    }

    Matrix4x4 LookRotation(const Vector3& forward, const Vector3& up) {
        Vector3 f = forward.Normalized();
        Vector3 r = up.Cross(f).Normalize();
        Vector3 u = f.Cross(r);

        float data[16] = {
            r.x, u.x, f.x, 0,
            r.y, u.y, f.y, 0,
            r.z, u.z, f.z, 0,
            0,   0,   0,   1
        };

        return Matrix4x4(data);
    }

    void MakeIdentity(Matrix4x4& out) {
        std::memset(out.GetData(), 0, 16 * sizeof(float));
        float* data = out.GetData();
        data[0] = data[5] = data[10] = data[15] = 1.0f;
    }

    void MakeScale(Matrix4x4& out, float x, float y, float z) {
        Matrix4x4 scale = Identity();
        float* s = scale.GetData();
        s[0] = x;
        s[1 * 4 + 1] = y;
        s[2 * 4 + 2] = z;

        out = scale * out;
    }

    void MakeScale(Matrix4x4& out, const Vector3& scaler) {
        MakeScale(out, scaler.x, scaler.y, scaler.z);
    }

    void MakeTranslate(Matrix4x4& out, float x, float y, float z) {
        Matrix4x4 trans = Identity();
        float* t = trans.GetData();
        t[0 * 4 + 3] = x;
        t[1 * 4 + 3] = y;
        t[2 * 4 + 3] = z;

        out = trans * out;
    }

    void MakeTranslate(Matrix4x4& out, const Vector3& tranlation) {
        MakeTranslate(out, tranlation.x, tranlation.y, tranlation.z);
    }

    void MakeRotateX(Matrix4x4& out, float radians) {
        Matrix4x4 rot = Identity();
        float c = std::cos(radians), s = std::sin(radians);
        float* r = rot.GetData();
        r[0] = 1;
        r[1 * 4 + 1] = c;
        r[1 * 4 + 2] = -s;
        r[2 * 4 + 1] = s;
        r[2 * 4 + 2] = c;
        r[3 * 4 + 3] = 1;

        out = rot * out;
    }

    void MakeRotateY(Matrix4x4& out, float radians) {
        Matrix4x4 rot = Identity();
        float c = std::cos(radians), s = std::sin(radians);
        float* r = rot.GetData();
        r[0] = c;
        r[2] = s;
        r[1 * 4 + 1] = 1;
        r[2 * 4 + 0] = -s;
        r[2 * 4 + 2] = c;
        r[3 * 4 + 3] = 1;

        out = rot * out;
    }

    void MakeRotateZ(Matrix4x4& out, float radians) {
        Matrix4x4 rot = Identity();
        float c = std::cos(radians), s = std::sin(radians);
        float* r = rot.GetData();
        r[0] = c;
        r[1] = -s;
        r[1 * 4 + 0] = s;
        r[1 * 4 + 1] = c;
        r[2 * 4 + 2] = 1;
        r[3 * 4 + 3] = 1;

        out = rot * out;
    }

    void MakeRotateXYZ(Matrix4x4& out, float rx, float ry, float rz) {
        Matrix4x4 rotX = Identity(), rotY = Identity(), rotZ = Identity();
        MakeRotateX(rotX, rx);
        MakeRotateY(rotY, ry);
        MakeRotateZ(rotZ, rz);
        out = (rotZ * rotY * rotX) * out;
    }

    void MakeRotateXYZ(Matrix4x4& out, const Vector3& radians) {
        MakeRotateXYZ(out, radians.x, radians.y, radians.z);
    }

    Vector3 MatrixToEuler(const Matrix4x4& m) {
        Vector3 euler;

        // yaw (Y-Achse)
        euler.y = atan2(m(0, 2), m(2, 2));

        // pitch (X-Achse)
        euler.x = -asin(m(1, 2));

        // roll (Z-Achse)
        euler.z = atan2(m(1, 0), m(1, 1));

        return euler;
    }
}