#include "CSharedObject.h"


/// @brief �R���X�g���N�^
/// @param
/// @return 
/// @note
CSharedObject::CSharedObject()
{
}

/// @brief �f�X�g���N�^
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
    for (UINT ii = 0; ii < IMAGE_ID_MAX;        ii++) {m_stImage[ii].update = FALSE;}
    for (UINT ii = 0; ii < IMGPROC_ID_MAX;      ii++) {m_stProcData[ii].posx = 0.0; m_stProcData[ii].posy = 0.0; m_stProcData[ii].enable = FALSE;}
    for (UINT ii = 0; ii < INCLINO_ID_MAX;      ii++) {m_stInclinoData[ii].data = 0.0;}
    for (UINT ii = 0; ii < PARAM_ID_MAX;        ii++) {m_u32Param[ii] = 0;}
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX;    ii++) {m_strParam[ii] = "";}
    for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX; ii++) {m_dParam[ii] = 0.0;}

    // ���L�f�[�^�A�N�Z�X�p�N���e�B�J���Z�N�V�����̏�����
    for (UINT ii = 0; ii < IMAGE_ID_MAX;        ii++) {InitializeCriticalSection(&csImage[ii]);}
    for (UINT ii = 0; ii < IMGPROC_ID_MAX;      ii++) {InitializeCriticalSection(&csProcData[ii]);}
    for (UINT ii = 0; ii < INCLINO_ID_MAX;      ii++) {InitializeCriticalSection(&csInclino[ii]);}
    for (UINT ii = 0; ii < PARAM_ID_MAX;        ii++) {InitializeCriticalSection(&csParam[ii]);}
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX;    ii++) {InitializeCriticalSection(&csStrParam[ii]);}
    for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX; ii++) {InitializeCriticalSection(&csDoubleParam[ii]);}
}

/// @brief �摜�f�[�^�ݒ菈��
/// @param
/// @return 
/// @note
INT CSharedObject::SetImage(UINT8 id, Mat image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csImage[id]);
    image.copyTo(m_stImage[id].image);
    m_stImage[id].update = TRUE;
    LeaveCriticalSection(&csImage[id]);

    return RESULT_OK;
}

/// @brief �摜�f�[�^�擾����
/// @param
/// @return 
/// @note
INT CSharedObject::GetImage(UINT8 id, Mat* image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}
    if (image == NULL)      {return RESULT_NG_INVALID;}
    // �X�V�L���̊m�F
    if (m_stImage[id].update == FALSE) {return RESULT_NG_SEQUENCE;}

    EnterCriticalSection(&csImage[id]);
    m_stImage[id].image.copyTo(*image);
    m_stImage[id].update = FALSE;
    LeaveCriticalSection(&csImage[id]);

    return RESULT_OK;
}

/// @brief �摜�����f�[�^�ݒ菈��
/// @param
/// @return 
/// @note
INT CSharedObject::SetProcData(UINT8 id, stMngProcData data)
{
    if (id >= IMGPROC_ID_MAX) {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csProcData[id]);
    m_stProcData[id].posx   = data.posx;
    m_stProcData[id].posy   = data.posy;
    m_stProcData[id].enable = data.enable;
    LeaveCriticalSection(&csProcData[id]);

    return RESULT_OK;
}

/// @brief �摜�����f�[�^�擾����
/// @param
/// @return 
/// @note
INT CSharedObject::GetProcData(UINT8 id, stMngProcData* data)
{
    if (id >= IMGPROC_ID_MAX) {return RESULT_NG_INVALID;}
    if (data == NULL)         {return RESULT_NG_INVALID;}

    EnterCriticalSection(&csProcData[id]);
    data->posx   = m_stProcData[id].posx;
    data->posy   = m_stProcData[id].posy;
    data->enable = m_stProcData[id].enable;
    LeaveCriticalSection(&csProcData[id]);

    return RESULT_OK;
}

/// @brief �X�Όv�f�[�^�ݒ菈��
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

/// @brief �X�Όv�f�[�^�擾����
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

/// @brief UINT32�p�����[�^�f�[�^�ݒ菈��
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

/// @brief UINT32�p�����[�^�f�[�^�擾����
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

/// @brief string�p�����[�^�f�[�^�ݒ菈��
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

/// @brief string�p�����[�^�f�[�^�擾����
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

/// @brief DOUBLE�p�����[�^�f�[�^�ݒ菈��
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

/// @brief DOUBLE�p�����[�^�f�[�^�擾����
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
