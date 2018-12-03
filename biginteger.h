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

    BigInteger& operator++();

    BigInteger& operator--();

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

    static val_t make_digit(std::string &s, size_t begin, size_t end);

    static void recursive_delete(BigInteger &temp, const BigInteger &other, std::vector<val_t> &digit);
private:
    static unsigned short power;
    static val_t modder;
    std::vector<val_t> digit;
    bool sign;
};

unsigned short BigInteger::power = 3;
val_t BigInteger::modder = static_cast<val_t>(1e3);

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
    if(other == 0) {
        digit.emplace_back(0);
    }
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
    std::string s;
    while(is >> c) {
        if(!(c >= '0' && c <= '9')) {
            break;
        }
        s += c;
    }

    other.digit.clear();
    for(int i = static_cast<int>(s.size()); i > 0; i -= BigInteger::power) {
        other.digit.emplace_back(BigInteger::make_digit(s, static_cast<size_t>(std::max(0, i - BigInteger::power)),
                                                        static_cast<size_t>(i)));
    }
    if(other.digit.empty()) {
        other = 0;
    }

    return is;
}

val_t BigInteger::make_digit(std::string &s, size_t begin, size_t end) {
    val_t number = 0;
    for(size_t i = begin; i < end; ++i) {
        number = number * 10 + (s[i] - '0');
    }
    return number;
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
    for(size_t i : digit) {
        if(i > 0) {
            return false;
        }
    }
    return true;
}

// bin search
BigInteger operator/(const BigInteger &first, const BigInteger &other) {
    if(other.IsZero()) {
        return {0};
    }
    BigInteger biggy, temp(first);
    std::vector<val_t> revdig;

    BigInteger::recursive_delete(temp, other, revdig);

    biggy.digit.resize(revdig.size());
    for(size_t i = 0; i < revdig.size(); ++i) {
        biggy.digit[i] = revdig[revdig.size() - i - 1];
    }

    biggy.sign = !(first.sign ^ other.sign);
    if(biggy.IsZero()) {
        biggy.sign = true;
    }
    if(biggy.digit.empty()) {
        biggy = 0;
    }
    return biggy;
}

void BigInteger::recursive_delete(BigInteger &temp, const BigInteger &other, std::vector<val_t> &digit) {
    temp.sign = true;
    if(temp.digit.size() < digit.size() || temp.LessAbs(other)) {
        return;
    }
    BigInteger biggy(temp >> (temp.digit.size() - other.digit.size()));
    if(biggy.LessAbs(other)) {
        swap(biggy, biggy << 1);
        biggy.digit[0] = temp.digit[temp.digit.size() - other.digit.size() - 1];
        if(digit.size() != 0) {
            digit.emplace_back(0);
        }
    }

    val_t left = 0;
    val_t right = BigInteger::modder;
    while(right - left > 0) {
        val_t mid = (left + right + 1) >> 1;
        if(!biggy.LessAbs(other * mid)) {
            left = mid;
        }
        else {
            right = mid - 1;
        }
    }
    digit.emplace_back(left);
    size_t prev = temp.digit.size();
    temp.sign = other.sign;
    BigInteger bad(other * left);
    temp -= bad << (temp.digit.size() - bad.digit.size());
    if(temp.IsZero() && prev > bad.digit.size()) {
        for(size_t i = 0; i < prev - bad.digit.size(); ++i) {
            digit.emplace_back(0);
        }
    }
    recursive_delete(temp, other, digit);
}

BigInteger operator%(const BigInteger &first, const BigInteger &other) {
    return first - first / other;
}

BigInteger& BigInteger::operator++() {
    swap(*this, *this + BigInteger(1));
    return *this;
}

BigInteger& BigInteger::operator--() {
    swap(*this, *this - BigInteger(1));
    return *this;
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
    return *this;
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

BigInteger::operator int() const{
    int a = 0;
    for(int i = static_cast<int>(digit.size()) - 1; i > -1; --i) {
        a = a * 10 + static_cast<int>(digit[i]);
    }
    return a;
}
#endif
