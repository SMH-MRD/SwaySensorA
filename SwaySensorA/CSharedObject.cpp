#include "CSharedObject.h"


/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CSharedObject::CSharedObject()
{
    Initialize();
}

/// @brief デストラクタ
/// @param
/// @return 
/// @note
CSharedObject::~CSharedObject()
{
}

/// @brief 
/// @param
/// @return 
/// @note
void CSharedObject::Initialize(void)
{
    //--------------------------------------------------------------------------
    // 
    // カメラ情報
    {
        m_caminfo.data.valid     = FALSE;
        m_caminfo.data.cycleTime = 0.0;
    }
    // RIO情報
    {
        m_rioinfo.data.error = RIO_ERR_NONE;
        for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
        {
            m_rioinfo.data.incldata[ii].dig = 0;    // 入力データ
            m_rioinfo.data.incldata[ii].cur = 0.0;  // 入力データ変換値(mA)
            m_rioinfo.data.incldata[ii].deg = 0.0;  // 入力データ変換値(deg.)    
        }
    }
    // 画像データ
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        m_imginfo[ii].data.update = FALSE;
    }
    // 処理情報
    {
        for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
        {
            m_procinfo.data.imgprocdata[ii].posx       = 0.0;
            m_procinfo.data.imgprocdata[ii].posy       = 0.0;
            m_procinfo.data.imgprocdata[ii].tgtsize    = 0;
            m_procinfo.data.imgprocdata[ii].roi.x      = 0;
            m_procinfo.data.imgprocdata[ii].roi.y      = 0;
            m_procinfo.data.imgprocdata[ii].roi.width  = 0;
            m_procinfo.data.imgprocdata[ii].roi.height = 0;
            m_procinfo.data.imgprocdata[ii].valid      = FALSE;
        }
        for (UINT ii = 0; ii < SWAY_MAX; ii++)
        {
            m_procinfo.data.sway[ii]    = 0.0;  // 振れ角
            m_procinfo.data.swaySpd[ii] = 0.0;  // 振れ角速度
        }
        m_procinfo.data.valid        = FALSE;   // 検出状態
        m_procinfo.data.exposureTime = 0.0;     // 露光時間[us]
        m_procinfo.data.procTime     = 0.0;     // 画処理時間[ms]
    }

    //--------------------------------------------------------------------------
    // 共有データアクセス用クリティカルセクションの初期化
    // カメラ設定
    {
        InitializeCriticalSection(&m_camparam.cs);
    }
    // 画像処理設定
    {
        InitializeCriticalSection(&m_imgprocparam.cs);
    }
     // RemoteIO設定
    {
        InitializeCriticalSection(&m_rioparam.cs);
    }
    // カメラ情報
    {
        InitializeCriticalSection(&m_caminfo.cs);
    }
    // RIO情報
    {
        InitializeCriticalSection(&m_rioinfo.cs);
    }
    // 画像データ
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        InitializeCriticalSection(&m_imginfo[ii].cs);
    }
    // 処理情報
    {
        InitializeCriticalSection(&m_procinfo.cs);
    }
}

/// @brief カメラ設定データ書込み
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(stCameraParamData data)
{
    EnterCriticalSection(&m_camparam.cs);
    m_camparam.data.imgsource  = data.imgsource;    // 画像取込み元(0:停止 1:カメラ 2:画像ファイル) 
    m_camparam.data.imgfname   = data.imgfname;     // 取込み画像ファイル名
    m_camparam.data.width      = data.width;        // カメラ画像サイズ横幅(32の倍数, 2592以下)
    m_camparam.data.height     = data.height;       // カメラ画像サイズ高さ(2の倍数, 2048以下)
    m_camparam.data.fps        = data.fps;          // フレームレート
    m_camparam.data.exptime    = data.exptime;      // 露光時間(usec)(初期値)
    m_camparam.data.exptimemin = data.exptimemin;   // 露光時間(usec)(最小値)
    m_camparam.data.exptimemax = data.exptimemax;   // 露光時間(usec)(最大値)
    LeaveCriticalSection(&m_camparam.cs);

    return RESULT_OK;
}

