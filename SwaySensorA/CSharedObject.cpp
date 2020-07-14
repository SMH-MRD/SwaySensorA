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
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        m_stImgData[ii].update = FALSE;
    }
    for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_stImgProcData[ii].posx       = 0.0;
        m_stImgProcData[ii].posy       = 0.0;
        m_stImgProcData[ii].roiSize    = 0;
        m_stImgProcData[ii].roi.x      = 0;
        m_stImgProcData[ii].roi.y      = 0;
        m_stImgProcData[ii].roi.width  = 0;
        m_stImgProcData[ii].roi.height = 0;
        m_stImgProcData[ii].expTime    = 0.0;
        m_stImgProcData[ii].enable     = FALSE;
    }
    for (UINT ii = 0; ii < INCLINO_ID_MAX; ii++)
    {
        m_stIncData[ii].data = 0.0;
    }
    for (UINT ii = 0; ii < PARAM_ID_MAX; ii++)
    {
        m_u32Param[ii] = 0;
    }
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++)
    {
        m_strParam[ii] = "";
    }
    for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX; ii++)
    {
        m_dParam[ii] = 0.0;
    }

    // 共有データアクセス用クリティカルセクションの初期化
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csImage[ii]);
    }
    for (UINT ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csProcData[ii]);
    }
    for (UINT ii = 0; ii < INCLINO_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csInclino[ii]);
    }
    for (UINT ii = 0; ii < PARAM_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csParam[ii]);
    }
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++)
    {
        InitializeCriticalSection(&csStrParam[ii]);
    }
    for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX; ii++)
    {
        InitializeCriticalSection(&csDoubleParam[ii]);
    }
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

/// @brief 画像処理データ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetProcData(UINT8 id, stImageProcData data)
{
    if (id >= IMGPROC_ID_MAX) {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csProcData[id]);
    m_stImgProcData[id].posx       = data.posx;
    m_stImgProcData[id].posy       = data.posy;
    m_stImgProcData[id].roiSize    = data.roiSize;
    m_stImgProcData[id].roi.x      = data.roi.x;
    m_stImgProcData[id].roi.y      = data.roi.y;
    m_stImgProcData[id].roi.width  = data.roi.width;
    m_stImgProcData[id].roi.height = data.roi.height;
    m_stImgProcData[id].expTime    = data.expTime;
    m_stImgProcData[id].enable     = data.enable;
    LeaveCriticalSection(&csProcData[id]);

    return RESULT_OK;
}

/// @brief 画像処理データ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetProcData(UINT8 id, stImageProcData* data)
{
    if (id >= IMGPROC_ID_MAX) {return RESULT_NG_INVALID;}
    if (data == NULL)         {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csProcData[id]);
    data->posx       = m_stImgProcData[id].posx;
    data->posy       = m_stImgProcData[id].posy;
    data->roiSize    = m_stImgProcData[id].roiSize;
    data->roi.x      = m_stImgProcData[id].roi.x;
    data->roi.y      = m_stImgProcData[id].roi.y;
    data->roi.width  = m_stImgProcData[id].roi.width;
    data->roi.height = m_stImgProcData[id].roi.height;
    data->expTime    = m_stImgProcData[id].expTime;
    data->enable     = m_stImgProcData[id].enable;
    LeaveCriticalSection(&csProcData[id]);

    return RESULT_OK;
}

/// @brief 傾斜計データ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetInclinoData(UINT8 id, DOUBLE data)
{
    if (id >= INCLINO_ID_MAX) {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csInclino[id]);
    m_stIncData[id].data = data;
    LeaveCriticalSection(&csInclino[id]);

    return RESULT_OK;
}

/// @brief 傾斜計データ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetInclinoData(UINT8 id, DOUBLE* data)
{
    if (id >= INCLINO_ID_MAX) {return RESULT_NG_INVALID;}
    if (data == NULL)         {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csInclino[id]);
    *data = m_stIncData[id].data;
    LeaveCriticalSection(&csInclino[id]);

    return RESULT_OK;
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

/// @brief DOUBLEパラメータデータ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(UINT8 id, DOUBLE data)
{
    if (id >= PARAM_ID_MAX) {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csDoubleParam[id]);
    m_dParam[id] = data;
    LeaveCriticalSection(&csDoubleParam[id]);

    return RESULT_OK;
}

/// @brief DOUBLEパラメータデータ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(UINT8 id, DOUBLE* data)
{
    if (id >= PARAM_ID_MAX) {return RESULT_NG_INVALID;}
    if (data == NULL)       {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csDoubleParam[id]);
    *data = m_dParam[id];
    LeaveCriticalSection(&csDoubleParam[id]);

    return RESULT_OK;
}
