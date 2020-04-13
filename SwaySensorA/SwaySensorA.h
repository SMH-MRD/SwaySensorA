#pragma once

#include "resource.h"

#include "CTaskObj.h"
#include "CSharedObject.h"
#include "CManager.h"


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



#define TASK_NUM						8				//登録タスク数

#define NAME_OF_INIFILE					L"app"		//iniファイルファイル名
#define EXT_OF_INIFILE					L"ini"		//iniファイル拡張子
#define PATH_OF_INIFILE					pszInifile	//オプショナルパス
#define PATH_OF_EXEFILE					wstrPathExe //EXEのPATH

