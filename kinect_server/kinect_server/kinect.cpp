/************************************************************
*
*	kinect.cpp
*		kinect���g�p���邽�߂̃v���O����
*
*	2014.7.18
*
*		Matsumoto Hirosato
*
*
*************************************************************/

#include "kinect.h"

#include <iostream>

// �R���X�g���N�^�@�e��ϐ�������������
Kinect::Kinect() :
	pNuiSensor(NULL),
	hNextColorFrameEvent(INVALID_HANDLE_VALUE),
	pColorStreamHandle(INVALID_HANDLE_VALUE),
	hNextDepthFrameEvent(INVALID_HANDLE_VALUE),
	pDepthStreamHandle(INVALID_HANDLE_VALUE),
	bInitialized(FALSE),
	isNearMode(FALSE),
	RESOLUTION(NUI_IMAGE_RESOLUTION_640x480),
	SIZE(WIDTH*HEIGHT)
{
}

// �f�X�g���N�^
Kinect::~Kinect() {
	if (bInitialized)
    {
        NuiShutdown();
    }

    if (hNextColorFrameEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hNextColorFrameEvent);
    }

	if(hNextDepthFrameEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hNextDepthFrameEvent);
	}
}

// ������
int Kinect::init() {
	// �L�l�N�g�̐��𐔂���
	int sensorCount;
	NuiGetSensorCount(&sensorCount);

	// �L�l�N�g��������Ȃ������Ƃ�
	if(sensorCount == 0) {
		std::cerr << "Kinect���ڑ�����Ă��܂���" << std::endl;
		return FAILD;
	}

	NuiCreateSensorByIndex(0, &pNuiSensor);

	// �L�l�N�g���g�����߂̊e��ϐ�������
	pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
	hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


	// RGB�Z���T�X�g���[���̎擾
	HRESULT hr;
	hr = pNuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, RESOLUTION, 0, 2, hNextColorFrameEvent, &pColorStreamHandle);
	if(FAILED(hr)) {
		std::cerr << "RGB�Z���T�X�g���[���̎擾�Ɏ��s���܂���" << std::endl;
		return FAILD;
	}

	// �[�x�Z���T�X�g���[���̎擾
	hr = pNuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, RESOLUTION, 0, 2, hNextDepthFrameEvent, &pDepthStreamHandle);
	if(FAILED(hr)) {
		std::cerr << "�[�x�Z���T�X�g���[���̎擾�Ɏ��s���܂���" << std::endl;
		return FAILD;
	}

	// ����������
	std::cout << "Kinect�̏������ɐ����I" << std::endl;
	bInitialized = TRUE;

	return SUCCESS;
}

void Kinect::getSize(int *width, int *height) {
	*width = WIDTH;
	*height = HEIGHT;
}

// RGB����ѐ[�x�Z���T�摜���擾����֐�(�摜��OpenCV��Mat�N���X)
int Kinect::getImage(cv::Mat &image, int type) {

	HANDLE frameEvent;
	HANDLE streamHandle;

	if(type == IMAGE_TYPE_COLOR) {
		frameEvent = hNextColorFrameEvent;
		streamHandle = pColorStreamHandle;
	}
	else {
		frameEvent = hNextDepthFrameEvent;
		streamHandle = pDepthStreamHandle;
	}

	const NUI_IMAGE_FRAME *pImageFrame = NULL;

	WaitForSingleObject(frameEvent, INFINITE);

	HRESULT hr = NuiImageStreamGetNextFrame(streamHandle, 30, &pImageFrame);
	if(FAILED(hr)) {
		return -1;
	}

	INuiFrameTexture *pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0) {
		BYTE *pBuffer = (BYTE*)LockedRect.pBits;
		memcpy(image.data, pBuffer, image.step * image.rows);
	}

	hr = NuiImageStreamReleaseFrame(streamHandle, pImageFrame);
	if(FAILED(hr)) {
		return -1;
	}

	return 0;
}

// RGB�摜���擾����֐�(�摜��OpenCV��Mat�N���X)
int Kinect::getColorImage(cv::Mat &color) {

	NUI_IMAGE_FRAME imageFrame = {0};

	WaitForSingleObject(hNextColorFrameEvent, INFINITE);

	HRESULT hr = pNuiSensor->NuiImageStreamGetNextFrame(pColorStreamHandle, 30, &imageFrame);
	if(FAILED(hr)) {
		return -1;
	}

	INuiFrameTexture *pTexture = imageFrame.pFrameTexture;

	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0) {
		BYTE *pBuffer = (BYTE*)LockedRect.pBits;
		memcpy(color.data, pBuffer, color.step * color.rows);
	}

	hr = pNuiSensor->NuiImageStreamReleaseFrame(pColorStreamHandle, &imageFrame);
	if(FAILED(hr)) {
		return -1;
	}

	return 0;
}

