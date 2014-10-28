#ifndef __INCLUDED_SOCKETSERVER_H__
#define __INCLUDED_SOCKETSERVER_H__

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#pragma comment(lib, "ws2_32.lib")

class SocketServer {
private:
	int portNum;
	SOCKET sock0;
	static const int BUFSIZE = 1024;
	

	typedef struct tPack {
		SOCKET sock0;
		char *data;
		int dataLen;
	} pack;

	pack args;

	fd_set fds, readfds;

	// ÉGÉâÅ[ï\é¶
	void printError(const char* func);
	
public:
	const static int FAILED = -1;
	const static int SUCCESS = 1;

	SocketServer();
	SocketServer(int portNum);
	~SocketServer(void);
	int init(void);
	int start();
	void setData(char* data, int size);
	static void ThreadProc(LPVOID args);
};

#endif