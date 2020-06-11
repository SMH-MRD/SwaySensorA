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
    for (UINT ii = 0; ii < IMAGE_ID_CAM_MAX;    ii++) {m_stImage[ii].update = false;}
    for (UINT ii = 0; ii < IMAGE_ID_PROC_MAX;   ii++) {m_stProcImage[ii].data.posx = 0.0; m_stProcImage[ii].data.posy = 0.0; m_stProcImage[ii].update = 0;}
    for (UINT ii = 0; ii < INCLINO_ID_MAX;      ii++) {m_stInclinoData[ii].data = 0.0;}
    for (UINT ii = 0; ii < PARAM_ID_MAX;        ii++) {m_u32Param[ii] = 0;}
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX;    ii++) {m_strParam[ii] = "";}
    for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX; ii++) {m_dParam[ii] = 0.0;}

    // 共有データアクセス用クリティカルセクションの初期化
    for (UINT ii = 0; ii < IMAGE_ID_CAM_MAX;    ii++) {InitializeCriticalSection(&csImage[ii]);}
    for (UINT ii = 0; ii < IMAGE_ID_PROC_MAX;   ii++) {InitializeCriticalSection(&csProcImage[ii]);}
    for (UINT ii = 0; ii < INCLINO_ID_MAX;      ii++) {InitializeCriticalSection(&csInclino[ii]);}
    for (UINT ii = 0; ii < PARAM_ID_MAX;        ii++) {InitializeCriticalSection(&csParam[ii]);}
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX;    ii++) {InitializeCriticalSection(&csStrParam[ii]);}
    for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX; ii++) {InitializeCriticalSection(&csDoubleParam[ii]);}
}

/// @brief 画像データ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetImage(UINT8 id, Mat image)
{
    if (id >= IMAGE_ID_CAM_MAX) {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csImage[id]);
    image.copyTo(m_stImage[id].image);
    m_stImage[id].update = TRUE;
    LeaveCriticalSection(&csImage[id]);

    return RESULT_OK;
}

/// @brief 画像データ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetImage(UINT8 id, Mat* image)
{
    if (id >= IMAGE_ID_CAM_MAX) {return RESULT_NG_INVALID;}
    if (image == NULL)          {return RESULT_NG_INVALID;}
    // 更新有無の確認
    if (m_stImage[id].update == FALSE) {return RESULT_NG_SEQUENCE;}

    EnterCriticalSection(&csImage[id]);
    m_stImage[id].image.copyTo(*image);
    m_stImage[id].update = FALSE;
    LeaveCriticalSection(&csImage[id]);

    return RESULT_OK;
}

/// @brief 処理画像データ設定処理
/// @param
/// @return 
/// @note
INT CSharedObject::SetProcImage(UINT8 id, STProcData data)
{
    if (id >= IMAGE_ID_PROC_MAX) {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csProcImage[id]);
    data.image.copyTo(m_stProcImage[id].data.image);
    m_stProcImage[id].data.posx = data.posx;
    m_stProcImage[id].data.posy = data.posy;
    m_stProcImage[id].update    = TRUE;
    LeaveCriticalSection(&csProcImage[id]);

    return RESULT_OK;
}

/// @brief 処理画像データ取得処理
/// @param
/// @return 
/// @note
INT CSharedObject::GetProcImage(UINT8 id, STProcData* data)
{
    if (id >= IMAGE_ID_PROC_MAX) {return RESULT_NG_INVALID;}
    if (data == NULL)            {return RESULT_NG_INVALID;}
    // 更新有無の確認
    if (m_stProcImage[id].update == FALSE) {return RESULT_NG_SEQUENCE;}

    EnterCriticalSection(&csProcImage[id]);
    m_stProcImage[id].data.image.copyTo(data->image);
    data->posx = m_stProcImage[id].data.posx;
    data->posy = m_stProcImage[id].data.posy;
    m_stProcImage[id].update = FALSE;
    LeaveCriticalSection(&csProcImage[id]);

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
    m_stInclinoData[id].data = data;
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
    *data = m_stInclinoData[id].data;
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
