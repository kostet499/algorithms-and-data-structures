#include "biginteger.h"

int main() {
    BigInteger k, o;
    std::cin >> k >> o;
    std::cout << k * o << std::endl;
    std::cout << k * 2 - k % o - k / o * o - k * o / o;
    return 0;
}