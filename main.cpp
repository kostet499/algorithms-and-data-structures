#include "biginteger.h"

int main() {
    BigInteger k(2000000000);
    BigInteger s(2000000000);
    std::cout << k + k + k + k + k;
    return 0;
}