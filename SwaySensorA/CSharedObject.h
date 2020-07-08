#pragma once
#include "framework.h"

// カメラ画像配列　INDEX
enum
{
    IMAGE_ID_RAW_A = 0, // 元画像A
    IMAGE_ID_RAW_B,     // 元画像B
    IMAGE_ID_MASK1_A,   // マスク画像1A
    IMAGE_ID_MASK1_B,   // マスク画像1B
    IMAGE_ID_MASK2_A,   // マスク画像2A
    IMAGE_ID_MASK2_B,   // マスク画像2B
    IMAGE_ID_PROC_A,    // 処理画像A
    IMAGE_ID_PROC_B,    // 処理画像B
    IMAGE_ID_MAX
};

//画像処理配列　INDEX
enum
{
    IMGPROC_ID_IMG_1,   // 画像1処理データ
    IMGPROC_ID_IMG_2,   // 画像2処理データ
    IMGPROC_ID_MAX
};

//傾斜計入力データ　INDEX
enum
{
    INCLINO_ID_PORT_1_ANALOG = 0,   // RIO PORT1入力値(生値) 
    INCLINO_ID_PORT_2_ANALOG,       // RIO PORT2入力値(生値)
    INCLINO_ID_PORT_1_MA,           // RIO PORT1入力値(mA)
    INCLINO_ID_PORT_2_MA,           // RIO PORT2入力値(mA)
    INCLINO_ID_PORT_1_RAD,			// RIO PORT1入力値(角度 radian)
    INCLINO_ID_PORT_2_RAD,			// RIO PORT2入力値(角度 radian)
    INCLINO_ID_MAX
};

//傾斜計/カメラ制御用パラメータ　INDEX
enum
{
    PARAM_ID_IMG_GRAB_CAMERA = 0,   // 画像取込み(カメラ)
    PARAM_ID_IMG_GRAB_FILE,         // 画像取込み(ファイル)
    PARAM_ID_CAM_EXPOSURE_TIME,     // カメラ設定(露光時間)
    PARAM_ID_CAM_FRAMERATE,         // カメラ設定(フレームレート)
    PARAM_ID_CAM_WIDTH,             // カメラ設定(キャプチャサイズ横幅)
    PARAM_ID_CAM_HEIGHT,            // カメラ設定(キャプチャサイズ高さ)
    PARAM_ID_CAM_READ_FRAMERATE,    // カメラ読出し(フレームレート)
    PARAM_ID_IMG_ROI_ENABLE,        // 画像処理設定(ROI有効)
    PARAM_ID_IMG_ROI_SIZE,          // 画像処理設定(ROIサイズ)
    PARAM_ID_IMG_MASK1_HLOW,        // 画像処理設定(画像マスク1(H)下限)
    PARAM_ID_IMG_MASK1_HUPP,        // 画像処理設定(画像マスク1(H)上限)
    PARAM_ID_IMG_MASK1_SLOW,        // 画像処理設定(画像マスク1(S)下限)
    PARAM_ID_IMG_MASK1_SUPP,        // 画像処理設定(画像マスク1(S)上限)
    PARAM_ID_IMG_MASK1_VLOW,        // 画像処理設定(画像マスク1(V)下限)
    PARAM_ID_IMG_MASK1_VUPP,        // 画像処理設定(画像マスク1(V)上限)
    PARAM_ID_IMG_MASK2_HLOW,        // 画像処理設定(画像マスク2(H)下限)
    PARAM_ID_IMG_MASK2_HUPP,        // 画像処理設定(画像マスク2(H)上限)
    PARAM_ID_IMG_MASK2_SLOW,        // 画像処理設定(画像マスク2(S)下限)
    PARAM_ID_IMG_MASK2_SUPP,        // 画像処理設定(画像マスク2(S)上限)
    PARAM_ID_IMG_MASK2_VLOW,        // 画像処理設定(画像マスク2(V)下限)
    PARAM_ID_IMG_MASK2_VUPP,        // 画像処理設定(画像マスク2(V)上限)
    PARAM_ID_IMG_NOISEFILTER1,      // 画像処理設定(ゴマ塩ノイズフィルタ)
    PARAM_ID_IMG_NOISEFILTERVAL1,   // 画像処理設定(ゴマ塩ノイズフィルタ値)
    PARAM_ID_IMG_NOISEFILTER2,      // 画像処理設定(穴埋めノイズフィルタ)
    PARAM_ID_IMG_NOISEFILTERVAL2,   // 画像処理設定(穴埋めノイズフィルタ値)
    PARAM_ID_IMG_ALGORITHM,         // 画像処理設定(重心位置算出アルゴリズム)
    PARAM_ID_RIO_TCPPORT,           // RIO TCPポート番号
    PARAM_ID_RIO_SLAVEADDR,         // RIOスレーブアドレス
    PARAM_ID_RIO_TIMEOUT,           // RIOタイムアウト
    PARAM_ID_RIO_XPORT,             // RIO傾斜計X軸データ接続ポート番号
    PARAM_ID_RIO_YPORT,             // RIO傾斜計Y軸データ接続ポート番号
    PARAM_ID_MAX
};