/// @brief カメラ設定データ読出し
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(stCameraParamData* data)
{
    EnterCriticalSection(&m_camparam.cs);
    data->imgsource  = m_camparam.data.imgsource;   // 画像取込み元(0:停止 1:カメラ 2:画像ファイル) 
    data->imgfname   = m_camparam.data.imgfname;    // 取込み画像ファイル名
    data->width      = m_camparam.data.width;       // カメラ画像サイズ横幅(32の倍数, 2592以下)
    data->height     = m_camparam.data.height;      // カメラ画像サイズ高さ(2の倍数, 2048以下)
    data->fps        = m_camparam.data.fps;         // フレームレート
    data->exptime    = m_camparam.data.exptime;     // 露光時間(usec)(初期値)
    data->exptimemin = m_camparam.data.exptimemin;  // 露光時間(usec)(最小値)
    data->exptimemax = m_camparam.data.exptimemax;  // 露光時間(usec)(最大値)
    LeaveCriticalSection(&m_camparam.cs);

    return RESULT_OK;
}

/// @brief 画像処理設定データ書込み
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(stImgProcParamData data)
{
    EnterCriticalSection(&m_imgprocparam.cs);
    m_imgprocparam.data.roi.valid = data.roi.valid;             // ROI有効
    m_imgprocparam.data.roi.scale = data.roi.scale;             // ROIスケール(検出したターゲットに対する倍率)
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_imgprocparam.data.maskvalid[ii] = data.maskvalid[ii]; // マスク画像選択(0=両方, 1=画像1のみ, 2=画像2のみ)
    }
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_imgprocparam.data.hsvl[ii].h =  data.hsvl[ii].h;      // H
        m_imgprocparam.data.hsvl[ii].s =  data.hsvl[ii].s;      // S
        m_imgprocparam.data.hsvl[ii].v =  data.hsvl[ii].v;      // V
        m_imgprocparam.data.hsvu[ii].h =  data.hsvu[ii].h;      // H
        m_imgprocparam.data.hsvu[ii].s =  data.hsvu[ii].s;      // S
        m_imgprocparam.data.hsvu[ii].v =  data.hsvu[ii].v;      // V
    }
    m_imgprocparam.data.filter1.type = data.filter1.type;       // ゴマ塩ノイズフィルタ:フィルタ有効
    m_imgprocparam.data.filter1.val  = data.filter1.val;        // ゴマ塩ノイズフィルタ:フィルタ値
    m_imgprocparam.data.filter2.type = data.filter2.type;       // 穴埋めノイズフィルタ:フィルタ有効
    m_imgprocparam.data.filter2.val  = data.filter2.val;        // 穴埋めノイズフィルタ:フィルタ値
    m_imgprocparam.data.algorithm    = data.algorithm;          // ターゲット検出アルゴリズム(0=全輪郭点, 1=最大輪郭面積, 2=最大輪郭長)
    LeaveCriticalSection(&m_imgprocparam.cs);

    return RESULT_OK;
}

/// @brief 画像処理設定データ読出し
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(stImgProcParamData* data)
{
    EnterCriticalSection(&m_imgprocparam.cs);
    data->roi.valid = m_imgprocparam.data.roi.valid;                // ROI有効
    data->roi.scale = m_imgprocparam.data.roi.scale;                // ROIスケール(検出したターゲットに対する倍率)
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        data->maskvalid[ii] = m_imgprocparam.data.maskvalid[ii];    // マスク画像選択(0=両方, 1=画像1のみ, 2=画像2のみ)
    }
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        data->hsvl[ii].h =  m_imgprocparam.data.hsvl[ii].h;         // H
        data->hsvl[ii].s =  m_imgprocparam.data.hsvl[ii].s;         // S
        data->hsvl[ii].v =  m_imgprocparam.data.hsvl[ii].v;         // V
        data->hsvu[ii].h =  m_imgprocparam.data.hsvu[ii].h;         // H
        data->hsvu[ii].s =  m_imgprocparam.data.hsvu[ii].s;         // S
        data->hsvu[ii].v =  m_imgprocparam.data.hsvu[ii].v;         // V
    }
    data->filter1.type = m_imgprocparam.data.filter1.type;          // ゴマ塩ノイズフィルタ:フィルタ有効
    data->filter1.val  = m_imgprocparam.data.filter1.val;           // ゴマ塩ノイズフィルタ:フィルタ値
    data->filter2.type = m_imgprocparam.data.filter2.type;          // 穴埋めノイズフィルタ:フィルタ有効
    data->filter2.val  = m_imgprocparam.data.filter2.val;           // 穴埋めノイズフィルタ:フィルタ値
    data->algorithm    = m_imgprocparam.data.algorithm;             // ターゲット検出アルゴリズム(0=全輪郭点, 1=最大輪郭面積, 2=最大輪郭長)
    LeaveCriticalSection(&m_imgprocparam.cs);

    return RESULT_OK;
}

