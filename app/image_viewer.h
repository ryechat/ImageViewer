#pragma once
#ifndef GUID_12D296409004455CB5F690D46983D9E2
#define GUID_12D296409004455CB5F690D46983D9E2

#ifndef STDAFX_H
#include <memory>
#include <list>
#endif

#include "thread.h"
#include "types.h"
#include "window.h"
#include "file_path.h"
#include "surface.h"
#include "movable.h"

namespace image_viewer {

class CListItem;

class CImageViewer final : public basis::Window {
public:
	using Contents	= CListItem;
	using Element	= std::shared_ptr<Contents>;
	using ListTy	= std::list<Element>;
	using iterator  = ListTy::iterator;
	using const_iterator = ListTy::const_iterator;

	using Window	= basis::Window;
	using Message	= basis::Message;
	using WM		= basis::Message;
	using FilePath  = basis::CFilePath;
	using Size		= basis::Size;
	using Rect      = basis::Rect;
	using Surface   = basis::Surface;

	CImageViewer(char *commandLine, int showWindow);
	~CImageViewer();

	CImageViewer(CImageViewer&) = delete;
	CImageViewer(CImageViewer&&) = delete;
	CImageViewer& operator=(CImageViewer&) = delete;
	CImageViewer& operator=(CImageViewer&&) = delete;

	static constexpr TCHAR *NAME_VERSION = TEXT("RyeImageViewer ver0.8");

	//! Event listener.
	virtual int onEvent(Window*, Message, WPARAM, LPARAM) override;

	//! Specifies a directory or file to show images.
	bool setPath(basis::CFilePath path);

	//! Shows previous image.
	void showPrev();

	//! Shows next image.
	void showNext();

	//! Shows first image in the directory.
	void showFirst();

	//! Shows last image in the directory.
	void showLast();

	//! Shows a specified image.
	bool setCurrent(const iterator &itr);

	int exitCode() { return m_exitCode; }

private:
	int onCommand(WPARAM wp);
	int onPaint();
	bool helper_show_must_loop(iterator iNext, const_iterator iLimit);

	void saveload(bool bSave);

	bool isMultiMaximized() const;
	bool toggleScreen();

	Size getDrawSize(const Size &image_size);
	Rect getDrawRect();
	Rect getDrawRect(const Size &drawSize);
	void invalidate() const;
	void invalidate(const Rect &rc) const;
	void invalidate_image();

	void move_image(Size diff) {
		m_offset += diff;
		invalidate(getDrawRect().unite(m_drawingRect));
	}

	void update() const;
	bool updateTitleBar() const;

	void reloadCurrent();

	basis::Size m_offset;
	basis::Rect m_drawingRect;

	//! Indicates whether show or not window titlebar temporally.
	bool m_bTemporaryShowTitle;

	FilePath m_dir;
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
	std::unique_ptr<Control> input;

	class CFiler;
	std::unique_ptr<CFiler> filer;

	class CDrawList;
	std::unique_ptr<CDrawList> list;

	class Loader;
	std::unique_ptr<Loader> loader;
};

}  // namesapce

#endif
