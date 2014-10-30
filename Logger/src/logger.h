#ifndef __INCLUDED_LOGGER_H_
#define __INCLUDED_LOGGER_H_

#include <cstdio>

class Logger {
private:
	char fileName[50];
	FILE *logFile;

public:
	Logger();
	int open(const char *fileName = NULL);
	int write(const char *str);
	int close();

};

#endif