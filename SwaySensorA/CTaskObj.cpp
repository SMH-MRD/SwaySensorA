#include "CTaskObj.h"

//# マルチスレッド基本クラス メンバー関数 

CTaskObj::CTaskObj() {//コンストラクタ
	inf.hndl = NULL;
	inf.cycle_ms = DEFAUT_TASK_CYCLE;
	inf.thread_com = REPEAT_INFINIT;
	wsprintf(inf.name, L"Who am I?");
	inf.act_count = inf.act_time = 0;
	inf.priority = THREAD_PRIORITY_NORMAL;
	inf.work_select = THREAD_WORK_IDLE;
	inf.hWnd_parent = inf.hWnd_work = inf.hWnd_opepane = inf.hWnd_msgStatics = NULL;
	inf.hBmp = NULL;
};

CTaskObj::~CTaskObj() {
	delete inf.hBmp;
};//デストラクタ

void CTaskObj::init_task(void *pobj) {
	return;
};

//# スレッドメイン関数
unsigned CTaskObj::run(void *param) {

	while (this->inf.thread_com != TERMINATE_THREAD) {
		inf.event_triggered = WaitForMultipleObjects(inf.n_active_events, ((CTaskObj*)param)->inf.hevents, FALSE, INFINITE);//メインスレッドからのSIGNAL状態待ち

		//処理周期確認用
		DWORD start_time = timeGetTime();
		inf.period = start_time - inf.start_time;
		inf.start_time = start_time;

		routine_work(param);

		inf.act_time = timeGetTime() - start_time;//実処理時間測定用
		if (this->inf.thread_com != REPEAT_INFINIT) break;//逐次処理の場合は抜ける
	}
	return 0;
}

//# タスクスレッドでの処理関数
void CTaskObj::routine_work(void *param) {
	return;
};
