#pragma once
#ifndef GUID_A2759F8254E74760A21048340607EAB4
#define GUID_A2759F8254E74760A21048340607EAB4

#include "window_message.h"

namespace image_viewer {

/*! �R���g���[��ID (0, 0x6FFF].
    100��101����g���̂�����炵���B
    COMMAND_BEGIN �ȍ~�̓v���t�@�C���\�ȃR�}���hID�B
    �R�}���hID �ɑΉ����镶����� Profile �N���X�ɂĒ�`�����B
    MENU_BEGIN �ȍ~�̓��j���[���ڂŁA����ID���ɕ��ԁB
*/
enum class ID : int {
	UNDEFINED = 0,
	LAST_PATH = 101,
	PROFILE_ENABLE,
    LOADER_RANGE_MIN,
	LOADER_RANGE_MAX,
	LOADER_MEMORY_CAP,
	LOADER_IMAGE_LOADED,
	LIST_EMPTY,
	CONFIRM_DELETE,

	WINDOW_REMINDER,
	WINDOW_ZOOMING,
	WINDOW_STYLE,
	WINDOW_POSITION,
	WINDOW_LEFT,
	WINDOW_TOP,
	WINDOW_RIGHT,
	WINDOW_BOTTOM,
	WINDOW_MAXIMIZE,

	IDM_USER = 150,

	COMMAND_BEGIN = 200,
	GRIP_IMAGE,
	UNGRIP_IMAGE,
	LIST_REMOVE,
	FILE_QUICK_DELETE,

	MENU_BEGIN,

	FILE_BACK,
	FILE_NEXT,
	FILE_FIRST,
	FILE_LAST,
	FILE_RELOAD,
	FILE_DELETE,
	USE_PROFILE,
	SCREEN_TOGGLE,

	VIEW_BEGIN,
	VIEW_POPUP,
	VIEW_FILENAME,
	VIEW_FILELIST,
	VIEW_CENTER,
	VIEW_UPSCALE,
	VIEW_DOWNSCALE,
	VIEW_END,

	SORT_BEGIN,
	SORT_GREATER_WRITE,
	SORT_LESSER_WRITE,
	SORT_GREATER_CREATION,
	SORT_LESSER_CREATION,
	SORT_GREATER_ACCESS,
	SORT_LESSER_ACCESS,
	SORT_END,

	SHOW_PROPERTY,
	WINDOW_CLOSE,

	MENU_END,

	COMMAND_END,
	IDM_USER_END,		// �I�[
}; // enum

ID inline next(ID id) {
    return static_cast<ID>(static_cast<int>(id) + 1);
}

ID inline prev(ID id) {
    return static_cast<ID>(static_cast<int>(id) - 1);
}

} // namespace

#endif