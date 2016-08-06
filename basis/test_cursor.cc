#include "cursor.h"

namespace basis {

UNIT_TEST_FUNC(Cursor)
{
    Cursor s;
    if (s.set(Cursor::ID::CROSS) == false)
        throw 0;
    auto pos = s.pos();
    if (s.pos(pos) == false)
        throw 0;
    s.reset();

    return true;
}

}  // namespace