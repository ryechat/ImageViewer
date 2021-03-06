#include "file_path.h"
#include "file_item.h"
#include "stdfnc.h"
#include "cdd_bitmap.h"

namespace basis {

  UNIT_TEST_FUNC(CDDBitmap)
  {
    using tstr = ::std::basic_string<TCHAR>;
    auto path = CFilePath::GetBootDirectory() + TEXT("img11.jpg");

    CFileItem file = path.open();
    if (!file)
      throw 0;

    auto size = file.getSize();
    if (size.HighPart > 0)
      return false;

    GlobalMemory heap;
    heap.alloc(size.LowPart);
    file.read(heap.address(), 0, INFINITE);

    CDDBitmap bitmap;
    if (bitmap.loadOleLoad(heap.handle()) == false)
      throw 0;
    if (bitmap.loadGdiplus(heap.address(), size.LowPart) == false)
      throw 0;
    return true;
  }

}  // namespace