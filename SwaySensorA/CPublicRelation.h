#pragma once
#include "CTaskObj.h"

extern vector<void*>	VectpCTaskObj;	//�^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask g_itask;

class CPublicRelation :	public CTaskObj
{
public:
	CPublicRelation();
	~CPublicRelation();

	HWND hCamDlg;
	
	HWND OpenCameraPanel();														//�����e�i���X�p�l���\��
	static LRESULT CALLBACK DispWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);	//�����e�i���X�p�l��Wnd�p�R�[���o�b�N�֐�

	//�^�X�N�ݒ�E�B���h�E�p���\�b�h
	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void set_panel_tip_txt();															//�^�u�p�l���̐����pStatic�e�L�X�g��ݒ�
	void set_panel_pb_txt() { return; };												//�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
	void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6);//�p�����[�^
	void init_task(void *pobj);
};

