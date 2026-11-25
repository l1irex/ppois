#include "SignedLong.h"
#include <algorithm>
#include <cctype>
#include <climits>
#include <stdexcept>

int* SignedLong::copyDigits(const int* src, size_t len) {
    int* dst = new int[len];
    for (size_t i = 0; i < len; ++i) dst[i] = src[i];
    return dst;
}

void SignedLong::normalize() {
    if (!digits || length == 0) {
        delete[] digits;
        digits = new int[1] { 0 };
        length = 1;
        sign = 0;
        return;
    }
    while (length > 1 && digits[length - 1] == 0) --length;
    bool zero = true;
    for (size_t i = 0; i < length; ++i) {
        if (digits[i] != 0) { zero = false; break; }
    }
    if (zero) {
        delete[] digits;
        digits = new int[1] { 0 };
        length = 1;
        sign = 0;
    }
    else if (sign == 0) {
        sign = 1;
    }
}

int SignedLong::compareAbs(const SignedLong& a, const SignedLong& b) {
    if (a.length != b.length) return (a.length < b.length) ? -1 : 1;
    for (size_t i = a.length; i > 0; --i) {
        if (a.digits[i - 1] != b.digits[i - 1]) {
            return (a.digits[i - 1] < b.digits[i - 1]) ? -1 : 1;
        }
    }
    return 0;
}

SignedLong SignedLong::addAbs(const SignedLong& a, const SignedLong& b) {
    const size_t n = std::max(a.length, b.length);
    SignedLong res;
    delete[] res.digits;
    res.digits = new int[n + 1] {};
    res.length = n + 1;

    int carry = 0;
    for (size_t i = 0; i < n; ++i) {
        int s = carry + (i < a.length ? a.digits[i] : 0) + (i < b.length ? b.digits[i] : 0);
        res.digits[i] = s % 10;
        carry = s / 10;
    }
    res.digits[n] = carry;
    res.sign = 1;
    res.normalize();
    return res;
}

SignedLong SignedLong::subAbs(const SignedLong& a, const SignedLong& b) {
    SignedLong res;
    delete[] res.digits;
    res.digits = new int[a.length] {};
    res.length = a.length;

    int borrow = 0;
    for (size_t i = 0; i < a.length; ++i) {
        int d = a.digits[i] - borrow - (i < b.length ? b.digits[i] : 0);
        if (d < 0) { d += 10; borrow = 1; }
        else { borrow = 0; }
        res.digits[i] = d;
    }
    res.sign = 1;
    res.normalize();
    return res;
}

SignedLong::SignedLong(long long val) : digits(nullptr), length(0), sign(0) {
    if (val == 0LL) {
        digits = new int[1] { 0 };
        length = 1;
        return;
    }
    sign = (val < 0LL) ? -1 : 1;
    unsigned long long absV = (val < 0LL)
        ? static_cast<unsigned long long>(-(val + 1LL)) + 1ULL
        : static_cast<unsigned long long>(val);

    std::string s = std::to_string(absV);
    length = s.size();
    digits = new int[length];
    for (size_t i = 0; i < length; ++i) digits[i] = s[length - 1 - i] - '0';
    normalize();
}

SignedLong::SignedLong(const std::string& str) : digits(nullptr), length(0), sign(0) {
    size_t i = 0;
    while (i < str.size() && std::isspace(static_cast<unsigned char>(str[i]))) ++i;
    if (i == str.size()) throw std::invalid_argument("Empty string");

    int sgn = 1;
    if (str[i] == '+' || str[i] == '-') {
        if (str[i] == '-') sgn = -1;
        ++i;
    }
    while (i < str.size() && str[i] == '0') ++i;

    size_t start = i, cnt = 0;
    while (i < str.size() && std::isdigit(static_cast<unsigned char>(str[i]))) { ++cnt; ++i; }
    while (i < str.size() && std::isspace(static_cast<unsigned char>(str[i]))) ++i;

    if (i != str.size() || (cnt == 0 && str.find('0') == std::string::npos)) {
        throw std::invalid_argument("Invalid SignedLong string");
    }
    if (cnt == 0) {
        digits = new int[1] { 0 };
        length = 1;
        return;
    }

    length = cnt;
    sign = sgn;
    digits = new int[length];
    for (size_t k = 0; k < length; ++k) digits[k] = str[start + length - 1 - k] - '0';
    normalize();
}

