#pragma once

#include "../NumberLib/Number.h"

#include <cmath>

class Vector {

public:

	Vector();
	Vector(Number, Number);

	double getRadius() const;
	double getAngle() const;

	Number getX() const;
	Number getY() const;

	Vector operator+(const Vector&) const;

private:
	
	Number x_;
	Number y_;

};

extern const Vector VEC_ZERO;
extern const Vector VEC_ONE;