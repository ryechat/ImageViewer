/*
Gdiplus Bitmap Class
	Windows XP, Windows 2000 Professional
	GDI+ 1.0
	Gdiplusheaders.h (include Gdiplus.h), Gdiplus.lib / Gdiplus.dll
*/
#ifndef STDAFX_H
#include <olectl.h>	// OleLoadPicture, IPicture(OCIdl.h)
#pragma comment(lib, "oleaut32.lib")

#include <shlwapi.h> // for SHCreateMemStream
#pragma comment( lib, "Shlwapi.lib" )
#endif

#include <basetsd.h>
// OLE_HANDLEはx64でも32bitのため、
// 各ハンドル型にキャストすることが不可能。
// ヘルパ関数LongToHandleを使う。

#include "gdiplus_init.h"
#include "ole_init.h"
#include "stdfnc.h"	// using SafeRelease

#include "cdd_bitmap.h"

namespace
{
	// IPicture->Renderによる等倍転送。
	// 非等倍転送はHALFTONEを考慮しないため画質が落ちるので、
	// IPictureのままクラスに保持していても使えない。
	void copyRender(IPicture &ip, HDC dest, basis::Size s)
	{
		OLE_XSIZE_HIMETRIC dxSrc;
		OLE_YSIZE_HIMETRIC dySrc;
		ip.get_Width(&dxSrc);
		ip.get_Height(&dySrc);
		ip.Render(dest, 0, 0, s.x, s.y,
			0, dySrc, dxSrc, -dySrc, nullptr);
		// src側のサイズはHIMETRIC単位。
		// さらに基準点は下からで、高さは負の値になる
	}

	struct Deleter_IPicture {
		void operator()(IPicture *ip) {
			ip->Release();
		}
	};
}

namespace basis {

bool CDDBitmap::
loadGdiplus(void *p, size_t size)
{
	SafeRelease<IStream> is{
		SHCreateMemStream(static_cast<const BYTE*>(p),
		static_cast<UINT>(size)),
		[](IStream* p) { p->Release(); }
	};

	using namespace Gdiplus;
	if (!is || !GdiplusInit().available())
		return false;

	HBITMAP hBitmap = 0;
	Color	background(255, 255, 255);

	if (Bitmap(is).GetHBITMAP(background, &hBitmap) != Ok)
		return false;

	return setBitmap(hBitmap);
}



bool CDDBitmap::
loadOleLoad(HGLOBAL hg)
{
	OleInitializer ole;
	if (!ole.isValid())
		return false;

	IStream *is = nullptr;
	if (!hg || CreateStreamOnHGlobal(hg, FALSE, &is))
		return false;

	// Create ip.
	IPicture *p = nullptr;	// Appears when fail.
	OleLoadPicture(is, 0, FALSE, IID_IPicture, (LPVOID*)&p);
	if (!p)
		return false;
	std::unique_ptr<IPicture, Deleter_IPicture> ip(p);


	// Makes ip to have hdc.
	Surface temp;
	temp.create(0, { 1, 1 });
	HDC	prev_dc = 0; // Will never change.
	OLE_HANDLE prev_bmp = 0;
	if (ip->SelectPicture(temp, &prev_dc, &prev_bmp) != S_OK)
		return false;

	// Duplicate bitmap.
	OLE_HANDLE hOle = 0;
	if (ip->get_Handle(&hOle) || !hOle)
		return false;

	auto s = Surface::GetSize(reinterpret_cast<HBITMAP>(LongToHandle(hOle)));
	create(temp, s);
	copyRender(*ip, *this, s);

	// Release ownership of hOle.
	temp.setBitmap(reinterpret_cast<HBITMAP>(LongToHandle(prev_bmp)));
	return true;

	// Scoping out will make IPicture, hOle and temp released.
}



CDDBitmap::TYPE CDDBitmap::
getType(IStream *is)
{
	if (!is)
		return TYPE::Error;
	BYTE bytes[kTypeCheckBytes];
	ULONG cbRead = 0;
	is->Seek({ 0, 0 }, STREAM_SEEK_SET, 0);
	is->Read(static_cast<void*>(&bytes), _countof(bytes), &cbRead);
	is->Seek({ 0, 0 }, STREAM_SEEK_SET, 0);
	return getType(bytes);
}



CDDBitmap::TYPE CDDBitmap::
getType(const BYTE p[kTypeCheckBytes])
{
	const BYTE end { 0x10 };
	BYTE formats[][kTypeCheckBytes]{
		{ 0xff, 0xd8, end },							// JPEG/Exif
		{ 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a, end },	// PNG
		{ 'B', 'M', end },								// BMP
		{ 'G', 'I', 'F', '8', '7', 'a', end },			// GIF
		{ 'G', 'I', 'F', '8', '9', 'a', end },			// GIF
		{ 1, 0, 0, 0, end },							// EMF
		{ 0, 0, 0, 1, end },							// ICO(.ico)
		{ 0, 0, 0, 2, end },							// ICO(.cur)
		{ 0xD7, 0xcd, 0xc6, 0x9a, 0, 0, end },			// WMF(APM)
		{ 0, 0, end },									// WMF(memory)
		{ 1, 0, end },									// WMF(disk)
	};

	for (auto &&format : formats) {
		const BYTE *i = p;
		for (auto &&ch : format) {
			if (ch == end)
				return static_cast<TYPE>(format[0]);
			if (*i != ch)
				break;
			++i;
		}
	}

	// TIFF
	if ((*p == 0x4d || *p == 0x49) && (*p == p[1]))
		return TYPE::TIFF;

	return TYPE::Error;
}

}  // namespace