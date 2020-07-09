// SwaySensorA.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "SwaySensorA.h"
#include "inifile.h"
#include "CHelper.h"

#include "CTaskObj.h"	// タスククラス

#include <windowsx.h>	// コモンコントロール用
#include <commctrl.h>	// コモンコントロール用
#include <shlwapi.h>	// Win32 APIでパスを扱うには shlwapi.h に定義されている関数群(Path Routines)を使用

using namespace std;

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE   hInst;                          // 現在のインターフェイス
WCHAR       szTitle[MAX_LOADSTRING];        // タイトル バーのテキスト
WCHAR       szWindowClass[MAX_LOADSTRING];  // メイン ウィンドウ クラス名

vector<void*>   VectpCTaskObj;      // タスクオブジェクトのポインタ
CSharedObject*  g_pSharedObject;    // タスク間共有データのポインタ
ST_iTask        g_itask;            // タスクID参照用グローバル変数

SYSTEMTIME  gAppStartTime;  // アプリケーション開始時間
LPWSTR      pszInifile;     // iniファイルのパス
wstring     wstrPathExe;    // 実行ファイルのパス

// スタティック変数:
// マルチタスク管理用
static ST_KNL_MANAGE_SET    knl_manage_set; // マルチスレッド管理用構造体
static vector<HANDLE>       VectHevent;     // マルチスレッド用イベントのハンドル
static CSharedObject*       cSharedData;    // 共有オブジェクトインスタンス
// メインウィンドウパネル用
static vector<HWND> VectTweetHandle;    // メインウィンドウのスレッドツイートメッセージ表示Staticハンドル
static HIMAGELIST   hImgListTaskIcon;   // タスクアイコン用イメージリスト
static HWND         hTabWnd;            // 操作パネル用タブコントロールウィンドウのハンドル
static HWND         hWnd_status_bar;    // ステータスバーのウィンドウのハンドル

// このコード モジュールに含まれる関数の宣言を転送します:
// # アプリケーション専用関数:	************************************
// コア関数
VOID    CALLBACK alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2); // マルチメディアタイマ処理関数　スレッドのイベントオブジェクト処理
int     Init_tasks(HWND hWnd, HINSTANCE hInstance);                                     // アプリケーション毎のタスクオブジェクトの初期設定
DWORD   knlTaskStartUp();                                                               // 実行させるタスクの起動処理
INT	    setIniParameter(ST_INI_INF* pInf, LPCWSTR pFileName);                           // iniファイルパラメータ設定処理
void    CreateSharedData(void);                                                         // 共有メモリCreate処理
static unsigned __stdcall thread_gate_func(void * pObj)                                 // スレッド実行のためのゲート関数
{
    CTaskObj* pthread_obj = (CTaskObj*)pObj;
    return pthread_obj->run(pObj);
}

// メインウィンドウパネル用
LRESULT CALLBACK TaskTabDlgProc(HWND, UINT, WPARAM, LPARAM);                            // 個別タスク設定タブウィンドウ用メッセージハンドリング関数
HWND    CreateStatusbarMain(HWND);                                                      // メインウィンドウステータスバー作成関数
HWND    CreateTaskSettingWnd(HWND hWnd);

// # Wizard Default関数:		************************************
ATOM                MyRegisterClass(HINSTANCE hInstance);								// ウィンドウ クラスを登録します。
BOOL                InitInstance(HINSTANCE, int);										// メインウィンドウクリエイト
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);								// ウィンドウプロシージャ
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/// @brief 
/// @param
/// @return 
/// @note
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

    for (unsigned i = 0; i < knl_manage_set.num_of_task; i++)
    {
        CTaskObj* pobj = (CTaskObj*)VectpCTaskObj[i];
        if (pobj != NULL) { delete pobj; }
    }

    return (int) msg.wParam;
}

/// @brief ウィンドウ クラスを登録します。
/// @param
/// @return 
/// @note
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

/// @brief インスタンス ハンドルを保存して、メイン ウィンドウを作成します
/// @param
/// @return 
/// @note この関数で、グローバル変数でインスタンス ハンドルを保存し、メイン プログラム ウィンドウを作成および表示します。
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;				// グローバル変数にインスタンス ハンドルを格納する

    GetSystemTime(&gAppStartTime);	// アプリケーション開始時刻取得

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,                     // lpClassName, lpWindowName, dwStyle,
                              MAIN_WND_INIT_POS_X, MAIN_WND_INIT_POS_Y,                                          // x, y, 
                              TAB_DIALOG_W + 40, (MSG_WND_H + MSG_WND_Y_SPACE)*TASK_NUM + TAB_DIALOG_H + 110,    // Width, nHeight,
                              nullptr, nullptr, hInstance, nullptr);                                             // hWndParent, hMenu, hInstance, lpParam

    if (!hWnd)  return FALSE;

    g_pSharedObject = new CSharedObject();
    // -タスク設定	
    Init_tasks(hWnd, hInst); // タスク個別設定

     // WM_PAINTを発生させてアイコンを描画させる
    InvalidateRect(hWnd, NULL, FALSE);

    // 実行させる各オブジェクトのスレッドを起動 マルチメディアタイマー起動
    knlTaskStartUp();

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

   return TRUE;
}

