#include "biginteger.h"

int main() {
    BigInteger k(99);
    BigInteger s(2);
    s = k * s - 2;
    --s;
    --s;
    --s;
    std::cout << s;
    return 0;
}