#pragma once
#include "CTaskObj.h"

extern vector<void*>	VectpCTaskObj;	//�^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask g_itask;

class CPublicRelation :	public CTaskObj
{
public:
	CPublicRelation();
	~CPublicRelation();

	HWND OpenCameraPanel();														//�����e�i���X�p�l���\��
	LRESULT CALLBACK DispWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);		//�����e�i���X�p�l��Wnd�p�R�[���o�b�N�֐�
};

