#include "imemory.h"
#include "singleton.h"
#include "image_viewer.h"

namespace {
void CheckLeaks() {
#ifdef _DEBUG
	auto i = basis::HeapMemory::TotalAmount();	// Images
	auto j = basis::CMemory::TotalAmount();		// Others
	if (i || j)
		*(int*)0 = 0;
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



int WINAPI
WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdl, int show)
{
	(void)(inst, prev);

	// Declairs checking memory leaks,
	// performed when the program is about to its end.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	PerformUnitTest();

	image_viewer::CImageViewer(cmdl, show).run();

	CheckLeaks();

	// Finalizes singleton objects not to be caught by CrtDbg.
	basis::SingletonFinalizers::Finalize();

	return 0;
}
