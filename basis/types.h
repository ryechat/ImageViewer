#pragma once
#ifndef GUID_096AB9D76F6D4D22B90A865B3CBC101A
#define GUID_096AB9D76F6D4D22B90A865B3CBC101A

#ifndef STDAFX_H
#include <wtypes.h>
#include <string>
#endif

namespace basis {

class Size;

class Point {
public:
	int x, y;
	Point() : Point(0, 0) {}
	Point(int cx, int cy) : x(cx), y(cy) {}
	explicit Point(POINT pt)
		: x(static_cast<int>(pt.x)), y(static_cast<int>(pt.y)) {}
	explicit operator POINT() const { return{ x, y }; }
	explicit operator Size() const;

	const Point operator+(const Point& rhs) { return Point(*this) += rhs; }
	Point& operator+=(const Point& rhs) {
		x += rhs.x, y += rhs.y;
		return *this;
	}
	const Point operator-(const Point& rhs) { return Point(*this) -= rhs; }
	Point& operator-=(const Point& rhs) {
		x -= rhs.x, y -= rhs.y;
		return *this;
	}
};

class Size {
public:
	int x, y;
	Size() : Size(0, 0) {}
	Size(int dx, int dy) : x(dx), y(dy) {}
	Size(const SIZE &s) : Size(s.cx, s.cy) {}

	explicit operator Point() const;

	operator SIZE() const { return{ x, y }; }

	bool operator==(const Size &s) const {
		return x == s.x && y == s.y;
	}
	bool operator!=(const Size &s) const {
		return x != s.x || y != s.y;
	}
	Size& operator+=(const Size &s) {
		x += s.x, y += s.y; return *this;
	}
	const Size operator+(const Size &s) const { return Size(*this) += s; }
	Size& operator-=(const Size &s) {
		x -= s.x, y -= s.y; return *this;
	}
	const Size operator-(const Size &s) const { return Size(*this) -= s; }

	Size& reset() {
		x = 0, y = 0; return *this;
	}
};



class Rect {
public:
	int left, top, right, bottom;
	Rect() : Rect(0, 0, 0, 0) {}
	Rect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
	Rect(const RECT &rc) : Rect(rc.left, rc.top, rc.right, rc.bottom) {}

	// Bans rect += int
	explicit Rect(int l) : Rect(l, 0, 0, 0) {}

	explicit operator bool() const { return !(left == right || top == bottom); }
	operator RECT() const { return{ left, top, right, bottom }; }

	bool operator==(const Rect &rhs) {
		return left == rhs.left
			&& top == rhs.top
			&& right == rhs.right
			&& bottom == rhs.bottom;
	}

	bool operator!=(const Rect &rhs) {
		return !(*this == rhs);
	}

	//! Adds respective members.
	Rect&operator+=(const Rect &rhs)
	{
		left += rhs.left;
		top += rhs.top;
		right += rhs.right;
		bottom += rhs.bottom;
		return *this;
	}

	//! Adds respective members.
	const Rect operator+(const Rect &rhs) const
	{
		Rect rc{ *this };
		return rc += rhs;
	}

	//! Subtructs respective members.
	Rect&operator-=(const Rect &rhs)
	{
		left -= rhs.left;
		top -= rhs.top;
		right -= rhs.right;
		bottom -= rhs.bottom;
		return *this;
	}

	//! Subtructs respective members.
	const Rect operator-(const Rect &rhs) const
	{
		Rect rc{ *this };
		return rc -= rhs;
	}

	Point lefttop() const
	{
		return{ left, top };
	}

	Point rightbottom() const
	{
		return{ right, bottom };
	}

	/*! Swaps members to be validated rectangle.
	This function makes sure that
	left <= right && top <= bottom.
	If it is an empty rectangle, all members set to be 0.
	@return true if empty rectangle, or false;
	*/
	bool validate() {
		int t;
		if (validate(left, right, t) && validate(top, bottom, t))
			return false;
		left = top = right = bottom = 0;
		return true;
	}

	bool isInclusive(const Point &pt) const {
		return left <= pt.x && pt.x < right &&
			top <= pt.y && pt.y < bottom;
	}

	bool isInclusive(const Rect &rhs) const
	{
		return left <= rhs.left && right >= rhs.right &&
			top <= rhs.top && bottom >= rhs.bottom;
	}

	Rect& unite(const Rect& rhs)
	{
		Rect rc{ rhs };
		if (rc.validate()) {
			validate();
			return *this;
		}
		if (validate())
			return *this = rc;

		left = min(left, rc.left);
		top = min(top, rc.top);
		right = max(right, rc.right);
		bottom = max(bottom, rc.bottom);
		return *this;
	}

	Rect& slide(int dx, int dy) {
		left += dx, right += dx;
		top += dy, bottom += dy;
		return *this;
	}

	Rect& slide(const Size &s) {
		return slide(s.x, s.y);
	}

	bool empty() const {
		return Rect(*this).validate();
	}

	Size size() const { return{ right - left, bottom - top }; }
	int width() const { return right - left; }
	int height() const { return bottom - top; }

	POINT begin() const { return{ left, top }; }
	POINT end() const { return{ right, bottom }; }
	void begin(const POINT &pt) { left = pt.x; top = pt.y; }
	void end(const POINT &pt) { right = pt.x; bottom = pt.y; }

private:
	// Swaps to be l < r. returns l != r
	inline bool validate(int &l, int &r, int &t) {
		if (l < r)
			return true;
		if (l == r)
			return false;
		t = l; l = r; r = t;
		return true;
	}
};



const Size operator*(const Size &s, double d);
const Size operator*(double d, const Size &s);

const Size operator/(const Size &s, double d);
const Size operator/(double d, const Size &s);

const Point operator+(const Point &pt, const Size &s);
const Point operator+(const Size &s, const Point &pt);

const Point operator-(const Point &pt, const Size &s);
const Point operator-(const Size &s, const Point &pt);

const Point operator*(const Point &pt, int n);
const Point operator/(const Point &pt, int n);



}  // namespace

#endif