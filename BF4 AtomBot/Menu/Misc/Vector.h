#pragma once
#include <math.h>
template <typename T>
struct Vector2
{
	T x, y;

	//! trivial ctor
	Vector2<T>() {}

	//! setting ctor
	Vector2<T>(const T x0, const T y0) : x(x0), y(y0) {}

	//! array indexing
	T &operator [](unsigned int i)
	{
		return *(&x + i);
	}

	//! array indexing
	const T &operator [](unsigned int i) const
	{
		return *(&x + i);
	}

	//! function call operator
	void operator ()(const T x0, const T y0)
	{
		x = x0; y = y0;
	}

	//! test for equality
	bool operator==(const Vector2<T> &v)
	{
		return (x == v.x && y == v.y);
	}

	//! test for inequality
	bool operator!=(const Vector2<T> &v)
	{
		return (x != v.x || y != v.y);
	}

	//! set to value
	const Vector2<T> &operator =(const Vector2<T> &v)
	{
		x = v.x; y = v.y;
		return *this;
	}

	//! negation
	const Vector2<T> operator -(void) const
	{
		return Vector2<T>(-x, -y);
	}

	//! addition
	const Vector2<T> operator +(const Vector2<T> &v) const
	{
		return Vector2<T>(x + v.x, y + v.y);
	}

	//! subtraction
	const Vector2<T> operator -(const Vector2<T> &v) const
	{
		return Vector2<T>(x - v.x, y - v.y);
	}

	//! uniform scaling
	const Vector2<T> operator *(const T num) const
	{
		Vector2<T> temp(*this);
		return temp *= num;
	}

	//! uniform scaling
	const Vector2<T> operator /(const T num) const
	{
		Vector2<T> temp(*this);
		return temp /= num;
	}

	//! addition
	const Vector2<T> &operator +=(const Vector2<T> &v)
	{
		x += v.x;	y += v.y;
		return *this;
	}

	//! subtraction
	const Vector2<T> &operator -=(const Vector2<T> &v)
	{
		x -= v.x;	y -= v.y;
		return *this;
	}

	//! uniform scaling
	const Vector2<T> &operator *=(const T num)
	{
		x *= num;	y *= num;
		return *this;
	}

	//! uniform scaling
	const Vector2<T> &operator /=(const T num)
	{
		x /= num;	y /= num;
		return *this;
	}

	//! dot product
	T operator *(const Vector2<T> &v) const
	{
		return x*v.x + y*v.y;
	}
};
