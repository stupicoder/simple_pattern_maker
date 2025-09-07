#pragma once

#include <cmath>
#include <algorithm>

constexpr double PI = 3.14159265358979323846;
constexpr double PI_HALF = PI / 2.0;
constexpr double PI_INV = 1.0 / PI;

constexpr double DEG_TO_RAD = PI / 180.0;
constexpr double RAD_TO_DEG = 180.0 / PI;

float DegreeToRadian(float degrees) {
    return degrees * DEG_TO_RAD;
}

float RadianToDegree(float radians) {
    return radians * RAD_TO_DEG;
}

template<typename T>
T clamp(T value, T min, T max)
{
    return std::max(min, std::min(value, max));
}

struct vec2i
{
    int x;
    int y;

    vec2i operator+(const vec2i& other) const {
        return {x + other.x, y + other.y};
    }
    vec2i operator-(const vec2i& other) const {
        return {x - other.x, y - other.y};
    }
    vec2i operator*(int scalar) const {
        return {x * scalar, y * scalar};
    }
    vec2i operator/(int scalar) const {
        return {x / scalar, y / scalar};
    }
};

struct vec2f 
{
    float x;
    float y;

    vec2f() : x(0.0f), y(0.0f) {}
    vec2f(float x, float y) : x(x), y(y) {}
    vec2f(const vec2i& other) 
        : x(static_cast<float>(other.x)), y(static_cast<float>(other.y)) {}

    vec2f operator+(const vec2f& other) const {
        return {x + other.x, y + other.y};
    }
    vec2f operator-(const vec2f& other) const {
        return {x - other.x, y - other.y};
    }
    vec2f operator*(const vec2f& other) const {
        return {x * other.x, y * other.y};
    }
    vec2f operator/(const vec2f& other) const {
        return {x / other.x, y / other.y};
    }
    vec2f operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }
    vec2f operator/(float scalar) const {
        return {x / scalar, y / scalar};
    }

};

struct vec3f 
{
    float x;
    float y;
    float z;

    vec3f operator+(const vec3f& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    vec3f& operator+=(const vec3f& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    vec3f operator-(const vec3f& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
    vec3f& operator-=(const vec3f& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    vec3f operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }
    vec3f operator*(const vec3f& other) const {
        return {x * other.x, y * other.y, z * other.z};
    }
    vec3f& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }
    vec3f operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }
    vec3f operator/(const vec3f& other) const {
        return {x / other.x, y / other.y, z / other.z};
    }
    vec3f& operator/=(float scalar) {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }

    static const vec3f Zero;
    static const vec3f One;
};
const vec3f vec3f::Zero = {0.0f, 0.0f, 0.0f};
const vec3f vec3f::One = {1.0f, 1.0f, 1.0f};

template<typename T>
float length(const T& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}
template<vec3f>
float length(const vec3f& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
float dot(const vec3f& a, const vec3f& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

struct mat2f
{
    union {
        float m[2][2];
        struct {
            float m00, m01;
            float m10, m11;
        };
    };

    mat2f operator*(const mat2f& other) const {
        mat2f result = {};
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                result.m[i][j] = m[i][0] * other.m[0][j] + m[i][1] * other.m[1][j];
            }
        }
        return result;
    }
    vec2f operator*(const vec2f& vec) const {
        return {
            m00 * vec.x + m01 * vec.y,
            m10 * vec.x + m11 * vec.y
        };
    }
    mat2f operator*(float scalar) const {
        mat2f result = {};
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }
    mat2f Inverse() const {
        mat2f result = {};
        float det = m00 * m11 - m01 * m10;
        if (det != 0.0f) {
            result.m00 = m11 / det;
            result.m01 = -m01 / det;
            result.m10 = -m10 / det;
            result.m11 = m00 / det;
        }
        return result;
    }

    static const mat2f Identity;
};
const mat2f mat2f::Identity = {1.0f, 0.0f, 0.0f, 1.0f};