/// @brief メイン ウィンドウのメッセージを処理します。
/// @param
/// @return 
/// @note WM_COMMAND  - アプリケーション メニューの処理
///       WM_PAINT    - メイン ウィンドウを描画する
///       WM_DESTROY  - 中止メッセージを表示して戻る
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // メインウィンドウにステータスバー付加
        hWnd_status_bar = CreateStatusbarMain(hWnd);
        break;
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
    case WM_NOTIFY:
        {
            int tab_index = TabCtrl_GetCurSel(((NMHDR *)lParam)->hwndFrom); // 選択タブID取得

            for (unsigned i = 0; i < VectpCTaskObj.size(); i++)
            {
                CTaskObj* pObj = (CTaskObj*)VectpCTaskObj[i];
                //              MoveWindow(pObj->inf.hWnd_opepane, TAB_POS_X, TAB_POS_Y + TAB_SIZE_H, TAB_DIALOG_W, TAB_DIALOG_H - TAB_SIZE_H, TRUE);
                if ((VectpCTaskObj.size() - 1 - pObj->inf.index) == tab_index)
                {
                    // タブ選択されたパネルウィンドウ表示
                    ShowWindow(pObj->inf.hWnd_opepane, SW_SHOW);
                    // パネルウィンドウにタブ選択されたパネルウィンドウ表示
                    HWND hname_static = GetDlgItem(pObj->inf.hWnd_opepane, IDC_STATIC_TASKNAME);
                    SetWindowText(hname_static, pObj->inf.name);
                    pObj->set_panel_pb_txt();
#if 0
                    // 実行関数の設定状況に応じてOption Checkボタンセット
                    if (pObj->inf.work_select == THREAD_WORK_OPTION1)
                    {
                        SendMessage(GetDlgItem(pObj->inf.hWnd_opepane, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
                        SendMessage(GetDlgItem(pObj->inf.hWnd_opepane, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
                    }
                    else if (pObj->inf.work_select == THREAD_WORK_OPTION2)
                    {
                        SendMessage(GetDlgItem(pObj->inf.hWnd_opepane, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
                        SendMessage(GetDlgItem(pObj->inf.hWnd_opepane, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_CHECKED, 0L);
                    }
                    else
                    {
                        SendMessage(GetDlgItem(pObj->inf.hWnd_opepane, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
                        SendMessage(GetDlgItem(pObj->inf.hWnd_opepane, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
                    }
#endif
                    // ウィンドウをフォアグラウンドに持ってくる 
                    SetForegroundWindow(pObj->inf.hWnd_opepane);
                }
                else
                {
                    ShowWindow(pObj->inf.hWnd_opepane, SW_HIDE);
                }
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            // タスクツィートメッセージアイコン描画
            for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) { ImageList_Draw(hImgListTaskIcon, i, hdc, 0, i*(MSG_WND_H + MSG_WND_Y_SPACE), ILD_NORMAL); }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        // 各タスクスレッド停止
        for (unsigned i = 0; i < knl_manage_set.num_of_task; i++)
        {
            CTaskObj* pobj = (CTaskObj*)VectpCTaskObj[i];
            pobj->inf.thread_com = TERMINATE_THREAD;
        }
        Sleep(100);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/// @brief バージョン情報ボックスのメッセージ ハンドラーです。
/// @param
/// @return 
/// @note 
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

/// @brief スレッドタスクの登録、設定
/// @param
/// @return 
/// @note 
int Init_tasks(HWND hWnd, HINSTANCE hInstance)
{
    HBITMAP     hBmp;
    CTaskObj*   ptempobj;
    int         task_index = 0;

    // コモンコントロール初期化
    InitCommonControls();
    // タスクアイコン用イメージリストクリエイト
    hImgListTaskIcon = ImageList_Create(32, 32, ILC_COLOR | ILC_MASK, 2, 0);

    CreateSharedData(); // 共有メモリクリエイト

    DWORD   str_num;
    // ###Task1 設定 MANAGER
#pragma region Task1
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CManager;
    VectpCTaskObj.push_back((void*)ptempobj);   // オブジェクトのポインタセット
    g_itask.mng = task_index;                   // タスクIDセット

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト
    // 定周期タイマーイベント
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // オプションイベント
//  ptempobj->inf.hevents[ID_OPT1_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL);   // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 1000;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MANAGER));    // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  MANAGER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, MANAGER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;
    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task1

    // ###Task2 設定 PLAYER
#pragma region Task2
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CPlayer;
    VectpCTaskObj.push_back((void*)ptempobj);
    g_itask.ply = task_index;

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト->リスト登録	
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 50;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYER)); // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  PLAYER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, PLAYER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task2

    // ###Task3 設定 COMCLIENT
#pragma region Task3
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CComClient;
    VectpCTaskObj.push_back((void*)ptempobj);
    g_itask.comc = task_index;

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト->リスト登録	
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 1000;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_COMCLIENT));  // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  CLIENT_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CLIENT_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task3

    // ###Task4 設定 COMRIO
#pragma region Task4
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CComRIO;
    VectpCTaskObj.push_back((void*)ptempobj);
    g_itask.comd1 = task_index;

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト->リスト登録	
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 50;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RIOCOM)); // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  RIO_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, RIO_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task4

    // ###Task5 設定 COMCCAMERA
#pragma region Task5
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CComCamera;
    VectpCTaskObj.push_back((void*)ptempobj);
    g_itask.comd2 = task_index;

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト->リスト登録	
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 50;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CAMERACOM));  // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  CAMERA_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CAMERA_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task5

    // ###Task6 設定 CLERK
#pragma region Task6
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CClerk();
    VectpCTaskObj.push_back((void*)ptempobj);
    g_itask.clerk = task_index;

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト->リスト登録	
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 100;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CLERK));  // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  CLERK_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CLERK_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task6

    // ###Task7 設定 ANALYST
#pragma region Task7
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CAnalyst;
    VectpCTaskObj.push_back((void*)ptempobj);
    g_itask.ana = task_index;

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト->リスト登録	
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 50;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ANALYST));    // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  ANALYST_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, ANALYST_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task7

    // ###Task8 設定 PUBLICRELATION
#pragma region Task8
    // -タスクインスタンス作成->リスト登録
    ptempobj = new CPublicRelation();
    VectpCTaskObj.push_back((void*)ptempobj);
    g_itask.pr = task_index;

    // -タスクインデクスセット
    ptempobj->inf.index = task_index++;

    // -イベントオブジェクトクリエイト->リスト登録	
    VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));    // 自動リセット,初期値非シグナル

    // -スレッド起動周期セット
    ptempobj->inf.cycle_ms = 50;

    // -ツイートメッセージ用iconセット
    hBmp = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PR)); // ビットマップ割り当て
    ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
    DeleteObject(hBmp);

    // オブジェクト名セット
    str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE,  PR_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name,  sizeof(ptempobj->inf.name)  / 2, PATH_OF_INIFILE);
    str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, PR_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

    // 実行関数選択
    ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

    // スレッド起動に使うイベント数（定周期タイマーのみの場合１）
    ptempobj->inf.n_active_events = 1;
