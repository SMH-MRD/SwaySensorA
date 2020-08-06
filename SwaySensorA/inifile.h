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

// ------------------ 共通設定 ---------------------------
#define	INI_SCT_COMMON              L"COMMON"               // セクション名：共通設定
#define INI_KEY_CMMN_CAMOFFSET_LX0  L"CameraOffsetLX0"      // キー名：吊具吊点～BOX吊点距離LX0[mm]
#define INI_KEY_CMMN_CAMOFFSET_LY0  L"CameraOffsetLY0"      // キー名：吊具吊点～BOX吊点距離LY0[mm]
#define INI_KEY_CMMN_CAMOFFSET_L0   L"CameraOffsetL0"       // キー名：BOX吊点～BOX可動部中心距離L0[mm]
#define INI_KEY_CMMN_CAMOFFSET_LC   L"CameraOffsetLC"       // キー名：BOX可動部中心～カメラ中心距離LC[mm]
#define INI_KEY_CMMN_CAMOFFSET_A0   L"CameraOffsetA0"       // キー名：BOX内カメラ取付角度θ0[deg]
#define INI_KEY_CMMN_CAMOFFSET_AC   L"CameraOffsetAC"       // キー名：BOX可動部中心～カメラ中心角度θc[deg]
#define INI_KEY_CMMN_CAMVIEWANGLE   L"CameraViewAngle"      // キー名：カメラ視野角[deg]
#define INI_KEY_CMMN_FILTER         L"Filter"               // キー名：フィルタ時定数
#define INI_KEY_CMMN_IMGSAVEFNAME   L"ImageSaveFileName"    // キー名：画像保存ファイル名

// ------------------ カメラ設定 ---------------------------
#define	INI_SCT_CAMERA              L"CAMERA"               // セクション名：カメラ設定
#define	INI_KEY_CAM_IMAGESIZE       L"ImageSize"            // キー名：カメラ撮影サイズ
#define	INI_KEY_CAM_FRAMERATE       L"FrameRate"            // キー名：フレームレート
#define	INI_KEY_CAM_EXPOSURE        L"ExposureTime"         // キー名：露光時間

// ------------------ 画像処理設定 ---------------------------
#define	INI_SCT_OPENCV              L"OPENCV"               // セクション名：画像処理設定
#define	INI_KEY_IMG_ROI             L"Roi"                  // キー名：ROI
#define	INI_KEY_IMG_MASK_IMAGE      L"MaskImage"            // キー名：マスク画像選択
#define	INI_KEY_IMG_MASK1_LOW       L"Mask1Low"             // キー名：画像1マスク下限(H,S,V)
#define	INI_KEY_IMG_MASK1_UPP       L"Mask1Upp"             // キー名：画像1マスク上限(H,S,V)
#define	INI_KEY_IMG_MASK2_LOW       L"Mask2Low"             // キー名：画像2マスク下限(H,S,V)
#define	INI_KEY_IMG_MASK2_UPP       L"Mask2Upp"             // キー名：画像2マスク上限(H,S,V)
#define	INI_KEY_IMG_NOISEFILTER1    L"NoiseFilter1"         // キー名：ゴマ塩ノイズフィルタ
#define	INI_KEY_IMG_NOISEFILTER2    L"NoiseFilter2"         // キー名：穴埋めノイズフィルタ
#define	INI_KEY_IMG_ALGORITHM       L"Algorithm"            // キー名：ターゲット検出アルゴリズム

// ------------------ RIO設定 ---------------------------
#define	INI_SCT_RIO                 L"RIO"              // セクション名：RIO設定
#define INI_KEY_RIO_IPADDR          L"IpAddr"           // キー名:RIO IPアドレス
#define INI_KEY_RIO_TCPPORTNUM      L"TcpPort"          // キー名:RIO TCPポート番号
#define INI_KEY_RIO_SLAVEADDR       L"SlaveAddr"        // キー名:RIOスレーブアドレス
#define INI_KEY_RIO_TIMEOUT         L"TimeOut"          // キー名:RIOタイムアウト
#define INI_KEY_RIO_XPORTNUM        L"XPortNum"         // キー名:RIO傾斜計Xデータ接続ポート番号
#define INI_KEY_RIO_YPORTNUM        L"YPortNum"         // キー名:RIO傾斜計Yデータ接続ポート番号

// ------------------ Iniファイル情報の構造体 ---------------------------
typedef struct _stIniInf    //***** Iniファイル情報 *****
{
    stCommonParamData   m_cmmnparam;                // 共通設定
    stCameraParamData   m_camparam;                 // カメラ設定データ
    stImgProcParamData  m_imgprocparam;             // 画像処理設定データ
    stRIOParamData      m_rioparam;                 // RemoteIO設定データ
} ST_INI_INF;
