#pragma once
#include "CTaskObj.h"

extern vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
extern ST_iTask g_itask;

class CPublicRelation :	public CTaskObj
{
public:
	CPublicRelation();
	~CPublicRelation();

	static HWND hCamDlg;
	
	HWND OpenCameraPanel();														//メンテナンスパネル表示
	static LRESULT CALLBACK DispWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);	//メンテナンスパネルWnd用コールバック関数

	//タスク設定ウィンドウ用メソッド
	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void set_panel_tip_txt();															//タブパネルの説明用Staticテキストを設定
	void set_panel_pb_txt();												//タブパネルのFunctionボタンのStaticテキストを設定
	void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6);//パラメータ
	void init_task(void *pobj);
};