#pragma endregion Task8

    hTabWnd = CreateTaskSettingWnd(hWnd);   // タブウィンドウ作成

    // 設定タスク数登録
    knl_manage_set.num_of_task = (unsigned int)VectpCTaskObj.size();

    // 各タスク用残初期設定
    for (unsigned i = 0; i < knl_manage_set.num_of_task; i++)
    {
        CTaskObj* pobj = (CTaskObj*)VectpCTaskObj[i];

        pobj->inf.index = i;    // 設定順でタスクインデックスセット

        pobj->inf.hWnd_parent = hWnd;       // 親ウィンドウのハンドルセット
        pobj->inf.hInstance = hInstance;    // アプリケーションのハンドル
        // -ツイートメッセージ用Static window作成->リスト登録	
        pobj->inf.hWnd_msgStatics = CreateWindow(L"STATIC", L"...", WS_CHILD | WS_VISIBLE, MSG_WND_ORG_X, MSG_WND_ORG_Y + MSG_WND_H * i + i * MSG_WND_Y_SPACE, MSG_WND_W, MSG_WND_H, hWnd, (HMENU)ID_STATIC_MAIN, hInst, NULL);
        VectTweetHandle.push_back(pobj->inf.hWnd_msgStatics);

        // その他設定
        pobj->inf.psys_counter = &knl_manage_set.sys_counter;   // システムカウンタ(基本周期でカウント）参照アドレスセット
        pobj->inf.act_count = 0;                                // 起動チェック用カウンタリセット

        if (pobj->inf.cycle_ms >= SYSTEM_TICK_ms) {pobj->inf.cycle_count = pobj->inf.cycle_ms / SYSTEM_TICK_ms;}   // タスクスレッド起動を掛けるカウント値設定
        else                                      {pobj->inf.cycle_count = 1;}

        pobj->init_task(pobj);  // 最後にタスク固有初期化関数呼び出し
    }

    return 1;
}

