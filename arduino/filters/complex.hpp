#ifndef COMPLEX_HPP
#define COMPLEX_HPP

struct complex_t {
    float re;
    float im;

    constexpr complex_t(float real, float imag) : re(real), im(imag) {};
    constexpr complex_t() : re(0), im(0) {};
    constexpr complex_t(float real) : re(real), im(0) {};

    constexpr complex_t operator+(const complex_t& other) const { return {re + other.re, im + other.im}; };
    constexpr complex_t operator+(float other) const { return {re + other, im}; };
    constexpr complex_t operator-(const complex_t& other) const { return {re - other.re, im - other.im}; };
    constexpr complex_t operator-(float other) const { return {re - other, im}; };
    constexpr complex_t operator*(const complex_t& other) const { return {re*other.re - im*other.im, re*other.im + im*other.re}; };
    constexpr complex_t operator*(float other) const { return {other*re, other*im}; };

    constexpr complex_t& operator+=(const complex_t& other) { re += other.re; im += other.im; return *this; };
    constexpr complex_t& operator-=(const complex_t& other) { re -= other.re; im -= other.im; return *this; };
    constexpr complex_t& operator*=(const complex_t& other) { *this = *this * other; return *this; };

    constexpr float abs() const { return re*re + im*im; };
};

inline constexpr complex_t operator*(float scalar, const complex_t& c) {
    return c * scalar;
}

#endif