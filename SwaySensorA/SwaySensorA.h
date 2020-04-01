#pragma once

#include "resource.h"

#include "CTaskObj.h"
#include "CSharedData.h"


///# �}���`�X���b�h�Ǘ��p�\����
typedef struct knl_manage_set_tag {
	WORD mmt_resolution = TARGET_RESOLUTION;			//�}���`���f�B�A�^�C�}�[�̕���\
	unsigned int cycle_base = SYSTEM_TICK_ms;			//�^�X�N�X���b�h�ŏ�����
	WORD KnlTick_TimerID;								//�}���`���f�B�A�^�C�}�[��ID
	unsigned int num_of_task = 0;						//�A�v���P�[�V�����ŗ��p����X���b�h��
	unsigned long sys_counter = 0;
	SYSTEMTIME Knl_Time;								//�A�v���P�[�V�����J�n����̌o�ߎ���
	unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//�^�X�N�̏����X�^�b�N�T�C�Y
}ST_KNL_MANAGE_SET, *P_ST_MANAGE_SET;

#define NAME_OF_INIFILE					L"app" //ini�t�@�C���t�@�C����
#define EXT_OF_INIFILE					L"ini" //ini�t�@�C���g���q

// �߂�l��`
#define RESULT_OK			0			// �߂�l:����
#define RESULT_NG_INVALID	-1			// �߂�l:���s(�����ُ�)
#define RESULT_NG_SYSERR	-2			// �߂�l:���s(�V�X�e���G���[)
#define RESULT_NG_SEQUENCE	-3			// �߂�l:���s(�V�[�P���X�G���[)
#define RESULT_NG_BUSY		-4			// �߂�l:���s(�r�W�[)