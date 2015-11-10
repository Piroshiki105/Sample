#ifndef __INCLUDED_LOGGER_H_
#define __INCLUDED_LOGGER_H_

#include <cstdio>
#include <fstream>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <tchar.h>

typedef enum eStamp { JFB, COLON } stamp_type;

class Logger {
private:
	// �t�@�C���X�g���[��
	std::ofstream *ofs;

	// �t�@�C����
	char mFileName[128];

	// �f�B���N�g�������݂��Ă��邩�`�F�b�N�A�Ȃ���΍��
	void checkExistDir(const char *dirName, size_t size);

	// �t�@�C���̍Ō�̃i���o�[���擾����
	int lastFileNum(const char *path, size_t size);

public:
	// �R���X�g���N�^
	// �����@�@�@fileName�F�t�@�C����(�Ȃ��̏ꍇ�̓f�t�H���g�l�[��)
	Logger(const char *fileName = NULL);

	// �X�g���[�����J��
	void open(const char *fileName = NULL);

	// �X�g���[�������
	void close();

	// �^�C���X�^���v
	void timestamp(stamp_type type);

	// �t�@�C���ɋL�q
	inline void write(const char   c) { *ofs << c; }
	inline void write(const char  *s) { *ofs << s; }
	inline void write(const int    i) { *ofs << i; }
	inline void write(const float  f) { *ofs << f; }
	inline void write(const double d) { *ofs << d; }

	// �t�@�C���ɋL�q
	Logger& operator<<(const char   c);
	Logger& operator<<(const char  *s);
	Logger& operator<<(const int    i);
	Logger& operator<<(const float  f);
	Logger& operator<<(const double d);
};

#endif