/// @brief [KNL]システム/ユーザタスクスタートアップ関数
/// @param
/// @return Win32APIエラーコード
/// @note 自プロセスのプライオリティ設定，カーネルの初期設定,タスク生成，基本周期設定
DWORD knlTaskStartUp()
{
    HANDLE  myPrcsHndl; // 本プログラムのプロセスハンドル
    // #自プロセスプライオリティ設定処理
    // -プロセスハンドル取得
    if ((myPrcsHndl = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION, FALSE, _getpid())) == NULL) { return(GetLastError()); }
    _RPT1(_CRT_WARN, "KNL Priority For Windows(before) = %d \n", GetPriorityClass(myPrcsHndl)); // VisualStudio 出力

    // -自プロセスのプライオリティを最優先ランクに設定
    if (SetPriorityClass(myPrcsHndl, REALTIME_PRIORITY_CLASS) == 0) { return(GetLastError()); }
    _RPT1(_CRT_WARN, "KNL Priority For NT(after) = %d \n", GetPriorityClass(myPrcsHndl));   // VisualStudio 出力

    // #アプリケーションタスク数が最大数を超えた場合は終了
    if (VectpCTaskObj.size() >= MAX_APL_TASK) { return((DWORD)ERROR_BAD_ENVIRONMENT); }

    // #アプリケーションスレッド生成処理	
    for (unsigned i = 0; i < VectpCTaskObj.size(); i++)
    {
        CTaskObj* pobj = (CTaskObj*)VectpCTaskObj[i];

        // タスクスレッド生成
        pobj->inf.hndl = (HANDLE)_beginthreadex((void *)NULL,                   // 他ﾌﾟﾛｾｽとの共有なし
                                                0,                              // スタック初期サイズ　デフォルト
                                                thread_gate_func,               // スレッド実行関数　引数で渡すオブジェクトで対象切り替え
                                                VectpCTaskObj[i],               // スレッド関数に渡すの引数（対象のオブジェクトのポインタ）
                                                (unsigned)0,                    // 即実行Createflags
                                                (unsigned *)&(pobj->inf.ID));   // スレッドID取り込み

        // タスクプライオリティ設定
        if (SetThreadPriority(pobj->inf.hndl, pobj->inf.priority) == 0) { return(GetLastError()); }
        _RPT2(_CRT_WARN, "Task[%d]_priority = %d\n", i, GetThreadPriority(pobj->inf.hndl));

        pobj->inf.act_count = 0;  // 基本ティックのカウンタ変数クリア
        pobj->inf.time_over_count = 0;  // 予定周期オーバーカウントクリア
    }

    // -マルチメディアタイマ起動
    {
        // > マルチメディアタイマ精度設定
        TIMECAPS wTc;   // マルチメディアタイマ精度構造体
        // ハードウェアのタイマー能力チェック
        if (timeGetDevCaps(&wTc, sizeof(TIMECAPS)) != TIMERR_NOERROR) { return((DWORD)FALSE); }

        knl_manage_set.mmt_resolution = MIN(MAX(wTc.wPeriodMin, TARGET_RESOLUTION), wTc.wPeriodMax);
        if (timeBeginPeriod(knl_manage_set.mmt_resolution) != TIMERR_NOERROR) { return((DWORD)FALSE); }
        _RPT1(_CRT_WARN, "MMTimer Resolution = %d\n", knl_manage_set.mmt_resolution);

        // > マルチメディアタイマセット
        knl_manage_set.KnlTick_TimerID = timeSetEvent(knl_manage_set.cycle_base,        // uDelay
                                                      knl_manage_set.mmt_resolution,    // uResolution
                                                      (LPTIMECALLBACK)alarmHandlar,     // lpTimeProc コールバック関数のアドレス
                                                      0,                                // dwUser ユーザー変数。このパラメータはコールバック関数に伝えられる
                                                      TIME_PERIODIC);                   // タイマーの動作モード

        // >マルチメディアタイマー起動失敗判定 メッセージBOX出してFALSE returen
        if (knl_manage_set.KnlTick_TimerID == 0)    // 失敗確認表示
        {
            LPVOID lpMsgBuf;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language*/(LPTSTR)&lpMsgBuf, 0, NULL);
                          MessageBox(NULL, (LPCWSTR)lpMsgBuf, L"MMT Failed!!", MB_OK | MB_ICONINFORMATION);   // Display the string.
            LocalFree(lpMsgBuf);    // Free the buffer.
            return((DWORD)FALSE);
        }
    }
    return 1;
}

