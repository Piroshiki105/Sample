#ifndef __INCLUDED_LOGGER_H_
#define __INCLUDED_LOGGER_H_

#include <cstdio>
#include <fstream>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <tchar.h>

class Logger {
private:
	// �t�@�C���X�g���[��
	std::ofstream *ofs;

	// �f�B���N�g�������݂��Ă��邩�`�F�b�N�A�Ȃ���΍��
	void checkExistDir(const char *dirName, size_t size);

public:
	// �R���X�g���N�^
	// �����@�@�@fileName�F�t�@�C����(�Ȃ��̏ꍇ�̓f�t�H���g�l�[��)
	Logger(const char *fileName = NULL);

	// ��������t�@�C���ɋL�q
	// �����@�@�@str�F�L�q������
	// �Ԃ�l�@�@�L�q�������������ۂ�
	int write(const char *str);

};

#endif