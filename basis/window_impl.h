#pragma once
#ifndef GUID_B58C4C5E7A9343598E1D86A2FE6F8F3A
#define GUID_B58C4C5E7A9343598E1D86A2FE6F8F3A
#ifndef STDAFX_H
#include <list>
#endif
#include "window.h"
#include "thread.h"
#include "critical_section.h"

namespace basis {

class Window::Impl {
public:
    Impl(Window* win) : parent(win) {}
    ~Impl() = default;

    HWND handle() { return m_h; }
    bool create();
    bool destroy();
    void waitToEnd();
    void join();

    //! �E�B���h�E�̃��X�i��o�^
    void hook(IEventHandler *p);

    //! �E�B���h�E�̃��X�i��o�^
    void hook(Listener f);

    //! ���X�i����������
    void unhook(IEventHandler *p);

    //! �t�b�N���ꂽ���X�i�ɃC�x���g��n��
    int broadcast(Message, WPARAM, LPARAM);

    //! ���b�Z�[�W��z�M����
    int dispatch(Window*, Message, WPARAM, LPARAM) const;

private:
    bool exist(const IEventHandler *p);

    //! �E�B���h�E�n���h���Ɋ֘A�t����ꂽ�C�x���g�n���h�����擾���Ϗ�����B
    static LRESULT CALLBACK Dispatch(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

    //! HWND�Ɏd���񂾃|�C���^���擾����
    static Window * GetInstance(HWND hWnd);

    ATOM createWindowAtom(const TCHAR *identifier, WNDPROC proc);

    //! CreateWindow���̃C���X�^���X
    static Window *ConstructingInstance;
    static CriticalSection m_cs;

    Window* parent;
    HWND m_h;
    basis::CThread m_thread;

    using Element = std::pair<IEventHandler*, Listener>;
    std::list<Element> m_hook;
};

}  // namespace
#endif