/// @brief マルチメディアタイマーイベント処理関数
/// @param
/// @return 
/// @note 
VOID CALLBACK alarmHandlar(UINT uID, UINT uMsg, DWORD	dwUser, DWORD dw1, DWORD dw2)
{
    LONG64 tmttl;
    knl_manage_set.sys_counter++;   // システムカウンタ更新

    // スレッド再開イベントセット処理
    for (unsigned i = 0; i < knl_manage_set.num_of_task; i++)   // 登録した各タスクチェック
    {
        CTaskObj* pobj = (CTaskObj*)VectpCTaskObj[i];
        pobj->inf.act_count++;
        if (pobj->inf.act_count >= pobj->inf.cycle_count)   // 登録したタスクの周期カウンタチェック
        {
            PulseEvent(VectHevent[i]);  // イベント送信してタスクスレッドのループを回す
            pobj->inf.act_count = 0;    // タスクの周期カウンタリセット
            pobj->inf.total_act++;      // タスクスレッド起動回数カウンタ更新
        }
    }

    // Statusバーに経過時間表示
    if (knl_manage_set.sys_counter % 40 == 0)   // 1sec毎
    {
        // 起動後経過時間計算
        tmttl = knl_manage_set.sys_counter * knl_manage_set.cycle_base; // アプリケーション起動後の経過時間msec
        knl_manage_set.Knl_Time.wMilliseconds = (WORD)(tmttl % 1000);   tmttl /= 1000;
        knl_manage_set.Knl_Time.wSecond = (WORD)(tmttl % 60);     tmttl /= 60;
        knl_manage_set.Knl_Time.wMinute = (WORD)(tmttl % 60);     tmttl /= 60;
        knl_manage_set.Knl_Time.wHour = (WORD)(tmttl % 60);     tmttl /= 24;
        knl_manage_set.Knl_Time.wDay = (WORD)(tmttl % 24);

        TCHAR tbuf[32];
        wsprintf(tbuf, L"%3dD %02d:%02d:%02d", knl_manage_set.Knl_Time.wDay, knl_manage_set.Knl_Time.wHour, knl_manage_set.Knl_Time.wMinute, knl_manage_set.Knl_Time.wSecond);
        SendMessage(hWnd_status_bar, SB_SETTEXT, 5, (LPARAM)tbuf);
    }
}

