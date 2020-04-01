// SwaySensorA.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "SwaySensorA.h"
#include "inifile.h"

#include "CTaskObj.h"		//タスククラス

#include <windowsx.h>	//コモンコントロール用
#include <commctrl.h>	//コモンコントロール用
#include <shlwapi.h>	//Win32 APIでパスを扱うには shlwapi.h に定義されている関数群(Path Routines)を使用

using namespace std;

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

vector<void*>	VectpCTaskObj;					//タスクオブジェクトのポインタ
SYSTEMTIME		gAppStartTime;					//アプリケーション開始時間
LPWSTR			pszInifile;						// iniファイルのパス
wstring			wstrPathExe;					// 実行ファイルのパス

// スタティック変数:
static ST_KNL_MANAGE_SET	knl_manage_set;		//マルチスレッド管理用構造体
static vector<HANDLE>		VectHevent;			//マルチスレッド用イベントのハンドル
static HIMAGELIST			hImgListTaskIcon;	//タスクアイコン用イメージリスト
static CSharedData*			cSharedData;		// 共有オブジェクトインスタンス




// このコード モジュールに含まれる関数の宣言を転送します:
// # アプリケーション専用関数:	************************************
VOID	CALLBACK alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);	//マルチメディアタイマ処理関数　スレッドのイベントオブジェクト処理
int		Init_tasks();																	//アプリケーション毎のタスクオブジェクトの初期設定
DWORD	knlTaskStartUp();																//実行させるタスクの起動処理
INT		setParameter(ST_INI_INF* pInf, LPCWSTR pFileName);								//パラメータ設定処理
void	GetIniInf(LPCWSTR file_name, LPCWSTR section_name, LPCWSTR key_name, LPCWSTR str_default, int value_type, void* p_param);//iniファイル読取処理
void	CreateSharedData(void);															//共有メモリCreate処理
static unsigned __stdcall thread_gate_func(void * pObj) {								//スレッド実行のためのゲート関数
	CTaskObj * pthread_obj = (CTaskObj *)pObj;return pthread_obj->run(pObj);
}

// # Wizard Default関数:		************************************
ATOM                MyRegisterClass(HINSTANCE hInstance);								// ウィンドウ クラスを登録します。
BOOL                InitInstance(HINSTANCE, int);										// メインウィンドウクリエイト
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);								// ウィンドウプロシージャ
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// # 関数: wWinMain				************************************
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SWAYSENSORA, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SWAYSENSORA));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

///#  関数: MyRegisterClass() ************************************************************************
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SWAYSENSORA));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SWAYSENSORA);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

///#  関数: InitInstance(HINSTANCE, int) *************************************************************
//
//   目的		: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//   コメント	: この関数で、グローバル変数でインスタンス ハンドルを保存し、メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;				// グローバル変数にインスタンス ハンドルを格納する

   GetSystemTime(&gAppStartTime);	//アプリケーション開始時刻取得


   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

///#  関数: WndProc(HWND, UINT, WPARAM, LPARAM) ******************************************************
//
//  目的: メイン ウィンドウのメッセージを処理します。
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

///# 関数: スレッドタスクの登録、設定 ***
int  Init_tasks() {
	HBITMAP hBmp;
	CTaskObj *ptempobj;
	int task_index = 0;

	//コモンコントロール初期化
	InitCommonControls();
	//タスクアイコン用イメージリストクリエイト
	hImgListTaskIcon = ImageList_Create(32, 32,					//int cx, int cy
										ILC_COLOR | ILC_MASK,	//UINT flags
										2, 0);					//int cInitial, int cGrow

	CreateSharedData();//共有メモリクリエイト

#if 0
	//###Task1 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CManager;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.mng = task_index;


		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;


		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

		/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 1000;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_MAN1");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, MANAGER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, MANAGER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;
		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}
	//###Task2 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CPlayer;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.ply = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																						   /// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PLY1");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, PLAYER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, PLAYER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}


	//###Task3 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CComClient;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.comc = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 1000;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_CCOM");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, CLIENT_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CLIENT_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task4 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CComDeviceRIO;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.comp = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 25;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PCOM");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task5 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CComDeviceCamera;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.comp = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PCOM");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task6 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CPublicRelation(hInst);
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.pr = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PR");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, PR_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, PR_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task7 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CClerk;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.clerk = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 1000;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_CLERK");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, CLERK_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CLERK_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task8 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CAnalyst;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.ana = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_ANALYST");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, ANALYST_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, ANALYST_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

