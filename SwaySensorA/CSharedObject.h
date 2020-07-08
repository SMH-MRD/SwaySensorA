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
    PARAM_ID_IMG_GRAB_CAMERA = 0,   // �摜�捞��(�J����)
    PARAM_ID_IMG_GRAB_FILE,         // �摜�捞��(�t�@�C��)
    PARAM_ID_CAM_EXPOSURE_TIME,     // �J�����ݒ�(�I������)
    PARAM_ID_CAM_FRAMERATE,         // �J�����ݒ�(�t���[�����[�g)
    PARAM_ID_CAM_WIDTH,             // �J�����ݒ�(�L���v�`���T�C�Y����)
    PARAM_ID_CAM_HEIGHT,            // �J�����ݒ�(�L���v�`���T�C�Y����)
    PARAM_ID_CAM_READ_FRAMERATE,    // �J�����Ǐo��(�t���[�����[�g)
    PARAM_ID_IMG_ROI_ENABLE,        // �摜�����ݒ�(ROI�L��)
    PARAM_ID_IMG_ROI_SIZE,          // �摜�����ݒ�(ROI�T�C�Y)
    PARAM_ID_IMG_MASK1_HLOW,        // �摜�����ݒ�(�摜�}�X�N1(H)����)
    PARAM_ID_IMG_MASK1_HUPP,        // �摜�����ݒ�(�摜�}�X�N1(H)���)
    PARAM_ID_IMG_MASK1_SLOW,        // �摜�����ݒ�(�摜�}�X�N1(S)����)
    PARAM_ID_IMG_MASK1_SUPP,        // �摜�����ݒ�(�摜�}�X�N1(S)���)
    PARAM_ID_IMG_MASK1_VLOW,        // �摜�����ݒ�(�摜�}�X�N1(V)����)
    PARAM_ID_IMG_MASK1_VUPP,        // �摜�����ݒ�(�摜�}�X�N1(V)���)
    PARAM_ID_IMG_MASK2_HLOW,        // �摜�����ݒ�(�摜�}�X�N2(H)����)
    PARAM_ID_IMG_MASK2_HUPP,        // �摜�����ݒ�(�摜�}�X�N2(H)���)
    PARAM_ID_IMG_MASK2_SLOW,        // �摜�����ݒ�(�摜�}�X�N2(S)����)
    PARAM_ID_IMG_MASK2_SUPP,        // �摜�����ݒ�(�摜�}�X�N2(S)���)
    PARAM_ID_IMG_MASK2_VLOW,        // �摜�����ݒ�(�摜�}�X�N2(V)����)
    PARAM_ID_IMG_MASK2_VUPP,        // �摜�����ݒ�(�摜�}�X�N2(V)���)
    PARAM_ID_IMG_NOISEFILTER1,      // �摜�����ݒ�(�S�}���m�C�Y�t�B���^)
    PARAM_ID_IMG_NOISEFILTERVAL1,   // �摜�����ݒ�(�S�}���m�C�Y�t�B���^�l)
    PARAM_ID_IMG_NOISEFILTER2,      // �摜�����ݒ�(�����߃m�C�Y�t�B���^)
    PARAM_ID_IMG_NOISEFILTERVAL2,   // �摜�����ݒ�(�����߃m�C�Y�t�B���^�l)
    PARAM_ID_IMG_ALGORITHM,         // �摜�����ݒ�(�d�S�ʒu�Z�o�A���S���Y��)
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
    PARAM_ID_DOUBLE_IMG_GRAB_TIME = 0,  // �摜�捞�ݎ���
    PARAM_ID_DOUBLE_PROC_TIME,          // �揈������
    PARAM_ID_DOUBLE_MAX
};

enum
{
    NOISEFILTER1_NONE = 0,           // �m�C�Y�t�B���^:�Ȃ�
    NOISEFILTER1_MEDIAN,             // �m�C�Y�t�B���^:�����l�t�B���^
    NOISEFILTER1_OPENNING,           // �m�C�Y�t�B���^:�I�[�v�j���O����
    NOIZEFILTER1_MAX
};

enum
{
    NOISEFILTER2_NONE = 0,           // �m�C�Y�t�B���^:�Ȃ�
    NOISEFILTER2_CLOSING,            // �m�C�Y�t�B���^:�N���[�W���O����
    NOIZEFILTER2_MAX
};

enum
{
    COG_ALGORITHM_ALL = 1,          // �d�S�ʒu�Z�o�A���S���Y��(�S�֊s�_)
    COG_ALGORITHM_AREA,             // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s�ʐ�)
    COG_ALGORITHM_LEN,              // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s��)
    COG_ALGORITHM_MAX
};

// �\���̒�`
typedef struct _stImageData
{
    cv::Mat image;
    BOOL    update; // image set��true get��false
} stImageData;

typedef struct _stImageProcData
{
    double      posx;
    double      posy;
    cv::Rect    roi;
    int         roisize;
    BOOL        enable;
} stImageProcData;

typedef struct _stInclinationData
{
    DOUBLE data;
} stInclinationData;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    void InitSharedObject(void);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetProcData(UINT8 id, stImageProcData data);
    INT GetProcData(UINT8 id, stImageProcData* data);

    INT SetInclinoData(UINT8 id, DOUBLE data);
    INT GetInclinoData(UINT8 id, DOUBLE* data);

    INT SetParam(UINT8 id, UINT32 data);
    INT GetParam(UINT8 id, UINT32* data);
    INT SetParam(UINT8 id, string str);
    INT GetParam(UINT8 id, string* str);
    INT SetParam(UINT8 id, DOUBLE data);
    INT GetParam(UINT8 id, DOUBLE* data);

    stImageData         m_stImgData[IMAGE_ID_MAX];
    stImageProcData     m_stImgProcData[IMGPROC_ID_MAX];
    stInclinationData   m_stIncData[INCLINO_ID_MAX];
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
