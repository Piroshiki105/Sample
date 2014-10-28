#include "MattoJpeg.h"

MattoJpeg::MattoJpeg(void)
{
}

MattoJpeg::~MattoJpeg(void)
{
}

int MattoJpeg::mattoJpeg(cv::Mat src, std::vector<uchar> *dst) {
	std::vector<int> param = std::vector<int>(2);
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 80;		// jpgÇÃïiéøÅi0-100Åj

	cv::imencode(".jpg", src, *dst, param);
	//jpgImage = cv::imdecode(cv::Mat(*dst), CV_LOAD_IMAGE_COLOR);

	//cv::imshow("jpg", jpgImage);

	return 0;
}