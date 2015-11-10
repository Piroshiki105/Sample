#include "SocketServer.h"

int main(void) {
	SocketServer ss;
	ss.init();
	char data[32] = "TEST";

	while(data[0] == 'T') {
		ss.setSendData("TEST", 5);
		ss.getRecvData(data, 32);
	}
	std::cout << data << std::endl;
	getchar();
	return 0;
}