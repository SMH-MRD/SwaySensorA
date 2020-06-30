#pragma once
#include "framework.h"

// �J�������H�ޗ��摜�z��@INDEX
enum
{
    IMAGE_ID_CAM_A = 0, // �J�����摜A
    IMAGE_ID_CAM_B,     // �J�����摜B
    IMAGE_ID_MASK1_A,   // �}�X�N�摜1A
    IMAGE_ID_MASK1_B,   // �}�X�N�摜1B
    IMAGE_ID_MASK2_A,   // �}�X�N�摜2A
    IMAGE_ID_MASK2_B,   // �}�X�N�摜2B
    IMAGE_ID_CAM_MAX
};

//�J�����o�͉摜�z��@INDEX
enum
{
    IMAGE_ID_PROC1_A = 0,   // ���H��摜1A
    IMAGE_ID_PROC1_B,       // ���H��摜1B
    IMAGE_ID_PROC2_A,       // ���H��摜2A
    IMAGE_ID_PROC2_B,       // ���H��摜2B
    IMAGE_ID_PROC_MAX
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
typedef struct _stMngImageData
{
    Mat     image;
    BOOL    update; // image set��true get��false
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
    BOOL        update; // data set��true get��false
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
