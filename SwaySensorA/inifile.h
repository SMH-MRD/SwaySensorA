#pragma once
#include <windows.h>

enum {
	INITYPE_CHAR = 0,
	INITYPE_INT,
	INITYPE_SHORT,
	INITYPE_LONG,
	INITYPE_FLOAT,
	INITYPE_DOUBLE
};

// ------------------ パラメータIniのセクション名 ---------------------------
#define	INI_SCT_CAMERA				L"CAMERA"							// セクション名：カメラ
#define	INI_SCT_OPENCV				L"OPENCV"							// セクション名：OPENCV
#define	INI_SCT_RIO					L"RIO"								// セクション名：RIO

// ------------------ カメラ設定セクションのキー名 ---------------------------
#define	INI_KEY_CAM_EXPOSURE		L"ExposureTime"						// キー名：露光時間
#define	INI_KEY_CAM_WIDTH			L"Width"							// キー名：カメラ撮影横幅
#define	INI_KEY_CAM_HEIGHT			L"Height"							// キー名：カメラ撮影高さ
#define	INI_KEY_CAM_FRAMERATE		L"FrameRate"						// キー名：フレームレート

// ------------------ OPENCV設定セクションのキー名 ---------------------------
#define	INI_KEY_OPENCV_MASK1_EN		L"Mask1En"							// キー名：OpenCVマスク1有効無効
#define	INI_KEY_OPENCV_MASK1_MIN	L"Mask1Min"							// キー名：OpenCVマスク1最小値
#define	INI_KEY_OPENCV_MASK1_MAX	L"Mask1Max"							// キー名：OpenCVマスク1最大値
#define	INI_KEY_OPENCV_MASK2_EN		L"Mask2En"							// キー名：OpenCVマスク2有効無効
#define	INI_KEY_OPENCV_MASK2_MIN	L"Mask2Min"							// キー名：OpenCVマスク2最小値
#define	INI_KEY_OPENCV_MASK2_MAX	L"Mask2Max"							// キー名：OpenCVマスク2最大値
#define	INI_KEY_OPENCV_MASK3_EN		L"Mask3En"							// キー名：OpenCVマスク3有効無効
#define	INI_KEY_OPENCV_MASK3_MIN	L"Mask3Min"							// キー名：OpenCVマスク3最小値
#define	INI_KEY_OPENCV_MASK3_MAX	L"Mask3Max"							// キー名：OpenCVマスク3最大値
#define	INI_KEY_OPENCV_PROC_ALGO	L"ProcAlgo"							// キー名：OpenCVアルゴリズム

// ------------------ RIO設定セクションのキー名 ---------------------------
#define INI_KEY_RIO_IPADDR			L"IpAddr"							// キー名:RIO IPアドレス
#define INI_KEY_RIO_TCPPORTNUM		L"TcpPort"							// キー名:RIO TCPポート番号
#define INI_KEY_RIO_SLAVEADDR		L"SlaveAddr"						// キー名:RIOスレーブアドレス
#define INI_KEY_RIO_TIMEOUT			L"TimeOut"							// キー名:RIOタイムアウト
#define INI_KEY_RIO_XPORTNUM		L"XPortNum"							// キー名:RIO傾斜計Xデータ接続ポート番号
#define INI_KEY_RIO_YPORTNUM		L"YPortNum"							// キー名:RIO傾斜計Yデータ接続ポート番号

// ------------------ Iniファイル情報の構造体 ---------------------------
typedef struct _stIniInf				//***** Iniファイル情報 *****
{
	INT exposureTime;					// 露光時間
	INT camWidth;						// カメラ取得横幅
	INT camHeight;						// カメラ取得高さ
	INT frameRate;						// フレームレート
	INT mask1En;						// 映像処理マスク1有効無効
	INT mask1Min;						// 映像処理マスク1最小
	INT mask1Max;						// 映像処理マスク1最大
	INT mask2En;						// 映像処理マスク2有効無効
	INT mask2Min;						// 映像処理マスク2最小
	INT mask2Max;						// 映像処理マスク2最大
	INT mask3En;						// 映像処理マスク3有効無効
	INT mask3Min;						// 映像処理マスク3最小
	INT mask3Max;						// 映像処理マスク3最大
	INT procAlgo;						// 画像処理アルゴリズム
	TCHAR rioIpAddr[256];				// RIO IPアドレス
	INT rioTcpPortNum;					// RIO TCPポート番号
	INT rioSlaveAddr;					// RIOスレーブアドレス
	INT rioTimeOut;						// RIOタイムアウト
	INT rioXPortNum;					// RIO傾斜計X軸データ接続ポート番号
	INT rioYPortNum;					// RIO傾斜計Y軸データ接続ポート番号
} ST_INI_INF;
