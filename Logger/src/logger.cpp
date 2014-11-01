#include "logger.h"

Logger::Logger(const char *fileName) {
	// ファイル名が空の時
	if(fileName == NULL) {
		fileName = "default.log";
	}

	// ディレクトリ名の設定１
	char dirName[32];
	strcpy(dirName, ".\\log\\");

	// ディレクトリが存在しているかチェックする
	checkExistDir(dirName, sizeof(dirName));

	// 日付の取得
	time_t t = time(NULL);
	struct tm *local = localtime(&t);

	// ディレクトリ名の設定２
	char tmp[32];
	sprintf(tmp, "%4d%02d%02d\\", local->tm_year+1900, local->tm_mon+1, local->tm_mday);
	strcat(dirName, tmp);

	// ディレクトリが存在しているかチェックする
	checkExistDir(dirName, sizeof(dirName));

	// パスの設定
	char path[128];
	strcpy(path, dirName);
	strcat(path, fileName);

	// ファイルストリームを開く
	ofs = new std::ofstream(path);
}

// ファイル記述
int Logger::write(const char *str) {
	*ofs << str;

	return 1;
}

// ディレクトリが存在しているかチェックする
void Logger::checkExistDir(const char *dirName, size_t size) {
	WIN32_FIND_DATA fd;
	TCHAR _tdirName[32];

#ifdef UNICODE
	mbstowcs(_tdirName, dirName, size);
#else
	_tcscpy(_tdirName, dirName);
#endif

	// ディレクトリが存在しているかチェック
	if(FindFirstFile(_tdirName, &fd) == INVALID_HANDLE_VALUE) {
		// なければ作る
		CreateDirectory(_tdirName, NULL);
	}
}