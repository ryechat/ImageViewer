#include "file_path.h"
#include "file_item.h"
#include "stdfnc.h"

namespace basis {

UNIT_TEST_FUNC(CFileItem)
{
	auto path = CFilePath::GetCurrentDirectory();
	path += TEXT("TEST_CFileItem.temp");

	auto content = "This is test file.";

	// ---------- write file --------- //
	{
		CFileItem file;
		if (path.exist())
			file = path.open(GENERIC_WRITE, FILE_SHARE_READ);
		else
			file = path.create();

		if (!file)
			throw 0;

		if (!file.write(reinterpret_cast<const void*>(content),
			static_cast<DWORD>(strlen(content)), INFINITE))
			throw 0;
	}

	// --------- read file --------- //
	{
		auto file = path.open();
		if (!file)
			throw 0;

		auto size = file.size();
		SafeRelease<char> sz(static_cast<char*>
			(malloc(size.LowPart + 1)),
			[](void *p) { free(p); });

		if (!file.read(reinterpret_cast<void*>(sz.get()),
			size.LowPart, INFINITE))
			throw 0;

		sz[size.LowPart] = '\0';
		if (strcmp(content, sz))
			throw 0;
	}
	
	path.erase();
	return true;
}

}  // namespace