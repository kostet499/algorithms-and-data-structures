#include <iostream>
#include <vector>
#include <string>

#ifndef BIGINTEGER_BIGINTEGER_H
#define BIGINTEGER_BIGINTEGER_H

using val_t = unsigned long long;
class BigInteger {
public:
    BigInteger(const BigInteger &other);

    BigInteger(BigInteger &&other) noexcept;

    friend void swap(BigInteger &a, BigInteger &b);

    friend void swap(BigInteger &a, BigInteger &&b);

    BigInteger&operator=(BigInteger other);

    BigInteger(int other);

    BigInteger();

    bool friend operator==(const BigInteger &first, const BigInteger &other);

    bool friend operator!=(const BigInteger &first, const BigInteger &other);

    bool friend operator<(const BigInteger &first, const BigInteger &other);

    bool friend operator>(const BigInteger &first, const BigInteger &other);

    bool friend operator<=(const BigInteger &first, const BigInteger &other);

    bool friend operator>=(const BigInteger &first, const BigInteger &other);

    BigInteger friend operator+(const BigInteger &first, const BigInteger &other);

    BigInteger friend operator-(const BigInteger &first, const BigInteger &other);

    BigInteger& operator-=(const BigInteger &other);

    BigInteger& operator+=(const BigInteger &other);

    BigInteger& operator*=(const BigInteger &other);

    BigInteger& operator/=(const BigInteger &other);

    BigInteger& operator%=(const BigInteger &other);

    void operator++();

    void operator--();

    BigInteger operator-() const;

    std::string toString() const;

    friend std::ostream&operator<<(std::ostream &os, const BigInteger &other);

    friend std::istream&operator>>(std::istream &is, BigInteger &other);

    explicit operator bool() const;

    explicit operator int() const;

    BigInteger friend operator*(const BigInteger &first, const BigInteger &other);

    BigInteger friend operator/(const BigInteger &first, const BigInteger &other);

    BigInteger friend operator%(const BigInteger &first, const BigInteger &other);

    BigInteger operator<<(size_t num) const;

    BigInteger operator>>(size_t num) const;

    BigInteger operator*(val_t number) const;

    bool IsZero() const;

    const BigInteger operator++(int);

    const BigInteger operator--(int);
private:
    static void sum(const std::vector<val_t> &a, const std::vector<val_t> &b, std::vector<val_t> &res);

    static void dif(const std::vector<val_t> &a, const std::vector<val_t> &b, std::vector<val_t> &res);

    bool LessAbs(const BigInteger &other) const;

    std::string valToString(val_t value) const;
private:
    static unsigned short power;
    static val_t modder;
    std::vector<val_t> digit;
    bool sign;
};

unsigned short BigInteger::power = 1;
val_t BigInteger::modder = static_cast<val_t>(1e1);

void BigInteger::sum(const std::vector<val_t> &a,
                                                const std::vector<val_t> &b, std::vector<val_t> &res) {
    res.resize(std::max(b.size(), a.size()));
    val_t lefting = 0;
    for(size_t i = 0; i < res.size(); ++i) {
        val_t current_digit = lefting;
        if(i < a.size()) {
            current_digit += a[i];
        }
        if(i < b.size()) {
            current_digit += b[i];
        }
        res[i] = current_digit % modder;
        lefting = current_digit / modder;
    }
    if(lefting > 0) {
        res.emplace_back(lefting);
    }
}

void BigInteger::dif(const std::vector<val_t> &a, const std::vector<val_t> &b, std::vector<val_t> &res) {
    res.resize(std::max(b.size(), a.size()), 0);
    val_t lefting = 0;
    size_t last_not_zero = 0;
    for(size_t i = 0; i < res.size(); ++i) {
        val_t have = i < a.size() ? a[i] : 0;
        val_t need = i < b.size() ? b[i] + lefting : lefting;
        if(have >= need) {
            lefting = 0;
            res[i] = have - need;
        }
        else {
            lefting = 1;
            res[i] = (have + modder) - need;
        }
        if(res[i] > 0) {
            last_not_zero = i;
        }
    }
    res.resize(last_not_zero + 1);
}

