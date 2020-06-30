#pragma once
#include "framework.h"

// カメラ加工材料画像配列　INDEX
enum
{
    IMAGE_ID_CAM_A = 0, // カメラ画像A
    IMAGE_ID_CAM_B,     // カメラ画像B
    IMAGE_ID_MASK1_A,   // マスク画像1A
    IMAGE_ID_MASK1_B,   // マスク画像1B
    IMAGE_ID_MASK2_A,   // マスク画像2A
    IMAGE_ID_MASK2_B,   // マスク画像2B
    IMAGE_ID_CAM_MAX
};

//カメラ出力画像配列　INDEX
enum
{
    IMAGE_ID_PROC1_A = 0,   // 加工後画像1A
    IMAGE_ID_PROC1_B,       // 加工後画像1B
    IMAGE_ID_PROC2_A,       // 加工後画像2A
    IMAGE_ID_PROC2_B,       // 加工後画像2B
    IMAGE_ID_PROC_MAX
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
    PARAM_ID_CAM_PROC = 0,          // カメラ制御
    PARAM_ID_CAM_EXPOSURE_TIME,     // カメラ設定(露光時間)
    PARAM_ID_CAM_FRAMERATE,         // カメラ設定(フレームレート)
    PARAM_ID_CAM_WIDTH,             // カメラ設定(キャプチャサイズ横幅)
    PARAM_ID_CAM_HEIGHT,            // カメラ設定(キャプチャサイズ高さ)
    PARAM_ID_CAM_READ_FRAMERATE,    // カメラ読出し(フレームレート)
    PARAM_ID_PIC_PROC_FLAG,         // 画像解析実行フラグ
    PARAM_ID_PIC_MASK1_VALID,       // 画像処理設定(画像マスク1有効無効)
    PARAM_ID_PIC_MASK1_HLOW,        // 画像処理設定(画像マスク1(H)下限)
    PARAM_ID_PIC_MASK1_HUPP,        // 画像処理設定(画像マスク1(H)上限)
    PARAM_ID_PIC_MASK1_SLOW,        // 画像処理設定(画像マスク1(S)下限)
    PARAM_ID_PIC_MASK1_SUPP,        // 画像処理設定(画像マスク1(S)上限)
    PARAM_ID_PIC_MASK1_VLOW,        // 画像処理設定(画像マスク1(V)下限)
    PARAM_ID_PIC_MASK1_VUPP,        // 画像処理設定(画像マスク1(V)上限)
    PARAM_ID_PIC_MASK2_VALID,       // 画像処理設定(画像マスク2有効無効)
    PARAM_ID_PIC_MASK2_HLOW,        // 画像処理設定(画像マスク2(H)下限)
    PARAM_ID_PIC_MASK2_HUPP,        // 画像処理設定(画像マスク2(H)上限)
    PARAM_ID_PIC_MASK2_SLOW,        // 画像処理設定(画像マスク2(S)下限)
    PARAM_ID_PIC_MASK2_SUPP,        // 画像処理設定(画像マスク2(S)上限)
    PARAM_ID_PIC_MASK2_VLOW,        // 画像処理設定(画像マスク2(V)下限)
    PARAM_ID_PIC_MASK2_VUPP,        // 画像処理設定(画像マスク2(V)上限)
    PARAM_ID_PIC_NOISEFILTER,       // 画像処理設定(ノイズフィルタ)
    PARAM_ID_PIC_NOISEFILTERVAL,    // 画像処理設定(ノイズフィルタ)
    PARAM_ID_PIC_ALGORITHM,         // 画像処理設定(重心位置算出アルゴリズム)
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
    PARAM_ID_DOUBLE_PROC_TIME = 0,  // 画処理時間
    PARAM_ID_DOUBLE_MAX
};

enum
{
    NOISEFILTER_NONE = 0,           // ノイズフィルタ:なし
    NOISEFILTER_MEDIAN,             // ノイズフィルタ:中央値フィルタ
    NOISEFILTER_OPENNING,           // ノイズフィルタ:オープニング処理
    NOIZEFILTER
};

enum
{
    COG_ALGORITHM_ALL = 1,          // 重心位置算出アルゴリズム(全輪郭点)
    COG_ALGORITHM_LEN,              // 重心位置算出アルゴリズム(最大輪郭長)
    COG_ALGORITHM
};

// 構造体定義
typedef struct _stMngImageData
{
    Mat     image;
    BOOL    update; // image setでtrue getでfalse
} STMngImageData;

typedef struct _stProcData
{
    Mat     image;
    double  posx;
    double  posy;
    BOOL    enable;
} STProcData;

typedef struct _stMngProcData
{
    STProcData  data;
    BOOL        update; // data setでtrue getでfalse
} STMngProcData;

typedef struct _stMngInclinoData
{
    DOUBLE data;
} STMngInclinoData;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    void InitSharedObject(void);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetProcImage(UINT8 id, STProcData data);
    INT GetProcImage(UINT8 id, STProcData* data);

    INT SetInclinoData(UINT8 id, DOUBLE data);
    INT GetInclinoData(UINT8 id, DOUBLE* data);

    INT SetParam(UINT8 id, UINT32 data);
    INT GetParam(UINT8 id, UINT32* data);
    INT SetParam(UINT8 id, string str);
    INT GetParam(UINT8 id, string* str);
    INT SetParam(UINT8 id, DOUBLE data);
    INT GetParam(UINT8 id, DOUBLE* data);

    STMngImageData      m_stImage[IMAGE_ID_CAM_MAX];
    STMngProcData       m_stProcImage[IMAGE_ID_PROC_MAX];
    STMngInclinoData    m_stInclinoData[INCLINO_ID_MAX];
    UINT32              m_u32Param[PARAM_ID_MAX];
    string              m_strParam[PARAM_ID_STR_MAX];
    DOUBLE              m_dParam[PARAM_ID_DOUBLE_MAX];

    CRITICAL_SECTION csImage[IMAGE_ID_CAM_MAX];
    CRITICAL_SECTION csProcImage[IMAGE_ID_PROC_MAX];
    CRITICAL_SECTION csInclino[INCLINO_ID_MAX];
    CRITICAL_SECTION csParam[PARAM_ID_MAX];
    CRITICAL_SECTION csStrParam[PARAM_ID_STR_MAX];
    CRITICAL_SECTION csDoubleParam[PARAM_ID_DOUBLE_MAX];
};
