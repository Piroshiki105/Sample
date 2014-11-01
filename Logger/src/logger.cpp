#include "logger.h"

Logger::Logger(const char *fileName) {
	// �t�@�C��������̎�
	if(fileName == NULL) {
		fileName = "default.log";
	}

	// �f�B���N�g�����̐ݒ�P
	char dirName[32];
	strcpy(dirName, ".\\log\\");

	// �f�B���N�g�������݂��Ă��邩�`�F�b�N����
	checkExistDir(dirName, sizeof(dirName));

	// ���t�̎擾
	time_t t = time(NULL);
	struct tm *local = localtime(&t);

	// �f�B���N�g�����̐ݒ�Q
	char tmp[32];
	sprintf(tmp, "%4d%02d%02d\\", local->tm_year+1900, local->tm_mon+1, local->tm_mday);
	strcat(dirName, tmp);

	// �f�B���N�g�������݂��Ă��邩�`�F�b�N����
	checkExistDir(dirName, sizeof(dirName));

	// �p�X�̐ݒ�
	char path[128];
	strcpy(path, dirName);
	strcat(path, fileName);

	// �t�@�C���X�g���[�����J��
	ofs = new std::ofstream(path);
}

// �t�@�C���L�q
int Logger::write(const char *str) {
	*ofs << str;

	return 1;
}

// �f�B���N�g�������݂��Ă��邩�`�F�b�N����
void Logger::checkExistDir(const char *dirName, size_t size) {
	WIN32_FIND_DATA fd;
	TCHAR _tdirName[32];

#ifdef UNICODE
	mbstowcs(_tdirName, dirName, size);
#else
	_tcscpy(_tdirName, dirName);
#endif

	// �f�B���N�g�������݂��Ă��邩�`�F�b�N
	if(FindFirstFile(_tdirName, &fd) == INVALID_HANDLE_VALUE) {
		// �Ȃ���΍��
		CreateDirectory(_tdirName, NULL);
	}
}