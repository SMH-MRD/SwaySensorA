#pragma once
#include "framework.h"

// H, S, V
#define IMAGE_HSV_H_MIN                 0
#define IMAGE_HSV_H_MAX                 179
#define IMAGE_HSV_S_MIN                 0
#define IMAGE_HSV_S_MAX                 255
#define IMAGE_HSV_V_MIN                 0
#define IMAGE_HSV_V_MAX                 255

// RIOエラービット
#define RIO_ERR_NONE                    0x0000  // エラーなし
#define RIO_ERR_INIT_INCOMPLETE         0x0001  // RIO初期化未完了
#define RIO_ERR_SET_IOLINKMODE          0x0010  // IO LINK MODE設定エラー
#define RIO_ERR_SET_PARAM_VALID         0x0020  // 有効化設定エラー
#define RIO_ERR_SET_PARAM_AI            0x0040  // AI設定エラー
#define RIO_ERR_GET_AI_READ             0x0100  // データ読み込みエラー応答

// Process Data Error
#define S7CMPTBL_FORMAT_OVERRANGE       0x7FFF  // Overrange
#define S7CMPTBL_FORMAT_UNDERRANGE      0x8000  // Open circuit/short circuit/underrange
#define S7CMPTBL_FORMAT_SIGNBIT         0x8000  // Sign bit

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

//傾斜計入力データ
enum
{
    RIO_PORT_1 = 0, // RIO入力ポート1 
    RIO_PORT_2,     // RIO入力ポート2 
    RIO_PORT_MAX
};

//傾斜計/カメラ制御用パラメータ　INDEX
enum
{
    PARAM_ID_IMG_GRAB_CAMERA = 0,   // 画像取込み(カメラ)
    PARAM_ID_IMG_GRAB_FILE,         // 画像取込み(ファイル)
    PARAM_ID_CAM_WIDTH,             // カメラ設定(キャプチャサイズ横幅)
    PARAM_ID_CAM_HEIGHT,            // カメラ設定(キャプチャサイズ高さ)
    PARAM_ID_CAM_FRAMERATE,         // カメラ設定(フレームレート)
    PARAM_ID_CAM_EXPOSURE_TIME_MIN, // カメラ設定(露光時間最小値)
    PARAM_ID_CAM_EXPOSURE_TIME_MAX, // カメラ設定(露光時間最大値)
    PARAM_ID_CAM_EXPOSURE_TIME,     // カメラ設定(露光時間)
    PARAM_ID_IMG_ROI_ENABLE,        // 画像処理設定(ROI有効)
    PARAM_ID_IMG_ROI_SIZE,          // 画像処理設定(ROIサイズ)
    PARAM_ID_IMG_MASK_TYPE,         // 画像処理設定(マスク画像選択)
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
    MASK_IMG_ALL = 0,               // マスク画像選択(両方)
    MASK_IMG_IMAGE1 ,               // マスク画像選択(画像1のみ)
    MASK_IMG_IMAGE2,                // マスク画像選択(画像2のみ)
    MASK_IMG_MAX
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
    COG_ALGORITHM_ALL = 0,          // 重心位置算出アルゴリズム(全輪郭点)
    COG_ALGORITHM_AREA,             // 重心位置算出アルゴリズム(最大輪郭面積)
    COG_ALGORITHM_LEN,              // 重心位置算出アルゴリズム(最大輪郭長)
    COG_ALGORITHM_MAX
};

// 構造体定義
typedef struct _stCameraInfo
{
    BOOL    valid;              // カメラ状態
    DOUBLE  cycleTime;          // 画像取込み間隔[ms]
} stCameraInfo;

typedef struct _stImageData
{
    cv::Mat image;
    BOOL    update;             // image setでtrue getでfalse
} stImageData;

typedef struct _stRIOInfoData
{
    int16_t dig;
    double  cur;    // 入力データ変換値(mA)
    double  deg;    // 入力データ変換値(deg.)
} stRIOInfoData;

typedef struct _stRIOInfo   // RIO情報
{
    int32_t         error;  // エラー情報
    stRIOInfoData   data[RIO_PORT_MAX];
} stRIOInfo;

typedef struct _stImageProcData
{
    double      posx;           // 検出位置X
    double      posy;           // 検出位置Y
    int         roiSize;        // ROIサイズ
    cv::Rect    roi;            // ROI
    double      expTime;        // 露光時間
    BOOL        valid;          // 検出状態
} stImageProcData;

typedef struct _stProcInfo
{
    stImageProcData data[IMGPROC_ID_MAX];   // 画像処理結果
    DOUBLE          exposureTime;           // 露光時間[us]
    DOUBLE          procTime;               // 処理時間[ms]
} stProcInfo;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    void InitSharedObject(void);

    INT SetParam(UINT8 id, UINT32 data);
    INT GetParam(UINT8 id, UINT32* data);
    INT SetParam(UINT8 id, string str);
    INT GetParam(UINT8 id, string* str);

    INT SetCameraInfo(stCameraInfo info);
    INT GetCameraInfo(stCameraInfo* info);

    INT SetRIOInfo(stRIOInfo info);
    INT GetRIOInfo(stRIOInfo* info);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetProcInfo(stProcInfo info);
    INT GetProcInfo(stProcInfo* info);

private:
    UINT32              m_u32Param[PARAM_ID_MAX];
    string              m_strParam[PARAM_ID_STR_MAX];
    stCameraInfo        m_stCameraInfo;
    stRIOInfo           m_stRIOInfo;
    stImageData         m_stImgData[IMAGE_ID_MAX];
    stProcInfo          m_stProcInfo;

    CRITICAL_SECTION    csParam[PARAM_ID_MAX];
    CRITICAL_SECTION    csStrParam[PARAM_ID_STR_MAX];
    CRITICAL_SECTION    csCameraInfo;
    CRITICAL_SECTION    csRIOInfo;
    CRITICAL_SECTION    csImage[IMAGE_ID_MAX];
    CRITICAL_SECTION    csProcInfo;
};
