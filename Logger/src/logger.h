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
	// ファイルストリーム
	std::ofstream *ofs;

	// ディレクトリが存在しているかチェック、なければ作る
	void checkExistDir(const char *dirName, size_t size);

public:
	// コンストラクタ
	// 引数　　　fileName：ファイル名(なしの場合はデフォルトネーム)
	Logger(const char *fileName = NULL);

	// 文字列をファイルに記述
	// 引数　　　str：記述文字列
	// 返り値　　記述が成功したか否か
	int write(const char *str);

};

#endif