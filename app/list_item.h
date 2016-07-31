#pragma once
#ifndef GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF
#define GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF

#include "file_path.h"
#include "cdd_bitmap.h"
#include "iunit_test.h"

namespace image_viewer {

UNIT_TEST(CListItem)

/*!
	CImageViewer::Impl::Filerにて
	リストで管理する画像ファイルクラス。
	CFindDataからのみ生成される
*/
class CListItem {
public:
	using tstr = std::basic_string<TCHAR>;
	CListItem(const WIN32_FIND_DATA&);
	~CListItem() = default;

	CListItem&operator=(const CListItem&) = default;
	CListItem(CListItem &&) = default;
	CListItem&operator=(CListItem && ) = default;

	enum class Status : BYTE {
		Undefined = 0,
		NotExist = 1,
		CannotOpen = 2,
		SizeError = 4,
		MemoryError = 8,
		TypeError = 16,
		LoadError = 32,
		Loaded = 64,
		Release = 128,
	};

	/*!	画像をロードする。
		ファイルが開けなかった場合はStatus::CannotOpenを返す。
		それ以外の理由で失敗した場合、m_typeをTYPE::Errorに設定し、
		対応するエラーステータスを返す。
		成功した場合はStatus::Loadedを返す。
	*/
	Status	loadImage(::basis::CFilePath path);

	// 外部使用。ビットマップを解放するかどうかの判定に使う
	bool bUnload;

	// 外部使用。リスト内インデックス
	int index;

	const TCHAR * fileName() const { return m_fileName.c_str(); }

	bool	isLoadingFailed() { return m_type == TYPE::Error; }

	void	unload() { m_image.reset(); }

	bool	isLoaded() const { return m_image.operator bool(); }

	bool draw(HDC dest, const RECT& rcDest, const RECT& rcSrc) {
		return m_image.transfer(dest, rcDest, rcSrc);
	}

	basis::Size	getSize()   const { return m_image.getSize(); }
	basis::Rect rect() const {
		auto size = m_image.getSize();
		return{ 0, 0, size.x, size.y };
	}

	FILETIME ftAccess() const { return m_access; }
	FILETIME ftCreate() const { return m_create; }
	FILETIME ftWrite()  const { return m_write;  }

private:
	basis::Surface m_image;
	basis::StringBuffer m_fileName;
	FILETIME  m_access;
	FILETIME  m_create;
	FILETIME  m_write;

	using TYPE = basis::CDDBitmap::TYPE;
	TYPE m_type;
};

}  // namespace

#endif