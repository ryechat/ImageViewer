#include "imemory.h"
#include "cdd_bitmap.h"
#include "file_path.h"
#include "file_item.h"
#include "list_item.h"

namespace image_viewer {

class CListItem::Impl {
public:
    Impl(const WIN32_FIND_DATA &fd) : m_fileName(fd.cFileName),
        m_access(fd.ftLastAccessTime), m_create(fd.ftCreationTime),
        m_write(fd.ftLastWriteTime), m_type(TYPE::Undefined)
    {}

    using TYPE = basis::CDDBitmap::TYPE;
    TYPE m_type;
    basis::Surface m_image;
    std::basic_string<TCHAR> m_fileName;
    FILETIME  m_access;
    FILETIME  m_create;
    FILETIME  m_write;
};



CListItem::CListItem(const WIN32_FIND_DATA &fd)
    : impl(new Impl(fd)), weight(fd.nFileSizeLow)
{}



CListItem::~CListItem() = default;



CListItem::Status CListItem::
loadImage(basis::CFilePath path)
{
    using CDDBitmap = basis::CDDBitmap;
    using IMemory = basis::IMemory;

    auto file = path.open();
    if (!file) {
        if (path.exist())
            return Status::CannotOpen;

        impl->m_type = Impl::TYPE::Error;
        return Status::NotExist;
    }

    LARGE_INTEGER file_size = file.getSize();
    if (file_size.HighPart > 0) {
        impl->m_type = Impl::TYPE::Error;
        return Status::SizeError;
    }

    // Gdiplus or OleLoadPicture
    bool constexpr kUseGdiplus = true;

    std::unique_ptr<IMemory> mem(kUseGdiplus
        ? static_cast<IMemory*>(new basis::HeapMemory)
        : static_cast<IMemory*>(new basis::GlobalMemory));

    mem->alloc(file_size.LowPart, false);
    BYTE *p = static_cast<BYTE*>(mem->address());
    if (!p) {
        return Status::MemoryError;
    }

    if (!file.read(p, CDDBitmap::kTypeCheckBytes, INFINITE))
        throw 0;

    impl->m_type = CDDBitmap::getType(p);
    if (impl->m_type == Impl::TYPE::Error)
        return Status::TypeError;

    if (!file.read(static_cast<void*>
        (p + CDDBitmap::kTypeCheckBytes), 0, INFINITE))
        throw 0;

    CDDBitmap bmp;

    bool const succeeded = (kUseGdiplus)
        ? bmp.loadGdiplus(mem->address(), file_size.LowPart)
        : bmp.loadOleLoad(dynamic_cast<basis::GlobalMemory*>
            (mem.get())->handle());

    if (succeeded) {
        weight = static_cast<int>(bmp.usage());
        impl->m_image = std::move(bmp);
        return Status::Loaded;
    }
    impl->m_type = Impl::TYPE::Error;
    return Status::LoadError;
}



const TCHAR * CListItem::
fileName() const
{
    return impl->m_fileName.c_str();
}



bool CListItem::
isLoadingFailed() {
    return impl->m_type == Impl::TYPE::Error;
}



void CListItem::unload()
{
    impl->m_image.reset();
}



bool CListItem::
isLoaded() const
{
    return impl->m_image.operator bool();
}



bool CListItem::
draw(HDC dest, const RECT & rcDest, const RECT & rcSrc)
{
    return isLoaded() && impl->m_image.transfer(dest, rcDest, rcSrc);
}



basis::Size CListItem::
size() const
{
    return impl->m_image.size();
}



basis::Rect CListItem::
rect() const
{
    return{ 0, 0, size().x, size().y };
}



FILETIME CListItem::
ftAccess() const
{
    return impl->m_access;
}



FILETIME CListItem::
ftCreate() const
{
    return impl->m_create;
}



FILETIME CListItem::
ftWrite() const
{
    return impl->m_write;
}

}  // namespace