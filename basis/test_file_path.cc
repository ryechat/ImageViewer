#include "file_path.h"
#include "file_item.h"

namespace basis {

UNIT_TEST_FUNC(CFilePath)
{
    auto path = CFilePath::GetCurrentDirectory();
    if (!path.exist())
        throw 0;
    if (!path.isDirectory())
        throw 0;

    auto file = path + TEXT("temp");
    if (file.getFileName() != TEXT("temp"))
        throw 0;

    if (file.exist())
        if (!file.erase())
            throw 0;

    if (path != file.getDir())
        throw 0;

    if (!file.createAsDirectory())
        throw 0;

    if (!file.erase())
        throw 0;

    // ファイル作成のテストはCFileItemが行う
    return true;
}

}  // namespace