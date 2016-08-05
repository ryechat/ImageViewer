#include "window_hook.h"
#include "ievent_handler.h"

#ifndef STDAFX_H
#include <algorithm>
#endif

namespace basis {



void WindowHook::
push(IEventHandler * p)
{
    if (p && !exist(p))
        listeners.push_front({ p, {} });
}



void WindowHook::
push(Function f)
{
    listeners.push_front({ nullptr, std::move(f) });
}



void WindowHook::
unhook(IEventHandler *p)
{
    if (!p) return;

    auto i = listeners.begin();
    while (i != listeners.end()) {
        if (i->first == p)
            i = listeners.erase(i);
        else
            ++i;
    }
}



void WindowHook::
clear()
{
	listeners.clear();
}



int WindowHook::
dispatch(Window *win, Message msg, WPARAM wp, LPARAM lp) const
{
    int ret;
	for (auto &i : listeners) {
        if (i.first)
            ret = i.first->onEvent(win, msg, wp, lp);
        else
            ret = i.second(win, msg, wp, lp);
        if (ret)
            return ret;
	}
	return 0;
}



bool WindowHook::
exist(const IEventHandler *p) const
{
	return p && std::any_of(listeners.cbegin(), listeners.cend(),
		[p](const Element& e) { return (e.first == p); }
	);
}

}  // namespace
