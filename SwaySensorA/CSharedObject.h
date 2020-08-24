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

// �O������
#define EXTN_ROPELEN_MIN                1000.0      // ���[�v��(�ŏ�)
#define EXTN_ROPELEN_MAX                50000.0     // ���[�v��(�ő�)

#define EXTN_CAMBOXANGL_MIN             -45.0       // �J����BOX�X��(�ŏ�)
#define EXTN_CAMBOXANGL_MAX             45.0        // �J����BOX�X��(�ő�)

// �J�����摜�z��@INDEX
enum
{
    IMAGE_ID_RAW_A = 0,                         // ���摜A
    IMAGE_ID_RAW_B,                             // ���摜B
    IMAGE_ID_MASK1_A,                           // �}�X�N�摜1A
    IMAGE_ID_MASK1_B,                           // �}�X�N�摜1B
    IMAGE_ID_MASK2_A,                           // �}�X�N�摜2A
    IMAGE_ID_MASK2_B,                           // �}�X�N�摜2B
    IMAGE_ID_PROC_A,                            // �����摜A
    IMAGE_ID_PROC_B,                            // �����摜B
    IMAGE_ID_MAX
};

// �摜�����z��@INDEX
enum
{
    IMGPROC_ID_IMG_1 = 0,                       // �摜1�����f�[�^
    IMGPROC_ID_IMG_2,                           // �摜2�����f�[�^
    IMGPROC_ID_MAX
};

// �X�Όv���̓f�[�^
enum
{
    RIO_PORT_1 = 0,                             // RIO���̓|�[�g1 
    RIO_PORT_2,                                 // RIO���̓|�[�g2 
    RIO_PORT_MAX
};

// �J��������p�p�����[�^
enum
{
    GRAB_IMG_STOP = 0,                          // �摜�捞��(��~)
    GRAB_IMG_GRAB_CAMERA,                       // �摜�捞��(�J����)
    GRAB_IMG_GRAB_FILE                          // �摜�捞��(�t�@�C��)
};

enum
{
    MASK_IMG_ALL = 0,                           // �}�X�N�摜�I��(����)
    MASK_IMG_IMAGE1 ,                           // �}�X�N�摜�I��(�摜1�̂�)
    MASK_IMG_IMAGE2,                            // �}�X�N�摜�I��(�摜2�̂�)
};

enum
{
    NOISEFILTER1_NONE = 0,                      // �m�C�Y�t�B���^:�Ȃ�
    NOISEFILTER1_MEDIAN,                        // �m�C�Y�t�B���^:�����l�t�B���^
    NOISEFILTER1_OPENNING,                      // �m�C�Y�t�B���^:�I�[�v�j���O����
    NOIZEFILTER1_MAX
};

enum
{
    NOISEFILTER2_NONE = 0,                      // �m�C�Y�t�B���^:�Ȃ�
    NOISEFILTER2_CLOSING,                       // �m�C�Y�t�B���^:�N���[�W���O����
    NOIZEFILTER2_MAX
};

enum
{
    COG_ALGORITHM_ALL = 0,                      // �d�S�ʒu�Z�o�A���S���Y��(�S�֊s�_)
    COG_ALGORITHM_AREA,                         // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s�ʐ�)
    COG_ALGORITHM_LEN,                          // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s��)
    COG_ALGORITHM_MAX
};

enum
{
    AXIS_X = 0,                                 // X��
    AXIS_Y,                                     // Y��
    AXIS_MAX
};

// �\���̒�`
// ���ʐݒ�
typedef struct _stConfigParamData               // �\����`�f�[�^
{
    double camoffsetLX0;                        // �݋�ݓ_�`BOX�ݓ_����LX0[mm]
    double camoffsetLY0;                        // �݋�ݓ_�`BOX�ݓ_����LY0[mm]
    double camoffsetL0;                         // BOX�ݓ_�`BOX�������S����L0[mm]
    double camoffsetLC;                         // BOX�������S�`�J�������S����LC[mm]
    double camoffsetA0;                         // BOX���J������t�p�x��0[deg]
    double camoffsetAC;                         // BOX�������S�`�J�������S�p�x��c[deg]
    double camviewangl;                         // �J��������p[deg]
} stConfigParamData;
typedef struct _stCommonParamData               // ���ʐݒ�f�[�^
{
    stConfigParamData   cnfg[AXIS_MAX];         // �\����`�f�[�^
    double              filter;                 // �t�B���^���萔
    string              imgsavefname;           // �摜�ۑ��t�@�C����
} stCommonParamData;
typedef struct _stCommonParam                   // ���ʐݒ�
{
    CRITICAL_SECTION    cs;
    stCommonParamData   data;                   // ���ʐݒ�f�[�^
} stCommonParam;

