#pragma once
#include "CTaskObj.h"

extern vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
extern ST_iTask g_itask;

class CPublicRelation :	public CTaskObj
{
public:
	CPublicRelation();
	~CPublicRelation();

	HWND OpenCameraPanel();														//メンテナンスパネル表示
	LRESULT CALLBACK DispWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);	//メンテナンスパネルWnd用コールバック関数
};