SignedLong::SignedLong(const SignedLong& other)
    : digits(copyDigits(other.digits, other.length)), length(other.length), sign(other.sign) {}

SignedLong& SignedLong::operator=(const SignedLong& other) {
    if (this != &other) {
        int* copy = copyDigits(other.digits, other.length);
        delete[] digits;
        digits = copy;
        length = other.length;
        sign = other.sign;
    }
    return *this;
}

SignedLong::~SignedLong() {
    delete[] digits;
    digits = nullptr;
}

SignedLong::operator long long() const {
    if (sign == 0) return 0LL;
    unsigned long long acc = 0ULL;
    for (size_t i = length; i > 0; --i) {
        const unsigned long long d = static_cast<unsigned long long>(digits[i - 1]);
        if (acc > (ULLONG_MAX - d) / 10ULL) throw std::overflow_error("Overflow long long");
        acc = acc * 10ULL + d;
    }
    if (sign > 0) {
        if (acc > static_cast<unsigned long long>(LLONG_MAX)) throw std::overflow_error("Overflow LLONG_MAX");
        return static_cast<long long>(acc);
    }
    const unsigned long long maxNeg = static_cast<unsigned long long>(LLONG_MAX) + 1ULL;
    if (acc > maxNeg) throw std::overflow_error("Overflow LLONG_MIN");
    return (acc == maxNeg) ? LLONG_MIN : -static_cast<long long>(acc);
}

SignedLong SignedLong::operator-() const {
    SignedLong r(*this);
    if (r.sign != 0) r.sign = -r.sign;
    return r;
}

SignedLong& SignedLong::operator+=(const SignedLong& rhs) {
    if (rhs.sign == 0) return *this;
    if (sign == 0) return (*this = rhs);
    if (sign == rhs.sign) {
        const int s = sign;
        *this = addAbs(*this, rhs);
        sign = s;
    }
    else {
        const int cmp = compareAbs(*this, rhs);
        if (cmp == 0) { *this = SignedLong(0LL); }
        else if (cmp > 0) { const int s = sign; *this = subAbs(*this, rhs); sign = s; }
        else { const int s = rhs.sign; *this = subAbs(rhs, *this); sign = s; }
    }
    return *this;
}

SignedLong SignedLong::operator+(const SignedLong& rhs) const { SignedLong r(*this); return r += rhs; }
SignedLong& SignedLong::operator-=(const SignedLong& rhs) { return (*this += (-rhs)); }
SignedLong SignedLong::operator-(const SignedLong& rhs) const { SignedLong r(*this); return r -= rhs; }

SignedLong& SignedLong::operator*=(const SignedLong& rhs) {
    if (sign == 0 || rhs.sign == 0) { *this = SignedLong(0LL); return *this; }
    const int resSign = sign * rhs.sign;
    const size_t resLen = length + rhs.length;
    int* prod = new int[resLen] {};
    for (size_t i = 0; i < length; ++i) {
        int carry = 0;
        for (size_t j = 0; j < rhs.length; ++j) {
            int cur = prod[i + j] + digits[i] * rhs.digits[j] + carry;
            prod[i + j] = cur % 10;
            carry = cur / 10;
        }
        prod[i + rhs.length] += carry;
    }
    delete[] digits;
    digits = prod;
    length = resLen;
    sign = resSign;
    normalize();
    return *this;
}

SignedLong SignedLong::operator*(const SignedLong& rhs) const { SignedLong r(*this); return r *= rhs; }