/// @brief RemoteIO設定データ書込み
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(stRIOParamData data)
{
    EnterCriticalSection(&m_rioparam.cs);
    memcpy(m_rioparam.data.ipaddrs, data.ipaddrs, sizeof(m_rioparam.data.ipaddrs)); // RIO IPアドレス
    m_rioparam.data.tcpport    = data.tcpport;                                      // TCPポート番号
    m_rioparam.data.slaveaddrs = data.slaveaddrs;                                   // スレーブアドレス
    m_rioparam.data.timeout    = data.timeout;                                      // 通信タイムアウト(msec)
    m_rioparam.data.portx      = data.portx;                                        // 傾斜計Xデータ接続ポート番号(1～8)
    m_rioparam.data.porty      = data.porty;                                        // 傾斜計Yデータ接続ポート番号(1～8)
    LeaveCriticalSection(&m_rioparam.cs);

    return RESULT_OK;
}

/// @brief RemoteIO設定データ読出し
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(stRIOParamData* data)
{
    EnterCriticalSection(&m_rioparam.cs);
    memcpy(data->ipaddrs, m_rioparam.data.ipaddrs, sizeof(data->ipaddrs));  // RIO IPアドレス    
    data->tcpport    = m_rioparam.data.tcpport;                             // TCPポート番号
    data->slaveaddrs = m_rioparam.data.slaveaddrs;                          // スレーブアドレス
    data->timeout    = m_rioparam.data.timeout;                             // 通信タイムアウト(msec)
    data->portx      = m_rioparam.data.portx;                               // 傾斜計Xデータ接続ポート番号(1～8)
    data->porty      = m_rioparam.data.porty;                               // 傾斜計Yデータ接続ポート番号(1～8)
    LeaveCriticalSection(&m_rioparam.cs);

    return RESULT_OK;
}

/// @brief 画像データ書込み
/// @param
/// @return 
/// @note
INT CSharedObject::SetImage(UINT8 id, cv::Mat image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&m_imginfo[id].cs);
    image.copyTo(m_imginfo[id].data.image);
    m_imginfo[id].data.update = TRUE;
    LeaveCriticalSection(&m_imginfo[id].cs);

    return RESULT_OK;
}

/// @brief 画像データ読出し
/// @param
/// @return 
/// @note
INT CSharedObject::GetImage(UINT8 id, Mat* image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}
    if (image == NULL)      {return RESULT_NG_INVALID;}
    // 更新有無の確認
    if (m_imginfo[id].data.update == FALSE) {return RESULT_NG_SEQUENCE;}
    EnterCriticalSection(&m_imginfo[id].cs);
    m_imginfo[id].data.image.copyTo(*image);
    m_imginfo[id].data.update = FALSE;
    LeaveCriticalSection(&m_imginfo[id].cs);

    return RESULT_OK;
}

/// @brief カメラ情報書込み
/// @param
/// @return 
/// @note
INT CSharedObject::SetInfo(stCameraInfoData data)
{
    EnterCriticalSection(&m_caminfo.cs);
    m_caminfo.data.valid     = data.valid;
    m_caminfo.data.cycleTime = data.cycleTime;
    LeaveCriticalSection(&m_caminfo.cs);

    return RESULT_OK;
}

/// @brief カメラ情報読出し
/// @param
/// @return 
/// @note
INT CSharedObject::GetInfo(stCameraInfoData* data)
{
    if (data == NULL) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&m_caminfo.cs);
    data->valid     = m_caminfo.data.valid;
    data->cycleTime = m_caminfo.data.cycleTime;
    LeaveCriticalSection(&m_caminfo.cs);

    return RESULT_OK;
}

/// @brief RIO情報書込み
/// @param
/// @return 
/// @note
INT CSharedObject::SetInfo(stRIOInfoData data)
{
    EnterCriticalSection(&m_rioinfo.cs);
    m_rioinfo.data.error = data.error;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        m_rioinfo.data.incldata[ii].dig = data.incldata[ii].dig;    // 入力データ
        m_rioinfo.data.incldata[ii].cur = data.incldata[ii].cur;    // 入力データ変換値(mA)
        m_rioinfo.data.incldata[ii].deg = data.incldata[ii].deg;    // 入力データ変換値(deg.)
    }
    LeaveCriticalSection(&m_rioinfo.cs);

    return RESULT_OK;
}

