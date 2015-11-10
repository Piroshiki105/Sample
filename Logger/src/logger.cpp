#include "logger.h"

Logger::Logger(const char *fileName, const char *extension) {
	// �X�g���[�����J��
	open(fileName, extension);
}

Logger::~Logger() {
	if (ofs != NULL) {
		ofs->close();
	}
}

// �f�B���N�g�������݂��Ă��邩�`�F�b�N����
void Logger::checkExistDir(const char *dirName, size_t size) {
	WIN32_FIND_DATA fd;
	TCHAR _tdirName[32];

#ifdef UNICODE
	size_t ret;
	mbstowcs_s(&ret, _tdirName, 32, dirName, size);
#else
	_tcscpy_s(_tdirName, dirName);
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
	size_t ret;
	mbstowcs_s(&ret, _tpath, 32, path, size);
#else
	_tcscpy_s(_tdirName, dirName);
#endif

	// ���C���h�J�[�h3���̐��l�Ɗg���q��t����
	_tcscat_s(_tpath, _T("???.*"));

	// �t�@�C�������݂��Ă��邩�`�F�b�N
	HANDLE hFiles = FindFirstFile(_tpath, &fd);

	if(hFiles != INVALID_HANDLE_VALUE) {
		// �Ō�̃t�@�C���܂ŉ������Ȃ�
		while(FindNextFile(hFiles, &fd));
		// �t�@�C�����̃i���o�[�����ȍ~�݂̂����o��
		_TCHAR subStr[32];
		_tcsncpy_s(subStr, fd.cFileName+strlen(mFileName), 8);
		_stscanf_s(subStr, _T("%d.%*s"), &number);
		number++;
	}

	// �n���h�����N���[�Y
	FindClose(hFiles);

	return number;
}

// �X�g���[�����J��
void Logger::open(const char *fileName, const char *extension) {
	// �t�@�C��������̎�
	if(fileName == NULL) {
		strcpy_s(mFileName, "default");
	}
	else {
		strcpy_s(mFileName, fileName);
	}

	if (extension == NULL) {
		strcpy_s(mExtension, "log");
	}
	else {
		strcpy_s(mExtension, extension);
	}


	// �f�B���N�g�����̐ݒ�P
	char dirName[32];
	strcpy_s(dirName, ".\\log\\");

	// �f�B���N�g�������݂��Ă��邩�`�F�b�N����
	checkExistDir(dirName, sizeof(dirName));

	// ���t�̎擾
	time_t t = time(NULL);
	struct tm local;
	localtime_s(&local, &t);

	// �f�B���N�g�����̐ݒ�Q
	char tmp[32];
	sprintf_s(tmp, "%4d%02d%02d\\", local.tm_year+1900, local.tm_mon+1, local.tm_mday);
	strcat_s(dirName, tmp);

	// �f�B���N�g�������݂��Ă��邩�`�F�b�N����
	checkExistDir(dirName, sizeof(dirName));

	// �p�X�̐ݒ�(�g���q�ƃt�@�C���i���o�[������)
	char path[128];
	strcpy_s(path, dirName);
	strcat_s(path, mFileName);


	// �t�@�C���̐����J�E���g����
	int fileNum = lastFileNum(path, sizeof(path));
	char fileNumStr[4];
	sprintf_s(fileNumStr, "%03d", fileNum);


	// �p�X�̐ݒ�
	strcat_s(path, fileNumStr);
	strcat_s(path, ".");
	strcat_s(path, mExtension);

	// �t�@�C���X�g���[�����J��
	ofs = new std::ofstream(path);
}

// �X�g���[�������
void Logger::close() {
	ofs->close();
	ofs = NULL;
}

// �t�@�C���ɏ������s��
void Logger::flush() {
	ofs->flush();
}

// �^�C���X�^���v�̔��s
void Logger::timestamp(stamp_type type) {
	time_t now = time(NULL);
	struct tm pnow;
	localtime_s(&pnow, &now);
	
	char buf[128];
	// �����b�ł̕\��
	if (type == JFB) {
		sprintf_s(buf, "%02d��%02d��%02d�b", pnow.tm_hour, pnow.tm_min, pnow.tm_sec);
	}
	else if(type == COLON) {
		sprintf_s(buf, "%02d:%02d:%02d", pnow.tm_hour, pnow.tm_min, pnow.tm_sec);
	}
	*ofs << buf;
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