/// @brief ini file読み込みパラメータ設定
/// @param
/// @return 
/// @note 
INT setIniParameter(ST_INI_INF* pInf, LPCWSTR pFileName)
{
    TCHAR   str[256];

    //--------------------------------------------------------------------------
    // 設定ファイル存在チェック
    if (!(PathFileExists(pFileName)) || PathIsDirectory(pFileName)) { return RESULT_NG_INVALID; }

    //--------------------------------------------------------------------------
    // カメラ設定
    CHelper::GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_WIDTH,     L"640",       INITYPE_INT, &(pInf->camWidth));     // カメラ撮影横幅
    CHelper::GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_HEIGHT,    L"480",       INITYPE_INT, &(pInf->camHeight));    // カメラ撮影高さ
    CHelper::GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_FRAMERATE, L"30",        INITYPE_INT, &(pInf->frameRate));    // フレームレート
    CHelper::GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_EXPOSURE,  L"1,1000,10", INITYPE_CHAR, str);  // 露光時間
    if (3 != _stscanf_s(str, _T("%d,%d,%d"), &pInf->expTimeMin, &pInf->expTimeMax, &pInf->expTime))
    {
        pInf->expTimeMin = 1;
        pInf->expTimeMax = 1000;
        pInf->expTime    = 10;
    }

    //--------------------------------------------------------------------------
    // 画像処理設定
    // ROI有効
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_ROI, L"1,100", INITYPE_CHAR, str);                   // ROI設定
    // 画像1設定
    if (2 != _stscanf_s(str, _T("%d,%d"), &pInf->roiEnable, &pInf->roiSize))
    {
        pInf->roiEnable = 0;
        pInf->roiSize   = 10;
    }
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_MASK1_LOW, L"0,0,0", INITYPE_CHAR, str);               // 画像1マスク下限(H,S,V)
    if (3 != _stscanf_s(str, _T("%d,%d,%d"), &pInf->mask1HLow, &pInf->mask1SLow, &pInf->mask1VLow))
    {
        pInf->mask1HLow = 0;
        pInf->mask1SLow = 0;
        pInf->mask1VLow = 0;
    }
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_MASK1_UPP, L"0,0,0", INITYPE_CHAR, str);               // 画像1マスク上限(H,S,V)
    if (3 != _stscanf_s(str, _T("%d,%d,%d"), &pInf->mask1HUpp, &pInf->mask1SUpp, &pInf->mask1VUpp))
    {
        pInf->mask1HUpp = 0;
        pInf->mask1SUpp = 0;
        pInf->mask1VUpp = 0;
    }
    // 画像2設定
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_MASK2_LOW, L"0,0,0", INITYPE_CHAR, str);               // 画像2マスク下限(H,S,V)
    if (3 != _stscanf_s(str, _T("%d,%d,%d"), &pInf->mask2HLow, &pInf->mask2SLow, &pInf->mask2VLow))
    {
        pInf->mask2HLow = 0;
        pInf->mask2SLow = 0;
        pInf->mask2VLow = 0;
    }
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_MASK2_UPP, L"0,0,0", INITYPE_CHAR, str);               // 画像2マスク上限(H,S,V)
    if (3 != _stscanf_s(str, _T("%d,%d,%d"), &pInf->mask2HUpp, &pInf->mask2SUpp, &pInf->mask2VUpp))
    {
        pInf->mask2HUpp = 0;
        pInf->mask2SUpp = 0;
        pInf->mask2VUpp = 0;
    }
    // ノイズフィルタ(ゴマ塩)
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_NOISEFILTER1,  L"0,1", INITYPE_CHAR, str);  // ノイズフィルタ
    if (2 != _stscanf_s(str, _T("%d,%d"), &pInf->noiseFilter1, &pInf->noiseFilterVal1))
    {
        pInf->noiseFilter1    = NOISEFILTER1_NONE;
        pInf->noiseFilterVal1 = 1;
    }
    // ノイズフィルタ(穴埋め)
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_NOISEFILTER2,  L"0,1", INITYPE_CHAR, str);  // ノイズフィルタ
    if (2 != _stscanf_s(str, _T("%d,%d"), &pInf->noiseFilter2, &pInf->noiseFilterVal2))
    {
        pInf->noiseFilter1    = NOISEFILTER2_NONE;
        pInf->noiseFilterVal1 = 1;
    }
    // ターゲット検出アルゴリズム
    CHelper::GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_IMG_ALGORITHM, L"1", INITYPE_INT, &(pInf->algorithm)); // ターゲット検出アルゴリズム
    if ((pInf->algorithm < COG_ALGORITHM_ALL) || (pInf->algorithm >= COG_ALGORITHM_MAX)) {pInf->algorithm = COG_ALGORITHM_ALL;}

    //--------------------------------------------------------------------------
    // RIO設定
    CHelper::GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_IPADDR,     L"192.168.0.1", INITYPE_CHAR, &(pInf->rioIpAddr));       // RIO IPアドレス
    CHelper::GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_TCPPORTNUM, L"502",         INITYPE_INT,  &(pInf->rioTcpPortNum));   // RIO TCPポート番号
    CHelper::GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_SLAVEADDR,  L"1",           INITYPE_INT,  &(pInf->rioSlaveAddr));    // RIOスレーブアドレス
    CHelper::GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_TIMEOUT,    L"2000",        INITYPE_INT,  &(pInf->rioTimeOut));      // RIOタイムアウト
    CHelper::GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_XPORTNUM,   L"1",           INITYPE_INT,  &(pInf->rioXPortNum));     // RIO傾斜計Xデータ接続ポート番号
    CHelper::GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_YPORTNUM,   L"2",           INITYPE_INT,  &(pInf->rioYPortNum));     // RIO傾斜計Yデータ接続ポート番号

    return S_OK;
}

