#include "Number.h"

Number::Number() {
	value_ = 0;
}

Number::Number(int value) {
	value_ = value;
}

Number Number::operator+(const Number& a) const {
	return Number(value_ + a.value_);
}

Number Number::operator-(const Number& a) const {
	return Number(value_ - a.value_);
}

Number Number::operator*(const Number& a) const {
	return Number(value_ * a.value_);
}

Number Number::operator/(const Number& a) const {
	return Number(value_ / a.value_);
}

void Number::setValue(int value) {
	value_ = value;
}

int Number::getValue() const {
	return value_;
}


const Number ZERO(0);
const Number ONE(1);