std::string BigInteger::valToString(val_t value) const {
    std::string res;
    for(size_t i = 0; i < power; ++i){
        res += static_cast<char>(value % 10 + static_cast<int>('0'));
        value /= 10;
    }
    for(size_t i = 0; i < (res.size() >> 1); ++i) {
        std::swap(res[i], res[res.size() - i - 1]);
    }
    return res;
}

BigInteger::BigInteger(const BigInteger &other) {
    digit = other.digit;
    sign = other.sign;
}

BigInteger::BigInteger(BigInteger &&other) noexcept {
    swap(*this, other);
}

BigInteger& BigInteger::operator=(BigInteger other) {
    swap(*this, other);
    return *this;
}

void swap(BigInteger &a, BigInteger &b) {
    using std::swap;
    swap(a.digit, b.digit);
    swap(a.sign, b.sign);
}

BigInteger::BigInteger(int other) {
    sign = other >= 0;
    while(other > 0) {
        digit.emplace_back(other % modder);
        other /= modder;
    }
}

BigInteger::BigInteger() : BigInteger(0){}

bool operator==(const BigInteger &first, const BigInteger &other){
    bool equal = first.sign == other.sign && first.digit.size() == other.digit.size();
    for(int i = static_cast<int>(first.digit.size()) - 1; equal && i > -1; --i) {
        equal &= first.digit[i] == other.digit[i];
    }
    return equal;
}

bool operator!=(const BigInteger &first, const BigInteger &other) {
    return !(first == other);
}

bool operator<(const BigInteger &first, const BigInteger &other) {
    if(!first.sign && other.sign) {
        return true;
    }

    if(first.sign && !other.sign) {
        return false;
    }

    return first.LessAbs(other);
}

bool operator>(const BigInteger &first, const BigInteger &other) {
    return other < first;
}

bool operator<=(const BigInteger &first, const BigInteger &other) {
    return !(first > other);
}

bool operator>=(const BigInteger &first, const BigInteger &other) {
    return other <= first;
}

bool BigInteger::LessAbs(const BigInteger &other) const {
    if(digit.size() < other.digit.size()) {
        return true;
    }

    if(digit.size() > other.digit.size()) {
        return false;
    }

    for(int i = static_cast<int>(digit.size()) - 1; i > -1; --i) {
        if(digit[i] < other.digit[i]) {
            return true;
        }
    }

    return false;
}

BigInteger operator+(const BigInteger &first, const BigInteger &other) {
    BigInteger biggy;
    if(first.sign == other.sign) {
        biggy.sign = first.sign;
        BigInteger::sum(first.digit, other.digit, biggy.digit);
        return biggy;
    }

    if(first.LessAbs(other)) {
        biggy.sign = other.sign;
        BigInteger::dif(other.digit, first.digit, biggy.digit);
    }
    else {
        biggy.sign = first.sign;
        BigInteger::dif(first.digit, other.digit, biggy.digit);
    }

    if(biggy.digit.back() == 0) {
        biggy.sign = true;
    }
    return biggy;
}

BigInteger operator-(const BigInteger &first, const BigInteger &other) {
    BigInteger biggy;
    if(first.sign ^ other.sign) {
        biggy.sign = first.sign;
        BigInteger::sum(first.digit, other.digit, biggy.digit);
        return biggy;
    }

    if(first.LessAbs(other)) {
        biggy.sign = !other.sign;
        BigInteger::dif(other.digit, first.digit, biggy.digit);
    }
    else {
        biggy.sign = first.sign;
        BigInteger::dif(first.digit, other.digit, biggy.digit);
    }

    if(biggy.digit.back() == 0) {
        biggy.sign = true;
    }
    return biggy;
}

std::string BigInteger::toString() const {
    std::string res;
    if(!sign) {
        res += '-';
    }
    if(IsZero()) {
        return "0";
    }

    std::string temp = valToString(digit.back());
    size_t zero = 0;
    while(temp[zero] == '0' && zero < temp.size()) {
        ++zero;
    }
    for(size_t i = zero; i < temp.size(); ++i) {
        res += temp[i];
    }

    for(auto i = static_cast<int>(digit.size()) - 2; i > -1; --i) {
        res += valToString(digit[i]);
    }
    return res;
}

