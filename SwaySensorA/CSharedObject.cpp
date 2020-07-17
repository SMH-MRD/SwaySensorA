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
    //--------------------------------------------------------------------------
    // 
    // UINT32�p�����[�^�f�[�^
    for (UINT ii = 0; ii < PARAM_ID_MAX; ii++)
    {
        m_u32Param[ii] = 0;
    }
    // string�p�����[�^�f�[�^
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++)
    {
        m_strParam[ii] = "";
    }
    // �J�������
    {
        m_stCameraInfo.valid     = FALSE;
        m_stCameraInfo.cycleTime = 0.0;
    }
    // RIO���
    {
        m_stRIOInfo.error = RIO_ERR_NONE;
        for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
        {
            m_stRIOInfo.data[ii].dig = 0;   // ���̓f�[�^
            m_stRIOInfo.data[ii].cur = 0.0; // ���̓f�[�^�ϊ��l(mA)
            m_stRIOInfo.data[ii].deg = 0.0; // ���̓f�[�^�ϊ��l(deg.)    
        }
    }
    // �摜�f�[�^
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        m_stImgData[ii].update = FALSE;
    }
    // �������
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
        m_stProcInfo.exposureTime = 0.0;    // �I������[us]
        m_stProcInfo.procTime     = 0.0;    // �揈������[ms]
    }

    //--------------------------------------------------------------------------
    // ���L�f�[�^�A�N�Z�X�p�N���e�B�J���Z�N�V�����̏�����
    // UINT32�p�����[�^�f�[�^
    for (UINT ii = 0; ii < PARAM_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csParam[ii]);
    }
    // string�p�����[�^�f�[�^
    for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++)
    {
        InitializeCriticalSection(&csStrParam[ii]);
    }
    // �J�������
    {
        InitializeCriticalSection(&csCameraInfo);
    }
    // RIO���
    {
        InitializeCriticalSection(&csRIOInfo);
    }
    // �摜�f�[�^
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        InitializeCriticalSection(&csImage[ii]);
    }
    // �������
    {
        InitializeCriticalSection(&csProcInfo);
    }
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

/// @brief �J�������ݒ菈��
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

/// @brief �J�������擾����
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

/// @brief RIO���ݒ菈��
/// @param
/// @return 
/// @note
INT CSharedObject::SetRIOInfo(stRIOInfo info)
{
    EnterCriticalSection(&csRIOInfo);
    m_stRIOInfo.error = info.error;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        m_stRIOInfo.data[ii].dig = info.data[ii].dig;   // ���̓f�[�^
        m_stRIOInfo.data[ii].cur = info.data[ii].cur;   // ���̓f�[�^�ϊ��l(mA)
        m_stRIOInfo.data[ii].deg = info.data[ii].deg;   // ���̓f�[�^�ϊ��l(deg.)
    }
    LeaveCriticalSection(&csRIOInfo);

    return RESULT_OK;
}

/// @brief RIO���擾����
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
        info->data[ii].dig = m_stRIOInfo.data[ii].dig;  // ���̓f�[�^
        info->data[ii].cur = m_stRIOInfo.data[ii].cur;  // ���̓f�[�^�ϊ��l(mA)
        info->data[ii].deg = m_stRIOInfo.data[ii].deg;  // ���̓f�[�^�ϊ��l(deg.)
    }
    LeaveCriticalSection(&csRIOInfo);

    return RESULT_OK;
}

/// @brief �摜�f�[�^�ݒ菈��
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

/// @brief �摜�f�[�^�擾����
/// @param
/// @return 
/// @note
INT CSharedObject::GetImage(UINT8 id, Mat* image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}
    if (image == NULL)      {return RESULT_NG_INVALID;}
    // �X�V�L���̊m�F
    if (m_stImgData[id].update == FALSE) {return RESULT_NG_SEQUENCE;}
    EnterCriticalSection(&csImage[id]);
    m_stImgData[id].image.copyTo(*image);
    m_stImgData[id].update = FALSE;
    LeaveCriticalSection(&csImage[id]);

    return RESULT_OK;
}

/// @brief �����f�[�^�ݒ菈��
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
    m_stProcInfo.exposureTime = info.exposureTime;  // �I������[us]
    m_stProcInfo.procTime     = info.procTime;      // ��������[ms]
    LeaveCriticalSection(&csProcInfo);

    return RESULT_OK;
}

/// @brief �����f�[�^�擾����
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
    info->exposureTime = m_stProcInfo.exposureTime; // �I������[us]
    info->procTime     = m_stProcInfo.procTime;     // �揈������[ms]
    LeaveCriticalSection(&csProcInfo);

    return RESULT_OK;
}
