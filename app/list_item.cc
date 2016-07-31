#include "imemory.h"
#include "file_item.h"
#include "list_item.h"

namespace image_viewer {

CListItem::CListItem(const WIN32_FIND_DATA &fd)
	: m_fileName(fd.cFileName), m_access(fd.ftLastAccessTime),
	m_create(fd.ftCreationTime), m_write(fd.ftLastWriteTime),
	m_type(TYPE::Undefined)
{}



CListItem::Status CListItem::
loadImage(basis::CFilePath path)
{
	using CDDBitmap = basis::CDDBitmap;
	using IMemory = basis::IMemory;

	auto file = path.open();
	if (!file) {
		if (path.exist())
			return Status::CannotOpen;

		m_type = TYPE::Error;
		return Status::NotExist;
	}

	LARGE_INTEGER file_size = file.size();
	if (file_size.HighPart > 0) {
		m_type = TYPE::Error;
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

	m_type = CDDBitmap::getType(p);
	if (m_type == TYPE::Error)
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
		m_image = std::move(bmp);
		return Status::Loaded;
	}
	m_type = TYPE::Error;
	return Status::LoadError;
}

}  // namespace