std::ostream&operator<<(std::ostream &os, const BigInteger &other) {
    os << other.toString();
    return os;
}

std::istream&operator>>(std::istream &is, BigInteger &other) {
    char c;
    std::vector<val_t> revdig;
    val_t current = 0;
    while(is >> c) {
        if(!(c >= '0' && c <= '9')) {
            break;
        }
        int value = c - '0';
        current = current * 10 + value;
        if(current > BigInteger::modder) {
            revdig.emplace_back(current % BigInteger::modder);
            current /= BigInteger::modder;
        }
    }
    other.digit.clear();
    for(int i = static_cast<int>(revdig.size()) - 1; i > -1; --i) {
        other.digit.emplace_back(revdig[i]);
    }
    other.digit.emplace_back(current);
}

BigInteger::operator bool() const {
    BigInteger zero(0);
    return *this != zero;
}

BigInteger operator*(const BigInteger &first, const BigInteger &other) {
    BigInteger biggy;

    for(size_t i = 0; i < first.digit.size(); ++i) {
        biggy = biggy + ((other * first.digit[i]) << i);
    }

    biggy.sign = !(first.sign ^ other.sign) || biggy.IsZero();
    return biggy;
}

BigInteger BigInteger::operator*(const val_t number) const {
    BigInteger biggy;
    val_t leftings = 0;
    biggy.digit.resize(digit.size());
    for(size_t i = 0; i < digit.size(); ++i) {
        val_t value = digit[i] * number + leftings;
        biggy.digit[i] = value % modder;
        leftings = value / modder;
    }
    if(leftings > 0) {
        biggy.digit.emplace_back(leftings);
    }
    biggy.sign = sign;
    return biggy;
}

BigInteger BigInteger::operator<<(size_t num) const {
    BigInteger biggy;
    if(IsZero()) {
        return biggy;
    }

    biggy.sign = sign;
    biggy.digit.resize(num + digit.size());
    for(size_t i = 0; i < num; ++i) {
        biggy.digit[i] = 0;
    }

    for(size_t i = 0; i < digit.size(); ++i) {
        biggy.digit[num + i] = digit[i];
    }
    return biggy;
}

BigInteger BigInteger::operator>>(size_t num) const {
    BigInteger biggy;
    if(digit.size() <= num) {
        return biggy;
    }

    biggy.sign = sign;
    biggy.digit.resize(digit.size() - num);
    for(size_t i = 0; i < biggy.digit.size(); ++i) {
        biggy.digit[i] = digit[i + num];
    }

    return biggy;
}

bool BigInteger::IsZero() const {
    return digit.size() == 1 && digit[0] == 0;
}

// bin search
BigInteger operator/(const BigInteger &first, const BigInteger &other) {
    BigInteger biggy;

    return biggy;
}

BigInteger operator%(const BigInteger &first, const BigInteger &other) {
    return first - first / other;
}

void BigInteger::operator++() {
    swap(*this, *this + BigInteger(1));
}

void BigInteger::operator--() {
    swap(*this, *this - BigInteger(1));
}

BigInteger BigInteger::operator-() const {
    if(IsZero()) {
        return *this;
    }

    BigInteger biggy(*this);
    biggy.sign = !this->sign;
    return biggy;
}

void swap(BigInteger& a, BigInteger &&b) {
    swap(a, b);
}

BigInteger& BigInteger::operator+=(const BigInteger &other) {
    swap(*this, *this + other);
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger &other) {
    swap(*this, *this - other);
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger &other) {
    swap(*this, *this * other);
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger &other) {
    swap(*this, *this / other);
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger &other) {
    swap(*this, *this % other);
}

const BigInteger BigInteger::operator++(int) {
    BigInteger biggy(*this);
    ++*this;
    return biggy;
}

const BigInteger BigInteger::operator--(int) {
    BigInteger biggy(*this);
    --*this;
    return biggy;
}
#endif
