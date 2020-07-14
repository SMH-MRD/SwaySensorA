#pragma once
#include <windows.h>

// ------------------タスク設定 ---------
#define OBJ_NAME_SECT_OF_INIFILE    L"OBJECT_NAME"      // セクション名：オブジェクト名
#define OBJ_SNAME_SECT_OF_INIFILE   L"OBJECT_SNAME"     // セクション名：オブジェクト略称
#define OBJ_CYCLE_SECT_OF_INIFILE   L"OBJECT_CYCLE"     // セクション名：オブジェクトスレッド周期
#define MANAGER_KEY_OF_INIFILE      L"MANAGER"
#define CLIENT_COM_KEY_OF_INIFILE   L"CCOM"
#define CLERK_KEY_OF_INIFILE        L"CLERK"
#define RIO_COM_KEY_OF_INIFILE      L"DCOM1"
#define CAMERA_COM_KEY_OF_INIFILE   L"DCOM2"
#define ANALYST_KEY_OF_INIFILE      L"ANALYST"
#define PLAYER_KEY_OF_INIFILE       L"PLAYER"
#define PR_KEY_OF_INIFILE           L"PR"

// ------------------ カメラ設定 ---------------------------
#define	INI_SCT_CAMERA              L"CAMERA"           // セクション名：カメラ
#define	INI_KEY_CAM_WIDTH           L"Width"            // キー名：カメラ撮影横幅
#define	INI_KEY_CAM_HEIGHT          L"Height"           // キー名：カメラ撮影高さ
#define	INI_KEY_CAM_FRAMERATE       L"FrameRate"        // キー名：フレームレート
#define	INI_KEY_CAM_EXPOSURE        L"ExposureTime"     // キー名：露光時間

// ------------------ 画像処理設定 ---------------------------
#define	INI_SCT_OPENCV              L"OPENCV"           // セクション名：画像処理
#define	INI_KEY_IMG_ROI             L"Roi"              // キー名：ROI
#define	INI_KEY_IMG_MASK_IMAGE      L"MaskImage"        // キー名：マスク画像選択
#define	INI_KEY_IMG_MASK1_LOW       L"Mask1Low"         // キー名：画像1マスク下限(H,S,V)
#define	INI_KEY_IMG_MASK1_UPP       L"Mask1Upp"         // キー名：画像1マスク上限(H,S,V)
#define	INI_KEY_IMG_MASK2_LOW       L"Mask2Low"         // キー名：画像2マスク下限(H,S,V)
#define	INI_KEY_IMG_MASK2_UPP       L"Mask2Upp"         // キー名：画像2マスク上限(H,S,V)
#define	INI_KEY_IMG_NOISEFILTER1    L"NoiseFilter1"     // キー名：ゴマ塩ノイズフィルタ
#define	INI_KEY_IMG_NOISEFILTER2    L"NoiseFilter2"     // キー名：穴埋めノイズフィルタ
#define	INI_KEY_IMG_ALGORITHM       L"Algorithm"        // キー名：ターゲット検出アルゴリズム

// ------------------ RIO設定 ---------------------------
#define	INI_SCT_RIO                 L"RIO"              // セクション名：RIO
#define INI_KEY_RIO_IPADDR          L"IpAddr"           // キー名:RIO IPアドレス
#define INI_KEY_RIO_TCPPORTNUM      L"TcpPort"          // キー名:RIO TCPポート番号
#define INI_KEY_RIO_SLAVEADDR       L"SlaveAddr"        // キー名:RIOスレーブアドレス
#define INI_KEY_RIO_TIMEOUT         L"TimeOut"          // キー名:RIOタイムアウト
#define INI_KEY_RIO_XPORTNUM        L"XPortNum"         // キー名:RIO傾斜計Xデータ接続ポート番号
#define INI_KEY_RIO_YPORTNUM        L"YPortNum"         // キー名:RIO傾斜計Yデータ接続ポート番号

// ------------------ Iniファイル情報の構造体 ---------------------------
typedef struct _stIniInf    //***** Iniファイル情報 *****
{
    INT     camWidth;           // カメラ取得横幅
    INT     camHeight;          // カメラ取得高さ
    INT     frameRate;          // フレームレート
    INT     expTimeMin;         // 露光時間(最小値)
    INT     expTimeMax;         // 露光時間(最大値)
    INT     expTime;            // 露光時間

    INT     roiEnable;          // ROI有効
    INT     roiSize;            // ROIサイズ
    INT     maskImage;          // マスク画像選択
    INT     mask1HLow;          // 画像マスク1(H)下限
    INT     mask1HUpp;          // 画像マスク1(H)上限
    INT     mask1SLow;          // 画像マスク1(S)下限
    INT     mask1SUpp;          // 画像マスク1(S)上限
    INT     mask1VLow;          // 画像マスク1(V)下限
    INT     mask1VUpp;          // 画像マスク1(V)上限
    INT     mask2HLow;          // 画像マスク2(H)下限
    INT     mask2HUpp;          // 画像マスク2(H)上限
    INT     mask2SLow;          // 画像マスク2(S)下限
    INT     mask2SUpp;          // 画像マスク2(S)上限
    INT     mask2VLow;          // 画像マスク2(V)下限
    INT     mask2VUpp;          // 画像マスク2(V)上限
    INT     noiseFilter1;       // ゴマ塩ノイズフィルタ
    INT     noiseFilterVal1;    // ゴマ塩ノイズフィルタ値
    INT     noiseFilter2;       // 穴埋めノイズフィルタ
    INT     noiseFilterVal2;    // 穴埋めノイズフィルタ値
    INT     algorithm;          // ターゲット検出アルゴリズム

    TCHAR   rioIpAddr[256];     // RIO IPアドレス
    INT     rioTcpPortNum;      // RIO TCPポート番号
    INT     rioSlaveAddr;       // RIOスレーブアドレス
    INT     rioTimeOut;         // RIOタイムアウト
    INT     rioXPortNum;        // RIO傾斜計X軸データ接続ポート番号
    INT     rioYPortNum;        // RIO傾斜計Y軸データ接続ポート番号
} ST_INI_INF;
