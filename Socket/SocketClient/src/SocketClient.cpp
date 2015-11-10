#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

int main(void) {
	WSADATA wsaData;
	SOCKET sock;
	struct sockaddr_in server;
	char buf[32];

	WSAStartup(MAKEWORD(2,0), &wsaData);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(9999);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	connect(sock, (struct sockaddr*)&server, sizeof(server));

	memset(buf, 0, sizeof(buf));
	// send(sock, "HELLO", 6, 0);

	//do {
		recv(sock, buf, 5, 0);
		//if (strcmp(buf, "") != 0)
		std::cout << buf << std::endl;
	//} while (strcmp(buf, "FIN"));

	closesocket(sock);

	WSACleanup();

	return 0;
}