#endif
	///各タスク用設定ウィンドウ作成
	InitCommonControls();	//コモンコントロール初期化
//	hTabWnd = CreateTaskSettingWnd(hWnd);//タブウィンドウ作成

	//各タスク残パラメータセット
	knl_manage_set.num_of_task = (unsigned int)VectpCTaskObj.size();//タスク数登録
	for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) {
		CTaskObj * pobj = (CTaskObj *)VectpCTaskObj[i];

		pobj->inf.index = i;	//タスクインデックスセット

		//その他設定
		pobj->inf.psys_counter = &knl_manage_set.sys_counter;
		pobj->inf.act_count = 0;//起動チェック用カウンタリセット
		 //起動周期カウント値
		if (pobj->inf.cycle_ms >= SYSTEM_TICK_ms)	pobj->inf.cycle_count = pobj->inf.cycle_ms / SYSTEM_TICK_ms;
		else pobj->inf.cycle_count = 1;

		//最後に初期化関数呼び出し
		pobj->init_task(pobj);
	}

	return 1;
}

///# 関数: マルチタスクスタートアップ処理関数 ***
DWORD knlTaskStartUp()	//実行させるオブジェクトのリストのスレッドを起動
{
	//機能	：[KNL]システム/ユーザタスクスタートアップ関数
	//処理	：自プロセスのプライオリティ設定，カーネルの初期設定,タスク生成，基本周期設定
	//戻り値：Win32APIエラーコード

	HANDLE	myPrcsHndl;	/* 本プログラムのプロセスハンドル */
						///# 自プロセスプライオリティ設定処理
						//-プロセスハンドル取得
	if ((myPrcsHndl = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION, FALSE, _getpid())) == NULL)	return(GetLastError());
	_RPT1(_CRT_WARN, "KNL Priority For Windows(before) = %d \n", GetPriorityClass(myPrcsHndl));

	//-自プロセスのプライオリティを最優先ランクに設定
	if (SetPriorityClass(myPrcsHndl, REALTIME_PRIORITY_CLASS) == 0)	return(GetLastError());
	_RPT1(_CRT_WARN, "KNL Priority For NT(after) = %d \n", GetPriorityClass(myPrcsHndl));

	///# アプリケーションタスク数が最大数を超えた場合は終了
	if (VectpCTaskObj.size() >= MAX_APL_TASK)	return((DWORD)ERROR_BAD_ENVIRONMENT);

	///#    アプリケーションスレッド生成処理	
	for (unsigned i = 0; i < VectpCTaskObj.size(); i++) {

		CTaskObj * pobj = (CTaskObj *)VectpCTaskObj[i];

		// タスク生成(スレッド生成)
		// 他ﾌﾟﾛｾｽとの共有なし,スタック初期サイズ　デフォルト, スレッド実行関数　引数で渡すオブジェクトで対象切り替え,スレッド関数の引数（対象のオブジェクトのポインタ）, 即実行Createflags, スレッドID取り込み
		pobj->inf.hndl = (HANDLE)_beginthreadex((void *)NULL, 0, thread_gate_func, VectpCTaskObj[i], (unsigned)0, (unsigned *)&(pobj->inf.ID));

		// タスクプライオリティ設定
		if (SetThreadPriority(pobj->inf.hndl, pobj->inf.priority) == 0)
			return(GetLastError());
		_RPT2(_CRT_WARN, "Task[%d]_priority = %d\n", i, GetThreadPriority(pobj->inf.hndl));

		pobj->inf.act_count = 0;		// 基本ティックのカウンタ変数クリア
		pobj->inf.time_over_count = 0;	// 予定周期オーバーカウントクリア
	}

	return 1;
}

///# バージョン情報ボックスのメッセージ ハンドラーです。 *********************************************
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

