#include "logger.h"

Logger::Logger(const char *fileName, const char *extension) {
	// ストリームを開く
	open(fileName, extension);
}

Logger::~Logger() {
	if (ofs != NULL) {
		ofs->close();
	}
}

// ディレクトリが存在しているかチェックする
void Logger::checkExistDir(const char *dirName, size_t size) {
	WIN32_FIND_DATA fd;
	TCHAR _tdirName[32];

#ifdef UNICODE
	size_t ret;
	mbstowcs_s(&ret, _tdirName, 32, dirName, size);
#else
	_tcscpy_s(_tdirName, dirName);
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
	size_t ret;
	mbstowcs_s(&ret, _tpath, 32, path, size);
#else
	_tcscpy_s(_tdirName, dirName);
#endif

	// ワイルドカード3桁の数値と拡張子を付ける
	_tcscat_s(_tpath, _T("???.*"));

	// ファイルが存在しているかチェック
	HANDLE hFiles = FindFirstFile(_tpath, &fd);

	if(hFiles != INVALID_HANDLE_VALUE) {
		// 最後のファイルまで何もしない
		while(FindNextFile(hFiles, &fd));
		// ファイル名のナンバー部分以降のみを取り出す
		_TCHAR subStr[32];
		_tcsncpy_s(subStr, fd.cFileName+strlen(mFileName), 8);
		_stscanf_s(subStr, _T("%d.%*s"), &number);
		number++;
	}

	// ハンドルをクローズ
	FindClose(hFiles);

	return number;
}

// ストリームを開く
void Logger::open(const char *fileName, const char *extension) {
	// ファイル名が空の時
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


	// ディレクトリ名の設定１
	char dirName[32];
	strcpy_s(dirName, ".\\log\\");

	// ディレクトリが存在しているかチェックする
	checkExistDir(dirName, sizeof(dirName));

	// 日付の取得
	time_t t = time(NULL);
	struct tm local;
	localtime_s(&local, &t);

	// ディレクトリ名の設定２
	char tmp[32];
	sprintf_s(tmp, "%4d%02d%02d\\", local.tm_year+1900, local.tm_mon+1, local.tm_mday);
	strcat_s(dirName, tmp);

	// ディレクトリが存在しているかチェックする
	checkExistDir(dirName, sizeof(dirName));

	// パスの設定(拡張子とファイルナンバーを除く)
	char path[128];
	strcpy_s(path, dirName);
	strcat_s(path, mFileName);


	// ファイルの数をカウントする
	int fileNum = lastFileNum(path, sizeof(path));
	char fileNumStr[4];
	sprintf_s(fileNumStr, "%03d", fileNum);


	// パスの設定
	strcat_s(path, fileNumStr);
	strcat_s(path, ".");
	strcat_s(path, mExtension);

	// ファイルストリームを開く
	ofs = new std::ofstream(path);
}

// ストリームを閉じる
void Logger::close() {
	ofs->close();
	ofs = NULL;
}

// ファイルに書込を行う
void Logger::flush() {
	ofs->flush();
}

// タイムスタンプの発行
void Logger::timestamp(stamp_type type) {
	time_t now = time(NULL);
	struct tm pnow;
	localtime_s(&pnow, &now);
	
	char buf[128];
	// 時分秒での表示
	if (type == JFB) {
		sprintf_s(buf, "%02d時%02d分%02d秒", pnow.tm_hour, pnow.tm_min, pnow.tm_sec);
	}
	else if(type == COLON) {
		sprintf_s(buf, "%02d:%02d:%02d", pnow.tm_hour, pnow.tm_min, pnow.tm_sec);
	}
	*ofs << buf;
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