/// @brief 共有オブジェクト初期化
/// @param
/// @return 
/// @note 
void CreateSharedData(void)
{
    // ini file path
    static WCHAR dstpath[_MAX_PATH], szDrive[_MAX_DRIVE], szPath[_MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];
    // 実行ファイルのパス取得
    GetModuleFileName(NULL, dstpath, (sizeof(TCHAR) * _MAX_PATH) / 2);
    // 実行ファイルのパスを分割
    _wsplitpath_s(dstpath, szDrive, sizeof(szDrive) / 2, szPath, sizeof(szPath) / 2, szFName, sizeof(szFName) / 2, szExt, sizeof(szExt) / 2);
    // ini file path構成
    wstrPathExe = szDrive; wstrPathExe += szPath;
    _wmakepath_s(dstpath, sizeof(dstpath) / 2, szDrive, szPath, NAME_OF_INIFAILE_APP, EXT_OF_INIFILE);
    pszInifile = dstpath;

    ST_INI_INF ini;
    setIniParameter(&ini, dstpath);

//  cSharedData = new CSharedObject();
    g_pSharedObject->InitSharedObject();

    //--------------------------------------------------------------------------
    // カメラ設定
    g_pSharedObject->SetParam(PARAM_ID_CAM_WIDTH,             (UINT32)ini.camWidth);
    g_pSharedObject->SetParam(PARAM_ID_CAM_HEIGHT,            (UINT32)ini.camHeight);
    g_pSharedObject->SetParam(PARAM_ID_CAM_FRAMERATE,         (UINT32)ini.frameRate);
    g_pSharedObject->SetParam(PARAM_ID_CAM_EXPOSURE_TIME_MIN, (UINT32)ini.expTimeMin);
    g_pSharedObject->SetParam(PARAM_ID_CAM_EXPOSURE_TIME_MAX, (UINT32)ini.expTimeMax);
    g_pSharedObject->SetParam(PARAM_ID_CAM_EXPOSURE_TIME,     (UINT32)ini.expTime);

    //--------------------------------------------------------------------------
    // 画像処理設定
    g_pSharedObject->SetParam(PARAM_ID_IMG_ROI_ENABLE,      (UINT32)ini.roiEnable);
    g_pSharedObject->SetParam(PARAM_ID_IMG_ROI_SIZE,        (UINT32)ini.roiSize);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_HLOW,      (UINT32)ini.mask1HLow);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_HUPP,      (UINT32)ini.mask1HUpp);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_SLOW,      (UINT32)ini.mask1SLow);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_SUPP,      (UINT32)ini.mask1SUpp);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_VLOW,      (UINT32)ini.mask1VLow);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_VUPP,      (UINT32)ini.mask1VUpp);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_HLOW,      (UINT32)ini.mask2HLow);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_HUPP,      (UINT32)ini.mask2HUpp);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_SLOW,      (UINT32)ini.mask2SLow);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_SUPP,      (UINT32)ini.mask2SUpp);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_VLOW,      (UINT32)ini.mask2VLow);
    g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_VUPP,      (UINT32)ini.mask2VUpp);
    g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTER1,    (UINT32)ini.noiseFilter1);
    g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTERVAL1, (UINT32)ini.noiseFilterVal1);
    g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTER2,    (UINT32)ini.noiseFilter2);
    g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTERVAL2, (UINT32)ini.noiseFilterVal2);
    g_pSharedObject->SetParam(PARAM_ID_IMG_ALGORITHM,       (UINT32)ini.algorithm);

    //--------------------------------------------------------------------------
    // RIO設定
    char* cstr = (char*)malloc(sizeof(ini.rioIpAddr));
    if (cstr != NULL)
    {
        size_t size;
        wcstombs_s(&size, cstr, sizeof(ini.rioIpAddr), ini.rioIpAddr, sizeof(ini.rioIpAddr));
        string str = cstr;
        g_pSharedObject->SetParam(PARAM_ID_STR_RIO_IPADDR, str);
        free(cstr);
    }
    g_pSharedObject->SetParam(PARAM_ID_RIO_TCPPORT,   (UINT32)ini.rioTcpPortNum);
    g_pSharedObject->SetParam(PARAM_ID_RIO_SLAVEADDR, (UINT32)ini.rioSlaveAddr);
    g_pSharedObject->SetParam(PARAM_ID_RIO_TIMEOUT,   (UINT32)ini.rioTimeOut);
    g_pSharedObject->SetParam(PARAM_ID_RIO_XPORT,     (UINT32)ini.rioXPortNum);
    g_pSharedObject->SetParam(PARAM_ID_RIO_YPORT,     (UINT32)ini.rioYPortNum);
}

