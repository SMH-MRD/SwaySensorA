#pragma once
#include "framework.h"

// H, S, V
#define IMAGE_HSV_H_MIN                 0
#define IMAGE_HSV_H_MAX                 179
#define IMAGE_HSV_S_MIN                 0
#define IMAGE_HSV_S_MAX                 255
#define IMAGE_HSV_V_MIN                 0
#define IMAGE_HSV_V_MAX                 255

// RIO�G���[�r�b�g
#define RIO_ERR_NONE                    0x0000  // �G���[�Ȃ�
#define RIO_ERR_INIT_INCOMPLETE         0x0001  // RIO������������
#define RIO_ERR_SET_IOLINKMODE          0x0010  // IO LINK MODE�ݒ�G���[
#define RIO_ERR_SET_PARAM_VALID         0x0020  // �L�����ݒ�G���[
#define RIO_ERR_SET_PARAM_AI            0x0040  // AI�ݒ�G���[
#define RIO_ERR_GET_AI_READ             0x0100  // �f�[�^�ǂݍ��݃G���[����

// Process Data Error
#define S7CMPTBL_FORMAT_OVERRANGE       0x7FFF  // Overrange
#define S7CMPTBL_FORMAT_UNDERRANGE      0x8000  // Open circuit/short circuit/underrange
#define S7CMPTBL_FORMAT_SIGNBIT         0x8000  // Sign bit

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

//�X�Όv���̓f�[�^
enum
{
    RIO_PORT_1 = 0, // RIO���̓|�[�g1 
    RIO_PORT_2,     // RIO���̓|�[�g2 
    RIO_PORT_MAX
};

//�X�Όv/�J��������p�p�����[�^�@INDEX
enum
{
    PARAM_ID_IMG_GRAB_CAMERA = 0,   // �摜�捞��(�J����)
    PARAM_ID_IMG_GRAB_FILE,         // �摜�捞��(�t�@�C��)
    PARAM_ID_CAM_WIDTH,             // �J�����ݒ�(�L���v�`���T�C�Y����)
    PARAM_ID_CAM_HEIGHT,            // �J�����ݒ�(�L���v�`���T�C�Y����)
    PARAM_ID_CAM_FRAMERATE,         // �J�����ݒ�(�t���[�����[�g)
    PARAM_ID_CAM_EXPOSURE_TIME_MIN, // �J�����ݒ�(�I�����ԍŏ��l)
    PARAM_ID_CAM_EXPOSURE_TIME_MAX, // �J�����ݒ�(�I�����ԍő�l)
    PARAM_ID_CAM_EXPOSURE_TIME,     // �J�����ݒ�(�I������)
    PARAM_ID_IMG_ROI_ENABLE,        // �摜�����ݒ�(ROI�L��)
    PARAM_ID_IMG_ROI_SIZE,          // �摜�����ݒ�(ROI�T�C�Y)
    PARAM_ID_IMG_MASK_TYPE,         // �摜�����ݒ�(�}�X�N�摜�I��)
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
    MASK_IMG_ALL = 0,               // �}�X�N�摜�I��(����)
    MASK_IMG_IMAGE1 ,               // �}�X�N�摜�I��(�摜1�̂�)
    MASK_IMG_IMAGE2,                // �}�X�N�摜�I��(�摜2�̂�)
    MASK_IMG_MAX
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
    COG_ALGORITHM_ALL = 0,          // �d�S�ʒu�Z�o�A���S���Y��(�S�֊s�_)
    COG_ALGORITHM_AREA,             // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s�ʐ�)
    COG_ALGORITHM_LEN,              // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s��)
    COG_ALGORITHM_MAX
};

// �\���̒�`
typedef struct _stCameraInfo
{
    BOOL    valid;              // �J�������
    DOUBLE  cycleTime;          // �摜�捞�݊Ԋu[ms]
} stCameraInfo;

typedef struct _stImageData
{
    cv::Mat image;
    BOOL    update;             // image set��true get��false
} stImageData;

typedef struct _stRIOInfoData
{
    int16_t dig;
    double  cur;    // ���̓f�[�^�ϊ��l(mA)
    double  deg;    // ���̓f�[�^�ϊ��l(deg.)
} stRIOInfoData;

typedef struct _stRIOInfo   // RIO���
{
    int32_t         error;  // �G���[���
    stRIOInfoData   data[RIO_PORT_MAX];
} stRIOInfo;

typedef struct _stImageProcData
{
    double      posx;           // ���o�ʒuX
    double      posy;           // ���o�ʒuY
    int         roiSize;        // ROI�T�C�Y
    cv::Rect    roi;            // ROI
    double      expTime;        // �I������
    BOOL        valid;          // ���o���
} stImageProcData;

typedef struct _stProcInfo
{
    stImageProcData data[IMGPROC_ID_MAX];   // �摜��������
    DOUBLE          exposureTime;           // �I������[us]
    DOUBLE          procTime;               // ��������[ms]
} stProcInfo;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    void InitSharedObject(void);

    INT SetParam(UINT8 id, UINT32 data);
    INT GetParam(UINT8 id, UINT32* data);
    INT SetParam(UINT8 id, string str);
    INT GetParam(UINT8 id, string* str);

    INT SetCameraInfo(stCameraInfo info);
    INT GetCameraInfo(stCameraInfo* info);

    INT SetRIOInfo(stRIOInfo info);
    INT GetRIOInfo(stRIOInfo* info);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetProcInfo(stProcInfo info);
    INT GetProcInfo(stProcInfo* info);

private:
    UINT32              m_u32Param[PARAM_ID_MAX];
    string              m_strParam[PARAM_ID_STR_MAX];
    stCameraInfo        m_stCameraInfo;
    stRIOInfo           m_stRIOInfo;
    stImageData         m_stImgData[IMAGE_ID_MAX];
    stProcInfo          m_stProcInfo;

    CRITICAL_SECTION    csParam[PARAM_ID_MAX];
    CRITICAL_SECTION    csStrParam[PARAM_ID_STR_MAX];
    CRITICAL_SECTION    csCameraInfo;
    CRITICAL_SECTION    csRIOInfo;
    CRITICAL_SECTION    csImage[IMAGE_ID_MAX];
    CRITICAL_SECTION    csProcInfo;
};
