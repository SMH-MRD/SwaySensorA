#pragma once
#include <windows.h>

// ------------------�^�X�N�ݒ� ---------
#define OBJ_NAME_SECT_OF_INIFILE    L"OBJECT_NAME"      // �Z�N�V�������F�I�u�W�F�N�g��
#define OBJ_SNAME_SECT_OF_INIFILE   L"OBJECT_SNAME"     // �Z�N�V�������F�I�u�W�F�N�g����
#define OBJ_CYCLE_SECT_OF_INIFILE   L"OBJECT_CYCLE"     // �Z�N�V�������F�I�u�W�F�N�g�X���b�h����
#define MANAGER_KEY_OF_INIFILE      L"MANAGER"
#define CLIENT_COM_KEY_OF_INIFILE   L"CCOM"
#define CLERK_KEY_OF_INIFILE        L"CLERK"
#define RIO_COM_KEY_OF_INIFILE      L"DCOM1"
#define CAMERA_COM_KEY_OF_INIFILE   L"DCOM2"
#define ANALYST_KEY_OF_INIFILE      L"ANALYST"
#define PLAYER_KEY_OF_INIFILE       L"PLAYER"
#define PR_KEY_OF_INIFILE           L"PR"

// ------------------ �J�����ݒ� ---------------------------
#define	INI_SCT_CAMERA              L"CAMERA"           // �Z�N�V�������F�J����
#define	INI_KEY_CAM_WIDTH           L"Width"            // �L�[���F�J�����B�e����
#define	INI_KEY_CAM_HEIGHT          L"Height"           // �L�[���F�J�����B�e����
#define	INI_KEY_CAM_FRAMERATE       L"FrameRate"        // �L�[���F�t���[�����[�g
#define	INI_KEY_CAM_EXPOSURE        L"ExposureTime"     // �L�[���F�I������

// ------------------ �摜�����ݒ� ---------------------------
#define	INI_SCT_OPENCV              L"OPENCV"           // �Z�N�V�������F�摜����
#define	INI_KEY_IMG_ROI             L"Roi"              // �L�[���FROI
#define	INI_KEY_IMG_MASK_IMAGE      L"MaskImage"        // �L�[���F�}�X�N�摜�I��
#define	INI_KEY_IMG_MASK1_LOW       L"Mask1Low"         // �L�[���F�摜1�}�X�N����(H,S,V)
#define	INI_KEY_IMG_MASK1_UPP       L"Mask1Upp"         // �L�[���F�摜1�}�X�N���(H,S,V)
#define	INI_KEY_IMG_MASK2_LOW       L"Mask2Low"         // �L�[���F�摜2�}�X�N����(H,S,V)
#define	INI_KEY_IMG_MASK2_UPP       L"Mask2Upp"         // �L�[���F�摜2�}�X�N���(H,S,V)
#define	INI_KEY_IMG_NOISEFILTER1    L"NoiseFilter1"     // �L�[���F�S�}���m�C�Y�t�B���^
#define	INI_KEY_IMG_NOISEFILTER2    L"NoiseFilter2"     // �L�[���F�����߃m�C�Y�t�B���^
#define	INI_KEY_IMG_ALGORITHM       L"Algorithm"        // �L�[���F�^�[�Q�b�g���o�A���S���Y��

// ------------------ RIO�ݒ� ---------------------------
#define	INI_SCT_RIO                 L"RIO"              // �Z�N�V�������FRIO
#define INI_KEY_RIO_IPADDR          L"IpAddr"           // �L�[��:RIO IP�A�h���X
#define INI_KEY_RIO_TCPPORTNUM      L"TcpPort"          // �L�[��:RIO TCP�|�[�g�ԍ�
#define INI_KEY_RIO_SLAVEADDR       L"SlaveAddr"        // �L�[��:RIO�X���[�u�A�h���X
#define INI_KEY_RIO_TIMEOUT         L"TimeOut"          // �L�[��:RIO�^�C���A�E�g
#define INI_KEY_RIO_XPORTNUM        L"XPortNum"         // �L�[��:RIO�X�ΌvX�f�[�^�ڑ��|�[�g�ԍ�
#define INI_KEY_RIO_YPORTNUM        L"YPortNum"         // �L�[��:RIO�X�ΌvY�f�[�^�ڑ��|�[�g�ԍ�

// ------------------ Ini�t�@�C�����̍\���� ---------------------------
typedef struct _stIniInf    //***** Ini�t�@�C����� *****
{
    INT     camWidth;           // �J�����擾����
    INT     camHeight;          // �J�����擾����
    INT     frameRate;          // �t���[�����[�g
    INT     expTimeMin;         // �I������(�ŏ��l)
    INT     expTimeMax;         // �I������(�ő�l)
    INT     expTime;            // �I������

    INT     roiEnable;          // ROI�L��
    INT     roiSize;            // ROI�T�C�Y
    INT     maskImage;          // �}�X�N�摜�I��
    INT     mask1HLow;          // �摜�}�X�N1(H)����
    INT     mask1HUpp;          // �摜�}�X�N1(H)���
    INT     mask1SLow;          // �摜�}�X�N1(S)����
    INT     mask1SUpp;          // �摜�}�X�N1(S)���
    INT     mask1VLow;          // �摜�}�X�N1(V)����
    INT     mask1VUpp;          // �摜�}�X�N1(V)���
    INT     mask2HLow;          // �摜�}�X�N2(H)����
    INT     mask2HUpp;          // �摜�}�X�N2(H)���
    INT     mask2SLow;          // �摜�}�X�N2(S)����
    INT     mask2SUpp;          // �摜�}�X�N2(S)���
    INT     mask2VLow;          // �摜�}�X�N2(V)����
    INT     mask2VUpp;          // �摜�}�X�N2(V)���
    INT     noiseFilter1;       // �S�}���m�C�Y�t�B���^
    INT     noiseFilterVal1;    // �S�}���m�C�Y�t�B���^�l
    INT     noiseFilter2;       // �����߃m�C�Y�t�B���^
    INT     noiseFilterVal2;    // �����߃m�C�Y�t�B���^�l
    INT     algorithm;          // �^�[�Q�b�g���o�A���S���Y��

    TCHAR   rioIpAddr[256];     // RIO IP�A�h���X
    INT     rioTcpPortNum;      // RIO TCP�|�[�g�ԍ�
    INT     rioSlaveAddr;       // RIO�X���[�u�A�h���X
    INT     rioTimeOut;         // RIO�^�C���A�E�g
    INT     rioXPortNum;        // RIO�X�ΌvX���f�[�^�ڑ��|�[�g�ԍ�
    INT     rioYPortNum;        // RIO�X�ΌvY���f�[�^�ڑ��|�[�g�ԍ�
} ST_INI_INF;
