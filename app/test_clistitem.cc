#include "find_file.h"
#include "stdfnc.h"
#include "list_item.h"
#include "file_path.h"

namespace image_viewer {

UNIT_TEST_FUNC(CListItem) {
	{
		tstr dir = basis::CFilePath::GetBootDirectory().path().c_str();
		tstr name = TEXT("img11.jpg");

		basis::CFindFile e((dir + TEXT("\\*")).c_str());
		WIN32_FIND_DATA fd{};

		while (e.nextFile()) {
			if (name == e.get().cFileName) {
				fd = e.get();
				break;
			}
		}
		if (e.empty())
			throw 0;

		DWORD size = fd.nFileSizeLow;
		if (size == 0 || fd.nFileSizeLow == INVALID_FILE_SIZE) {
			throw 0;
		}
		
		CListItem item(fd);
		item.isLoaded();

		tstr sz = dir + TEXT("\\") + name;

		if (item.loadImage(sz.c_str())
			!= CListItem::Status::Loaded)
			throw 0;

		if (!item.isLoaded())
			throw 0;
	}

	return true;
}

}  // namespace