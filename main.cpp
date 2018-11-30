#include "biginteger.h"

int main() {
    BigInteger k(4), s = k--;
    std::cout << s << " " << k;
    return 0;
}