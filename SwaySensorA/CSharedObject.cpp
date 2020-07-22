#include "CSharedObject.h"


/// @brief �R���X�g���N�^
/// @param
/// @return 
/// @note
CSharedObject::CSharedObject()
{
    Initialize();
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
void CSharedObject::Initialize(void)
{
    //--------------------------------------------------------------------------
    // 
    // �J�������
    {
        m_caminfo.data.valid     = FALSE;
        m_caminfo.data.cycleTime = 0.0;
    }
    // RIO���
    {
        m_rioinfo.data.error = RIO_ERR_NONE;
        for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
        {
            m_rioinfo.data.incldata[ii].dig = 0;    // ���̓f�[�^
            m_rioinfo.data.incldata[ii].cur = 0.0;  // ���̓f�[�^�ϊ��l(mA)
            m_rioinfo.data.incldata[ii].deg = 0.0;  // ���̓f�[�^�ϊ��l(deg.)    
        }
    }
    // �摜�f�[�^
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        m_imginfo[ii].data.update = FALSE;
    }
    // �������
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
            m_procinfo.data.sway[ii]    = 0.0;  // �U��p
            m_procinfo.data.swaySpd[ii] = 0.0;  // �U��p���x
        }
        m_procinfo.data.valid        = FALSE;   // ���o���
        m_procinfo.data.exposureTime = 0.0;     // �I������[us]
        m_procinfo.data.procTime     = 0.0;     // �揈������[ms]
    }

    //--------------------------------------------------------------------------
    // ���L�f�[�^�A�N�Z�X�p�N���e�B�J���Z�N�V�����̏�����
    // �J�����ݒ�
    {
        InitializeCriticalSection(&m_camparam.cs);
    }
    // �摜�����ݒ�
    {
        InitializeCriticalSection(&m_imgprocparam.cs);
    }
     // RemoteIO�ݒ�
    {
        InitializeCriticalSection(&m_rioparam.cs);
    }
    // �J�������
    {
        InitializeCriticalSection(&m_caminfo.cs);
    }
    // RIO���
    {
        InitializeCriticalSection(&m_rioinfo.cs);
    }
    // �摜�f�[�^
    for (UINT ii = 0; ii < IMAGE_ID_MAX; ii++)
    {
        InitializeCriticalSection(&m_imginfo[ii].cs);
    }
    // �������
    {
        InitializeCriticalSection(&m_procinfo.cs);
    }
}

/// @brief �J�����ݒ�f�[�^������
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(stCameraParamData data)
{
    EnterCriticalSection(&m_camparam.cs);
    m_camparam.data.imgsource  = data.imgsource;    // �摜�捞�݌�(0:��~ 1:�J���� 2:�摜�t�@�C��) 
    m_camparam.data.imgfname   = data.imgfname;     // �捞�݉摜�t�@�C����
    m_camparam.data.width      = data.width;        // �J�����摜�T�C�Y����(32�̔{��, 2592�ȉ�)
    m_camparam.data.height     = data.height;       // �J�����摜�T�C�Y����(2�̔{��, 2048�ȉ�)
    m_camparam.data.fps        = data.fps;          // �t���[�����[�g
    m_camparam.data.exptime    = data.exptime;      // �I������(usec)(�����l)
    m_camparam.data.exptimemin = data.exptimemin;   // �I������(usec)(�ŏ��l)
    m_camparam.data.exptimemax = data.exptimemax;   // �I������(usec)(�ő�l)
    LeaveCriticalSection(&m_camparam.cs);

    return RESULT_OK;
}

/// @brief �J�����ݒ�f�[�^�Ǐo��
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(stCameraParamData* data)
{
    EnterCriticalSection(&m_camparam.cs);
    data->imgsource  = m_camparam.data.imgsource;   // �摜�捞�݌�(0:��~ 1:�J���� 2:�摜�t�@�C��) 
    data->imgfname   = m_camparam.data.imgfname;    // �捞�݉摜�t�@�C����
    data->width      = m_camparam.data.width;       // �J�����摜�T�C�Y����(32�̔{��, 2592�ȉ�)
    data->height     = m_camparam.data.height;      // �J�����摜�T�C�Y����(2�̔{��, 2048�ȉ�)
    data->fps        = m_camparam.data.fps;         // �t���[�����[�g
    data->exptime    = m_camparam.data.exptime;     // �I������(usec)(�����l)
    data->exptimemin = m_camparam.data.exptimemin;  // �I������(usec)(�ŏ��l)
    data->exptimemax = m_camparam.data.exptimemax;  // �I������(usec)(�ő�l)
    LeaveCriticalSection(&m_camparam.cs);

    return RESULT_OK;
}

