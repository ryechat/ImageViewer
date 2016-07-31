#pragma once
#ifndef GUID_65F96D9BE4D14C599276F442B3038637
#define GUID_65F96D9BE4D14C599276F442B3038637

#ifndef STDAFX_H
#include <wtypes.h>
#include <list>
#endif

namespace basis {

class IEventHandler;
class Window;
enum class Message : int;

//! �E�B���h�E�t�b�N
/*!	<p>���b�Z�\�W���󂯎��C�x���g���X�i���t�b�N�ł���N���X�B
	</p>
	<p>�E�B���h�E�N���X�Ŏg���A�E�B���h�E���������
	�E�B���h�E���g�̃��X�i�݂̂��i�[���邱�ƂɂȂ�B
*/
class WindowHook {
public:
	WindowHook() = default;
	virtual ~WindowHook() = default;

	WindowHook(const WindowHook &) = default;
	WindowHook&operator=(const WindowHook &) = default;
	WindowHook(WindowHook &&) = default;
	WindowHook&operator=(WindowHook &&) = default;

	//! �C�x���g���X�i�N���X��擪�Ƀt�b�N����
	//! �q�N���X�̃��X�i�͂�����Ă�
	void push_front(IEventHandler *p);

	//! �C�x���g���X�i�N���X�𖖔��Ƀt�b�N����
	//! �e�N���X�̃��X�i�͂�����Ă�
	void push_back(IEventHandler *p);

	//! ���X�i����������
	void unhook(IEventHandler *p);

	//! �S����
	void clear();

	//! �擪���珇�Ƀ��b�Z�[�W��z�M����
	int dispatch(Window *win, Message msg,
		WPARAM wp, LPARAM lp) const;

	//! �w�肵�����X�i���o�^����Ă��邩�H
	bool exist(const IEventHandler *p) const;

private:
	using Container = std::list<IEventHandler*>;
	Container eventHandlers;
};

}  // namespace

#endif
