#include <stdexcept>
#include <cmath>

#include "CoreLib\Math\Vector2.h"
#include "CoreLib\Math\Vector3.h"
#include "CoreLib\Math\Matrix.h"
#include "CoreLib\Math\MathUtil.h"
#include "CoreLib\FormatUtils.h"

#include "CoreLib\Math\Vector4.h"

const Vector4 Vector4::one(1, 1, 1, 1);
const Vector4 Vector4::zero(0, 0, 0, 0);

Vector4::Vector4() {
}
Vector4::Vector4(float value)
    : x(value), y(value), z(value), w(value) {
}
Vector4::Vector4(float x, float y, float z, float w) 
    : x(x), y(y), z(z), w(w) {
} 
Vector4::Vector4(const Vector2& vec, float z, float w) 
    : x(vec.x), y(vec.y), z(z), w(w) {
}
Vector4::Vector4(const Vector2& a, const Vector2& b) 
    : x(a.x), y(a.y), z(b.x), w(b.y) {
}
Vector4::Vector4(const Vector3& vec, float w) 
    : x(vec.x), y(vec.y), z(vec.z), w(w) {
}

std::string Vector4::ToString() const {
    std::ostringstream oss;
    oss << "[" << x << ", " << y << ", " << z << ", " << w << "]";
    return oss.str();
}
Matrix Vector4::ToMatrix4x1() const {
    float data[4] = { x, y, z, w };
    return Matrix(4, 1, data);
}

Matrix Vector4::ToMatrix1x4() const {
    float data[4] = { x, y, z, w };
    return Matrix(1, 4, data);
}

Vector4& Vector4::Set(const Vector4& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Vector4& Vector4::Set(float _x, float _y, float _z, float _w) {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
    return *this;
}

Vector4& Vector4::Set(float fill) {
    x = fill;
    y = fill;
    z = fill;
    w = fill;
    return *this;
}

Vector4& Vector4::Normalize() {
    float len = Magnitude();
    if (len > 0) {
        *this /= len;
    }
    return *this;
}

Vector4 Vector4::Normalized() const {
    Vector4 copy = *this;
    copy.Normalize();
    return copy;
}

Vector4 Vector4::Normalize(const Vector4& vec) {
    float len = vec.Magnitude();
    if (len > 0) {
        return vec / len;
    }
    return vec;
}

float Vector4::Magnitude() const {
    return std::sqrt(SquaredMagnitude());
}

float Vector4::SquaredMagnitude() const {
    return x * x + y * y + z * z + w * w;
}

float Vector4::Dot(const Vector4& other) const {
    return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
}

float Vector4::Dot(const Vector4& a, const Vector4& b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, float t) {
    return Vector4(
        MathUtil::Lerp(a.x, b.x, t),
        MathUtil::Lerp(a.y, b.y, t),
        MathUtil::Lerp(a.z, b.z, t),
        MathUtil::Lerp(a.w, b.w, t)
    );
}

float Vector4::Distance(const Vector4& a, const Vector4& b) {
    return (a - b).Magnitude();
}

float Vector4::SquaredDistance(const Vector4& a, const Vector4& b) {
    return (a - b).SquaredMagnitude();
}

Vector4& Vector4::operator+=(const Vector4& other) {
    x += other.x; y += other.y;
    z += other.z; w += other.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& other) {
    x -= other.x; y -= other.y;
    z -= other.z; w -= other.w;
    return *this;
}

Vector4& Vector4::operator*=(const Vector4& other) {
    x *= other.x; y *= other.y;
    z *= other.z; w *= other.w;
    return *this;
}

Vector4& Vector4::operator/=(const Vector4& other) {
    if (other.x == 0 || other.y == 0 || other.z == 0 || other.w == 0) {
        std::ostringstream oss;
        oss << "Vector4: Division by zero (" << other.x << ", " << other.y << ", " << other.z << ", " << other.w << ")";
        throw std::runtime_error(oss.str());
    }
    x /= other.x; y /= other.y;
    z /= other.z; w /= other.w;
    return *this;
}

Vector4& Vector4::operator+=(float scalar) {
    x += scalar;
    y += scalar;
    z += scalar;
    w += scalar;
    return *this;
}

Vector4& Vector4::operator-=(float scalar) {
    x -= scalar;
    y -= scalar;
    z -= scalar;
    w -= scalar;
    return *this;
}

Vector4& Vector4::operator*=(float scalar) {
    x *= scalar; y *= scalar;
    z *= scalar; w *= scalar;
    return *this;
}

Vector4& Vector4::operator/=(float scalar) {
    if (scalar == 0) {
        throw std::runtime_error("Vector4: Division by zero is not possible");
    }
    x /= scalar; y /= scalar;
    z /= scalar; w /= scalar;
    return *this;
}

Vector4 Vector4::operator+(const Vector4& other) const {
    return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4 Vector4::operator-(const Vector4& other) const {
    return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4 Vector4::operator*(const Vector4& other) const {
    return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
}

Vector4 Vector4::operator/(const Vector4& other) const {
    if (other.x == 0 || other.y == 0 || other.z == 0 || other.w == 0) {
        std::ostringstream oss;
        oss << "Vector4: Division by zero (" << other.x << ", " << other.y << ", " << other.z << ", " << other.w << ")";
        throw std::runtime_error(oss.str());
    }
    return Vector4(x / other.x, y / other.y, z / other.z, w / other.w);
}

Vector4 Vector4::operator+(float scalar) const {
    return Vector4(x + scalar, y + scalar, z + scalar, w + scalar);
}

Vector4 Vector4::operator-(float scalar) const {
    return Vector4(x - scalar, y - scalar, z - scalar, w - scalar);
}

Vector4 Vector4::operator*(float scalar) const {
    return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4 Vector4::operator/(float scalar) const {
    if (scalar == 0) {
        throw std::runtime_error("Vector4: Division by zero is not allowed");
    }
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

bool Vector4::operator==(const Vector4& other) const {
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Vector4::operator!=(const Vector4& other) const {
    return !(*this == other);
}

float& Vector4::operator[](int index) {
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;
    if (index == 3) return w;
    throw std::out_of_range("Vector4: index out of range");
}

const float& Vector4::operator[](int index) const {
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;
    if (index == 3) return w;
    throw std::out_of_range("Vector4: index out of range");
}

Vector4 operator+(float scalar, const Vector4& other) {
    return other + scalar;
}

Vector4 operator-(float scalar, const Vector4& other) {
    return Vector4(scalar - other.x, scalar - other.y, scalar - other.z, scalar - other.w);
}

Vector4 operator*(float scalar, const Vector4& other) {
    return other * scalar;
}

Vector4 operator/(float scalar, const Vector4& other) {
    if (other.x == 0 || other.y == 0 || other.z == 0 || other.w == 0) {
        std::ostringstream oss;
        oss << "Vector4: Division by zero (" << other.x << ", " << other.y << ", " << other.z << ", " << other.w << ")";
        throw std::runtime_error(oss.str());
    }
    return Vector4(scalar / other.x, scalar / other.y, scalar / other.z, scalar / other.w);
}