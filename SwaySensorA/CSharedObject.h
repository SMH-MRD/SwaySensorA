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
    IMAGE_ID_RAW_A = 0,                         // 元画像A
    IMAGE_ID_RAW_B,                             // 元画像B
    IMAGE_ID_MASK1_A,                           // マスク画像1A
    IMAGE_ID_MASK1_B,                           // マスク画像1B
    IMAGE_ID_MASK2_A,                           // マスク画像2A
    IMAGE_ID_MASK2_B,                           // マスク画像2B
    IMAGE_ID_PROC_A,                            // 処理画像A
    IMAGE_ID_PROC_B,                            // 処理画像B
    IMAGE_ID_MAX
};

// 画像処理配列　INDEX
enum
{
    IMGPROC_ID_IMG_1 = 0,                       // 画像1処理データ
    IMGPROC_ID_IMG_2,                           // 画像2処理データ
    IMGPROC_ID_MAX
};

// 傾斜計入力データ
enum
{
    RIO_PORT_1 = 0,                             // RIO入力ポート1 
    RIO_PORT_2,                                 // RIO入力ポート2 
    RIO_PORT_MAX
};

// カメラ制御用パラメータ
enum
{
    GRAB_IMG_STOP = 0,                          // 画像取込み(停止)
    GRAB_IMG_GRAB_CAMERA,                       // 画像取込み(カメラ)
    GRAB_IMG_GRAB_FILE                          // 画像取込み(ファイル)
};

enum
{
    MASK_IMG_ALL = 0,                           // マスク画像選択(両方)
    MASK_IMG_IMAGE1 ,                           // マスク画像選択(画像1のみ)
    MASK_IMG_IMAGE2,                            // マスク画像選択(画像2のみ)
};

enum
{
    NOISEFILTER1_NONE = 0,                      // ノイズフィルタ:なし
    NOISEFILTER1_MEDIAN,                        // ノイズフィルタ:中央値フィルタ
    NOISEFILTER1_OPENNING,                      // ノイズフィルタ:オープニング処理
    NOIZEFILTER1_MAX
};

enum
{
    NOISEFILTER2_NONE = 0,                      // ノイズフィルタ:なし
    NOISEFILTER2_CLOSING,                       // ノイズフィルタ:クロージング処理
    NOIZEFILTER2_MAX
};

enum
{
    COG_ALGORITHM_ALL = 0,                      // 重心位置算出アルゴリズム(全輪郭点)
    COG_ALGORITHM_AREA,                         // 重心位置算出アルゴリズム(最大輪郭面積)
    COG_ALGORITHM_LEN,                          // 重心位置算出アルゴリズム(最大輪郭長)
    COG_ALGORITHM_MAX
};

enum
{
    SWAY_X = 0,                                 // 振れX
    SWAY_Y,                                     // 振れY
    SWAY_MAX
};

// 構造体定義
// カメラ設定
typedef struct _stCameraParamData               // カメラ設定データ
{
    int     imgsource;                          // 画像取込み元(0:停止 1:カメラ 2:画像ファイル) 
    string  imgfname;                           // 取込み画像ファイル名

    int     width;                              // カメラ画像サイズ横幅(32の倍数, 2592以下)
    int     height;                             // カメラ画像サイズ高さ(2の倍数, 2048以下)
    double  fps;                                // フレームレート
    double  exptime;                            // 露光時間(usec)(初期値)
    double  exptimemin;                         // 露光時間(usec)(最小値)
    double  exptimemax;                         // 露光時間(usec)(最大値)
} stCameraParamData;
typedef struct _stCameraParam                   // カメラ設定
{
    CRITICAL_SECTION    cs;
    stCameraParamData   data;                   // カメラ設定データ
} stCameraParam;

// 画像処理設定
typedef struct _stROIParam                      // ROI設定
{
    int     valid;                              // ROI有効
    double  scale;                              // ROIスケール(検出したターゲットに対する倍率)
} stROIParam;
typedef struct _stHSVParam                      // HSVマスク判定値
{
    int h;                                      // H
    int s;                                      // S
    int v;                                      // V
} stHSVParam;
typedef struct _stFilterParam                   // ノイズフィルタ
{
    int type;                                   // フィルタ有効
    int val;                                    // フィルタ値
} stFilterParam;
typedef struct _stImgProcParamData              // 画像処理設定データ
{
    stROIParam      roi;                        // ROI設定
    int             maskvalid[IMGPROC_ID_MAX];  // マスク画像選択(0=両方, 1=画像1のみ, 2=画像2のみ)
    stHSVParam      hsvl[IMGPROC_ID_MAX];       // HSVマスク判定値(下限)
    stHSVParam      hsvu[IMGPROC_ID_MAX];       // HSVマスク判定値(上限)
    stFilterParam   filter1;                    // ゴマ塩ノイズフィルタ(0=なし, 1=中央値フィルタ, 2=オープニング処理), フィルタ値(中央値フィルタ=1,3,5,... オープニング処理=1,2,...)
    stFilterParam   filter2;                    // 穴埋めノイズフィルタ(0=なし, 1=クロージング処理), フィルタ値(1,2,...)
    int             algorithm;                  // ターゲット検出アルゴリズム(0=全輪郭点, 1=最大輪郭面積, 2=最大輪郭長)
} stImgProcParamData;
typedef struct _stImgProcParam                  // 画像処理設定
{
    CRITICAL_SECTION    cs;
    stImgProcParamData  data;                   // 画像処理設定データ
} stImgProcParam;

