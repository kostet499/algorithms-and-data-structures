#include "biginteger.h"

int main() {
    int a = 1590159, b = 159;
    BigInteger k (a);
    k %= b;
    std::cout << k;
    return 0;
}