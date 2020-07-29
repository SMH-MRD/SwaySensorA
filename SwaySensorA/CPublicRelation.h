#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define DISP_IMG_WIDTH      450                                     // 表示画像サイズ横幅
#define DISP_IMG_HEIGHT     360                                     // 表示画像サイズ高さ
#define DISP_IMG_RAW_X0     15                                      // 表示画像原点座標X
#define DISP_IMG_RAW_Y0     50                                      // 表示画像原点座標Y
#define DISP_IMG_MSK_X0     DISP_IMG_RAW_X0 + DISP_IMG_WIDTH + 40   // 表示画像原点座標X
#define DISP_IMG_MSK_Y0     DISP_IMG_RAW_Y0                         // 表示画像原点座標Y

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

    // タスク設定ウィンドウ用メソッド
    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    void set_panel_tip_txt();   // タブパネルの説明用Staticテキストを設定
    void set_panel_pb_txt();    // タブパネルのFunctionボタンのStaticテキストを設定
    void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6);    // パラメータ

private:
    HWND OpenCameraPanel(); // メンテナンスパネル表示
    static LRESULT CALLBACK CameraWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);   // メンテナンスパネルWnd用コールバック関数

private:
    static HWND                 m_hCamDlg;
    static cv::Mat              m_imgsrc;
    static BOOL                 m_cursor;
    static POINT                m_cursorpt;
    static UINT                 m_selimg;
    static stCommonParamData    m_cmmnparam;        // 構造設定データ
    static stCameraParamData    m_camparam;         // カメラ設定データ
    static stImgProcParamData   m_imgprocparam;     // 画像処理設定データ
    static stExtnInfoData       m_extninfo;         // 外部入力データ
    static BOOL                 m_scaleImgsrc;
    static SCROLLINFO           m_scrinfImgsrcH;
    static SCROLLINFO           m_scrinfImgsrcV;
    static BOOL                 m_scaleImgmask;
    static SCROLLINFO           m_scrinfImgmaskH;
    static SCROLLINFO           m_scrinfImgmaskV;
};
