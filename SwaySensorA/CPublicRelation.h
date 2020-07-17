#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define DISP_IMG_RAW_X0     15      // 表示画像原点座標X
#define DISP_IMG_RAW_Y0     50      // 表示画像原点座標Y
#define DISP_IMG_MSK_X0     360     // 表示画像原点座標X
#define DISP_IMG_MSK_Y0     50      // 表示画像原点座標Y
#define DISP_IMG_WIDTH      320.0   // 表示画像サイズ横幅
#define DISP_IMG_HEIGHT     240.0   // 表示画像サイズ高さ

class CPublicRelation : public CTaskObj
{
public:
    CPublicRelation();
    ~CPublicRelation();

    void init_task(void *pobj);
    void routine_work(void* param);

    // タスク設定ウィンドウ用メソッド
    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    void set_panel_tip_txt();   // タブパネルの説明用Staticテキストを設定
    void set_panel_pb_txt();    // タブパネルのFunctionボタンのStaticテキストを設定
    void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6);    // パラメータ

private:
    HWND OpenCameraPanel(); // メンテナンスパネル表示
    static LRESULT CALLBACK CameraWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);   // メンテナンスパネルWnd用コールバック関数

    static HWND     m_hCamDlg;
    static cv::Mat  m_mtSaveImage;
    static BOOL     m_bCursor;
    static POINT    m_pntCursor;
    static UINT     m_iSelImg;
};
