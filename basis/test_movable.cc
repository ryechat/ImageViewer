#include "movable.h"

namespace basis {

	UNIT_TEST_FUNC(CMovable)
	{
		CMovable a;
		if (a.pt().x != 0 || a.pt().y != 0)
			throw 0;

		CMovable b(&a);
		if (a.moveTo({ 10, 20 }) == false)
			throw 0;
		if (b.move({ 100, 100 }) == false)
			throw 0;
		if (b.pt().x != 110 || b.pt().y != 120)
			throw 0;
		return true;
	}

}  // namespace