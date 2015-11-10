#include "logger.h"

Logger::Logger(const char *fileName) {
	// �X�g���[�����J��
	open(fileName);
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

// �t�@�C�����̍Ō�̃i���o�[���擾����
int Logger::lastFileNum(const char *path, size_t size) {
	int number = 0;

	WIN32_FIND_DATA fd;
	TCHAR _tpath[32];

#ifdef UNICODE
	mbstowcs(_tpath, path, size);
#else
	_tcscpy(_tdirName, dirName);
#endif

	// ���C���h�J�[�h3���̐��l�Ɗg���q��t����
	_tcscat(_tpath, _T("???.log"));

	// �t�@�C�������݂��Ă��邩�`�F�b�N
	HANDLE hFiles = FindFirstFile(_tpath, &fd);

	if(hFiles != INVALID_HANDLE_VALUE) {
		// �Ō�̃t�@�C���܂ŉ������Ȃ�
		while(FindNextFile(hFiles, &fd));
		// �t�@�C�����̃i���o�[�����ȍ~�݂̂����o��
		_TCHAR subStr[32];
		_tcsncpy(subStr, fd.cFileName+strlen(mFileName), 8);
		_stscanf(subStr, _T("%d.log"), &number);
		number++;
	}

	// �n���h�����N���[�Y
	FindClose(hFiles);

	return number;
}

// �X�g���[�����J��
void Logger::open(const char *fileName) {
	// �t�@�C��������̎�
	if(fileName == NULL) {
		strcpy(mFileName, "default");
	}
	else {
		strcpy(mFileName, fileName);
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

	// �p�X�̐ݒ�(�g���q�ƃt�@�C���i���o�[������)
	char path[128];
	strcpy(path, dirName);
	strcat(path, mFileName);

	// �t�@�C���̐����J�E���g����
	int fileNum = lastFileNum(path, sizeof(path));
	char fileNumStr[4];
	sprintf(fileNumStr, "%03d", fileNum);

	// �p�X�̐ݒ�
	strcat(path, fileNumStr);
	strcat(path, ".log");

	// �t�@�C���X�g���[�����J��
	ofs = new std::ofstream(path);
}

// �X�g���[�������
void Logger::close() {
	ofs->close();
}

// �t�@�C���L�q
Logger& Logger::operator<<(const char c) {
	*ofs << c;
	return *this;
}

Logger& Logger::operator<<(const char *s) {
	*ofs << s;
	return *this;
}

Logger& Logger::operator<<(const int i) {
	*ofs << i;
	return *this;
}

Logger& Logger::operator<<(const float f) {
	*ofs << f;
	return *this;
}

Logger& Logger::operator<<(const double d) {
	*ofs << d;
	return *this;
}