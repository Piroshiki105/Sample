#ifndef __INCLUDED_KINECT_H_
#define __INCLUDED_KINECT_H_


#include <windows.h>
#include <NuiApi.h>

#include <opencv2/core/core.hpp>

class Kinect {
private:
	// Kinectセンサー
	INuiSensor* pNuiSensor;

	// RGB画像用ハンドル
	HANDLE hNextColorFrameEvent;
	HANDLE pColorStreamHandle;

	// Depth画像用ハンドル
	HANDLE hNextDepthFrameEvent;
	HANDLE pDepthStreamHandle;

	// 初期化ができたか
	BOOL bInitialized;

	// ニアモードを使っているか
	BOOL isNearMode;

	// カメラ画像の幅・高さ
	static const int WIDTH = 640;
	static const int HEIGHT = 480;

	// 解像度の設定
	const NUI_IMAGE_RESOLUTION RESOLUTION;

	// 画像配列のサイズ
	const DWORD SIZE;

public:
	// getImage関数にて使う　取得する画像の種類
	static const int IMAGE_TYPE_COLOR = 0;
	static const int IMAGE_TYPE_DEPTH = 1;
	// static const int IMAGE_TYPE_SKELETON = 2;

	static const int SUCCESS = 1;
	static const int FAILD = -1;

	// コンストラクタ
	Kinect();
	// デストラクタ
	~Kinect();
	// 初期化
	int init();
	// 画像の大きさを返す
	void getSize(int *width, int *height);
	// チルトサーボの角度を変える
	void setTiltAngle(LONG angle);
	LONG getTiltAngle(void);
	// ニアモードをオンにする
	int useNearMode();

	// 画像を取得する(IMAGE_TYPE_COLOR：カラー画像、IMAGE_TYPE_DEPTH：深度画像)
	int getImage(cv::Mat &image, int type = IMAGE_TYPE_COLOR);
	// カラー画像の取得
	int getColorImage(cv::Mat &color);
	// 深度画像の取得
	int getDepthImage(NUI_DEPTH_IMAGE_PIXEL** depthPixel);
	int getDepthImage(NUI_DEPTH_IMAGE_PIXEL** depthPixel, cv::Mat& depth);

	// カメラの露出・ホワイトバランスの自動調節機能をオフにする
	int changeCameraSettings();

	// 深度画像の座標系をカラー画像の座標系に合わせる
	void changeCoordinateDepthtoImage(cv::Mat depth, cv::Mat *dst);
	void changeCoordinateDepthtoImage(cv::Mat depth, cv::Mat *depth2, cv::Rect roi);
	// void changeCoordinateDepthtoImage(cv::Mat depth, DepthMap *depthMap, cv::Rect roi);

	// デプス画像から3次元座標を取得する
	Vector4 getRealPointFromDepth(cv::Mat depth, int x, int y);
	Vector4 getRealPointFromDepth(cv::Mat depth, cv::Point p);

	// カラー画像の座標から3次元座標を取得する
	int getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* depthPixels, Vector4* realPoint, int colorX, int colorY);
	int getRealPointFromColorPixel(NUI_DEPTH_IMAGE_PIXEL* depthPixels, Vector4* realPoint, cv::Point colorPoint);

	// カラー画像の座標系に3次元座標を重ねる
	int getRealPointFrameFromColorFrame(NUI_DEPTH_IMAGE_PIXEL* pDepthPixels, Vector4 *realPointFrame);

};

#endif