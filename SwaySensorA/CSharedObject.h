#pragma once
#include "framework.h"

// �J�����摜�z��@INDEX
enum
{
    IMAGE_ID_RAW_A = 0, // ���摜A
    IMAGE_ID_RAW_B,     // ���摜B
    IMAGE_ID_MASK1_A,   // �}�X�N�摜1A
    IMAGE_ID_MASK1_B,   // �}�X�N�摜1B
    IMAGE_ID_MASK2_A,   // �}�X�N�摜2A
    IMAGE_ID_MASK2_B,   // �}�X�N�摜2B
    IMAGE_ID_PROC_A,    // �����摜A
    IMAGE_ID_PROC_B,    // �����摜B
    IMAGE_ID_MAX
};

//�摜�����z��@INDEX
enum
{
    IMGPROC_ID_IMG_1,   // �摜1�����f�[�^
    IMGPROC_ID_IMG_2,   // �摜2�����f�[�^
    IMGPROC_ID_MAX
};

//�X�Όv���̓f�[�^�@INDEX
enum
{
    INCLINO_ID_PORT_1_ANALOG = 0,   // RIO PORT1���͒l(���l) 
    INCLINO_ID_PORT_2_ANALOG,       // RIO PORT2���͒l(���l)
    INCLINO_ID_PORT_1_MA,           // RIO PORT1���͒l(mA)
    INCLINO_ID_PORT_2_MA,           // RIO PORT2���͒l(mA)
    INCLINO_ID_PORT_1_RAD,			// RIO PORT1���͒l(�p�x radian)
    INCLINO_ID_PORT_2_RAD,			// RIO PORT2���͒l(�p�x radian)
    INCLINO_ID_MAX
};

//�X�Όv/�J��������p�p�����[�^�@INDEX
enum
{
    PARAM_ID_CAM_PROC = 0,          // �J��������
    PARAM_ID_CAM_EXPOSURE_TIME,     // �J�����ݒ�(�I������)
    PARAM_ID_CAM_FRAMERATE,         // �J�����ݒ�(�t���[�����[�g)
    PARAM_ID_CAM_WIDTH,             // �J�����ݒ�(�L���v�`���T�C�Y����)
    PARAM_ID_CAM_HEIGHT,            // �J�����ݒ�(�L���v�`���T�C�Y����)
    PARAM_ID_CAM_READ_FRAMERATE,    // �J�����Ǐo��(�t���[�����[�g)
    PARAM_ID_PIC_PROC_FLAG,         // �摜��͎��s�t���O
    PARAM_ID_PIC_MASK1_VALID,       // �摜�����ݒ�(�摜�}�X�N1�L������)
    PARAM_ID_PIC_MASK1_HLOW,        // �摜�����ݒ�(�摜�}�X�N1(H)����)
    PARAM_ID_PIC_MASK1_HUPP,        // �摜�����ݒ�(�摜�}�X�N1(H)���)
    PARAM_ID_PIC_MASK1_SLOW,        // �摜�����ݒ�(�摜�}�X�N1(S)����)
    PARAM_ID_PIC_MASK1_SUPP,        // �摜�����ݒ�(�摜�}�X�N1(S)���)
    PARAM_ID_PIC_MASK1_VLOW,        // �摜�����ݒ�(�摜�}�X�N1(V)����)
    PARAM_ID_PIC_MASK1_VUPP,        // �摜�����ݒ�(�摜�}�X�N1(V)���)
    PARAM_ID_PIC_MASK2_VALID,       // �摜�����ݒ�(�摜�}�X�N2�L������)
    PARAM_ID_PIC_MASK2_HLOW,        // �摜�����ݒ�(�摜�}�X�N2(H)����)
    PARAM_ID_PIC_MASK2_HUPP,        // �摜�����ݒ�(�摜�}�X�N2(H)���)
    PARAM_ID_PIC_MASK2_SLOW,        // �摜�����ݒ�(�摜�}�X�N2(S)����)
    PARAM_ID_PIC_MASK2_SUPP,        // �摜�����ݒ�(�摜�}�X�N2(S)���)
    PARAM_ID_PIC_MASK2_VLOW,        // �摜�����ݒ�(�摜�}�X�N2(V)����)
    PARAM_ID_PIC_MASK2_VUPP,        // �摜�����ݒ�(�摜�}�X�N2(V)���)
    PARAM_ID_PIC_NOISEFILTER,       // �摜�����ݒ�(�m�C�Y�t�B���^)
    PARAM_ID_PIC_NOISEFILTERVAL,    // �摜�����ݒ�(�m�C�Y�t�B���^)
    PARAM_ID_PIC_ALGORITHM,         // �摜�����ݒ�(�d�S�ʒu�Z�o�A���S���Y��)
    PARAM_ID_RIO_TCPPORT,           // RIO TCP�|�[�g�ԍ�
    PARAM_ID_RIO_SLAVEADDR,         // RIO�X���[�u�A�h���X
    PARAM_ID_RIO_TIMEOUT,           // RIO�^�C���A�E�g
    PARAM_ID_RIO_XPORT,             // RIO�X�ΌvX���f�[�^�ڑ��|�[�g�ԍ�
    PARAM_ID_RIO_YPORT,             // RIO�X�ΌvY���f�[�^�ڑ��|�[�g�ԍ�
    PARAM_ID_MAX
};

