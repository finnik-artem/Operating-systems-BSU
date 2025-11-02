#include <iostream>

#include "../NumberLib/Number.h"
#include "../VectorLib/Vector.h"

int main() {
    std::cout << "Number::ZERO = " << ZERO.getValue()
        << ", Number::ONE = " << ONE.getValue() << "\n";

    std::cout << "(0,0) = (" << VEC_ZERO.getX().getValue()
        << ", " << VEC_ZERO.getY().getValue() << ")\n";
    std::cout << "(1,1) = (" << VEC_ONE.getX().getValue()
        << ", " << VEC_ONE.getY().getValue() << ")\n";

    Number nx(3);
    Number ny(4);
    Vector a(nx, ny);

    std::cout << "a.x = " << a.getX().getValue() << std::endl;
    std::cout << "a.y = " << a.getY().getValue() << std::endl;
    std::cout << "a.x + a.y = " << (a.getX() + a.getY()).getValue() << std::endl;

    Vector c = a + VEC_ONE;

    std::cout << "a = (" << a.getX().getValue() << ", " << a.getY().getValue() << ")\n";
    std::cout << "c = a + (1,1) = (" << c.getX().getValue()
        << ", " << c.getY().getValue() << ")\n";

    std::cout << "|a| = " << a.getRadius() << "\n";
    std::cout << "angle(a) = " << a.getAngle() << " rad\n";

    return 0;
}