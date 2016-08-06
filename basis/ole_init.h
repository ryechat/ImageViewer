#pragma once
#ifndef GUID_E7B2179BBB074D02BEF92772FB25260C
#define GUID_E7B2179BBB074D02BEF92772FB25260C

// Windows 2000 Professional[desktop apps only]
#ifndef STDAFX_H
#include <ole2.h>
#pragma comment(lib, "ole32.lib")
#endif

/*!    OLE�������I�u�W�F�N�g.
    ���s�X���b�h�ɂ�����OLE�̏������ƏI���������s���B
    @par
    OLE�̓X���b�h�Z�[�t�łȂ����߁A�X���b�h���ƂɎ��̂����ۂ��B
    ���̂��߃X���b�h���Ƃɏ����������Ȃ��Ƃ����Ȃ��B
*/
class OleInitializer {
public:
    OleInitializer() : status(OleInitialize(0)) {}
    ~OleInitializer() {
        // ����������S_FALSE�����������Ƃ����I�������͍s���B
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