/// @brief �摜�����ݒ�f�[�^������
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(stImgProcParamData data)
{
    EnterCriticalSection(&m_imgprocparam.cs);
    m_imgprocparam.data.roi.valid = data.roi.valid;             // ROI�L��
    m_imgprocparam.data.roi.scale = data.roi.scale;             // ROI�X�P�[��(���o�����^�[�Q�b�g�ɑ΂���{��)
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_imgprocparam.data.maskvalid[ii] = data.maskvalid[ii]; // �}�X�N�摜�I��(0=����, 1=�摜1�̂�, 2=�摜2�̂�)
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
    m_imgprocparam.data.filter1.type = data.filter1.type;       // �S�}���m�C�Y�t�B���^:�t�B���^�L��
    m_imgprocparam.data.filter1.val  = data.filter1.val;        // �S�}���m�C�Y�t�B���^:�t�B���^�l
    m_imgprocparam.data.filter2.type = data.filter2.type;       // �����߃m�C�Y�t�B���^:�t�B���^�L��
    m_imgprocparam.data.filter2.val  = data.filter2.val;        // �����߃m�C�Y�t�B���^:�t�B���^�l
    m_imgprocparam.data.algorithm    = data.algorithm;          // �^�[�Q�b�g���o�A���S���Y��(0=�S�֊s�_, 1=�ő�֊s�ʐ�, 2=�ő�֊s��)
    LeaveCriticalSection(&m_imgprocparam.cs);

    return RESULT_OK;
}

/// @brief �摜�����ݒ�f�[�^�Ǐo��
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(stImgProcParamData* data)
{
    EnterCriticalSection(&m_imgprocparam.cs);
    data->roi.valid = m_imgprocparam.data.roi.valid;                // ROI�L��
    data->roi.scale = m_imgprocparam.data.roi.scale;                // ROI�X�P�[��(���o�����^�[�Q�b�g�ɑ΂���{��)
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        data->maskvalid[ii] = m_imgprocparam.data.maskvalid[ii];    // �}�X�N�摜�I��(0=����, 1=�摜1�̂�, 2=�摜2�̂�)
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
    data->filter1.type = m_imgprocparam.data.filter1.type;          // �S�}���m�C�Y�t�B���^:�t�B���^�L��
    data->filter1.val  = m_imgprocparam.data.filter1.val;           // �S�}���m�C�Y�t�B���^:�t�B���^�l
    data->filter2.type = m_imgprocparam.data.filter2.type;          // �����߃m�C�Y�t�B���^:�t�B���^�L��
    data->filter2.val  = m_imgprocparam.data.filter2.val;           // �����߃m�C�Y�t�B���^:�t�B���^�l
    data->algorithm    = m_imgprocparam.data.algorithm;             // �^�[�Q�b�g���o�A���S���Y��(0=�S�֊s�_, 1=�ő�֊s�ʐ�, 2=�ő�֊s��)
    LeaveCriticalSection(&m_imgprocparam.cs);

    return RESULT_OK;
}

/// @brief RemoteIO�ݒ�f�[�^������
/// @param
/// @return 
/// @note
INT CSharedObject::SetParam(stRIOParamData data)
{
    EnterCriticalSection(&m_rioparam.cs);
    memcpy(m_rioparam.data.ipaddrs, data.ipaddrs, sizeof(m_rioparam.data.ipaddrs)); // RIO IP�A�h���X
    m_rioparam.data.tcpport    = data.tcpport;                                      // TCP�|�[�g�ԍ�
    m_rioparam.data.slaveaddrs = data.slaveaddrs;                                   // �X���[�u�A�h���X
    m_rioparam.data.timeout    = data.timeout;                                      // �ʐM�^�C���A�E�g(msec)
    m_rioparam.data.portx      = data.portx;                                        // �X�ΌvX�f�[�^�ڑ��|�[�g�ԍ�(1�`8)
    m_rioparam.data.porty      = data.porty;                                        // �X�ΌvY�f�[�^�ڑ��|�[�g�ԍ�(1�`8)
    LeaveCriticalSection(&m_rioparam.cs);

    return RESULT_OK;
}

