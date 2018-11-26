#include <iostream>
#include <vector>
#include <string>

#ifndef BIGINTEGER_BIGINTEGER_H
#define BIGINTEGER_BIGINTEGER_H

class BigInteger {
public:
    BigInteger(const BigInteger &other);

    BigInteger(BigInteger &&other) noexcept;

    friend void swap(BigInteger &a, BigInteger &b);

    BigInteger&operator=(BigInteger other);

    BigInteger(const int other);

    bool operator==(const BigInteger &other) const;

    bool operator!=(const BigInteger &other) const;

    bool operator<(const BigInteger &other) const;

    bool operator>(const BigInteger &other) const;

    bool operator<=(const BigInteger &other) const;

    bool operator>=(const BigInteger &other) const;

private:
    static size_t power;
    std::vector<unsigned long long> digit;
    bool sign;
};

size_t BigInteger::power = 32;

BigInteger::BigInteger(const BigInteger &other) {
    digit = other.digit;
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
}

BigInteger::BigInteger(const int other) {
    sign = other > 0;
    digit.emplace_back(other);
}

bool BigInteger::operator==(const BigInteger &other) const {
    bool equal = sign == other.sign && digit.size() == other.digit.size();
    for(size_t i = 0; equal && i < digit.size(); ++i) {
        equal &= digit[i] == other.digit[i];
    }
    return equal;
}

bool BigInteger::operator!=(const BigInteger &other) const {
    return !(*this == other);
}

bool BigInteger::operator<(const BigInteger &other) const {
    if(!sign && other.sign) {
        return true;
    }

    if(sign && !other.sign) {
        return false;
    }

    if(digit.size() < other.digit.size()) {
        return true;
    }

    if(digit.size() > other.digit.size()) {
        return false;
    }

    for(size_t i = 0; i < digit.size(); ++i) {
        if(digit[i] < other.digit[i]) {
            return true;
        }
    }

    return false;
}

bool BigInteger::operator>(const BigInteger &other) const {
    return other < *this;
}

bool BigInteger::operator<=(const BigInteger &other) const {
    if(!sign && other.sign) {
        return true;
    }

    if(sign && !other.sign) {
        return false;
    }

    if(digit.size() < other.digit.size()) {
        return true;
    }

    if(digit.size() > other.digit.size()) {
        return false;
    }

    for(size_t i = 0; i < digit.size(); ++i) {
        if(digit[i] < other.digit[i]) {
            return true;
        }
    }

    return true;
}

bool BigInteger::operator>=(const BigInteger &other) const {
    return other <= *this;
}

#endif
