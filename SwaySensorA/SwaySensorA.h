#pragma once

#include "resource.h"

#include "CTaskObj.h"
#include "CSharedData.h"


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

#define NAME_OF_INIFILE					L"app" //iniファイルファイル名
#define EXT_OF_INIFILE					L"ini" //iniファイル拡張子

// 戻り値定義
#define RESULT_OK			0			// 戻り値:成功
#define RESULT_NG_INVALID	-1			// 戻り値:失敗(引数異常)
#define RESULT_NG_SYSERR	-2			// 戻り値:失敗(システムエラー)
#define RESULT_NG_SEQUENCE	-3			// 戻り値:失敗(シーケンスエラー)
#define RESULT_NG_BUSY		-4			// 戻り値:失敗(ビジー)