#include "window_hook.h"
#include "ievent_handler.h"

#ifndef STDAFX_H
#include <algorithm>
#endif

namespace basis {
	
void WindowHook::
push_front(IEventHandler *p)
{
	if (!exist(p))
		eventHandlers.push_front(p);
}

void WindowHook::
push_back(IEventHandler *p)
{
	if (!exist(p))
		eventHandlers.push_back(p);
}

void WindowHook::
unhook(IEventHandler *p)
{
	eventHandlers.remove(p);
}

void WindowHook::
clear()
{
	eventHandlers.clear();
}

// ”ñ‚O‚ð‚©‚¦‚·‚Ü‚ÅƒŠƒXƒi‚ð‡‚ÉŒÄ‚Ño‚·
int WindowHook::
dispatch(Window *win, Message msg, WPARAM wp, LPARAM lp) const
{
	for (auto i : eventHandlers) {
		int lr = i->onEvent(win, msg, wp, lp);
		if (lr)
			return lr;
	}
	return 0;
}



bool WindowHook::
exist(const IEventHandler *p) const
{
	return std::any_of(eventHandlers.cbegin(), eventHandlers.cend(),
		[p](const IEventHandler *h) { return (h == p); }
	);
}

}  // namespace
