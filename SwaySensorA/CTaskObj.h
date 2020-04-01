#pragma once

#include "framework.h"
#include <process.h>								//threads and processes関数ヘッダ

using namespace std;

//#タスク条件設定部
#define TARGET_RESOLUTION			1				//マルチメディアタイマー精度
#define SYSTEM_TICK_ms				25				//タスクスレッド呼び出し周期単位　この倍数の周期となる
#define MAX_APL_TASK				9				//登録可能タスク最大数
#define TASK_NUM					8				//登録タスク数
#define INITIAL_TASK_STACK_SIZE		16384
#define DEFAUT_TASK_CYCLE			50				//タスクスレッド呼び出しデフォルト周期
#define TASK_EVENT_MAX				8				//各タスクスレッド呼び出しイベント最大数

//inf.thread_com用　スレッドループ制御用
#define REPEAT_INFINIT				0				//永久ループ
#define TERMINATE_THREAD			1				//スレッド中断
#define REPEAT_ONCE					-1				//逐次処理

//定周期起動スレッドで実行する関数の内容選択
#define THREAD_WORK_IDLE			0				//アイドル処理（処理無し）
#define THREAD_WORK_ROUTINE			1				//通常処理
#define THREAD_WORK_OPTION1			2				//オプション処理
#define THREAD_WORK_OPTION2			3				//オプション処理

/***********************************************************************
タスクオブジェクトの個別管理情報構造体
************************************************************************/
typedef struct {
	//-オブジェクト識別情報
	TCHAR			name[24];						//オブジェクト名
	TCHAR			sname[8];						//オブジェクト略称
	HBITMAP			hBmp;							//オブジェクト識別用ビットマップ

	//-スレッド設定内容
	int				index;							//スレッドIndex
	unsigned int	ID;								//スレッドID
	HANDLE			hndl;							//スレッドハンドル
	HANDLE			hevents[TASK_EVENT_MAX];		//イベントハンドル
	int				n_active_events = 1;			//有効なイベント数
	int				event_triggered;				//発生したイベントの番号
	unsigned int	cycle_ms;						//スレッド実行設定周期
	unsigned int	cycle_count;					//スレッド実行設定周期　Tick count（ms/system tick)
	int				trigger_type;					//スレッド起動条件　定周期orイベント
	int				priority;						//スレッドのプライオリティ
	int				thread_com;						//スレッド制御フラグ 　0:繰り返し　1:中断　それ以外：逐次処理:

	//-スレッドモニタ情報
	DWORD			start_time;						//現スキャンのスレッド開始時間
	DWORD			act_time;						//1スキャンのスレッド実処理時間
	DWORD			period;							//スレッドループ周期実績
	DWORD			act_count;						//スレッドループカウンタ
	DWORD			total_act;						//起動積算カウンタ
	unsigned int	time_over_count;				//予定周期をオーバーした回数

	//-関連ウィンドウハンドル
	HWND			hWnd_parent;					//親ウィンドウのハンドル
	HWND			hWnd_msgStatics;				//親ウィンドウメッセージ表示用ウィンドウへのハンドル
	HWND			hWnd_opepane;					//自メインウィンドウのハンドル（メインフレーム上に配置）
	HWND			hWnd_msgList;					//自メインウィンドウのメッセージ表示用リストコントロールへのハンドル
	HWND			hWnd_work;						//自専用作業用ウィンドウのハンドル

	//-操作パネル関連
	int				cnt_PNLlist_msg = 0;			//パネルメッセージリストのカウント数
	int				panel_func_id = 1;				//パネルfunctionボタンの選択内容
	int				panel_type_id = 1;				//パネルtypeボタンの選択内容

	//-外部インターフェース
	unsigned long	*psys_counter;					//メインシステムカウンターの参照先ポインタ
	unsigned		work_select = 0;				//スレッド実行の関数の種類を指定

}ST_TASK_INFO, *PST_TASKD_INFO;


class CTaskObj
{

public:
	ST_TASK_INFO inf;

	CTaskObj();
	virtual ~CTaskObj();							//デストラクタ

	unsigned __stdcall run(void *param);			//スレッド実行対象関数

	virtual void init_task(void *pobj);
	virtual void routine_work(void *param);
};

