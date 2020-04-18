#pragma once

#include "resource.h"

#include "CTaskObj.h"
#include "CSharedObject.h"
#include "CManager.h"
#include "CPlayer.h"
#include "CComClient.h"
#include "CComRIO.h"
#include "CComCamera.h"
#include "CPublicRelation.h"
#include "CClerk.h"
#include "CAnalyst.h"

///# マルチスレッド管理用構造体
typedef struct knl_manage_set_tag {
	WORD mmt_resolution = TARGET_RESOLUTION;			//マルチメディアタイマーの分解能
	unsigned int cycle_base = SYSTEM_TICK_ms;			//タスクスレッド最小周期
	WORD KnlTick_TimerID;								//マルチメディアタイマーのID
	unsigned int num_of_task = 0;						//アプリケーションで利用するスレッド数
	unsigned long sys_counter = 0;
	SYSTEMTIME Knl_Time;								//アプリケーション開始からの経過時間
	unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//タスクの初期スタックサイズ
}ST_KNL_MANAGE_SET, *P_ST_MANAGE_SET;


 
#define TASK_NUM						8			//登録タスク数
#define NAME_OF_INIFILE					L"app"		//iniファイルファイル名
#define EXT_OF_INIFILE					L"ini"		//iniファイル拡張子
#define PATH_OF_INIFILE					pszInifile	//オプショナルパス
#define PATH_OF_EXEFILE					wstrPathExe //EXEのPATH


//-Main Windowの初期サイズ、位置設定
#define MAIN_WND_INIT_POS_X			20
#define MAIN_WND_INIT_POS_Y			20

//-Tweet Message表示用Static Windowのサイズ設定値
#define MSG_WND_H					20		//Height of window
#define MSG_WND_W					590		//Width of window
#define MSG_WND_ORG_X				35		//Origin X of window arrangement 
#define MSG_WND_ORG_Y				10		//Origin Y of window arrangement 
#define MSG_WND_Y_SPACE				12		//Space between windows Y direction 
#define TWEET_IMG_ORG_X				5		//Origin X of bitmap icon arrangement

//-各タスクアイコン用イメージリスト設定値
#define ICON_IMG_W					32		//イメージ幅
#define ICON_IMG_H					32		//イメージ高さ

//-各タスク設定用タブ関連
#define TAB_DIALOG_W				620
#define TAB_DIALOG_H				300
#define TAB_POS_X					10
#define TAB_POS_Y					MSG_WND_ORG_Y+(MSG_WND_H+MSG_WND_Y_SPACE)*TASK_NUM+10
#define TAB_SIZE_H					35

//-ID定義 Mainスレッド用　WM_USER + 1000 + α
#define ID_STATUS					WM_USER + 1001
#define ID_STATIC_MAIN				WM_USER + 1002
#define ID_TASK_SET_TAB				WM_USER + 1003