/// @brief ステータスバー作成
/// @param
/// @return 
/// @note 
HWND CreateStatusbarMain(HWND hWnd)
{
    HWND hSBWnd;
    int sb_size[] = { 100, 200, 300, 400, 525, 615 }; // ステータス区切り位置

    InitCommonControls();
    hSBWnd = CreateWindowEx(0,                          // 拡張スタイル
                            STATUSCLASSNAME,            // ウィンドウクラス
                            NULL,                       // タイトル
                            WS_CHILD | SBS_SIZEGRIP,    // ウィンドウスタイル
                            0, 0,                       // 位置
                            0, 0,                       // 幅、高さ
                            hWnd,                       // 親ウィンドウ
                            (HMENU)ID_STATUS,           // ウィンドウのＩＤ
                            hInst,                      // インスタンスハンドル
                            NULL);
    SendMessage(hSBWnd, SB_SETPARTS, (WPARAM)6, (LPARAM)(LPINT)sb_size);    // 6枠で各枠の仕切り位置をパラーメータ指定
    ShowWindow(hSBWnd, SW_SHOW);
    return hSBWnd;
}

/// @brief タブ付タスクウィンドウ作成
/// @param
/// @return 
/// @note 
HWND CreateTaskSettingWnd(HWND hWnd)
{
    RECT    rc;
    TC_ITEM tc[TASK_NUM];

    GetClientRect(hWnd, &rc);
    HWND hTab = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_BUTTONS,
                               rc.left + TAB_POS_X, rc.top + TAB_POS_Y, TAB_DIALOG_W, TAB_DIALOG_H,
                               hWnd, (HMENU)ID_TASK_SET_TAB, hInst, NULL);

    for (unsigned i = 0; i < VectpCTaskObj.size(); i++) // Task Setting用タブ作成
    {
        CTaskObj* pObj = (CTaskObj*)VectpCTaskObj[i];

        tc[i].mask = (TCIF_TEXT | TCIF_IMAGE);
        tc[i].pszText = pObj->inf.sname;
        tc[i].iImage = pObj->inf.index;
        SendMessage(hTab, TCM_INSERTITEM, (WPARAM)0, (LPARAM)&tc[i]);
        pObj->inf.hWnd_opepane = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_TASKSET), hWnd, (DLGPROC)TaskTabDlgProc);
        pObj->set_panel_pb_txt();
        MoveWindow(pObj->inf.hWnd_opepane, TAB_POS_X, TAB_POS_Y + TAB_SIZE_H, TAB_DIALOG_W, TAB_DIALOG_H - TAB_SIZE_H, TRUE);

        // 初期値はindex 最後のウィンドウを表示
        if (i == VectpCTaskObj.size() - 1)
        {
            ShowWindow(pObj->inf.hWnd_opepane, SW_SHOW);
            SetWindowText(GetDlgItem(pObj->inf.hWnd_opepane, IDC_STATIC_TASKNAME), pObj->inf.name); // タスク名をスタティックテキストに表示
        }
        else
        {
            ShowWindow(pObj->inf.hWnd_opepane, SW_HIDE);
        }
    }

    // タブコントロールにイメージリストセット
    SendMessage(hTab, TCM_SETIMAGELIST, (WPARAM)0, (LPARAM)hImgListTaskIcon);

    return hTab;
}

static LVCOLUMNA lvcol;

/// @brief 
/// @param
/// @return 
/// @note 
LRESULT CALLBACK TaskTabDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        {
            InitCommonControls();

            // メッセージ用リスト
            LVCOLUMN lvcol;
            LPTSTR strItem0[] = { (LPTSTR)(L"time"), (LPTSTR)(L"message") };  // 列ラベル
            int CX[] = { 60, 600 };   // 列幅

            // リストコントロール設定
            HWND hList = GetDlgItem(hDlg, IDC_LIST_TASK);
            lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvcol.fmt  = LVCFMT_LEFT;
            for (int i = 0; i < 2; i++)
            {
                lvcol.cx       = CX[i];         // 表示位置
                lvcol.pszText  = strItem0[i];   // 見出し
                lvcol.iSubItem = i;             // サブアイテムの番号
                ListView_InsertColumn(hList, i, &lvcol);
            }
            // リスト行追加
            LVITEM item;
            item.mask = LVIF_TEXT;
            for (int i = 0; i < MSG_LIST_MAX; i++)
            {
                item.pszText  = (LPWSTR)L".";   // テキスト
                item.iItem    = i;              // 番号
                item.iSubItem = 0;              // サブアイテムの番号
                ListView_InsertItem(hList, &item);
            }
            return TRUE;
        }
        break;
    case WM_COMMAND:
        {
            CTaskObj* pObj = (CTaskObj*)VectpCTaskObj[VectpCTaskObj.size() - TabCtrl_GetCurSel(hTabWnd) - 1];
            pObj->PanelProc(hDlg, msg, wp, lp);
        }
        break;
    }
    return FALSE;
}