// �J�����ݒ�
typedef struct _stCameraParamData               // �J�����ݒ�f�[�^
{
    int     imgsource;                          // �摜�捞�݌�(0:��~ 1:�J���� 2:�摜�t�@�C��) 
    string  imgfname;                           // �捞�݉摜�t�@�C����

    int     size[AXIS_MAX];                     // �J�����摜�T�C�Y(X(32�̔{��, 2592�ȉ�), Y(2�̔{��, 2048�ȉ�))
    double  fps;                                // �t���[�����[�g
    double  exptime;                            // �I������(usec)(�����l)
    double  exptimemin;                         // �I������(usec)(�ŏ��l)
    double  exptimemax;                         // �I������(usec)(�ő�l)
} stCameraParamData;
typedef struct _stCameraParam                   // �J�����ݒ�
{
    CRITICAL_SECTION    cs;
    stCameraParamData   data;                   // �J�����ݒ�f�[�^
} stCameraParam;

// �摜�����ݒ�
typedef struct _stROIParam                      // ROI�ݒ�
{
    int     valid;                              // ROI�L��
    double  scale;                              // ROI�X�P�[��(���o�����^�[�Q�b�g�ɑ΂���{��)
} stROIParam;
typedef struct _stHSVParam                      // HSV�}�X�N����l
{
    int h;                                      // H
    int s;                                      // S
    int v;                                      // V
} stHSVParam;
typedef struct _stFilterParam                   // �m�C�Y�t�B���^
{
    int type;                                   // �t�B���^�L��
    int val;                                    // �t�B���^�l
} stFilterParam;
typedef struct _stImgProcParamData              // �摜�����ݒ�f�[�^
{
    stROIParam      roi;                        // ROI�ݒ�
    int             maskvalid[IMGPROC_ID_MAX];  // �}�X�N�摜�I��(0=����, 1=�摜1�̂�, 2=�摜2�̂�)
    stHSVParam      hsvl[IMGPROC_ID_MAX];       // HSV�}�X�N����l(����)
    stHSVParam      hsvu[IMGPROC_ID_MAX];       // HSV�}�X�N����l(���)
    stFilterParam   filter1;                    // �S�}���m�C�Y�t�B���^(0=�Ȃ�, 1=�����l�t�B���^, 2=�I�[�v�j���O����), �t�B���^�l(�����l�t�B���^=1,3,5,... �I�[�v�j���O����=1,2,...)
    stFilterParam   filter2;                    // �����߃m�C�Y�t�B���^(0=�Ȃ�, 1=�N���[�W���O����), �t�B���^�l(1,2,...)
    int             algorithm;                  // �^�[�Q�b�g���o�A���S���Y��(0=�S�֊s�_, 1=�ő�֊s�ʐ�, 2=�ő�֊s��)
} stImgProcParamData;
typedef struct _stImgProcParam                  // �摜�����ݒ�
{
    CRITICAL_SECTION    cs;
    stImgProcParamData  data;                   // �摜�����ݒ�f�[�^
} stImgProcParam;

// RemoteIO�ݒ�
typedef struct _stRIOParamData                  // RemoteIO�ݒ�f�[�^
{
    string  ipaddrs;                            // RIO IP�A�h���X
    int     tcpport;                            // TCP�|�[�g�ԍ�
    int     slaveaddrs;                         // �X���[�u�A�h���X
    int     timeout;                            // �ʐM�^�C���A�E�g(msec)
    int     portx;                              // �X�ΌvX�f�[�^�ڑ��|�[�g�ԍ�(1�`8)
    int     porty;                              // �X�ΌvY�f�[�^�ڑ��|�[�g�ԍ�(1�`8)
} stRIOParamData;
typedef struct _stRIOParam                      // RemoteIO�ݒ�
{
    CRITICAL_SECTION    cs;
    stRIOParamData      data;                   // RemoteIO�ݒ�f�[�^
} stRIOParam;

