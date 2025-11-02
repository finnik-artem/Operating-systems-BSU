#include "Vector.h"

Vector::Vector() {
	x_ = ZERO;
	y_ = ZERO;
}

Vector::Vector(Number x, Number y) {
	x_ = x;
	y_ = y;
}

double Vector::getRadius() const {
	return std::sqrt((x_ * x_ + y_ * y_).getValue());
}

double Vector::getAngle() const {
	return std::atan(static_cast<double>((y_/x_).getValue()));
}

Number Vector::getX() const {
	return x_;
}

Number Vector::getY() const {
	return y_;
}

Vector Vector::operator+(const Vector& a) const {
	return Vector(x_ + a.x_, y_ + a.y_);
}

const Vector VEC_ZERO(0, 0);
const Vector VEC_ONE(1, 1);