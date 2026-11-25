#pragma once
#include <iostream>
#include <string>

/**
 * @class SignedLong
 * @brief Класс, реализующий длинное целое число со знаком неограниченной длины.
 *
 * Хранит десятичные цифры в динамически выделенном массиве и знак числа.
 * Поддерживает правило трёх, потоковый ввод/вывод, приведение к встроенному
 * целому типу, арифметические операции и операции сравнения.
 */
class SignedLong {
private:
    int* digits;      ///< Динамический массив десятичных цифр (от младших к старшим или в прямом порядке)
    size_t length;    ///< Количество цифр в числе (для 0 хранится 1 цифра: 0)
    int sign;         ///< Знак числа: -1 (отрицательное), 0 (ноль), 1 (положительное)


    void normalize();
    static int* copyDigits(const int* src, size_t len);
    static int compareAbs(const SignedLong& a, const SignedLong& b);
    static SignedLong addAbs(const SignedLong& a, const SignedLong& b);
    static SignedLong subAbs(const SignedLong& a, const SignedLong& b);

public:
    SignedLong(long long val = 0LL);
    explicit SignedLong(const std::string& str);
    SignedLong(const SignedLong& other);
    SignedLong& operator=(const SignedLong& other);
    ~SignedLong();
    explicit operator long long() const;

    SignedLong operator-() const;



    SignedLong& operator+=(const SignedLong& rhs);
    SignedLong operator+(const SignedLong& rhs) const;

    SignedLong& operator-=(const SignedLong& rhs);
    SignedLong operator-(const SignedLong& rhs) const;

    SignedLong& operator*=(const SignedLong& rhs);
    SignedLong operator*(const SignedLong& rhs) const;

    SignedLong& operator/=(const SignedLong& rhs);
    SignedLong operator/(const SignedLong& rhs) const;



    SignedLong& operator+=(long long rhs);
    SignedLong operator+(long long rhs) const;

    SignedLong& operator-=(long long rhs);
    SignedLong operator-(long long rhs) const;

    SignedLong& operator*=(long long rhs);
    SignedLong operator*(long long rhs) const;

    SignedLong& operator/=(long long rhs);
    SignedLong operator/(long long rhs) const;



    SignedLong& operator++();
    SignedLong operator++(int);

    SignedLong& operator--();
    SignedLong operator--(int);

    // Сравнение с другим SignedLong
    bool operator==(const SignedLong& rhs) const;
    bool operator!=(const SignedLong& rhs) const;
    bool operator<(const SignedLong& rhs) const;
    bool operator<=(const SignedLong& rhs) const;
    bool operator>(const SignedLong& rhs) const;
    bool operator>=(const SignedLong& rhs) const;

    // Сравнение с обычным целым числом (long long)
    bool operator==(long long rhs) const;
    bool operator!=(long long rhs) const;
    bool operator<(long long rhs) const;
    bool operator<=(long long rhs) const;
    bool operator>(long long rhs) const;
    bool operator>=(long long rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const SignedLong& num);
    friend std::istream& operator>>(std::istream& is, SignedLong& num);
};