// RemoteIO設定
typedef struct _stRIOParamData                  // RemoteIO設定データ
{
    TCHAR   ipaddrs[256];                       // RIO IPアドレス
    int     tcpport;                            // TCPポート番号
    int     slaveaddrs;                         // スレーブアドレス
    int     timeout;                            // 通信タイムアウト(msec)
    int     portx;                              // 傾斜計Xデータ接続ポート番号(1～8)
    int     porty;                              // 傾斜計Yデータ接続ポート番号(1～8)
} stRIOParamData;
typedef struct _stRIOParam                      // RemoteIO設定
{
    CRITICAL_SECTION    cs;
    stRIOParamData      data;                   // RemoteIO設定データ
} stRIOParam;

// 画像情報
typedef struct _stImageData
{
    cv::Mat image;
    BOOL    update;                             // image setでtrue getでfalse
} stImageData;
typedef struct _stImageInfo
{
    CRITICAL_SECTION    cs;
    stImageData         data;
} stImageInfo;

// カメラ情報
typedef struct _stCameraInfoData
{
    BOOL    valid;                              // カメラ状態
    DOUBLE  cycleTime;                          // 画像取込み間隔[ms]
} stCameraInfoData;
typedef struct _stCameraInfo
{
    CRITICAL_SECTION    cs;
    stCameraInfoData    data;
} stCameraInfo;

// RIO情報
typedef struct _stInclinometerData              // 傾斜計データ
{
    int16_t dig;
    double  cur;                                // 入力データ変換値(mA)
    double  deg;                                // 入力データ変換値(deg.)
} stInclinometerData;
typedef struct _stRIOInfoData                   // RIOデータ
{
    int32_t             error;                  // エラー情報
    stInclinometerData  incldata[RIO_PORT_MAX]; // 傾斜計データ
} stRIOInfoData;
typedef struct _stRIOInfo
{
    CRITICAL_SECTION    cs;
    stRIOInfoData       data;
} stRIOInfo;

// 検出情報
typedef struct _stImageProcData
{
    double      posx;                           // 検出位置X
    double      posy;                           // 検出位置Y
    int         tgtsize;                        // 検出サイズ
    cv::Rect    roi;                            // ROI
    double      expTime;                        // 露光時間
    BOOL        valid;                          // 検出状態
} stImageProcData;
typedef struct _stProcInfoData
{
    stImageProcData imgprocdata[IMGPROC_ID_MAX];    // 画像処理結果
    DOUBLE          exposureTime;                   // 露光時間[us](@@@画像を評価してコントロールするようにする)
    DOUBLE          sway[SWAY_MAX];                 // 振れ角
    DOUBLE          swaySpd[SWAY_MAX];              // 振れ角速度
    BOOL            valid;                          // 検出状態
    DOUBLE          procTime;                       // 処理時間[ms]
} stProcInfoData;
typedef struct _stProcInfo
{
    CRITICAL_SECTION    cs;
    stProcInfoData      data;
} stProcInfo;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    INT SetParam(stCameraParamData data);
    INT GetParam(stCameraParamData* data);
    INT SetParam(stImgProcParamData data);
    INT GetParam(stImgProcParamData* data);
    INT SetParam(stRIOParamData data);
    INT GetParam(stRIOParamData* data);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetInfo(stCameraInfoData data);
    INT GetInfo(stCameraInfoData* data);
    INT SetInfo(stRIOInfoData data);
    INT GetInfo(stRIOInfoData* data);
    INT SetInfo(stProcInfoData data);
    INT GetInfo(stProcInfoData* data);

private:
    stCameraParam       m_camparam;                 // カメラ設定
    stImgProcParam      m_imgprocparam;             // 画像処理設定
    stRIOParam          m_rioparam;                 // RemoteIO設定

    stImageInfo         m_imginfo[IMAGE_ID_MAX];
    stCameraInfo        m_caminfo;
    stRIOInfo           m_rioinfo;
    stProcInfo          m_procinfo;

private:
    void Initialize(void);
};
