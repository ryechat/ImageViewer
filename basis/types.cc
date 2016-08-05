#include "types.h"

namespace basis {


Point::operator Size() const {
	return Size(x, y);
}

Rect Size::toRect() {
    return{ 0, 0, x, y };
}

Size::operator Point() const {
	return{ x, y };
}

const Point operator*(const Point &pt, int n) {
	return Point(pt.x * n, pt.y * n);
}

const Point operator/(const Point &pt, int n) {
	return Point(pt.x / n, pt.y / n);
}

const Point operator+(const Point &pt, const Size &s)
{
	return Point(pt.x + s.x, pt.y + s.y);
}

const Point operator+(const Size &s, const Point &pt)
{
	return Point(pt.x + s.x, pt.y + s.y);
}

const Point operator-(const Point &pt, const Size &s)
{
	return Point(pt.x - s.x, pt.y - s.y);
}

const Point operator-(const Size &s, const Point &pt)
{
	return Point(s.x - pt.x, s.y - pt.y);
}

const Size operator*(const Size &s, double d)
{
	return{ static_cast<int>(s.x * d), static_cast<int>(s.y * d) };
}

const Size operator*(double d, const Size &s)
{
	return s * d;
}

const Size operator/(const Size &s, double d)
{
	return{ static_cast<int>(s.x / d), static_cast<int>(s.y / d) };
}

const Size operator/(double d, const Size &s)
{
	return{ static_cast<int>(d / s.x), static_cast<int>(d / s.y) };
}


}  // namespace