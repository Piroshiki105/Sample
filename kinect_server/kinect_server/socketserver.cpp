#include "socketserver.h"

// �R���X�g���N�^
SocketServer::SocketServer() {
	SocketServer(9999);
}

SocketServer::SocketServer(int portNum) {
	this->portNum = portNum;
	args.sock0 = NULL;
	args.data = NULL;
	args.dataLen = 0;
}

// �f�X�g���N�^
SocketServer::~SocketServer() {
	WSACleanup();
}

// ������
int SocketServer::init() {
	WSADATA wsaData;
	WORD version = MAKEWORD(2,0);
	int ret;

	// �������i�������F�g�p����o�[�W����=2.0�j
	ret = WSAStartup(version, &wsaData);
	if(ret != 0) {
		printError("startup");
		//switch(ret) {
		//	case WSASYSNOTREADY:
		//		std::cerr << "�l�b�g���[�N�T�u�V�X�e�����l�b�g���[�N�ւ̐ڑ��������ł��Ă��Ȃ�" << std::endl;
		//		break;
		//	case WSAVERNOTSUPPORTED:
		//		std::cerr << "�v�����ꂽwinsock�̃o�[�W�������T�|�[�g����Ă��Ȃ�" << std::endl;
		//		break;
		//	case WSAEINPROGRESS:
		//		std::cerr << "�u���b�L���O����̎��s���ł��邩�A �܂��̓T�[�r�X�v���o�C�_���R�[���o�b�N�֐����������Ă���" << std::endl;
		//		break;
		//	case WSAEPROCLIM:
		//		std::cerr << "winsock�������ł���ő�v���Z�X���ɒB����" << std::endl;
		//		break;
		//	case WSAEFAULT:
		//		std::cerr << "�������ł���lpWSAData �͗L���ȃ|�C���^�ł͂Ȃ�" << std::endl;
		//		break;
		//}
		WSACleanup();
		return FAILED;
	}


	std::cout << "Winsock������" << std::endl;

	// �\�P�b�g�̍쐬
	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	if(sock0 == INVALID_SOCKET) {
		printError("socket");
		WSACleanup();
		return FAILED;
	}
	std::cout << "�\�P�b�g����" << std::endl;

	SOCKADDR_IN addr;
	// �ݒ�
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNum);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;	// �T�[�o���Ŋ��蓖�Ă��Ă���IP�������Őݒ�

	// �\�P�b�g��IP,�|�[�g�����蓖��
	if(bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		printError("bind");
		WSACleanup();
		return FAILED;
	}

	// �ڑ����󂯕t����L���[���T�ɍ쐬
	if(listen(sock0, 1) != 0) {
		printError("listen");
		WSACleanup();
		return FAILED;
	}

	std::cout << "�ڑ��҂�" << std::endl;

	return SUCCESS;
}

// ���炷
int SocketServer::start() {
	HANDLE hThread;
	DWORD dwThreadID;

	args.sock0 = sock0;

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, (LPVOID)&args, 0, &dwThreadID);
	if(hThread == NULL) {
		return false;
	}
	return true;
}

void SocketServer::ThreadProc(LPVOID pArgs) {
	int ret;
	char buf[BUFSIZE];

	SOCKET sock;
	struct sockaddr_in client;
	int len = sizeof(struct sockaddr);

	SOCKET sock0 = ((pack*)pArgs)->sock0;

	while(true) {

		// �ڑ�
		sock = accept(sock0, (struct sockaddr *)&client, &len);
		if(sock == INVALID_SOCKET) {
			std::cerr << "ERROR[class=SocketServer, function=accept, code=" << WSAGetLastError() << "]" << std::endl;
			if(sock0 != 0) {
				closesocket(sock0);
			}
			ExitThread(0);
		}

		memset(buf, 0, BUFSIZE);
		ret = recv(sock, buf, BUFSIZE, 0);
		if(ret == SOCKET_ERROR) {
			std::cerr << "ERROR[class=SocketServer, function=recv, code=" << WSAGetLastError() << "]" << std::endl;
			closesocket(sock);
			ExitThread(0);
		}

		if(strcmp(buf, "HELLO") != 0) std::cout << "recv=" << buf << std::endl;

		if(((pack*)pArgs)->data != NULL && ((pack*)pArgs)->dataLen > 0) {
			ret = send(sock, ((pack*)pArgs)->data, ((pack*)pArgs)->dataLen, 0);
			if(ret == SOCKET_ERROR) {
				std::cerr << "ERROR[class=SocketServer, function=send, code=" << WSAGetLastError() << "]" << std::endl;
				closesocket(sock);
				ExitThread(0);
			}


		}
		cv::waitKey(30);

		closesocket(sock);
	}
}

void SocketServer::setData(char *data, int dataLen) {
	if(args.data != NULL) {
		delete[] args.data;
	}
	args.data = new char[dataLen];
	memcpy(args.data, data, (size_t)dataLen);
	args.dataLen = dataLen;
}

//	// ��M
//	if(recvBufLen > 0 && recvBuf != NULL) {
//		char *buf = new char[BUFSIZE];
//		int recvLenSum = 0;
//
//		// �o�b�t�@�̏�����
//		memset(recvBuf, 0, sizeof(recvBuf));
//
//		while(1) {
//			memset(buf, 0, BUFLEN);
//			recvLen = recv(sock, buf, BUFLEN, 0);
//
//			// ��M�G���[
//			if(recvLen == SOCKET_ERROR) {
//				printError("recv");
//				return -1;
//			}
//			// ��M�I��
//			else if(recvLen == 0) {
//				recvBuf[recvLenSum] = '\0';
//				break;
//			}
//			// ���܂Ŏ�M�������v
//			if(recvBufLen <= recvLenSum + recvLen) {
//				std::cout << "�o�b�t�@�̑傫���𒴂��܂���(buffer length:" << recvBufLen << ")" << std::endl;
//				break;
//			}
//			// �A��
//			for(int i=0; i<recvLen; i++) {
//				recvBuf[recvLenSum + i] = buf[i];
//			}
//			recvLenSum += recvLen;
//		}
//	}
//
//	// ���M
//	if(sendBufLen > 0 && sendBuf != NULL) {
//		if(send(sock, sendBuf, sendBufLen, 0) == SOCKET_ERROR) {
//			printError("send");
//			return -1;
//		}
//	}
//
//	// �\�P�b�g�����
//	closesocket(sock);
//
//	return 0;
//}

// �G���[�\��
void SocketServer::printError(const char* func) {
	std::cerr << "ERROR[class=SocketServer, function=" << func << ", code=" << WSAGetLastError() << "]" << std::endl;
}