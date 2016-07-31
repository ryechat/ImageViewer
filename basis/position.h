#pragma once
#ifndef GUID_70CA2EA915334398A9B81C0554FAE7DA
#define GUID_70CA2EA915334398A9B81C0554FAE7DA

#ifndef STDAFX_H
#include <wtypes.h>
#endif

namespace basis {

class CPosition {
public:
	CPosition() { clear(); }
	explicit CPosition(POINT p) { clear(); pt = p; }
	CPosition(LONG x, LONG y) { clear(); set(x,y); }
	~CPosition() = default;

	explicit CPosition(const CPosition &rhs) = default;
	CPosition& operator= (const CPosition &rhs) = default;

	operator POINT() { return pt; }

	LONG x() { return pt.x; }
	LONG y() { return pt.y; }

	const POINT& base() { return bs; }
	const POINT& pos() { return pt; }

	void setBase(LONG x, LONG y) { pt.x = x, pt.y = y; }
	void set(POINT p) { pt = p; }
	void set(LONG cx, LONG cy) { pt.x = cx; pt.y = cy; }

	void reset() { pt = bs; }
	void clear() { pt.x = pt.y = bs.x = bs.y = 0; }

	void move(POINT d) { pt.x += d.x; pt.y += d.y; }
	void move(LONG dx, LONG dy) { pt.x += dx; pt.y += dy; }

private:
	POINT pt;	// åªç›à íu
	POINT bs;	// èâä˙à íu
};

}

#endif
