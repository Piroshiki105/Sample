/************************************************************
*
*	kinect.cpp
*		kinectを使用するためのプログラム
*
*	2014.7.18
*
*		Matsumoto Hirosato
*
*
*************************************************************/

#include "kinect.h"

#include <iostream>

// コンストラクタ　各種変数を初期化する
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

// デストラクタ
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

// 初期化
int Kinect::init() {
	// キネクトの数を数える
	int sensorCount;
	NuiGetSensorCount(&sensorCount);

	// キネクトが見つからなかったとき
	if(sensorCount == 0) {
		std::cerr << "Kinectが接続されていません" << std::endl;
		return FAILD;
	}

	NuiCreateSensorByIndex(0, &pNuiSensor);

	// キネクトを使うための各種変数初期化
	pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
	hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


	// RGBセンサストリームの取得
	HRESULT hr;
	hr = pNuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, RESOLUTION, 0, 2, hNextColorFrameEvent, &pColorStreamHandle);
	if(FAILED(hr)) {
		std::cerr << "RGBセンサストリームの取得に失敗しました" << std::endl;
		return FAILD;
	}

	// 深度センサストリームの取得
	hr = pNuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, RESOLUTION, 0, 2, hNextDepthFrameEvent, &pDepthStreamHandle);
	if(FAILED(hr)) {
		std::cerr << "深度センサストリームの取得に失敗しました" << std::endl;
		return FAILD;
	}

	// 初期化完了
	std::cout << "Kinectの初期化に成功！" << std::endl;
	bInitialized = TRUE;

	return SUCCESS;
}

void Kinect::getSize(int *width, int *height) {
	*width = WIDTH;
	*height = HEIGHT;
}

// RGBおよび深度センサ画像を取得する関数(画像はOpenCVのMatクラス)
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

// RGB画像を取得する関数(画像はOpenCVのMatクラス)
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

// 深度センサ画像を取得する関数
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

