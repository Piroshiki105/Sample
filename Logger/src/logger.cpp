#include "logger.h"

Logger::Logger() {
}

// ファイルオープン
int Logger::open(const char *fileName) {
	// ファイルの名前が空の時
	if(fileName == NULL) {
		fileName = "default name";
	}

	// ファイルを開く
	logFile = fopen(fileName, "w");

	// ファイルの取得に失敗したとき
	if(logFile == NULL) return 0;
	
	return 1;
}

// ファイルストリーム書き込み
int Logger::write(const char *str) {
	
	if(fileName != NULL) {
		fputs(str, logFile);
		return 1;
	}

	// 書き込みできていない
	return 0;
}

// ファイルクローズ
int Logger::close() {
	// ログファイルが取得できているとき
	if(logFile != NULL) {
		// ファイルのクローズに成功したとき
		if(fclose(logFile) == 0) return 1;
	}

	// ファイル書き込みできていない
	return 0;
}