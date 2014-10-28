#include "socketserver.h"

// コンストラクタ
SocketServer::SocketServer() {
	SocketServer(9999);
}

SocketServer::SocketServer(int portNum) {
	this->portNum = portNum;
	args.sock0 = NULL;
	args.data = NULL;
	args.dataLen = 0;
}

// デストラクタ
SocketServer::~SocketServer() {
	WSACleanup();
}

// 初期化
int SocketServer::init() {
	WSADATA wsaData;
	WORD version = MAKEWORD(2,0);
	int ret;

	// 初期化（第一引数：使用するバージョン=2.0）
	ret = WSAStartup(version, &wsaData);
	if(ret != 0) {
		printError("startup");
		//switch(ret) {
		//	case WSASYSNOTREADY:
		//		std::cerr << "ネットワークサブシステムがネットワークへの接続を準備できていない" << std::endl;
		//		break;
		//	case WSAVERNOTSUPPORTED:
		//		std::cerr << "要求されたwinsockのバージョンがサポートされていない" << std::endl;
		//		break;
		//	case WSAEINPROGRESS:
		//		std::cerr << "ブロッキング操作の実行中であるか、 またはサービスプロバイダがコールバック関数を処理している" << std::endl;
		//		break;
		//	case WSAEPROCLIM:
		//		std::cerr << "winsockが処理できる最大プロセス数に達した" << std::endl;
		//		break;
		//	case WSAEFAULT:
		//		std::cerr << "第二引数であるlpWSAData は有効なポインタではない" << std::endl;
		//		break;
		//}
		WSACleanup();
		return FAILED;
	}


	std::cout << "Winsock初期化" << std::endl;

	// ソケットの作成
	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	if(sock0 == INVALID_SOCKET) {
		printError("socket");
		WSACleanup();
		return FAILED;
	}
	std::cout << "ソケット生成" << std::endl;

	SOCKADDR_IN addr;
	// 設定
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNum);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;	// サーバ側で割り当てられているIPを自動で設定

	// ソケットにIP,ポートを割り当て
	if(bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		printError("bind");
		WSACleanup();
		return FAILED;
	}

	// 接続を受け付けるキューを５に作成
	if(listen(sock0, 1) != 0) {
		printError("listen");
		WSACleanup();
		return FAILED;
	}

	std::cout << "接続待ち" << std::endl;

	return SUCCESS;
}

// 走らす
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

		// 接続
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

//	// 受信
//	if(recvBufLen > 0 && recvBuf != NULL) {
//		char *buf = new char[BUFSIZE];
//		int recvLenSum = 0;
//
//		// バッファの初期化
//		memset(recvBuf, 0, sizeof(recvBuf));
//
//		while(1) {
//			memset(buf, 0, BUFLEN);
//			recvLen = recv(sock, buf, BUFLEN, 0);
//
//			// 受信エラー
//			if(recvLen == SOCKET_ERROR) {
//				printError("recv");
//				return -1;
//			}
//			// 受信終了
//			else if(recvLen == 0) {
//				recvBuf[recvLenSum] = '\0';
//				break;
//			}
//			// 今まで受信した合計
//			if(recvBufLen <= recvLenSum + recvLen) {
//				std::cout << "バッファの大きさを超えました(buffer length:" << recvBufLen << ")" << std::endl;
//				break;
//			}
//			// 連結
//			for(int i=0; i<recvLen; i++) {
//				recvBuf[recvLenSum + i] = buf[i];
//			}
//			recvLenSum += recvLen;
//		}
//	}
//
//	// 送信
//	if(sendBufLen > 0 && sendBuf != NULL) {
//		if(send(sock, sendBuf, sendBufLen, 0) == SOCKET_ERROR) {
//			printError("send");
//			return -1;
//		}
//	}
//
//	// ソケットを閉じる
//	closesocket(sock);
//
//	return 0;
//}

// エラー表示
void SocketServer::printError(const char* func) {
	std::cerr << "ERROR[class=SocketServer, function=" << func << ", code=" << WSAGetLastError() << "]" << std::endl;
}