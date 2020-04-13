#pragma once
#include <windows.h>

// ------------------ �p�����[�^Ini�̃Z�N�V������ ---------------------------
#define OBJ_NAME_SECT_OF_INIFILE	L"OBJECT_NAME"						//�I�u�W�F�N�g�t���l�[���Z�N�V����
#define OBJ_SNAME_SECT_OF_INIFILE	L"OBJECT_SNAME"						//�I�u�W�F�N�g���̃Z�N�V����

#define	INI_SCT_CAMERA				L"CAMERA"							// �Z�N�V�������F�J����
#define	INI_SCT_OPENCV				L"OPENCV"							// �Z�N�V�������FOPENCV
#define	INI_SCT_RIO					L"RIO"								// �Z�N�V�������FRIO


// ------------------ �e�Z�N�V�����̊e�^�X�N�p�̏����Q�Ƃ��邽�߂̃L�[ ---------
#define MAIN_KEY_OF_INIFILE			L"MAIN"
#define CLIENT_COM_KEY_OF_INIFILE	L"CCOM"
#define CLERK_KEY_OF_INIFILE		L"CLERK"
#define PARTNER_COM_KEY_OF_INIFILE	L"PCOM"
#define ANALYST_KEY_OF_INIFILE		L"ANALYST"
#define PLAYER_KEY_OF_INIFILE		L"PLAYER"
#define MANAGER_KEY_OF_INIFILE		L"MANAGER"
#define PR_KEY_OF_INIFILE			L"PR"
#define DEFAULT_KEY_OF_INIFILE		L"DEFAULT"
#define HELPER_KEY_OF_INIFILE		L"HELPER"
#define DUMMY_KEY_OF_INIFILE		L"DUMMY"

// ------------------ �J�����ݒ�Z�N�V�����̃L�[�� ---------------------------
#define	INI_KEY_CAM_EXPOSURE		L"ExposureTime"						// �L�[���F�I������
#define	INI_KEY_CAM_WIDTH			L"Width"							// �L�[���F�J�����B�e����
#define	INI_KEY_CAM_HEIGHT			L"Height"							// �L�[���F�J�����B�e����
#define	INI_KEY_CAM_FRAMERATE		L"FrameRate"						// �L�[���F�t���[�����[�g

// ------------------ OPENCV�ݒ�Z�N�V�����̃L�[�� ---------------------------
#define	INI_KEY_OPENCV_MASK1_EN		L"Mask1En"							// �L�[���FOpenCV�}�X�N1�L������
#define	INI_KEY_OPENCV_MASK1_MIN	L"Mask1Min"							// �L�[���FOpenCV�}�X�N1�ŏ��l
#define	INI_KEY_OPENCV_MASK1_MAX	L"Mask1Max"							// �L�[���FOpenCV�}�X�N1�ő�l
#define	INI_KEY_OPENCV_MASK2_EN		L"Mask2En"							// �L�[���FOpenCV�}�X�N2�L������
#define	INI_KEY_OPENCV_MASK2_MIN	L"Mask2Min"							// �L�[���FOpenCV�}�X�N2�ŏ��l
#define	INI_KEY_OPENCV_MASK2_MAX	L"Mask2Max"							// �L�[���FOpenCV�}�X�N2�ő�l
#define	INI_KEY_OPENCV_MASK3_EN		L"Mask3En"							// �L�[���FOpenCV�}�X�N3�L������
#define	INI_KEY_OPENCV_MASK3_MIN	L"Mask3Min"							// �L�[���FOpenCV�}�X�N3�ŏ��l
#define	INI_KEY_OPENCV_MASK3_MAX	L"Mask3Max"							// �L�[���FOpenCV�}�X�N3�ő�l
#define	INI_KEY_OPENCV_PROC_ALGO	L"ProcAlgo"							// �L�[���FOpenCV�A���S���Y��

// ------------------ RIO�ݒ�Z�N�V�����̃L�[�� ---------------------------
#define INI_KEY_RIO_IPADDR			L"IpAddr"							// �L�[��:RIO IP�A�h���X
#define INI_KEY_RIO_TCPPORTNUM		L"TcpPort"							// �L�[��:RIO TCP�|�[�g�ԍ�
#define INI_KEY_RIO_SLAVEADDR		L"SlaveAddr"						// �L�[��:RIO�X���[�u�A�h���X
#define INI_KEY_RIO_TIMEOUT			L"TimeOut"							// �L�[��:RIO�^�C���A�E�g
#define INI_KEY_RIO_XPORTNUM		L"XPortNum"							// �L�[��:RIO�X�ΌvX�f�[�^�ڑ��|�[�g�ԍ�
#define INI_KEY_RIO_YPORTNUM		L"YPortNum"							// �L�[��:RIO�X�ΌvY�f�[�^�ڑ��|�[�g�ԍ�

// ------------------ Ini�t�@�C�����̍\���� ---------------------------
typedef struct _stIniInf				//***** Ini�t�@�C����� *****
{
	INT exposureTime;					// �I������
	INT camWidth;						// �J�����擾����
	INT camHeight;						// �J�����擾����
	INT frameRate;						// �t���[�����[�g
	INT mask1En;						// �f�������}�X�N1�L������
	INT mask1Min;						// �f�������}�X�N1�ŏ�
	INT mask1Max;						// �f�������}�X�N1�ő�
	INT mask2En;						// �f�������}�X�N2�L������
	INT mask2Min;						// �f�������}�X�N2�ŏ�
	INT mask2Max;						// �f�������}�X�N2�ő�
	INT mask3En;						// �f�������}�X�N3�L������
	INT mask3Min;						// �f�������}�X�N3�ŏ�
	INT mask3Max;						// �f�������}�X�N3�ő�
	INT procAlgo;						// �摜�����A���S���Y��
	TCHAR rioIpAddr[256];				// RIO IP�A�h���X
	INT rioTcpPortNum;					// RIO TCP�|�[�g�ԍ�
	INT rioSlaveAddr;					// RIO�X���[�u�A�h���X
	INT rioTimeOut;						// RIO�^�C���A�E�g
	INT rioXPortNum;					// RIO�X�ΌvX���f�[�^�ڑ��|�[�g�ԍ�
	INT rioYPortNum;					// RIO�X�ΌvY���f�[�^�ڑ��|�[�g�ԍ�
} ST_INI_INF;