SignedLong& SignedLong::operator/=(const SignedLong& rhs) {
    if (rhs.sign == 0) throw std::domain_error("Division by zero");
    if (sign == 0 || compareAbs(*this, rhs) < 0) { *this = SignedLong(0LL); return *this; }

    const int resSign = sign * rhs.sign;
    SignedLong div(rhs); div.sign = 1;
    SignedLong q;
    delete[] q.digits;
    q.digits = new int[length] {};
    q.length = length;
    q.sign = 1;

    SignedLong cur;
    for (size_t i = length; i > 0; --i) {
        const int d = digits[i - 1];
        if (cur.sign == 0) {
            cur = SignedLong(static_cast<long long>(d));
        }
        else {
            int* nd = new int[cur.length + 1];
            nd[0] = d;
            for (size_t k = 0; k < cur.length; ++k) nd[k + 1] = cur.digits[k];
            delete[] cur.digits;
            cur.digits = nd;
            cur.length += 1;
            cur.sign = 1;
            cur.normalize();
        }
        int cnt = 0;
        while (compareAbs(cur, div) >= 0) { cur = subAbs(cur, div); ++cnt; }
        q.digits[i - 1] = cnt;
    }
    q.sign = resSign;
    q.normalize();
    *this = q;
    return *this;
}

SignedLong SignedLong::operator/(const SignedLong& rhs) const { SignedLong r(*this); return r /= rhs; }

SignedLong& SignedLong::operator+=(long long rhs) { return (*this += SignedLong(rhs)); }
SignedLong SignedLong::operator+(long long rhs) const { return (*this + SignedLong(rhs)); }
SignedLong& SignedLong::operator-=(long long rhs) { return (*this -= SignedLong(rhs)); }
SignedLong SignedLong::operator-(long long rhs) const { return (*this - SignedLong(rhs)); }
SignedLong& SignedLong::operator*=(long long rhs) { return (*this *= SignedLong(rhs)); }
SignedLong SignedLong::operator*(long long rhs) const { return (*this * SignedLong(rhs)); }
SignedLong& SignedLong::operator/=(long long rhs) { return (*this /= SignedLong(rhs)); }
SignedLong SignedLong::operator/(long long rhs) const { return (*this / SignedLong(rhs)); }

SignedLong& SignedLong::operator++() { *this += 1LL; return *this; }
SignedLong SignedLong::operator++(int) { SignedLong t(*this); ++(*this); return t; }
SignedLong& SignedLong::operator--() { *this -= 1LL; return *this; }
SignedLong SignedLong::operator--(int) { SignedLong t(*this); --(*this); return t; }

bool SignedLong::operator==(const SignedLong& rhs) const {
    if (sign != rhs.sign || length != rhs.length) return false;
    for (size_t i = 0; i < length; ++i) if (digits[i] != rhs.digits[i]) return false;
    return true;
}
bool SignedLong::operator!=(const SignedLong& rhs) const { return !(*this == rhs); }
bool SignedLong::operator<(const SignedLong& rhs) const {
    if (sign != rhs.sign) return sign < rhs.sign;
    if (sign == 0) return false;
    const int c = compareAbs(*this, rhs);
    return (sign > 0) ? (c < 0) : (c > 0);
}
bool SignedLong::operator<=(const SignedLong& rhs) const { return (*this < rhs) || (*this == rhs); }
bool SignedLong::operator>(const SignedLong& rhs) const { return !(*this <= rhs); }
bool SignedLong::operator>=(const SignedLong& rhs) const { return !(*this < rhs); }

bool SignedLong::operator==(long long rhs) const { return *this == SignedLong(rhs); }
bool SignedLong::operator!=(long long rhs) const { return *this != SignedLong(rhs); }
bool SignedLong::operator<(long long rhs) const { return *this < SignedLong(rhs); }
bool SignedLong::operator<=(long long rhs) const { return *this <= SignedLong(rhs); }
bool SignedLong::operator>(long long rhs) const { return *this > SignedLong(rhs); }
bool SignedLong::operator>=(long long rhs) const { return *this >= SignedLong(rhs); }

std::ostream& operator<<(std::ostream& os, const SignedLong& num) {
    if (num.sign < 0) os << '-';
    for (size_t i = num.length; i > 0; --i) os << num.digits[i - 1];
    return os;
}

std::istream& operator>>(std::istream& is, SignedLong& num) {
    std::string tok;
    if (!(is >> tok)) return is;
    try { num = SignedLong(tok); }
    catch (const std::invalid_argument&) { is.setstate(std::ios::failbit); }
    return is;
}