#pragma once
#ifndef GUID_65F96D9BE4D14C599276F442B3038637
#define GUID_65F96D9BE4D14C599276F442B3038637

#ifndef STDAFX_H
#include <wtypes.h>
#include <list>
#include <functional>
#endif

namespace basis {

class IEventHandler;
class Window;
enum class Message : int;

//! �E�B���h�E�t�b�N
/*!    <p>���b�Z�\�W���󂯎��C�x���g���X�i���t�b�N�ł���N���X�B
    </p>
    <p>�E�B���h�E�N���X�Ŏg���A�E�B���h�E���������
    �E�B���h�E���g�̃��X�i�݂̂��i�[���邱�ƂɂȂ�B
*/
class WindowHook {
public:
    using Function = std::function<int(Window*, Message, WPARAM, LPARAM)>;
    using Element = std::pair<IEventHandler*, Function>;
    using Container = std::list<Element>;
    WindowHook() = default;
    virtual ~WindowHook() = default;

    WindowHook(const WindowHook &) = default;
    WindowHook&operator=(const WindowHook &) = default;
    WindowHook(WindowHook &&) = default;
    WindowHook&operator=(WindowHook &&) = default;

    //! �E�B���h�E�̃��X�i��o�^
    void push(IEventHandler *p);

    //! �E�B���h�E�̃��X�i��o�^
    void push(Function f);

    //! ���X�i����������
    void unhook(IEventHandler *p);

    /*! ���b�Z�[�W���e���X�i��(�V�����ق�����)�z�M����.
        @return
        ��0��Ԃ����̂ɏo������炻�̒l��Ԃ��B
        ���ׂẴ��X�i��0��Ԃ����Ƃ��A�߂�l��0�B
    */
    int dispatch(Window *win, Message msg, WPARAM wp, LPARAM lp) const;

protected:

    //! �S����
    void clear();

    //! �w�肵�����X�i���o�^����Ă��邩�H
    bool exist(const IEventHandler *p) const;

private:
    // IEventHandler�C���^�t�F�[�X���p������N���X�ւ̃|�C���^�A
    // �������͊֐��I�u�W�F�N�g�Ƃ��āA���X�i�����X�g�����ĊǗ�
    Container listeners;
};

}  // namespace

#endif