// �[�x�Z���T�摜���擾����֐�
int Kinect::getDepthImage(NUI_DEPTH_IMAGE_PIXEL** ppDepthPixels) {

	NUI_IMAGE_FRAME imageFrame = {0};

	WaitForSingleObject(hNextDepthFrameEvent, INFINITE);

	HRESULT hr = pNuiSensor->NuiImageStreamGetNextFrame(pDepthStreamHandle, 30, &imageFrame);
	if(FAILED(hr)) {
		return -1;
	}

	INuiFrameTexture *pTexture = NULL;

	hr = pNuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(pDepthStreamHandle, &imageFrame, &isNearMode, &pTexture);
	if(FAILED(hr)) {
		return -1;
	}

	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	*ppDepthPixels = (NUI_DEPTH_IMAGE_PIXEL*)LockedRect.pBits;

	hr = pNuiSensor->NuiImageStreamReleaseFrame(pDepthStreamHandle, &imageFrame);
	if(FAILED(hr)) {
		return -1;
	}

	return 0;
}

// �[�x�Z���T�摜���擾����֐�
int Kinect::getDepthImage(NUI_DEPTH_IMAGE_PIXEL** ppDepthPixels, cv::Mat &depth) {

	// �f�v�X�摜�̎擾
	if(getDepthImage(ppDepthPixels) == -1) {
		return -1;
	}

#ifndef USE_PLAYER_INDEX
	long size = WIDTH * HEIGHT;
	// �f�v�X�摜���쐬
	for(long i = 0; i < size; i++) {
		depth.data[i*2] = (uchar)((*ppDepthPixels)[i].depth & 0xff);
		depth.data[i*2+1] = (uchar)((*ppDepthPixels)[i].depth >> 8);
	}

#else

	// �f�v�X�摜���쐬
	for ( int y = 0; y < HEIGHT; y++ ) {
		for ( int x = 0; x < WIDTH; x++ ) {
			int depthIndex = (y * WIDTH) + x;
			int cameraIndex = depthIndex * 3;

			// �v���C���[��F�����Ă��ȏꍇ
			if ((*ppDepthPixels)[depthIndex].playerIndex == 0 ) {
				unsigned char gray = (unsigned char)~((*ppDepthPixels)[depthIndex].depth * 255 / 0x3FFF);

				// 4m��艓���Ƃ���͐Ԃ��ۂ�
				if ( (*ppDepthPixels)[depthIndex].depth > 4000 ) {
					depth.data[cameraIndex] = gray;
					depth.data[cameraIndex + 1] = gray;
					depth.data[cameraIndex + 2] = 255;
				}
				// 4m���߂��ꍇ�͒ʏ탂�[�h
				else {
					depth.data[cameraIndex] = gray;
					depth.data[cameraIndex + 1] = gray;
					depth.data[cameraIndex + 2] = gray;
				}
			}

			// �v���C���[��F�������ꍇ
			else {
					depth.data[cameraIndex] = 0;
					depth.data[cameraIndex + 1] = 255;
					depth.data[cameraIndex + 2] = 0;
			}
		}
	}
#endif

	return 0;
}


// �L�l�N�g�̃`���g�T�[�{�̊p�x��ݒ肷��
void Kinect::setTiltAngle(LONG angle) {
	if(angle < NUI_CAMERA_ELEVATION_MINIMUM) angle = NUI_CAMERA_ELEVATION_MINIMUM;
	if(angle > NUI_CAMERA_ELEVATION_MAXIMUM) angle = NUI_CAMERA_ELEVATION_MAXIMUM;

	LONG currentAngle = 0;
	NuiCameraElevationGetAngle(&currentAngle);		

	if(angle != currentAngle) {
		NuiCameraElevationSetAngle(angle);
	}
}

// �L�l�N�g�̃`���g�T�[�{�̊p�x���擾����
LONG Kinect::getTiltAngle(void) {
	LONG currentAngle = 0;
	NuiCameraElevationGetAngle(&currentAngle);		

	return currentAngle;
}


