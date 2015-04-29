#include "SocketServer.h"

// �X���b�h����
DWORD WINAPI SocketServer::_threadProc(LPVOID lpParam) {
	return ((SocketServer*)lpParam)->threadProc();
}

// �X���b�h����
DWORD WINAPI SocketServer::threadProc() {
	SOCKET sockWork = INVALID_SOCKET;

	// �^�C���A�E�g�܂ł̎��Ԃ�ݒ�
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	// �X���b�h�I���t���O�����܂�
	while(!bExitThread) {
		FD_ZERO(&fdRead);
		FD_SET(sockListen, &fdRead);
		if(sockWork != INVALID_SOCKET)
			FD_SET(sockWork, &fdRead);

		int ret = select(0, &fdRead, NULL, NULL, &tv);
		if(ret == SOCKET_ERROR) {
			std::cout << "SOCKET ERROR" << std::endl;
			break;
		} else if(ret == 0) {
			continue;
		}

		if(FD_ISSET(sockListen, &fdRead)) {
			struct sockaddr_in client;
			int len = sizeof(client);
			// �ڑ��҂�
			sockWork = accept(sockListen, (struct sockaddr*)&client, &len);
			if(sockWork == INVALID_SOCKET) {
				std::cout << "accept error" << std::endl;
				closesocket(sockListen);
				return -1;
			}
		}

		if(sockWork != INVALID_SOCKET && FD_ISSET(sockWork, &fdRead)) {
			recvDataLen = recv(sockWork, recvData, bufSize, 0);
			if(recvDataLen == 0) {
				shutdown(sockWork, SD_BOTH);
				closesocket(sockWork);
				sockWork = INVALID_SOCKET;
			}
			else if(recvDataLen > 0) {
				send(sockWork, sendData, sendDataLen, 0);
			}
		}
	}

	closesocket(sockWork);
	ExitThread(0);
	return 0;
}

// �R���X�g���N�^
SocketServer::SocketServer() {
	hThread = NULL;
	port = 9999;
	bExitThread = false;
	
	sendData = NULL;
	sendDataLen  = 0;

	recvDataLen = 0;
	bufSize = 256;
}

int SocketServer::init() {
	// Winsock2�g�p����
	WSAStartup(MAKEWORD(2,0), &wsaData);

	// �\�P�b�g�쐬
	sockListen = socket(AF_INET, SOCK_STREAM, 0);
	if(sockListen == INVALID_SOCKET) {
		return -1;
	}

	// �A�h���X�Ȃǐݒ�
	struct sockaddr_in addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	// �o�C���h
	if(bind(sockListen, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		closesocket(sockListen);
		return -1;
	}

	// ���b�X��
	if(listen(sockListen, 1) == SOCKET_ERROR) {
		closesocket(sockListen);
		return -1;
	}

	// �X���b�h�쐬
	std::cout << "Socket Server Thread Start" << std::endl;
	hThread = CreateThread(NULL, 0, _threadProc, (void*)this, 0, &ThreadId);

	return 0;
}

// �f�X�g���N�^
SocketServer::~SocketServer() {
	if(hThread != NULL) {
		bExitThread = true;
		if(WaitForSingleObject(hThread, 1000) == WAIT_TIMEOUT) {
			TerminateThread(hThread, 0);
		}
		CloseHandle(hThread);
		std::cout << "Close Thread Handle." << std::endl;
	}

	if(sockListen != INVALID_SOCKET) {
		closesocket(sockListen);
		std::cout << "Close Socket." << std::endl;
		WSACleanup();
	}

	if(sendData != NULL) delete[] sendData;
}

// ����f�[�^���Z�b�g����
void SocketServer::setSendData(const char *data, int len) {
	if(sendDataLen != len) {
		if(sendData != NULL)
			delete[] sendData;
		sendData = new char[len];
		sendDataLen = len;
	}
	memcpy(sendData, data, sizeof(char)*sendDataLen);
}

// �f�[�^���󂯎��
void SocketServer::getRecvData(void* data, int len) {
	if(recvDataLen > 0) {
		memcpy(data, recvData, len);
	}
}