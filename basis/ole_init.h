#pragma once
#ifndef GUID_E7B2179BBB074D02BEF92772FB25260C
#define GUID_E7B2179BBB074D02BEF92772FB25260C

// Windows 2000 Professional[desktop apps only]
#ifndef STDAFX_H
#include <ole2.h>
#pragma comment(lib, "ole32.lib")
#endif

/*!    OLE初期化オブジェクト.
    実行スレッドにおけるOLEの初期化と終了処理を行う。
    @par
    OLEはスレッドセーフでないため、スレッドごとに実体を持つぽい。
    そのためスレッドごとに初期化をしないといけない。
*/
class OleInitializer {
public:
    OleInitializer() : status(OleInitialize(0)) {}
    ~OleInitializer() {
        // 初期化時にS_FALSEをかえしたときも終了処理は行う。
        if (isValid())
            OleUninitialize();
    }
    bool isValid() {
        return status == S_OK || status == S_FALSE;
    }
private:
    HRESULT status;
};



#endif