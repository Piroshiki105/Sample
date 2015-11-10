#include "logger.h"

Logger::Logger(const char *fileName) {
	// ストリームを開く
	open(fileName);
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

// ファイル名の最後のナンバーを取得する
int Logger::lastFileNum(const char *path, size_t size) {
	int number = 0;

	WIN32_FIND_DATA fd;
	TCHAR _tpath[32];

#ifdef UNICODE
	mbstowcs(_tpath, path, size);
#else
	_tcscpy(_tdirName, dirName);
#endif

	// ワイルドカード3桁の数値と拡張子を付ける
	_tcscat(_tpath, _T("???.log"));

	// ファイルが存在しているかチェック
	HANDLE hFiles = FindFirstFile(_tpath, &fd);

	if(hFiles != INVALID_HANDLE_VALUE) {
		// 最後のファイルまで何もしない
		while(FindNextFile(hFiles, &fd));
		// ファイル名のナンバー部分以降のみを取り出す
		_TCHAR subStr[32];
		_tcsncpy(subStr, fd.cFileName+strlen(mFileName), 8);
		_stscanf(subStr, _T("%d.log"), &number);
		number++;
	}

	// ハンドルをクローズ
	FindClose(hFiles);

	return number;
}

// ストリームを開く
void Logger::open(const char *fileName) {
	// ファイル名が空の時
	if(fileName == NULL) {
		strcpy(mFileName, "default");
	}
	else {
		strcpy(mFileName, fileName);
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

	// パスの設定(拡張子とファイルナンバーを除く)
	char path[128];
	strcpy(path, dirName);
	strcat(path, mFileName);

	// ファイルの数をカウントする
	int fileNum = lastFileNum(path, sizeof(path));
	char fileNumStr[4];
	sprintf(fileNumStr, "%03d", fileNum);

	// パスの設定
	strcat(path, fileNumStr);
	strcat(path, ".log");

	// ファイルストリームを開く
	ofs = new std::ofstream(path);
}

// ストリームを閉じる
void Logger::close() {
	ofs->close();
}

// ファイル記述
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