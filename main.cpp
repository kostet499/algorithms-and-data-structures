#include "biginteger.h"

int main() {
    BigInteger k, o;
    std::cin >> k >> o;
    if(k == o) {
        std::cout << "kke" << std::endl;
    }
    std::cout << k * 2 - k % o - k / o * o - k * o / o << std::endl;
    std::cout << ++BigInteger(-1231231)<< std::endl;
    return 0;
}