#pragma once

#include "resource.h"

#include "CTaskObj.h"
#include "CSharedObject.h"
#include "CManager.h"
#include "CPlayer.h"
#include "CComClient.h"
#include "CComRIO.h"
#include "CComCamera.h"
#include "CPublicRelation.h"
#include "CClerk.h"
#include "CAnalyst.h"

// #�}���`�X���b�h�Ǘ��p�\����
typedef struct knl_manage_set_tag
{
    WORD            mmt_resolution = TARGET_RESOLUTION;     // �}���`���f�B�A�^�C�}�[�̕���\
    unsigned int    cycle_base = SYSTEM_TICK_ms;            // �^�X�N�X���b�h�ŏ�����
    WORD            KnlTick_TimerID;                        // �}���`���f�B�A�^�C�}�[��ID
    unsigned int    num_of_task = 0;                        // �A�v���P�[�V�����ŗ��p����X���b�h��
    unsigned long   sys_counter = 0;
    SYSTEMTIME      Knl_Time;                               // �A�v���P�[�V�����J�n����̌o�ߎ���
    unsigned int    stackSize = INITIAL_TASK_STACK_SIZE;    // �^�X�N�̏����X�^�b�N�T�C�Y
} ST_KNL_MANAGE_SET, *P_ST_MANAGE_SET;

#define TASK_NUM        8           // �o�^�^�X�N��
#define NAME_OF_INIFILE L"app"      // ini�t�@�C���t�@�C����
#define EXT_OF_INIFILE  L"ini"      // ini�t�@�C���g���q
#define PATH_OF_INIFILE pszInifile  // �I�v�V���i���p�X
#define PATH_OF_EXEFILE wstrPathExe // EXE��PATH

// -Main Window�̏����T�C�Y�A�ʒu�ݒ�
#define MAIN_WND_INIT_POS_X 20
#define MAIN_WND_INIT_POS_Y 20

// -Tweet Message�\���pStatic Window�̃T�C�Y�ݒ�l
#define MSG_WND_H       20  // Height of window
#define MSG_WND_W       590 // Width of window
#define MSG_WND_ORG_X   35  // Origin X of window arrangement 
#define MSG_WND_ORG_Y   10  // Origin Y of window arrangement 
#define MSG_WND_Y_SPACE 12  // Space between windows Y direction 
#define TWEET_IMG_ORG_X 5   // Origin X of bitmap icon arrangement

// -�e�^�X�N�A�C�R���p�C���[�W���X�g�ݒ�l
#define ICON_IMG_W  32  // �C���[�W��
#define ICON_IMG_H  32  // �C���[�W����

// -�e�^�X�N�ݒ�p�^�u�֘A
#define TAB_DIALOG_W    620
#define TAB_DIALOG_H    300
#define TAB_POS_X       10
#define TAB_POS_Y       MSG_WND_ORG_Y+(MSG_WND_H+MSG_WND_Y_SPACE)*TASK_NUM+10
#define TAB_SIZE_H      35

// -ID��` Main�X���b�h�p�@WM_USER + 1000 + ��
#define ID_STATUS       WM_USER + 1001
#define ID_STATIC_MAIN  WM_USER + 1002
#define ID_TASK_SET_TAB WM_USER + 1003
