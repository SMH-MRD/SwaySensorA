#include "CSharedObject.h"


/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CSharedObject::CSharedObject()
{
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
void CSharedObject::InitSharedObject(void)
{
    //--------------------------------------------------------------------------
    // 
    // UINT32パラメータデータ
    for (UINT ii = 0; ii < PARAM_ID_MAX; ii++)
    {
        m_u32Param[ii] = 0;
    }
    // stringパラメータデータ
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++)
    {
        m_strParam[ii] = "";
    }
    // カメラ情報
    {
        m_stCameraInfo.valid     = FALSE;
        m_stCameraInfo.cycleTime = 0.0;
    }
    // RIO情報
    {
        m_stRIOInfo.error = RIO_ERR_NONE;
        for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
        {
            m_stRIOInfo.data[ii].dig = 0;   // 入力データ
            m_stRIOInfo.data[ii].cur = 0.0; // 入力データ変換値(mA)
            m_stRIOInfo.data[ii].deg = 0.0; // 入力データ変換値(deg.)    
        }
    }
    // 画像データ
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        m_stImgData[ii].update = FALSE;
    }
    // 処理情報
    {
        for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
        {
            m_stProcInfo.data[ii].posx       = 0.0;
            m_stProcInfo.data[ii].posy       = 0.0;
            m_stProcInfo.data[ii].roiSize    = 0;
            m_stProcInfo.data[ii].roi.x      = 0;
            m_stProcInfo.data[ii].roi.y      = 0;
            m_stProcInfo.data[ii].roi.width  = 0;
            m_stProcInfo.data[ii].roi.height = 0;
            m_stProcInfo.data[ii].valid      = FALSE;
        }
        for (UINT ii = 0; ii < SWAY_MAX; ii++)
        {
            m_stProcInfo.sway[ii]    = 0.0; // 振れ角
            m_stProcInfo.swaySpd[ii] = 0.0; // 振れ角速度
        }
        m_stProcInfo.valid        = FALSE;  // 検出状態
        m_stProcInfo.exposureTime = 0.0;    // 露光時間[us]
        m_stProcInfo.procTime     = 0.0;    // 画処理時間[ms]
    }

    //--------------------------------------------------------------------------
    // 共有データアクセス用クリティカルセクションの初期化
    // UINT32パラメータデータ
    for (UINT ii = 0; ii < PARAM_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csParam[ii]);
    }
    // stringパラメータデータ
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++)
    {
        InitializeCriticalSection(&csStrParam[ii]);
    }
    // カメラ情報
    {
        InitializeCriticalSection(&csCameraInfo);
    }
    // RIO情報
    {
        InitializeCriticalSection(&csRIOInfo);
    }
    // 画像データ
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csImage[ii]);
    }
    // 処理情報
    {
        InitializeCriticalSection(&csProcInfo);
    }
}

/// @brief UINT32パラメータデータ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(UINT8 id, UINT32 data)
{
    if (id >= PARAM_ID_MAX) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csParam[id]);
    m_u32Param[id] = data;
    LeaveCriticalSection(&csParam[id]);

    return RESULT_OK;
}

/// @brief UINT32パラメータデータ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(UINT8 id, UINT32* data)
{
    if (id >= PARAM_ID_MAX) {return RESULT_NG_INVALID;}
    if (data == NULL)       {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csParam[id]);
    *data = m_u32Param[id];
    LeaveCriticalSection(&csParam[id]);

    return RESULT_OK;
}

/// @brief stringパラメータデータ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(UINT8 id, string str)
{
    if (id >= PARAM_ID_MAX) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csStrParam[id]);
    m_strParam[id] = str;
    LeaveCriticalSection(&csStrParam[id]);

    return RESULT_OK;
}

/// @brief stringパラメータデータ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(UINT8 id, string* str)
{
    if (id >= PARAM_ID_MAX) {return RESULT_NG_INVALID;}
    if (str == NULL)        {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csStrParam[id]);
    *str = m_strParam[id];
    LeaveCriticalSection(&csStrParam[id]);

    return RESULT_OK;
}

/// @brief カメラ情報設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetCameraInfo(stCameraInfo info)
{
    EnterCriticalSection(&csCameraInfo);
    m_stCameraInfo.valid     = info.valid;
    m_stCameraInfo.cycleTime = info.cycleTime;
    LeaveCriticalSection(&csCameraInfo);

    return RESULT_OK;
}

/// @brief カメラ情報取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetCameraInfo(stCameraInfo* info)
{
    if (info == NULL) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csCameraInfo);
    info->valid     = m_stCameraInfo.valid;
    info->cycleTime = m_stCameraInfo.cycleTime;
    LeaveCriticalSection(&csCameraInfo);

    return RESULT_OK;
}