/// @brief RemoteIO�ݒ�f�[�^�Ǐo��
/// @param
/// @return 
/// @note
INT CSharedObject::GetParam(stRIOParamData* data)
{
    EnterCriticalSection(&m_rioparam.cs);
    memcpy(data->ipaddrs, m_rioparam.data.ipaddrs, sizeof(data->ipaddrs));  // RIO IP�A�h���X    
    data->tcpport    = m_rioparam.data.tcpport;                             // TCP�|�[�g�ԍ�
    data->slaveaddrs = m_rioparam.data.slaveaddrs;                          // �X���[�u�A�h���X
    data->timeout    = m_rioparam.data.timeout;                             // �ʐM�^�C���A�E�g(msec)
    data->portx      = m_rioparam.data.portx;                               // �X�ΌvX�f�[�^�ڑ��|�[�g�ԍ�(1�`8)
    data->porty      = m_rioparam.data.porty;                               // �X�ΌvY�f�[�^�ڑ��|�[�g�ԍ�(1�`8)
    LeaveCriticalSection(&m_rioparam.cs);

    return RESULT_OK;
}

/// @brief �摜�f�[�^������
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

/// @brief �摜�f�[�^�Ǐo��
/// @param
/// @return 
/// @note
INT CSharedObject::GetImage(UINT8 id, Mat* image)
{
    if (id >= IMAGE_ID_MAX) {return RESULT_NG_INVALID;}
    if (image == NULL)      {return RESULT_NG_INVALID;}
    // �X�V�L���̊m�F
    if (m_imginfo[id].data.update == FALSE) {return RESULT_NG_SEQUENCE;}
    EnterCriticalSection(&m_imginfo[id].cs);
    m_imginfo[id].data.image.copyTo(*image);
    m_imginfo[id].data.update = FALSE;
    LeaveCriticalSection(&m_imginfo[id].cs);

    return RESULT_OK;
}

/// @brief �J������񏑍���
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

/// @brief �J�������Ǐo��
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

/// @brief RIO��񏑍���
/// @param
/// @return 
/// @note
INT CSharedObject::SetInfo(stRIOInfoData data)
{
    EnterCriticalSection(&m_rioinfo.cs);
    m_rioinfo.data.error = data.error;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        m_rioinfo.data.incldata[ii].dig = data.incldata[ii].dig;    // ���̓f�[�^
        m_rioinfo.data.incldata[ii].cur = data.incldata[ii].cur;    // ���̓f�[�^�ϊ��l(mA)
        m_rioinfo.data.incldata[ii].deg = data.incldata[ii].deg;    // ���̓f�[�^�ϊ��l(deg.)
    }
    LeaveCriticalSection(&m_rioinfo.cs);

    return RESULT_OK;
}

/// @brief RIO���Ǐo��
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
        data->incldata[ii].dig = m_rioinfo.data.incldata[ii].dig;   // ���̓f�[�^
        data->incldata[ii].cur = m_rioinfo.data.incldata[ii].cur;   // ���̓f�[�^�ϊ��l(mA)
        data->incldata[ii].deg = m_rioinfo.data.incldata[ii].deg;   // ���̓f�[�^�ϊ��l(deg.)
    }
    LeaveCriticalSection(&m_rioinfo.cs);

    return RESULT_OK;
}

/// @brief �����f�[�^������
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
        m_procinfo.data.sway[ii]    = data.sway[ii];    // �U��p
        m_procinfo.data.swaySpd[ii] = data.swaySpd[ii]; // �U��p���x
    }
    m_procinfo.data.exposureTime = data.exposureTime;   // �I������[us]
    m_procinfo.data.valid        = data.valid;          // ���o���
    m_procinfo.data.procTime     = data.procTime;       // ��������[ms]
    LeaveCriticalSection(&m_procinfo.cs);

    return RESULT_OK;
}

/// @brief �����f�[�^�Ǐo��
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
        data->sway[ii]    = m_procinfo.data.sway[ii];       // �U��p
        data->swaySpd[ii] = m_procinfo.data.swaySpd[ii];    // �U��p���x
    }
    data->exposureTime = m_procinfo.data.exposureTime;      // �I������[us]
    data->valid        = m_procinfo.data.valid;             // ���o���
    data->procTime     = m_procinfo.data.procTime;          // �揈������[ms]
    LeaveCriticalSection(&m_procinfo.cs);

    return RESULT_OK;
}
