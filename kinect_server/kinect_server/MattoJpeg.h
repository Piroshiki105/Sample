#ifndef __INCLUDED_MATTOJPEG_H__
#define __INCLUDED_MATTOJPEG_H__

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class MattoJpeg
{
public:
	MattoJpeg(void);
	~MattoJpeg(void);

	static int mattoJpeg(cv::Mat src, std::vector<uchar>* dst);
};

#endif