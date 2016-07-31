#pragma once
#ifndef GUID_26B3AE867F134EBEBDD64CF3A85974B6
#define GUID_26B3AE867F134EBEBDD64CF3A85974B6

#ifndef STDAFX_H
#include <wtypes.h>
#endif

namespace basis {

class Window;
enum class Message : int;

/*!	ウィンドウメッセージのリスナインタフェース.
	ウィンドウクラスの各インスタンス、イベントリスナはこれを継承・実装する。
*/
class IEventHandler {
public:
	virtual ~IEventHandler() = default;
	virtual int onEvent(Window *, Message, WPARAM, LPARAM) = 0;
};

}  // namespace

#endif
