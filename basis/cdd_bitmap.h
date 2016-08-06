#pragma once
#ifndef GUID_AF784485DB6A4893BDE4F5A05D37669F
#define GUID_AF784485DB6A4893BDE4F5A05D37669F

#ifndef STDAFX_H
#include <memory>
#endif

#include "types.h"
#include "surface.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(CDDBitmap)



/*! Device Dependent Bitmap.
    CDDBitmap provides the way to load bitmap from memory image,
    loadOleLoad() and loadGdiplus().

    Loadable files are<br \>
    Gdiplus    : BMP, ICO, GIF, JPEG, Exif, PNG, TIFF, WMF, and EMF.<br \>
    OleLoadPicture : BMP, GIF, JPEG, TIFF, WMF

    It can also tells what kind of image the file is,
    before decoding image with using getType() function.

    To render loaded image, calling draw() function will
    invoke Surface::transfer() function.
    @sa

*/
class CDDBitmap : public Surface {
public:
    class Test;
    enum class TYPE : BYTE {
        Undefined = 0,
        Error = 1,
        TIFF = 0x49,    // 0b0100'1001
        BMP = 'B',        // 0b0100'0010
        GIF = 'G',        // 0b0100'0111
        PNG = 0x89,        // 0b1000'1001
        WMF = 0xD7,        // 0b1101'0111
        JPEG = 0xff,    // 0b1111'1111
    };

    // ‰æ‘œŒ`Ž®”»’è
    static constexpr unsigned kTypeCheckBytes = 9;
    static TYPE getType(IStream *is);
    static TYPE getType(const BYTE buf[kTypeCheckBytes]);

    void    unload() noexcept { reset(); }
    bool    isLoaded() const noexcept { return *this ? true : false; }

    bool    loadOleLoad(HGLOBAL hg);
    bool    loadGdiplus(void *p, size_t size);
};

}  // namespace

#endif
