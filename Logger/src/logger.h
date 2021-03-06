#ifndef __INCLUDED_LOGGER_H_
#define __INCLUDED_LOGGER_H_

#include <cstdio>
#include <fstream>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <tchar.h>

typedef enum eStamp { COLON, JFB } stamp_type;

class Logger {
private:
	// ファイルストリーム
	std::ofstream *ofs;

	// ファイル名
	char mFileName[128];

	// 拡張子
	char mExtension[8];

	// ディレクトリが存在しているかチェック、なければ作る
	void checkExistDir(const char *dirName, size_t size);

	// ファイルの最後のナンバーを取得する
	int lastFileNum(const char *path, size_t size);

public:
	// コンストラクタ
	// 引数　　　fileName：ファイル名(なしの場合はデフォルトネーム)
	Logger(const char *fileName = NULL, const char *extension = NULL);

	// デストラクタ
	Logger::~Logger();

	// ストリームを開く
	void open(const char *fileName = NULL, const char *extension = NULL);

	// ストリームを閉じる
	void close();

	// ファイルに書込を行う
	void flush();

	// タイムスタンプ
	void timestamp(stamp_type type = COLON);

	// ファイルに記述
	inline void write(const char   c) { *ofs << c; }
	inline void write(const char  *s) { *ofs << s; }
	inline void write(const int    i) { *ofs << i; }
	inline void write(const float  f) { *ofs << f; }
	inline void write(const double d) { *ofs << d; }

	// ファイルに記述
	Logger& operator<<(const char   c);
	Logger& operator<<(const char  *s);
	Logger& operator<<(const int    i);
	Logger& operator<<(const float  f);
	Logger& operator<<(const double d);
};

#endif