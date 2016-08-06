/*!/////////////////////////////////////////////////////////////////////////////

    @file
    Defines WinMain function, entry point of the program.
    This function will lanch the application class.

    Unit tests would be performed when WinMain is called
    if it was compiled in debug mode(_DEBUG is defined).

    Unit tests are declaired in their headers, xxxx.h,
    and defined in test_xxxx.cc files.

//////////////////////////////////////////////////////////////////////////////*/

#include "imemory.h"
#include "singleton.h"
#include "image_viewer.h"

namespace {
void CheckLeaks() {
#ifdef _DEBUG
    auto i = basis::HeapMemory::TotalAmount();    // Images
    auto j = basis::CMemory::TotalAmount();        // Others
    if (i || j)
        throw 0;
#endif
}

void PerformUnitTest() {
#ifdef _DEBUG
    for (auto e = IUnitTest::GetEnumlator(); e.next(); ) {
        assert(e.get()->test());
        CheckLeaks();
    }
#endif
}
}  // namespace



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int nShow)
{
    // Declairation to perform leak-checking when the program ends.
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    PerformUnitTest();

    image_viewer::CImageViewer().create().show(nShow).waitToEnd();

    CheckLeaks();

    // Finalizes singleton objects not to be caught by CrtDbg.
    basis::SingletonFinalizers::Finalize();
    return 0;
}
