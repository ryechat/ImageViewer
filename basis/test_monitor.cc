#include "monitor.h"

namespace basis {

UNIT_TEST_FUNC(Monitor)
{
	Monitor primary{ {}, false };
	if (primary.getName() != Monitor::GetPrimary().getName())
		throw 0;

	if (primary.getRect() != Monitor::GetPrimary().getRect())
		throw 0;

	return true;
}

}  // namespace