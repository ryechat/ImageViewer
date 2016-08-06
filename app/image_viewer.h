#pragma once
#ifndef GUID_12D296409004455CB5F690D46983D9E2
#define GUID_12D296409004455CB5F690D46983D9E2
#ifndef STDAFX_H
#include <memory>
#include <list>
#endif

#include "types.h"
#include "window.h"
#include "file_path.h"
#include "surface.h"

namespace basis { class CKey; }

/*! �摜�r���[�A�E�A�v���P�[�V����.
    ��{�I�Ȃӂ�܂��́A�p������Window�N���X�ɕ키�B
    CImageViewer().create().show().wait()�A�Ŏ��s�\�B
    �E�B���h�E�̍쐬�A����у��b�Z�[�W�|���v��Window�N���X��
    ���[�J�[�X���b�h���s���B
*/
namespace image_viewer {

class CListItem;
enum class ID : int;

class CImageViewer final : private basis::Window {
public:
    using Content   = CListItem;
    using Element    = std::shared_ptr<Content>;
    using ListTy    = std::list<Element>;
    using iterator  = ListTy::iterator;
    using const_iterator = ListTy::const_iterator;

    using Size        = basis::Size;
    using Rect      = basis::Rect;
    using Window    = basis::Window;
    using Message    = basis::Message;
    using WM        = basis::Message;
    using FilePath  = basis::CFilePath;
    using Surface   = basis::Surface;

    CImageViewer();
    ~CImageViewer();

    CImageViewer(CImageViewer&) = delete;
    CImageViewer(CImageViewer&&) = delete;
    CImageViewer& operator=(CImageViewer&) = delete;
    CImageViewer& operator=(CImageViewer&&) = delete;

    static constexpr int VERSION = 1000;
    static constexpr TCHAR *NAME_VERSION = TEXT("Stella Vista ver1.0");

    //! �E�B���h�E�𐶐����A���b�Z�[�W�|���v�X���b�h���ғ�������
    CImageViewer& create() {
        Window::create();
        return *this;
    }

    //! �E�B���h�E��\��
    CImageViewer& show(int nShow = SW_SHOW) {
        Window::show(nShow);
        return *this;
    }

    //! �E�B���h�E���\��
    CImageViewer& hide() {
        Window::hide();
        return *this;
    }

    //! �E�B���h�E��������܂őҋ@����
    void waitToEnd() {
        Window::waitToEnd();
    }

    //! ���܂̂Ƃ���_�~�[�֐�
    int exitCode() { return m_exitCode; }

private:
    virtual int onEvent(Window*, Message, WPARAM, LPARAM) final;
    int onCommand(WPARAM wp);
    int onPaint();

    bool setPath(FilePath path);
    bool setCurrent(iterator itr);
    void showPrev();
    void showNext();
    void showFirst();
    void showLast();
    bool helper_show_must_loop(iterator iNext, const_iterator iLimit);

    //! Gets the key, that was set to be related to the command ID.
    basis::CKey getKey(ID id, int n);

    void saveload(bool bSave);

    ID getSortWay() const;
    bool isMultiMaximized() const;
    bool toggleScreen();

    Size getDrawSize(const Size &image_size) const;
    Rect getDrawRect() const;
    Rect getDrawRect(const Size &drawSize) const;
    void invalidate() const;
    void invalidate(const Rect &rc) const { Window::invalidate(rc); }
    void invalidate_image() const;
    void move_image(Size diff);
    void reloadCurrent();

    void update() const;
    bool updateTitleBar() const;

    Size m_offset;
    Rect m_drawingRect;

    //! Indicates whether show or not window titlebar temporally.
    bool m_bTemporaryShowTitle;

    FilePath m_dir;
    FilePath m_lastPath;
    Surface m_offscreen;
    Surface m_backbuffer;

    std::basic_string<TCHAR> m_captionConfirmDelete;
    std::basic_string<TCHAR> m_textConfirmDelete;

    int m_exitCode;
    mutable bool isImageInvalidated;

    class Profile;
    std::unique_ptr<Profile> profile;

    class ContextMenu;
    std::unique_ptr<ContextMenu> menu;

    class Control;
    std::unique_ptr<Control> control;

    class Filer;
    std::unique_ptr<Filer> filer;

    class CDrawList;
    std::unique_ptr<CDrawList> list;

    class Loader;
    std::unique_ptr<Loader> loader;
};

}  // namesapce

#endif