// �摜���
typedef struct _stImageData
{
    cv::Mat image;
    BOOL    update;                             // image set��true get��false
} stImageData;
typedef struct _stImageInfo
{
    CRITICAL_SECTION    cs;
    stImageData         data;
} stImageInfo;

// �J�������
typedef struct _stCameraInfoData
{
    BOOL    valid;                              // �J�������
    DOUBLE  cycleTime;                          // �摜�捞�݊Ԋu[ms]
} stCameraInfoData;
typedef struct _stCameraInfo
{
    CRITICAL_SECTION    cs;
    stCameraInfoData    data;
} stCameraInfo;

// RIO���
typedef struct _stInclinometerData              // �X�Όv�f�[�^
{
    int16_t dig;
    double  cur;                                // ���̓f�[�^�ϊ��l(mA)
    double  deg;                                // ���̓f�[�^�ϊ��l(deg.)
} stInclinometerData;
typedef struct _stRIOInfoData                   // RIO�f�[�^
{
    int32_t             error;                  // �G���[���
    stInclinometerData  incldata[AXIS_MAX];     // �X�Όv�f�[�^
} stRIOInfoData;
typedef struct _stRIOInfo
{
    CRITICAL_SECTION    cs;
    stRIOInfoData       data;
} stRIOInfo;

// ���o���
typedef struct _stImageProcData
{
    double      posx;                           // ���o�ʒuX
    double      posy;                           // ���o�ʒuY
    int         tgtsize;                        // ���o�T�C�Y
    cv::Rect    roi;                            // ROI
    BOOL        valid;                          // ���o���
} stImageProcData;
typedef struct _stSwayData                      // �U�ꌟ�o�f�[�^
{
    double  pos;                                // �U��ʒu
    double  deg;                                // �U��p[deg]
    double  rad;                                // �U��p[rad]
    double  spd;                                // �U��p���x
} stSwayData;
typedef struct _stProcInfoData
{
    stImageProcData imgprocdata[IMGPROC_ID_MAX];    // �摜��������
    stSwayData      swaydata[AXIS_MAX];             // �U�ꌟ�o�f�[�^ 
    BOOL            valid;                          // ���o���
    double          exposureTime;                   // �I������[us](@@@�摜��]�����ăR���g���[������悤�ɂ���)
    double          procTime;                       // ��������[ms]
} stProcInfoData;
typedef struct _stProcInfo
{
    CRITICAL_SECTION    cs;
    stProcInfoData      data;
} stProcInfo;

// �O������
typedef struct _stExtnInfoData                      // �O�����̓f�[�^
{
    double  ropelen;                                // ���[�v��
    double  boxangle[AXIS_MAX];                     // BOX�X��
} stExtnInfoData;
typedef struct _stExtnInfo
{
    CRITICAL_SECTION    cs;
    stExtnInfoData      data;                       // �O�����̓f�[�^
} stExtnInfo;

///
class CSharedObject
{
public:
    CSharedObject();
    ~CSharedObject();

    INT SetParam(stCommonParamData data);
    INT GetParam(stCommonParamData* data);
    INT SetParam(stCameraParamData data);
    INT GetParam(stCameraParamData* data);
    INT SetParam(stImgProcParamData data);
    INT GetParam(stImgProcParamData* data);
    INT SetParam(stRIOParamData data);
    INT GetParam(stRIOParamData* data);

    INT SetImage(UINT8 id, Mat image);
    INT GetImage(UINT8 id, Mat* image);

    INT SetInfo(stCameraInfoData data);
    INT GetInfo(stCameraInfoData* data);
    INT SetInfo(stRIOInfoData data);
    INT GetInfo(stRIOInfoData* data);
    INT SetInfo(stProcInfoData data);
    INT GetInfo(stProcInfoData* data);
    INT SetInfo(stExtnInfoData data);
    INT GetInfo(stExtnInfoData* data);

private:
    stCommonParam       m_cmmnparam;                // ���ʐݒ�
    stCameraParam       m_camparam;                 // �J�����ݒ�
    stImgProcParam      m_imgprocparam;             // �摜�����ݒ�
    stRIOParam          m_rioparam;                 // RemoteIO�ݒ�

    stImageInfo         m_imginfo[IMAGE_ID_MAX];
    stCameraInfo        m_caminfo;
    stRIOInfo           m_rioinfo;
    stProcInfo          m_procinfo;
    stExtnInfo          m_extninfo;

private:
    void Initialize(void);
};