/// @brief RIO情報設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetRIOInfo(stRIOInfo info)
{
    EnterCriticalSection(&csRIOInfo);
    m_stRIOInfo.error = info.error;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        m_stRIOInfo.data[ii].dig = info.data[ii].dig;   // 入力データ
        m_stRIOInfo.data[ii].cur = info.data[ii].cur;   // 入力データ変換値(mA)
        m_stRIOInfo.data[ii].deg = info.data[ii].deg;   // 入力データ変換値(deg.)
    }
    LeaveCriticalSection(&csRIOInfo);

    return RESULT_OK;
}

/// @brief RIO情報取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetRIOInfo(stRIOInfo* info)
{
    if (info == NULL) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csRIOInfo);
    info->error = m_stRIOInfo.error;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        info->data[ii].dig = m_stRIOInfo.data[ii].dig;  // 入力データ
        info->data[ii].cur = m_stRIOInfo.data[ii].cur;  // 入力データ変換値(mA)
        info->data[ii].deg = m_stRIOInfo.data[ii].deg;  // 入力データ変換値(deg.)
    }
    LeaveCriticalSection(&csRIOInfo);

    return RESULT_OK;
}

/// @brief 画像データ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetImage(UINT8 id, cv::Mat image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csImage[id]);
    image.copyTo(m_stImgData[id].image);
    m_stImgData[id].update = TRUE;
    LeaveCriticalSection(&csImage[id]);

    return RESULT_OK;
}

/// @brief 画像データ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetImage(UINT8 id, Mat* image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}
    if (image == NULL)      {return RESULT_NG_INVALID;}
    // 更新有無の確認
    if (m_stImgData[id].update == FALSE) {return RESULT_NG_SEQUENCE;}
    EnterCriticalSection(&csImage[id]);
    m_stImgData[id].image.copyTo(*image);
    m_stImgData[id].update = FALSE;
    LeaveCriticalSection(&csImage[id]);

    return RESULT_OK;
}

/// @brief 処理データ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetProcInfo(stProcInfo info)
{
    EnterCriticalSection(&csProcInfo);
    for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_stProcInfo.data[ii].posx       = info.data[ii].posx;
        m_stProcInfo.data[ii].posy       = info.data[ii].posy;
        m_stProcInfo.data[ii].roiSize    = info.data[ii].roiSize;
        m_stProcInfo.data[ii].roi.x      = info.data[ii].roi.x;
        m_stProcInfo.data[ii].roi.y      = info.data[ii].roi.y;
        m_stProcInfo.data[ii].roi.width  = info.data[ii].roi.width;
        m_stProcInfo.data[ii].roi.height = info.data[ii].roi.height;
        m_stProcInfo.data[ii].valid      = info.data[ii].valid;
    }
    for (UINT ii = 0; ii < SWAY_MAX; ii++)
    {
        m_stProcInfo.sway[ii]    = info.sway[ii];       // 振れ角
        m_stProcInfo.swaySpd[ii] = info.swaySpd[ii];    // 振れ角速度
    }
    m_stProcInfo.exposureTime = info.exposureTime;  // 露光時間[us]
    m_stProcInfo.valid        = info.valid;         // 検出状態
    m_stProcInfo.procTime     = info.procTime;      // 処理時間[ms]
    LeaveCriticalSection(&csProcInfo);

    return RESULT_OK;
}

/// @brief 処理データ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetProcInfo(stProcInfo* info)
{
    if (info == NULL) {return RESULT_NG_INVALID;}
    EnterCriticalSection(&csProcInfo);
    for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        info->data[ii].posx       = m_stProcInfo.data[ii].posx;
        info->data[ii].posy       = m_stProcInfo.data[ii].posy;
        info->data[ii].roiSize    = m_stProcInfo.data[ii].roiSize;
        info->data[ii].roi.x      = m_stProcInfo.data[ii].roi.x;
        info->data[ii].roi.y      = m_stProcInfo.data[ii].roi.y;
        info->data[ii].roi.width  = m_stProcInfo.data[ii].roi.width;
        info->data[ii].roi.height = m_stProcInfo.data[ii].roi.height;
        info->data[ii].valid      = m_stProcInfo.data[ii].valid;
    }
    for (UINT ii = 0; ii < SWAY_MAX; ii++)
    {
        info->sway[ii]    = m_stProcInfo.sway[ii];      // 振れ角
        info->swaySpd[ii] = m_stProcInfo.swaySpd[ii];   // 振れ角速度
    }
    info->exposureTime = m_stProcInfo.exposureTime; // 露光時間[us]
    info->valid        = m_stProcInfo.valid;        // 検出状態
    info->procTime     = m_stProcInfo.procTime;     // 画処理時間[ms]
    LeaveCriticalSection(&csProcInfo);

    return RESULT_OK;
}
