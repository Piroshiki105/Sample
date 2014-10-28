#ifndef __INCLUDED_KINECT_H_
#define __INCLUDED_KINECT_H_


#include <windows.h>
#include <NuiApi.h>

#include <opencv2/core/core.hpp>

class Kinect {
private:
	// Kinect�Z���T�[
	INuiSensor* pNuiSensor;

	// RGB�摜�p�n���h��
	HANDLE hNextColorFrameEvent;
	HANDLE pColorStreamHandle;

	// Depth�摜�p�n���h��
	HANDLE hNextDepthFrameEvent;
	HANDLE pDepthStreamHandle;

	// ���������ł�����
	BOOL bInitialized;

	// �j�A���[�h���g���Ă��邩
	BOOL isNearMode;

	// �J�����摜�̕��E����
	static const int WIDTH = 640;
	static const int HEIGHT = 480;

	// �𑜓x�̐ݒ�
	const NUI_IMAGE_RESOLUTION RESOLUTION;

	// �摜�z��̃T�C�Y
	const DWORD SIZE;

public:
	// getImage�֐��ɂĎg���@�擾����摜�̎��
	static const int IMAGE_TYPE_COLOR = 0;
	static const int IMAGE_TYPE_DEPTH = 1;
	// static const int IMAGE_TYPE_SKELETON = 2;

	static const int SUCCESS = 1;
	static const int FAILD = -1;

	// �R���X�g���N�^
	Kinect();
	// �f�X�g���N�^
	~Kinect();
	// ������
	int init();
	// �摜�̑傫����Ԃ�
	void getSize(int *width, int *height);
	// �`���g�T�[�{�̊p�x��ς���
	void setTiltAngle(LONG angle);
	LONG getTiltAngle(void);
	// �j�A���[�h���I���ɂ���
	int useNearMode();

	// �摜���擾����(IMAGE_TYPE_COLOR�F�J���[�摜�AIMAGE_TYPE_DEPTH�F�[�x�摜)
	int getImage(cv::Mat &image, int type = IMAGE_TYPE_COLOR);
	// �J���[�摜�̎擾
	int getColorImage(cv::Mat &color);
	// �[�x�摜�̎擾
	int getDepthImage(NUI_DEPTH_IMAGE_PIXEL** depthPixel);
	int getDepthImage(NUI_DEPTH_IMAGE_PIXEL** depthPixel, cv::Mat& depth);

	// �J�����̘I�o�E�z���C�g�o�����X�̎������ߋ@�\���I�t�ɂ���
	int changeCameraSettings();

	// �[�x�摜�̍��W�n���J���[�摜�̍��W�n�ɍ��킹��
	void changeCoordinateDepthtoImage(cv::Mat depth, cv::Mat *dst);
	void changeCoordinateDepthtoImage(cv::Mat depth, cv::Mat *depth2, cv::Rect roi);
	// void changeCoordinateDepthtoImage(cv::Mat depth, DepthMap *depthMap, cv::Rect roi);

	// �f�v�X�摜����3�������W���擾����
	Vector4 getRealPointFromDepth(cv::Mat depth, int x, int y);
	Vector4 getRealPointFromDepth(cv::Mat depth, cv::Point p);

	// �J���[�摜�̍��W����3�������W���擾����
	int getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* depthPixels, Vector4* realPoint, int colorX, int colorY);
	int getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* depthPixels, Vector4* realPoint, cv::Point colorPoint);

	// �J���[�摜�̍��W�n��3�������W���d�˂�
	int getRealPointFrameFromColorFrame(NUI_DEPTH_IMAGE_PIXEL* pDepthPixels, Vector4 *realPointFrame);

};

#endif