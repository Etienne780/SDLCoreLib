#include <cmath>

#include "CoreLib\Math\Matrix.h"
#include "CoreLib\Math\MathUtils.h"
#include "CoreLib\FormatUtils.h"
#include "CoreLib\Math\Vector3.h"
#include "CoreLib\Math\Vector4.h"

#include "CoreLib\Math\Vector2.h"

const Vector2 Vector2::up(0, 1);
const Vector2 Vector2::down(0, -1);
const Vector2 Vector2::left(-1, 0);
const Vector2 Vector2::right(1, 0);
const Vector2 Vector2::one(1, 1);
const Vector2 Vector2::zero(0, 0);

Vector2::Vector2() {
}

Vector2::Vector2(float value)
    : x(value), y(value) {
}

Vector2::Vector2(float x, float y) 
    : x(x), y(y) {
}

Vector2::Vector2(const Vector3& vec) 
    : x(vec.x), y(vec.y) {
}

Vector2::Vector2(const Vector4& vec) 
    : x(vec.x), y(vec.y) {
}

std::string Vector2::ToString() const {
    return FormatUtils::formatString("[{}, {}]", x, y);
}

Matrix Vector2::ToMatrix2x1() const {
    float data[2] = { x, y };
    return Matrix(2, 1, data);
}

Matrix Vector2::ToMatrix1x2() const {
    float data[2] = { x, y };
    return Matrix(1, 2, data);
}

Vector2& Vector2::Set(float _x, float _y) {
    x = _x;
    y = _y;
    return *this;
}

Vector2& Vector2::Set(float fill) {
    x = fill;
    y = fill;
    return *this;
}

Vector2& Vector2::Normalize() {
    float len = Magnitude();
    if (len > 0) {
        *this /= len;
    }
    return *this;
}

Vector2 Vector2::Normalized() const {
    Vector2 copy = *this;
    copy.Normalize();
    return copy;
}

Vector2 Vector2::Normalize(const Vector2& vec) {
    float len = vec.Magnitude();
    if (len > 0) {
        return vec / len;
    }
    return vec;
}

float Vector2::Magnitude() const {
    return std::sqrt(SquaredMagnitude());
}

float Vector2::SquaredMagnitude() const {
    return (x * x + y * y);
}

float Vector2::Dot(const Vector2& other) const {
    return (x * other.x) + (y * other.y);
}

float Vector2::Cross(const Vector2& other) const {
    return x * other.y - y * other.x;
}


float Vector2::Dot(const Vector2& a, const Vector2& b) {
    return (a.x * b.x) + (a.y * b.y);
}

float Vector2::Cross(const Vector2& a, const Vector2& b) {
    return a.x * b.y - a.y * b.x;
}

Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, float t) {
    return Vector2(
        MathUtil::Lerp(a.x, b.x, t),
        MathUtil::Lerp(a.y, b.y, t)
    );
}

float Vector2::Distance(const Vector2& a, const Vector2& b) {
    return (a - b).Magnitude();
}

float Vector2::SquaredDistance(const Vector2& a, const Vector2& b) {
    return (a - b).SquaredMagnitude();
}

Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(const Vector2& other) {
    x *= other.x;
    y *= other.y;
    return *this;
}

Vector2& Vector2::operator/=(const Vector2& other) {
    if (other.x == 0 || other.y == 0) {
        std::ostringstream oss;
        oss << "Vector2: Division by zero (" << other.x << ", " << other.y << ")";
        throw std::runtime_error(oss.str());
    }
    x /= other.x;
    y /= other.y;
    return *this;
}

Vector2& Vector2::operator+=(float scalar) {
    x += scalar;
    y += scalar;
    return *this;
}

Vector2& Vector2::operator-=(float scalar) {
    x -= scalar;
    y -= scalar;
    return *this;
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator/=(float scalar) {
    if (scalar == 0) {
        throw std::runtime_error("Vector2: Division by zero is not possible");
    }
    x /= scalar;
    y /= scalar;
    return *this;
}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(const Vector2& other) const {
    return Vector2(x * other.x, y * other.y);
}

Vector2 Vector2::operator/(const Vector2& other) const {
    if (other.x == 0 || other.y == 0) {
        std::ostringstream oss;
        oss << "Vector2: Division by zero (" << other.x << ", " << other.y << ")";
        throw std::runtime_error(oss.str());
    }
    return Vector2(x / other.x, y / other.y);
}

Vector2 Vector2::operator+(float scalar) const {
    return Vector2(x + scalar, y + scalar);
}

Vector2 Vector2::operator-(float scalar) const {
    return Vector2(x - scalar, y - scalar);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    if (scalar == 0) {
        throw std::runtime_error("Vector2: Division by zero is not possible");
    }
    return Vector2(x / scalar, y / scalar);
}

bool Vector2::operator==(const Vector2& other) const {
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2& other) const {
    return !(*this == other);
}

float& Vector2::operator[](int index) {
    if (index == 0) return x;
    if (index == 1) return y;
    throw std::out_of_range("Vector2: index out of range");
}

const float& Vector2::operator[](int index) const {
    if (index == 0) return x;
    if (index == 1) return y;
    throw std::out_of_range("Vector2: index out of range");
}

Vector2 operator+(float scalar, const Vector2& other) {
    return other + scalar;
}

Vector2 operator-(float scalar, const Vector2& other) {
    return Vector2(scalar - other.x, scalar - other.y);
}

Vector2 operator*(float scalar, const Vector2& other) {
    return other * scalar;
}

Vector2 operator/(float scalar, const Vector2& other) {
    if (other.x == 0 || other.y == 0) {
        std::ostringstream oss;
        oss << "Vector2: Division by zero (" << other.x << ", " << other.y << ")";
        throw std::runtime_error(oss.str());
    }
    return Vector2(scalar / other.x, scalar / other.y);
}