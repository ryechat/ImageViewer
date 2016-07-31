/* APIs
FormatMessage
	Windows NT/2000：Windows NT 3.1 以降
	Windows 95/98：Windows 95 以降
	ヘッダー：Winbase.h 内で宣言、Windows.h をインクルード
	インポートライブラリ：Kernel32.lib を使用
	Unicode：Windows NT/2000 は Unicode 版と ANSI 版を実装
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

// APIエラー用の補助関数
// エラーコードをメッセージに変換する
std::string TranslateErrorCode(DWORD dwLastError)
{
	const int size = 512;
	PSTR buf = static_cast<PSTR>(malloc(size));

	// ロケールを実行環境に合わせる
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



// APIエラー用の例外メッセージを成形する
std::string GetErrorMessage(const char *func, const char *file, int line)
{
	std::string mes("Error:");

	mes += ToStrA(static_cast<int>(GetLastError()));
	mes += " " + TranslateErrorCode(GetLastError());
	mes += MakeLocation(func, file, line);

	return mes;
}



}  // namespace