///# 関数: マルチメディアタイマーイベント処理関数 　　　**********************************************
VOID CALLBACK alarmHandlar(UINT uID, UINT uMsg, DWORD	dwUser, DWORD dw1, DWORD dw2) {

	LONG64 tmttl;
	knl_manage_set.sys_counter++;									//システムカウンタ更新

	//スレッド再開イベントセット処理
	for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) {		//登録した各タスクチェック
		CTaskObj * pobj = (CTaskObj *)VectpCTaskObj[i];
		pobj->inf.act_count++;	
		if (pobj->inf.act_count >= pobj->inf.cycle_count) {			//登録したタスクの周期カウンタチェック
			PulseEvent(VectHevent[i]);								//イベント送信してタスクスレッドのループを回す
			pobj->inf.act_count = 0;								//タスクの周期カウンタリセット
			pobj->inf.total_act++;									//タスクスレッド起動回数カウンタ更新
		}
	}
}


///# 関数: ini file読み込みパラメータ設定　　　　　　　　*********************************************
INT setIniParameter(ST_INI_INF* pInf, LPCWSTR pFileName)
{
	//--------------------------------------------------------------------------
	// 設定ファイル存在チェック
	if (!(PathFileExists(pFileName)) || PathIsDirectory(pFileName))
	{
		return RESULT_NG_INVALID;
	}

	//--------------------------------------------------------------------------
	// 共通設定パラメータ
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_EXPOSURE,
		L"10000", INITYPE_INT, &(pInf->exposureTime));				// 露光時間
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_WIDTH,
		L"640", INITYPE_INT, &(pInf->camWidth));					// カメラ撮影横幅
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_HEIGHT,
		L"480", INITYPE_INT, &(pInf->camHeight));					// カメラ撮影高さ
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_FRAMERATE,
		L"30", INITYPE_INT, &(pInf->frameRate));					// フレームレート

	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK1_EN,
		L"1", INITYPE_INT, &(pInf->mask1En));						// マスク1有効無効
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK1_MIN,
		L"0", INITYPE_INT, &(pInf->mask1Min));						// マスク1最小
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK1_MAX,
		L"10", INITYPE_INT, &(pInf->mask1Max));						// マスク1最大
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK2_EN,
		L"1", INITYPE_INT, &(pInf->mask2En));						// マスク2有効無効
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK2_MIN,
		L"170", INITYPE_INT, &(pInf->mask2Min));					// マスク2最小
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK2_MAX,
		L"180", INITYPE_INT, &(pInf->mask2Max));					// マスク2最大
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK3_EN,
		L"1", INITYPE_INT, &(pInf->mask3En));						// マスク3有効無効
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK3_MIN,
		L"80", INITYPE_INT, &(pInf->mask3Min));						// マスク3最小
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK3_MAX,
		L"100", INITYPE_INT, &(pInf->mask3Max));					// マスク3最大
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_PROC_ALGO,
		L"100", INITYPE_INT, &(pInf->procAlgo));					// 画像処理アルゴリズム

	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_IPADDR,
		L"192.168.0.1", INITYPE_CHAR, &(pInf->rioIpAddr));			// RIO IPアドレス
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_TCPPORTNUM,
		L"502", INITYPE_INT, &(pInf->rioTcpPortNum));				// RIO TCPポート番号
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_SLAVEADDR,
		L"1", INITYPE_INT, &(pInf->rioSlaveAddr));					// RIOスレーブアドレス
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_TIMEOUT,
		L"2000", INITYPE_INT, &(pInf->rioTimeOut));					// RIOタイムアウト
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_XPORTNUM,
		L"1", INITYPE_INT, &(pInf->rioXPortNum));					// RIO傾斜計Xデータ接続ポート番号
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_YPORTNUM,
		L"2", INITYPE_INT, &(pInf->rioYPortNum));					// RIO傾斜計Yデータ接続ポート番号

	return S_OK;
}

///# 関数: INIファイル読出し ***************
void GetIniInf(LPCWSTR file_name, LPCWSTR section_name, LPCWSTR key_name, LPCWSTR str_default, int value_type, void* p_param)
{
	DWORD	state;
	WCHAR	buf[256];

	state = GetPrivateProfileString(section_name, key_name, str_default, buf, sizeof(buf) / 2, file_name);
	if (state <= 0)
	{
		_stprintf_s(buf, L"%s", str_default);
	}

	switch (value_type)
	{
	case INITYPE_CHAR:		_stprintf_s((LPWSTR)p_param, 256, buf);	break;
	case INITYPE_INT:		*((int*)p_param) = _wtoi(buf);			break;
	case INITYPE_SHORT:		*((short*)p_param) = (short)_wtoi(buf);	break;
	case INITYPE_LONG:		*((long*)p_param) = _wtoi(buf);			break;
	case INITYPE_FLOAT:		*((float*)p_param) = (float)_wtof(buf);	break;
	case INITYPE_DOUBLE:	*((double*)p_param) = _wtof(buf);		break;
	default:														break;
	}
}