/// @brief RIO情報読出し
/// @param
/// @return 
/// @note
INT CSharedObject::GetInfo(stRIOInfoData* data)
{
    if (data == NULL) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&m_rioinfo.cs);
    data->error = m_rioinfo.data.error;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        data->incldata[ii].dig = m_rioinfo.data.incldata[ii].dig;   // 入力データ
        data->incldata[ii].cur = m_rioinfo.data.incldata[ii].cur;   // 入力データ変換値(mA)
        data->incldata[ii].deg = m_rioinfo.data.incldata[ii].deg;   // 入力データ変換値(deg.)
    }
    LeaveCriticalSection(&m_rioinfo.cs);

    return RESULT_OK;
}

/// @brief 処理データ書込み
/// @param
/// @return 
/// @note
INT CSharedObject::SetInfo(stProcInfoData data)
{
    EnterCriticalSection(&m_procinfo.cs);
    for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_procinfo.data.imgprocdata[ii].posx       = data.imgprocdata[ii].posx;
        m_procinfo.data.imgprocdata[ii].posy       = data.imgprocdata[ii].posy;
        m_procinfo.data.imgprocdata[ii].tgtsize    = data.imgprocdata[ii].tgtsize;
        m_procinfo.data.imgprocdata[ii].roi.x      = data.imgprocdata[ii].roi.x;
        m_procinfo.data.imgprocdata[ii].roi.y      = data.imgprocdata[ii].roi.y;
        m_procinfo.data.imgprocdata[ii].roi.width  = data.imgprocdata[ii].roi.width;
        m_procinfo.data.imgprocdata[ii].roi.height = data.imgprocdata[ii].roi.height;
        m_procinfo.data.imgprocdata[ii].valid      = data.imgprocdata[ii].valid;
    }
    for (UINT ii = 0; ii < SWAY_MAX; ii++)
    {
        m_procinfo.data.sway[ii]    = data.sway[ii];    // 振れ角
        m_procinfo.data.swaySpd[ii] = data.swaySpd[ii]; // 振れ角速度
    }
    m_procinfo.data.exposureTime = data.exposureTime;   // 露光時間[us]
    m_procinfo.data.valid        = data.valid;          // 検出状態
    m_procinfo.data.procTime     = data.procTime;       // 処理時間[ms]
    LeaveCriticalSection(&m_procinfo.cs);

    return RESULT_OK;
}

/// @brief 処理データ読出し
/// @param
/// @return 
/// @note
INT CSharedObject::GetInfo(stProcInfoData* data)
{
    if (data == NULL) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&m_procinfo.cs);
    for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        data->imgprocdata[ii].posx       = m_procinfo.data.imgprocdata[ii].posx;
        data->imgprocdata[ii].posy       = m_procinfo.data.imgprocdata[ii].posy;
        data->imgprocdata[ii].tgtsize    = m_procinfo.data.imgprocdata[ii].tgtsize;
        data->imgprocdata[ii].roi.x      = m_procinfo.data.imgprocdata[ii].roi.x;
        data->imgprocdata[ii].roi.y      = m_procinfo.data.imgprocdata[ii].roi.y;
        data->imgprocdata[ii].roi.width  = m_procinfo.data.imgprocdata[ii].roi.width;
        data->imgprocdata[ii].roi.height = m_procinfo.data.imgprocdata[ii].roi.height;
        data->imgprocdata[ii].valid      = m_procinfo.data.imgprocdata[ii].valid;
    }
    for (UINT ii = 0; ii < SWAY_MAX; ii++)
    {
        data->sway[ii]    = m_procinfo.data.sway[ii];       // 振れ角
        data->swaySpd[ii] = m_procinfo.data.swaySpd[ii];    // 振れ角速度
    }
    data->exposureTime = m_procinfo.data.exposureTime;      // 露光時間[us]
    data->valid        = m_procinfo.data.valid;             // 検出状態
    data->procTime     = m_procinfo.data.procTime;          // 画処理時間[ms]
    LeaveCriticalSection(&m_procinfo.cs);

    return RESULT_OK;
}
