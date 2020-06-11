#pragma once
#include "CTaskObj.h"

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask         g_itask;

#define DISP_IMG_X0     30      // �\���摜���_���WX
#define DISP_IMG_Y0     30      // �\���摜���_���WY
#define DISP_IMG_WIDTH  320.0   // �\���摜�T�C�Y����
#define DISP_IMG_HEIGHT 240.0   // �\���摜�T�C�Y����

class CPublicRelation : public CTaskObj
{
public:
    CPublicRelation();
    ~CPublicRelation();

    void init_task(void *pobj);
    void routine_work(void* param);

    // �^�X�N�ݒ�E�B���h�E�p���\�b�h
    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    void set_panel_tip_txt();   // �^�u�p�l���̐����pStatic�e�L�X�g��ݒ�
    void set_panel_pb_txt();    // �^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6);    // �p�����[�^

private:
    HWND OpenCameraPanel(); // �����e�i���X�p�l���\��
    static LRESULT CALLBACK CameraWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);   // �����e�i���X�p�l��Wnd�p�R�[���o�b�N�֐�

    static HWND hCamDlg;
    static Mat m_mtSaveImage;
};
