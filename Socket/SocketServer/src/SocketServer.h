#ifndef _INCLUDED_SOCKET_SERVER_H_
#define _INCLUDED_SOCKET_SERVER_H_

#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


class SocketServer {
private:
	// WinSockデータ
	WSADATA wsaData;
	// スレッドハンドル
	HANDLE hThread;
	// スレッドID
	DWORD ThreadId;
	// ソケット
	SOCKET sockListen;
	unsigned int port;

	fd_set fdRead;
	struct timeval tv;

	// スレッド終了フラグ
	bool bExitThread;
	
	// バッファサイズ
	int bufSize;

	// 受け取るデータ
	char recvData[256];
	int recvDataLen;
	
	// 送るデータ
	char *sendData;
	int sendDataLen;

	// スレッド処理関数
	static DWORD WINAPI _threadProc(LPVOID lpParam);
	DWORD WINAPI threadProc();

public:
	SocketServer();
	~SocketServer();

	int init();

	void setSendData(const char* data, int len);
	void getRecvData(void* data, int len);

};

#endif