// Near���[�h���g�p����
int Kinect::useNearMode() {
	HRESULT hr;

	hr = NuiImageStreamSetImageFrameFlags(pDepthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
	if(FAILED(hr)) {
		std::cerr << "Near���[�h�̐ݒ�Ɏ��s���܂���" << std::endl;
		return -1;
	}

	isNearMode = TRUE;
	return 0;
}

// �J�����̐ݒ��ύX
int Kinect::changeCameraSettings() {

	HRESULT hr;

	// Kinect�̃J���[�J�����ݒ�
	INuiColorCameraSettings *cameraSettings;

	hr = pNuiSensor->NuiGetColorCameraSettings(&cameraSettings);
	if(FAILED(hr)) {
		std::cerr << "�J�����̐ݒ���̎擾�Ɏ��s���܂����B" << std::endl;
		return -1;
	}

	BOOL isAutoWhite = FALSE;
	BOOL isAutoExposure = FALSE;

	cameraSettings->GetAutoExposure(&isAutoExposure);
	cameraSettings->GetAutoWhiteBalance(&isAutoWhite);

	if(isAutoExposure) cameraSettings->SetAutoExposure(FALSE);
	if(isAutoWhite)    cameraSettings->SetAutoWhiteBalance(FALSE);

	return 0;
}

// �f�v�X����3�������W���擾����
Vector4 Kinect::getRealPointFromDepth(cv::Mat depth, int x, int y) {
	ushort *dp = (ushort*)depth.data;
	return NuiTransformDepthImageToSkeleton(x, y, dp[y*WIDTH+x], RESOLUTION);
}

// �f�v�X����3�������W���擾����
Vector4 Kinect::getRealPointFromDepth(cv::Mat depth, cv::Point p) {
	ushort *dp = (ushort*)depth.data;
	return NuiTransformDepthImageToSkeleton(p.x, p.y, dp[p.y*WIDTH+p.x], RESOLUTION);
}


// �f�v�X�f�[�^����O�������W���擾����
int Kinect::getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* pDepthPixels, Vector4 *realPoint, int colorX, int colorY) {

	HRESULT hr;
	INuiCoordinateMapper *pMapper;

	// �}�b�p�[�N���X�̎擾
	hr = pNuiSensor->NuiGetCoordinateMapper(&pMapper);
	if(FAILED(hr)) {
		return -1;
	}

	// �������̊m��
	NUI_DEPTH_IMAGE_POINT *pDepthPoints = new NUI_DEPTH_IMAGE_POINT[SIZE];

	// �J���[�摜�t���[���̍��W�n�ɐ[�x�摜�t���[�����d�˂�
	hr = pMapper->MapColorFrameToDepthFrame(NUI_IMAGE_TYPE_COLOR, RESOLUTION, RESOLUTION, SIZE, pDepthPixels, SIZE, pDepthPoints);
	if(!FAILED(hr)) {
		// �[�x�摜��̓_����3�������W�̈ʒu���擾����
		hr = pMapper->MapDepthPointToSkeletonPoint(RESOLUTION, &pDepthPoints[colorY * WIDTH + colorX], realPoint);
	}

	// �������̉��
	delete[] pDepthPoints;

	if(FAILED(hr)) {
		return -1;
	}

	return 0;
}

// �f�v�X�f�[�^����O�������W���擾����
int Kinect::getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* pDepthPixel, Vector4 *realPoint, cv::Point colorPoint) {
	return getRealPointFromColorPixel(pDepthPixel, realPoint, colorPoint.x, colorPoint.y);
}

// �J���[�摜�̍��W�n�ɂR�������W���d�˂�
int Kinect::getRealPointFrameFromColorFrame(NUI_DEPTH_IMAGE_PIXEL* pDepthPixels, Vector4 *realPointFrame) {
	HRESULT hr;
	INuiCoordinateMapper *pMapper;

	// �}�b�p�[�N���X�̎擾
	hr = pNuiSensor->NuiGetCoordinateMapper(&pMapper);
	if(FAILED(hr)) {
		return -1;
	}

	// �J���[�摜�̍��W�n�ɂR�������W���d�˂�
	hr = pMapper->MapColorFrameToSkeletonFrame(NUI_IMAGE_TYPE_COLOR, RESOLUTION, RESOLUTION, SIZE, pDepthPixels, SIZE, realPointFrame);
	if(FAILED(hr)) {
		std::cout << "erroo" << std::endl;
		return -1;
	}

	return 0;
}