// 深度センサ画像を取得する関数
int Kinect::getDepthImage(NUI_DEPTH_IMAGE_PIXEL** ppDepthPixels, cv::Mat &depth) {

	// デプス画像の取得
	if(getDepthImage(ppDepthPixels) == -1) {
		return -1;
	}

#ifndef USE_PLAYER_INDEX
	long size = WIDTH * HEIGHT;
	// デプス画像を作成
	for(long i = 0; i < size; i++) {
		depth.data[i*2] = (uchar)((*ppDepthPixels)[i].depth & 0xff);
		depth.data[i*2+1] = (uchar)((*ppDepthPixels)[i].depth >> 8);
	}

#else

	// デプス画像を作成
	for ( int y = 0; y < HEIGHT; y++ ) {
		for ( int x = 0; x < WIDTH; x++ ) {
			int depthIndex = (y * WIDTH) + x;
			int cameraIndex = depthIndex * 3;

			// プレイヤーを認識していな場合
			if ((*ppDepthPixels)[depthIndex].playerIndex == 0 ) {
				unsigned char gray = (unsigned char)~((*ppDepthPixels)[depthIndex].depth * 255 / 0x3FFF);

				// 4mより遠いところは赤っぽく
				if ( (*ppDepthPixels)[depthIndex].depth > 4000 ) {
					depth.data[cameraIndex] = gray;
					depth.data[cameraIndex + 1] = gray;
					depth.data[cameraIndex + 2] = 255;
				}
				// 4mより近い場合は通常モード
				else {
					depth.data[cameraIndex] = gray;
					depth.data[cameraIndex + 1] = gray;
					depth.data[cameraIndex + 2] = gray;
				}
			}

			// プレイヤーを認識した場合
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


// キネクトのチルトサーボの角度を設定する
void Kinect::setTiltAngle(LONG angle) {
	if(angle < NUI_CAMERA_ELEVATION_MINIMUM) angle = NUI_CAMERA_ELEVATION_MINIMUM;
	if(angle > NUI_CAMERA_ELEVATION_MAXIMUM) angle = NUI_CAMERA_ELEVATION_MAXIMUM;

	LONG currentAngle = 0;
	NuiCameraElevationGetAngle(&currentAngle);		

	if(angle != currentAngle) {
		NuiCameraElevationSetAngle(angle);
	}
}

// キネクトのチルトサーボの角度を取得する
LONG Kinect::getTiltAngle(void) {
	LONG currentAngle = 0;
	NuiCameraElevationGetAngle(&currentAngle);		

	return currentAngle;
}


// Nearモードを使用する
int Kinect::useNearMode() {
	HRESULT hr;

	hr = NuiImageStreamSetImageFrameFlags(pDepthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
	if(FAILED(hr)) {
		std::cerr << "Nearモードの設定に失敗しました" << std::endl;
		return -1;
	}

	isNearMode = TRUE;
	return 0;
}

// カメラの設定を変更
int Kinect::changeCameraSettings() {

	HRESULT hr;

	// Kinectのカラーカメラ設定
	INuiColorCameraSettings *cameraSettings;

	hr = pNuiSensor->NuiGetColorCameraSettings(&cameraSettings);
	if(FAILED(hr)) {
		std::cerr << "カメラの設定情報の取得に失敗しました。" << std::endl;
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

// デプスから3次元座標を取得する
Vector4 Kinect::getRealPointFromDepth(cv::Mat depth, int x, int y) {
	ushort *dp = (ushort*)depth.data;
	return NuiTransformDepthImageToSkeleton(x, y, dp[y*WIDTH+x], RESOLUTION);
}

// デプスから3次元座標を取得する
Vector4 Kinect::getRealPointFromDepth(cv::Mat depth, cv::Point p) {
	ushort *dp = (ushort*)depth.data;
	return NuiTransformDepthImageToSkeleton(p.x, p.y, dp[p.y*WIDTH+p.x], RESOLUTION);
}


// デプスデータから三次元座標を取得する
int Kinect::getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* pDepthPixels, Vector4 *realPoint, int colorX, int colorY) {

	HRESULT hr;
	INuiCoordinateMapper *pMapper;

	// マッパークラスの取得
	hr = pNuiSensor->NuiGetCoordinateMapper(&pMapper);
	if(FAILED(hr)) {
		return -1;
	}

	// メモリの確保
	NUI_DEPTH_IMAGE_POINT *pDepthPoints = new NUI_DEPTH_IMAGE_POINT[SIZE];

	// カラー画像フレームの座標系に深度画像フレームを重ねる
	hr = pMapper->MapColorFrameToDepthFrame(NUI_IMAGE_TYPE_COLOR, RESOLUTION, RESOLUTION, SIZE, pDepthPixels, SIZE, pDepthPoints);
	if(!FAILED(hr)) {
		// 深度画像上の点から3次元座標の位置を取得する
		hr = pMapper->MapDepthPointToSkeletonPoint(RESOLUTION, &pDepthPoints[colorY * WIDTH + colorX], realPoint);
	}

	// メモリの解放
	delete[] pDepthPoints;

	if(FAILED(hr)) {
		return -1;
	}

	return 0;
}

// デプスデータから三次元座標を取得する
int Kinect::getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* pDepthPixel, Vector4 *realPoint, cv::Point colorPoint) {
	return getRealPointFromColorPixel(pDepthPixel, realPoint, colorPoint.x, colorPoint.y);
}

// カラー画像の座標系に３次元座標を重ねる
int Kinect::getRealPointFrameFromColorFrame(NUI_DEPTH_IMAGE_PIXEL* pDepthPixels, Vector4 *realPointFrame) {
	HRESULT hr;
	INuiCoordinateMapper *pMapper;

	// マッパークラスの取得
	hr = pNuiSensor->NuiGetCoordinateMapper(&pMapper);
	if(FAILED(hr)) {
		return -1;
	}

	// カラー画像の座標系に３次元座標を重ねる
	hr = pMapper->MapColorFrameToSkeletonFrame(NUI_IMAGE_TYPE_COLOR, RESOLUTION, RESOLUTION, SIZE, pDepthPixels, SIZE, realPointFrame);
	if(FAILED(hr)) {
		std::cout << "erroo" << std::endl;
		return -1;
	}

	return 0;
}