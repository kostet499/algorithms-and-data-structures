#include "biginteger.h"

int main() {
    int a = 1600, b = 4;
    BigInteger k (a);
    k /= b;
    std::cout << k;
    return 0;
}