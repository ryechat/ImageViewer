#include "key_map.h"
#ifndef unless
#define unless(a) if(!(a))
#endif

namespace basis {

UNIT_TEST_FUNC(CKeyMap)
{

    CKeyMap map;

    // 例外送出する操作
    try {
        if (map.append(CKey(), 3))
            throw 0;
    }
    catch (std::invalid_argument& e) {
        (void)e;
    }

    try {
        if (map.append(VK_SHIFT, 0))
            throw 0;
    }
    catch (std::invalid_argument& e) {
        (void)e;
    }

    unless(map.size() == 0)
        throw 0;
    unless(map.capacity() > 0)
        throw 0;

    unless(map.append(VK_SHIFT, 5))
        throw 0;
    unless(map.getCommand(VK_SHIFT, false) == 5)
        throw 0;
    unless(map.getKey(5, 0) == VK_SHIFT)
        throw 0;
    unless(map.size() == 1)
        throw 0;

    unless(map.append(CKey::SHIFT + VK_F1, 1))
        throw 0;
    unless(map.append(CKey::CTRL | CKey::SHIFT | VK_F1, 2))
        throw 0;
    // ※１
    unless(map.getCommand(CKey::SHIFT + VK_F1, false) == 1)
        throw 0;
    unless(map.getCommand(CKey::SHIFT + CKey::CTRL + VK_F1, false) == 2)
        throw 0;

    unless(map.size() == 3)
        throw 0;
    map.eraseByCommand(5);
    unless(map.size() == 2)
        throw 0;
    unless(map.getCommand(VK_SHIFT, false) == 0)
        throw 0;
    unless(map.getKey(5, 0) == CKey())
        throw 0;

    // ※１　消去後の整合性チェック
    unless(map.getCommand(CKey::SHIFT + VK_F1, false) == 1)
        throw 0;
    unless(map.getCommand(CKey::SHIFT + CKey::CTRL + VK_F1, false) == 2)
        throw 0;

    return true;
}

}  // namespace