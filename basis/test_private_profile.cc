#include "stdfnc.h"
#include "file_path.h"
#include "private_profile.h"

namespace basis {

UNIT_TEST_FUNC(CPrivateProfile)
{
	// �I������temp�t�@�C���͍폜����
	SafeRelease<CFilePath> file {
		new CFilePath(CFilePath::GetBootDirectory() + TEXT("test_profile.temp")),
		[](CFilePath* p) { p->erase(); delete p; }
	};

	CPrivateProfile prof(file->path().c_str());
	if (prof.path() != file->path().c_str())
		throw 0;

	// �G���[�F�Z�N�V�������w��
	if (prof.write(0, TEXT_EMPTY) == true)
		throw 0;
	if (prof.eraseKey(0) == true)
		throw 0;
	if (prof.getAllKeyNames() == true)
		throw 0;

	// �o�b�t�@�L�����̊m�F
	if (*prof.c_str() != NULL)
		throw 0;

	// �o�b�t�@�̍Ċm�ۂł��邩�m�F
	if (file->path() != prof.read(0, file->path().c_str()))
		throw 0;

	// NULL*2
	if (prof.getAllSectionNames() == false)
		throw 0;
	if (*prof.c_str() || *(prof.c_str() + 1))
		throw 0;

	// �L�[�ǉ�
	tstr tx{ TEXT("writestring") };
	prof.section(TEXT("s"));
	if (prof.write(TEXT("k"), tx.c_str()) == false)
		throw 0;
	if (tx != prof.read(TEXT("k"), nullptr))
		throw 0;

	if (prof.getAllKeyNames() == false)
		throw 0;
	if (prof.c_str()[0] != _T('k'))
		throw 0;

	// NULL*2
	if (*(prof.c_str() + 1) || *(prof.c_str() + 2))
		throw 0;

	if (prof.getAllSectionNames() == false)
		throw 0;
	if (prof.c_str()[0] != _T('s'))
		throw 0;

	// NULL*2
	if (*(prof.c_str() + 1) || *(prof.c_str() + 2))
		throw 0;

	if (prof.read(TEXT("not_exist"), tx.c_str()) != tx)
		throw 0;
	if (prof.eraseKey(TEXT("k")) == false)
		throw 0;
	if (prof.clearSection() == false)
		throw 0;

	return true;
}

}  // namespace