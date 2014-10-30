#include "logger.h"

Logger::Logger() {
}

// �t�@�C���I�[�v��
int Logger::open(const char *fileName) {
	// �t�@�C���̖��O����̎�
	if(fileName == NULL) {
		fileName = "default name";
	}

	// �t�@�C�����J��
	logFile = fopen(fileName, "w");

	// �t�@�C���̎擾�Ɏ��s�����Ƃ�
	if(logFile == NULL) return 0;
	
	return 1;
}

// �t�@�C���X�g���[����������
int Logger::write(const char *str) {
	
	if(fileName != NULL) {
		fputs(str, logFile);
		return 1;
	}

	// �������݂ł��Ă��Ȃ�
	return 0;
}

// �t�@�C���N���[�Y
int Logger::close() {
	// ���O�t�@�C�����擾�ł��Ă���Ƃ�
	if(logFile != NULL) {
		// �t�@�C���̃N���[�Y�ɐ��������Ƃ�
		if(fclose(logFile) == 0) return 1;
	}

	// �t�@�C���������݂ł��Ă��Ȃ�
	return 0;
}