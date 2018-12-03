#include "biginteger.h"

int main() {
    BigInteger k, o(5);
    std::cin >> k;
    if(k == o) {
        std::cout << "kke" << std::endl;
    }
    std::cout << k * 2 - k % o - k / o * o - k * o / o << std::endl;
    std::cout << (k-=o) << std::endl;
    return 0;
}