enum
{
    PARAM_ID_STR_PROC_FILENAME = 0, // 解析対象画像ファイル名
    PARAM_ID_STR_RIO_IPADDR,        // RIO IPアドレス
    PARAM_ID_STR_MAX
};

enum
{
    PARAM_ID_DOUBLE_IMG_GRAB_TIME = 0,  // 画像取込み時間
    PARAM_ID_DOUBLE_PROC_TIME,          // 画処理時間
    PARAM_ID_DOUBLE_MAX
};

enum
{
    NOISEFILTER1_NONE = 0,           // ノイズフィルタ:なし
    NOISEFILTER1_MEDIAN,             // ノイズフィルタ:中央値フィルタ
    NOISEFILTER1_OPENNING,           // ノイズフィルタ:オープニング処理
    NOIZEFILTER1_MAX
};

enum
{
    NOISEFILTER2_NONE = 0,           // ノイズフィルタ:なし
    NOISEFILTER2_CLOSING,            // ノイズフィルタ:クロージング処理
    NOIZEFILTER2_MAX
};

enum
{
    COG_ALGORITHM_ALL = 1,          // 重心位置算出アルゴリズム(全輪郭点)
    COG_ALGORITHM_AREA,             // 重心位置算出アルゴリズム(最大輪郭面積)
    COG_ALGORITHM_LEN,              // 重心位置算出アルゴリズム(最大輪郭長)
    COG_ALGORITHM_MAX
};

// 構造体定義
typedef struct _stImageData
{
    cv::Mat image;
    BOOL    update; // image setでtrue getでfalse
} stImageData;

typedef struct _stImageProcData
{
    double      posx;
    double      posy;
    cv::Rect    roi;
    int         roisize;
    BOOL        enable;
} stImageProcData;

typedef struct _stInclinationData
{
    DOUBLE data;
} stInclinationData;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    void InitSharedObject(void);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetProcData(UINT8 id, stImageProcData data);
    INT GetProcData(UINT8 id, stImageProcData* data);

    INT SetInclinoData(UINT8 id, DOUBLE data);
    INT GetInclinoData(UINT8 id, DOUBLE* data);

    INT SetParam(UINT8 id, UINT32 data);
    INT GetParam(UINT8 id, UINT32* data);
    INT SetParam(UINT8 id, string str);
    INT GetParam(UINT8 id, string* str);
    INT SetParam(UINT8 id, DOUBLE data);
    INT GetParam(UINT8 id, DOUBLE* data);

    stImageData         m_stImgData[IMAGE_ID_MAX];
    stImageProcData     m_stImgProcData[IMGPROC_ID_MAX];
    stInclinationData   m_stIncData[INCLINO_ID_MAX];
    UINT32              m_u32Param[PARAM_ID_MAX];
    string              m_strParam[PARAM_ID_STR_MAX];
    DOUBLE              m_dParam[PARAM_ID_DOUBLE_MAX];

    CRITICAL_SECTION csImage[IMAGE_ID_MAX];
    CRITICAL_SECTION csProcData[IMGPROC_ID_MAX];
    CRITICAL_SECTION csInclino[INCLINO_ID_MAX];
    CRITICAL_SECTION csParam[PARAM_ID_MAX];
    CRITICAL_SECTION csStrParam[PARAM_ID_STR_MAX];
    CRITICAL_SECTION csDoubleParam[PARAM_ID_DOUBLE_MAX];
};
