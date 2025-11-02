#pragma once

class Number {

public:

	Number();
	Number(int);

	Number operator+(const Number&) const;
	Number operator-(const Number&) const;
	Number operator*(const Number&) const;
	Number operator/(const Number&) const;

	void setValue(int);
	int getValue() const;

private:

	int value_;
};

extern const Number ZERO;
extern const Number ONE;