#ifndef _INCLUDED_SOCKET_SERVER_H_
#define _INCLUDED_SOCKET_SERVER_H_

#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


class SocketServer {
private:
	// WinSock�f�[�^
	WSADATA wsaData;
	// �X���b�h�n���h��
	HANDLE hThread;
	// �X���b�hID
	DWORD ThreadId;
	// �\�P�b�g
	SOCKET sockListen;
	unsigned int port;

	fd_set fdRead;
	struct timeval tv;

	// �X���b�h�I���t���O
	bool bExitThread;
	
	// �o�b�t�@�T�C�Y
	int bufSize;

	// �󂯎��f�[�^
	char recvData[256];
	int recvDataLen;
	
	// ����f�[�^
	char *sendData;
	int sendDataLen;

	// �X���b�h�����֐�
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