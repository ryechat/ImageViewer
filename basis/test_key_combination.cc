#include "key_combination.h"
#include "stdfnc.h"

#ifndef unless
#define unless(x) if(!(x))
#endif

namespace basis {

UNIT_TEST_FUNC(CKey)
{
	CKey key;

	if (key)
		throw 0;
	if (key.vkey())
		throw 0;
	if (key.flags())
		throw 0;

	key = VK_SHIFT;
	if (key != VK_SHIFT)
		throw 0;
	if (key.vkey() != VK_SHIFT)
		throw 0;
	unless(key & VK_SHIFT)
		throw 0;
	unless(key.flags() & CKey::SHIFT)
		throw 0;
	unless(key & CKey::SHIFT)
		throw 0;

	key |= VK_CONTROL;
	if (key == VK_CONTROL)
		throw 0;
	unless(key & VK_CONTROL)
		throw 0;
	if (key.vkey() != VK_CONTROL)
		throw 0;

	if (!key.isModifier())
		throw 0;

	key |= VK_MENU;
	unless(key.vkey() == VK_MENU)
		throw 0;
	unless(key & VK_MENU)
		throw 0;
	unless(key.flags() & CKey::SHIFT)
		throw 0;
	unless(key.flags() & CKey::ALT)
		throw 0;
	unless(key.flags() & CKey::CTRL)
		throw 0;
	unless(key & CKey::SHIFT)
		throw 0;
	unless(key & CKey::ALT)
		throw 0;
	unless(key & CKey::CTRL)
		throw 0;
	if (key.flags() != (CKey::SHIFT | CKey::ALT | CKey::CTRL))
		throw 0;

	key = CKey(VK_LEFT) | key;
	if (key.vkey() != VK_LEFT)
		throw 0;
	if (key.flags() != (CKey::SHIFT | CKey::ALT | CKey::CTRL))
		throw 0;

	// ñ≥å¯Ç»ëÄçÏ
	try {
		key = CKey(VK_LEFT) | CKey(VK_RIGHT);
		throw 0;
	}
	catch (std::exception& e) {
		(void)e;
	}

	PCTSTR p = TEXT("Shift + F, Alt + 0xFF");
	if (!key.read(p))
		throw 0;
	unless(key.vkey() == 'F')
		throw 0;
	unless(key & VK_SHIFT)
		throw 0;
	unless(key.flags() == CKey::SHIFT)
		throw 0;

	key.read(p + 9);
	unless(key.vkey() == 0xFF)
		throw 0;
	unless(key & CKey::ALT)
		throw 0;
	unless(key.flags() == CKey::ALT)
		throw 0;

	key.read(TEXT("Ctrl + Shift + Alt + Esc"));
	unless(key.vkey() == VK_ESCAPE)
		throw 0;
	unless(key.flags() == (CKey::CTRL | CKey::ALT | CKey::SHIFT))
		throw 0;
	unless((key & VK_CONTROL)
		&& (key & VK_MENU)
		&& (key && VK_SHIFT))
		throw 0;

	return true;
}

}  // namespace