enum
{
    PARAM_ID_STR_PROC_FILENAME = 0, // ��͑Ώۉ摜�t�@�C����
    PARAM_ID_STR_RIO_IPADDR,        // RIO IP�A�h���X
    PARAM_ID_STR_MAX
};

enum
{
    PARAM_ID_DOUBLE_PROC_TIME = 0,  // �揈������
    PARAM_ID_DOUBLE_MAX
};

enum
{
    NOISEFILTER_NONE = 0,           // �m�C�Y�t�B���^:�Ȃ�
    NOISEFILTER_MEDIAN,             // �m�C�Y�t�B���^:�����l�t�B���^
    NOISEFILTER_OPENNING,           // �m�C�Y�t�B���^:�I�[�v�j���O����
    NOIZEFILTER
};

enum
{
    COG_ALGORITHM_ALL = 1,          // �d�S�ʒu�Z�o�A���S���Y��(�S�֊s�_)
    COG_ALGORITHM_LEN,              // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s��)
    COG_ALGORITHM
};

// �\���̒�`
typedef struct _stImageData
{
    cv::Mat image;
    BOOL    update; // image set��true get��false
} stImageData;

typedef struct _stMngProcData
{
    double  posx;
    double  posy;
    BOOL    enable;
} stMngProcData;

typedef struct _stMngInclinoData
{
    DOUBLE data;
} stMngInclinoData;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    void InitSharedObject(void);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetProcData(UINT8 id, stMngProcData data);
    INT GetProcData(UINT8 id, stMngProcData* data);

    INT SetInclinoData(UINT8 id, DOUBLE data);
    INT GetInclinoData(UINT8 id, DOUBLE* data);

    INT SetParam(UINT8 id, UINT32 data);
    INT GetParam(UINT8 id, UINT32* data);
    INT SetParam(UINT8 id, string str);
    INT GetParam(UINT8 id, string* str);
    INT SetParam(UINT8 id, DOUBLE data);
    INT GetParam(UINT8 id, DOUBLE* data);

    stImageData         m_stImage[IMAGE_ID_MAX];
    stMngProcData       m_stProcData[IMGPROC_ID_MAX];
    stMngInclinoData    m_stInclinoData[INCLINO_ID_MAX];
    UINT32              m_u32Param[PARAM_ID_MAX];
    string              m_strParam[PARAM_ID_STR_MAX];
    DOUBLE              m_dParam[PARAM_ID_DOUBLE_MAX];

    CRITICAL_SECTION csImage[IMAGE_ID_MAX];
    CRITICAL_SECTION csProcData[IMGPROC_ID_MAX];
    CRITICAL_SECTION csInclino[INCLINO_ID_MAX];
    CRITICAL_SECTION csParam[PARAM_ID_MAX];
    CRITICAL_SECTION csStrParam[PARAM_ID_STR_MAX];
    CRITICAL_SECTION csDoubleParam[PARAM_ID_DOUBLE_MAX];
};
