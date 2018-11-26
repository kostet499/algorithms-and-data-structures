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

    BigInteger&operator=(BigInteger other);

    BigInteger(int other);

    bool operator==(const BigInteger &other) const;

    bool operator!=(const BigInteger &other) const;

    bool operator<(const BigInteger &other) const;

    bool operator>(const BigInteger &other) const;

    bool operator<=(const BigInteger &other) const;

    bool operator>=(const BigInteger &other) const;

    bool LessAbs(const BigInteger &other) const;

    BigInteger operator+(const BigInteger &other) const;

    BigInteger operator-(const BigInteger &other) const;

    void operator++();

    void operator--();

    std::string toString() const;

    friend std::ostream&operator<<(std::ostream &os, const BigInteger &other);

    friend std::istream&operator>>(std::istream &is, BigInteger &other);

    operator int() const;

    operator bool() const;
private:
    void sum(const std::vector<val_t> &a, const std::vector<val_t> &b, std::vector<val_t> &res) const;

    void dif(const std::vector<val_t> &a, const std::vector<val_t> &b, std::vector<val_t> &res) const;

    std::string valToString(val_t value) const;
private:
    static short power;
    static val_t modder;
    std::vector<val_t> digit;
    bool sign;
};

short BigInteger::power = 10;
val_t BigInteger::modder = 1e10;

void BigInteger::sum(const std::vector<val_t> &a,
                                                const std::vector<val_t> &b, std::vector<val_t> &res) const {
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
        lefting = current_digit / modder;
        res[i] = current_digit % modder;
    }
    if(lefting > 0) {
        res.emplace_back(lefting);
    }
}

void BigInteger::dif(const std::vector<val_t> &a, const std::vector<val_t> &b, std::vector<val_t> &res) const {
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

    return this->LessAbs(other);
}

bool BigInteger::operator>(const BigInteger &other) const {
    return other < *this;
}

bool BigInteger::operator<=(const BigInteger &other) const {
    return !(*this > other);
}

bool BigInteger::operator>=(const BigInteger &other) const {
    return other <= *this;
}

bool BigInteger::LessAbs(const BigInteger &other) const {
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

BigInteger BigInteger::operator+(const BigInteger &other) const {
    BigInteger biggy(0);
    if(sign == other.sign) {
        biggy.sign = sign;
        sum(digit, other.digit, biggy.digit);
        return biggy;
    }

    if(this->LessAbs(other)) {
        biggy.sign = other.sign;
        dif(other.digit, digit, biggy.digit);
    }
    else {
        biggy.sign = sign;
        dif(digit, other.digit, biggy.digit);
    }

    if(biggy.digit.back() == 0) {
        biggy.sign = true;
    }
    return biggy;
}

BigInteger BigInteger::operator-(const BigInteger &other) const {
    BigInteger biggy(0);
    if(sign ^ other.sign) {
        biggy.sign = sign;
        sum(digit, other.digit, biggy.digit);
        return biggy;
    }

    if(this->LessAbs(other)) {
        biggy.sign = !other.sign;
        dif(other.digit, digit, biggy.digit);
    }
    else {
        biggy.sign = sign;
        dif(digit, other.digit, biggy.digit);
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
    if(digit.size() == 1 && digit[0] == 0) {
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

std::istream&operator>>(std::istream &is, const BigInteger &other) {

}

BigInteger::operator int() const {
    return static_cast<int>(digit[0]);
}

BigInteger::operator bool() const {
    BigInteger zero(0);
    return *this != zero;
}
#endif
