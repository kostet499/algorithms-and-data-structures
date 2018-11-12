#include "JsonField.h"
#include <iostream>

int main() {
    JsonField field("field.json");
    std::cout << field[2].x << " " << field[2].y << std::endl;
    std::cout << field.width() << " " << field.height();
    return 0;
}