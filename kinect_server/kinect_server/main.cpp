#include "socketserver.h"
#include "kinect.h"
#include "MattoJpeg.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {

	// RGB画像
	cv::Mat color(480, 640, CV_8UC4);

	Kinect kinect;
	if(kinect.init() == Kinect::FAILD) {
		std::cerr << "Kinectの初期化に失敗" << std::endl;
		return -1;
	}

	SocketServer ss(9999);
	if(ss.init() == SocketServer::FAILED) {
		std::cerr << "ソケットサーバの初期化に失敗" << std::endl;
		return -1;
	}

	char recvBuf[128];
	recvBuf[0] = '\0';

	ss.start();

	std::vector<uchar> buf;
	do {
		kinect.getColorImage(color);
		MattoJpeg::mattoJpeg(color, &buf);

//		sendBuf = (char *)&buf[0];
//		size = buf.size();
		ss.setData((char*)&buf[0], buf.size());

		if(recvBuf[0] != '\0') {
			std::cout << recvBuf << std::endl;
		}		
	} while(cv::waitKey(2) != 27);

	return 0;
}