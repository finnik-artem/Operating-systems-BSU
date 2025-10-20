#pragma once

#include "Number.h"

#ifdef VECTOR_EXPORTS
#define pref __declspec(dllexport)
#else
#define pref __declspec(dllimport)
#endif

#include <cmath>

class pref Vector {

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

extern const pref Vector VEC_ZERO;
extern const pref Vector VEC_ONE;