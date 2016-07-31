/* APIs
FormatMessage
	Windows NT/2000�FWindows NT 3.1 �ȍ~
	Windows 95/98�FWindows 95 �ȍ~
	�w�b�_�[�FWinbase.h ���Ő錾�AWindows.h ���C���N���[�h
	�C���|�[�g���C�u�����FKernel32.lib ���g�p
	Unicode�FWindows NT/2000 �� Unicode �ł� ANSI �ł�����
*/

#include "exception.h"

#ifndef STDAFX_H
#include <wtypes.h>
#endif

namespace {

std::string ToStrA(int i) {
	char sz[16] = { '\0' };
	if (sprintf_s(sz, "%d", i) < 0)
		return std::string();
	return std::string(sz);
}

// API�G���[�p�̕⏕�֐�
// �G���[�R�[�h�����b�Z�[�W�ɕϊ�����
std::string TranslateErrorCode(DWORD dwLastError)
{
	const int size = 512;
	PSTR buf = static_cast<PSTR>(malloc(size));

	// ���P�[�������s���ɍ��킹��
	setlocale(LC_ALL, setlocale(LC_CTYPE, ""));

	if (!FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwLastError, 0, buf, size, NULL))
		return "";

	std::string message(buf);
	free(buf);
	return message;
}

} // namespace

namespace basis {

std::string MakeLocation(const char *func, const char *file, int line)
{
	return std::string(func) + " in " + file + " line:" + ToStrA(line);
}



// API�G���[�p�̗�O���b�Z�[�W�𐬌`����
std::string GetErrorMessage(const char *func, const char *file, int line)
{
	std::string mes("Error:");

	mes += ToStrA(static_cast<int>(GetLastError()));
	mes += " " + TranslateErrorCode(GetLastError());
	mes += MakeLocation(func, file, line);

	return mes;
}



}  // namespace