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

// ------------------ ���ʐݒ� ---------------------------
#define	INI_SCT_COMMON              L"COMMON"               // �Z�N�V�������F���ʐݒ�
#define INI_KEY_CMMN_CAMOFFSET_LX0  L"CameraOffsetLX0"      // �L�[���F�݋�ݓ_�`BOX�ݓ_����LX0[mm]
#define INI_KEY_CMMN_CAMOFFSET_LY0  L"CameraOffsetLY0"      // �L�[���F�݋�ݓ_�`BOX�ݓ_����LY0[mm]
#define INI_KEY_CMMN_CAMOFFSET_L0   L"CameraOffsetL0"       // �L�[���FBOX�ݓ_�`BOX�������S����L0[mm]
#define INI_KEY_CMMN_CAMOFFSET_LC   L"CameraOffsetLC"       // �L�[���FBOX�������S�`�J�������S����LC[mm]
#define INI_KEY_CMMN_CAMOFFSET_A0   L"CameraOffsetA0"       // �L�[���FBOX���J������t�p�x��0[deg]
#define INI_KEY_CMMN_CAMOFFSET_AC   L"CameraOffsetAC"       // �L�[���FBOX�������S�`�J�������S�p�x��c[deg]
#define INI_KEY_CMMN_CAMVIEWANGLE   L"CameraViewAngle"      // �L�[���F�J��������p[deg]
#define INI_KEY_CMMN_FILTER         L"Filter"               // �L�[���F�t�B���^���萔
#define INI_KEY_CMMN_IMGSAVEFNAME   L"ImageSaveFileName"    // �L�[���F�摜�ۑ��t�@�C����

// ------------------ �J�����ݒ� ---------------------------
#define	INI_SCT_CAMERA              L"CAMERA"               // �Z�N�V�������F�J�����ݒ�
#define	INI_KEY_CAM_IMAGESIZE       L"ImageSize"            // �L�[���F�J�����B�e�T�C�Y
#define	INI_KEY_CAM_FRAMERATE       L"FrameRate"            // �L�[���F�t���[�����[�g
#define	INI_KEY_CAM_EXPOSURE        L"ExposureTime"         // �L�[���F�I������

// ------------------ �摜�����ݒ� ---------------------------
#define	INI_SCT_OPENCV              L"OPENCV"               // �Z�N�V�������F�摜�����ݒ�
#define	INI_KEY_IMG_ROI             L"Roi"                  // �L�[���FROI
#define	INI_KEY_IMG_MASK_IMAGE      L"MaskImage"            // �L�[���F�}�X�N�摜�I��
#define	INI_KEY_IMG_MASK1_LOW       L"Mask1Low"             // �L�[���F�摜1�}�X�N����(H,S,V)
#define	INI_KEY_IMG_MASK1_UPP       L"Mask1Upp"             // �L�[���F�摜1�}�X�N���(H,S,V)
#define	INI_KEY_IMG_MASK2_LOW       L"Mask2Low"             // �L�[���F�摜2�}�X�N����(H,S,V)
#define	INI_KEY_IMG_MASK2_UPP       L"Mask2Upp"             // �L�[���F�摜2�}�X�N���(H,S,V)
#define	INI_KEY_IMG_NOISEFILTER1    L"NoiseFilter1"         // �L�[���F�S�}���m�C�Y�t�B���^
#define	INI_KEY_IMG_NOISEFILTER2    L"NoiseFilter2"         // �L�[���F�����߃m�C�Y�t�B���^
#define	INI_KEY_IMG_ALGORITHM       L"Algorithm"            // �L�[���F�^�[�Q�b�g���o�A���S���Y��

// ------------------ RIO�ݒ� ---------------------------
#define	INI_SCT_RIO                 L"RIO"              // �Z�N�V�������FRIO�ݒ�
#define INI_KEY_RIO_IPADDR          L"IpAddr"           // �L�[��:RIO IP�A�h���X
#define INI_KEY_RIO_TCPPORTNUM      L"TcpPort"          // �L�[��:RIO TCP�|�[�g�ԍ�
#define INI_KEY_RIO_SLAVEADDR       L"SlaveAddr"        // �L�[��:RIO�X���[�u�A�h���X
#define INI_KEY_RIO_TIMEOUT         L"TimeOut"          // �L�[��:RIO�^�C���A�E�g
#define INI_KEY_RIO_XPORTNUM        L"XPortNum"         // �L�[��:RIO�X�ΌvX�f�[�^�ڑ��|�[�g�ԍ�
#define INI_KEY_RIO_YPORTNUM        L"YPortNum"         // �L�[��:RIO�X�ΌvY�f�[�^�ڑ��|�[�g�ԍ�

// ------------------ Ini�t�@�C�����̍\���� ---------------------------
typedef struct _stIniInf    //***** Ini�t�@�C����� *****
{
    stCommonParamData   m_cmmnparam;                // ���ʐݒ�
    stCameraParamData   m_camparam;                 // �J�����ݒ�f�[�^
    stImgProcParamData  m_imgprocparam;             // �摜�����ݒ�f�[�^
    stRIOParamData      m_rioparam;                 // RemoteIO�ݒ�f�[�^
} ST_INI_INF;
