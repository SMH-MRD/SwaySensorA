#pragma once
#include "CTaskObj.h"

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask         g_itask;

#define CLERK_LOG_MAX   8   // LOG�L�^�ő吔

class CClerk : public CTaskObj
{
public:
    CClerk();
    ~CClerk();

    CClerk* pClkObj;

    void init_task(void *pobj);
    void routine_work(void *param);

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    void set_panel_tip_txt();//�^�u�p�l����Static�e�L�X�g��ݒ�
    void set_panel_pb_txt();
    bool b_logact[CLERK_LOG_MAX];
    HWND CreateOwnWindow(HWND h_parent_wnd);
    BOOL InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);

    static LRESULT CALLBACK ChartWndProc(HWND, UINT, WPARAM, LPARAM);
};