///# 関数: 共有オブジェクト初期化 ********************************************************************
void CreateSharedData(void) {
	
	// ini file path
	static WCHAR dstpath[_MAX_PATH], szDrive[_MAX_DRIVE], szPath[_MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	//実行ファイルのパス取得
	GetModuleFileName(NULL, dstpath, (sizeof(TCHAR) * _MAX_PATH) / 2);
	//実行ファイルのパスを分割
	_wsplitpath_s(dstpath, szDrive, sizeof(szDrive) / 2, szPath, sizeof(szPath) / 2, szFName, sizeof(szFName) / 2, szExt, sizeof(szExt) / 2);
	// ini file path構成
	wstrPathExe = szDrive; wstrPathExe += szPath;
	_wmakepath_s(dstpath, sizeof(dstpath) / 2, szDrive, szPath, NAME_OF_INIFILE, EXT_OF_INIFILE);
	pszInifile = dstpath;


	ST_INI_INF ini;

#if 0
	setParameter(&ini, dstpath);

	cSharedData = new CSharedData();
	cSharedData->InitSharedData();

	cSharedData->SetParam(PARAM_ID_CAM_EXPOSURE_TIME, (UINT32)ini.exposureTime);
	cSharedData->SetParam(PARAM_ID_CAM_FRAMERATE, (UINT32)ini.frameRate);
	cSharedData->SetParam(PARAM_ID_CAM_WIDTH, (UINT32)ini.camWidth);
	cSharedData->SetParam(PARAM_ID_CAM_HEIGHT, (UINT32)ini.camHeight);

	cSharedData->SetParam(PARAM_ID_PIC_HUE1_EN, (UINT32)ini.mask1En);
	cSharedData->SetParam(PARAM_ID_PIC_HUE1_MIN, (UINT32)ini.mask1Min);
	cSharedData->SetParam(PARAM_ID_PIC_HUE1_MAX, (UINT32)ini.mask1Max);
	cSharedData->SetParam(PARAM_ID_PIC_HUE2_EN, (UINT32)ini.mask2En);
	cSharedData->SetParam(PARAM_ID_PIC_HUE2_MIN, (UINT32)ini.mask2Min);
	cSharedData->SetParam(PARAM_ID_PIC_HUE2_MAX, (UINT32)ini.mask2Max);
	cSharedData->SetParam(PARAM_ID_PIC_HUE3_EN, (UINT32)ini.mask3En);
	cSharedData->SetParam(PARAM_ID_PIC_HUE3_MIN, (UINT32)ini.mask3Min);
	cSharedData->SetParam(PARAM_ID_PIC_HUE3_MAX, (UINT32)ini.mask3Max);
	cSharedData->SetParam(PARAM_ID_PIC_COG_ALGO, (UINT32)ini.procAlgo);

	char* cstr = (char*)malloc(sizeof(ini.rioIpAddr));
	if (cstr != NULL) {
		size_t size;
		wcstombs_s(&size, cstr, sizeof(ini.rioIpAddr), ini.rioIpAddr, sizeof(ini.rioIpAddr));
		string str = cstr;
		cSharedData->SetParam(PARAM_ID_STR_RIO_IPADDR, str);
		free(cstr);
	}
	cSharedData->SetParam(PARAM_ID_RIO_TCPPORT, (UINT32)ini.rioTcpPortNum);
	cSharedData->SetParam(PARAM_ID_RIO_SLAVEADDR, (UINT32)ini.rioSlaveAddr);
	cSharedData->SetParam(PARAM_ID_RIO_TIMEOUT, (UINT32)ini.rioTimeOut);
	cSharedData->SetParam(PARAM_ID_RIO_XPORT, (UINT32)ini.rioXPortNum);
	cSharedData->SetParam(PARAM_ID_RIO_YPORT, (UINT32)ini.rioYPortNum);

#endif
}
