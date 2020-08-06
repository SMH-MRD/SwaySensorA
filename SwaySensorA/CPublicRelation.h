#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask         g_itask;

#define DISP_IMG_WIDTH      450                                     // �\���摜�T�C�Y����
#define DISP_IMG_HEIGHT     360                                     // �\���摜�T�C�Y����
#define DISP_IMG_RAW_X0     15                                      // �\���摜���_���WX
#define DISP_IMG_RAW_Y0     50                                      // �\���摜���_���WY
#define DISP_IMG_MSK_X0     DISP_IMG_RAW_X0 + DISP_IMG_WIDTH + 40   // �\���摜���_���WX
#define DISP_IMG_MSK_Y0     DISP_IMG_RAW_Y0                         // �\���摜���_���WY

#define SCALAR_WHITE        cv::Scalar(255, 255, 255)
#define SCALAR_BLUE         cv::Scalar(255, 0, 0)
#define SCALAR_GREEN        cv::Scalar(0, 255, 0)
#define SCALAR_RED          cv::Scalar(0, 0, 255)
#define SCALAR_YELLOW       cv::Scalar(0, 255, 255)
#define SCALAR_MAGENTA      cv::Scalar(255, 0, 255)

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

private:
    static HWND                 m_hCamDlg;
    static cv::Mat              m_imgsrc;
    static BOOL                 m_cursor;
    static POINT                m_cursorpt;
    static UINT                 m_selimg;
    static stCommonParamData    m_cmmnparam;        // �\���ݒ�f�[�^
    static stCameraParamData    m_camparam;         // �J�����ݒ�f�[�^
    static stImgProcParamData   m_imgprocparam;     // �摜�����ݒ�f�[�^
    static stExtnInfoData       m_extninfo;         // �O�����̓f�[�^
    static BOOL                 m_scaleImgsrc;
    static SCROLLINFO           m_scrinfImgsrcH;
    static SCROLLINFO           m_scrinfImgsrcV;
    static BOOL                 m_scaleImgmask;
    static SCROLLINFO           m_scrinfImgmaskH;
    static SCROLLINFO           m_scrinfImgmaskV;
};
