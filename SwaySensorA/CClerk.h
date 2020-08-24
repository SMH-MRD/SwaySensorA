#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"
#include "CMKlog.h"
#include "CMKChart.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define CLERK_LOG_MAX   8   // LOG記録最大数

class CClerk : public CTaskObj
{
public:
    CClerk();
    ~CClerk();

    CClerk* pClkObj;

    void init_task(void *pobj);
    void routine_work(void *param);

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    void set_panel_tip_txt();//タブパネルのStaticテキストを設定
    void set_panel_pb_txt();
    bool b_logact[CLERK_LOG_MAX];
    HWND CreateOwnWindow(HWND h_parent_wnd);
    BOOL InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);

    static LRESULT CALLBACK ChartWndProc(HWND, UINT, WPARAM, LPARAM);

private:
    stProcInfoData  m_procinfo;   // 画像処理結果
};
