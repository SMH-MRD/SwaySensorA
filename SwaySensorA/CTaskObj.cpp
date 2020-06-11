#include "CTaskObj.h"
#include "CHelper.h"
#include "Resource.h"


/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CTaskObj::CTaskObj()
{
    // タスク関連の初期値設定
    inf.hndl       = NULL;                      // スレッドハンドル
    inf.cycle_ms   = DEFAUT_TASK_CYCLE;         // スレッドの呼び出し周期
    inf.thread_com = REPEAT_INFINIT;            // スレッドの呼び出しタイプ
    wsprintf(inf.name, L"Who am I?");           // タスク名
    inf.act_count   = inf.act_time = 0;         // スレッド周期制御用カウンタリセット
    inf.priority    = THREAD_PRIORITY_NORMAL;   // タスクプライオリティ設定（将来用）
    inf.work_select = THREAD_WORK_IDLE;         // スレッド実行時の処理関数
    inf.hWnd_parent = inf.hWnd_work = inf.hWnd_opepane = inf.hWnd_msgStatics = NULL;    // 各種表示関連ハンドル
    inf.hBmp        = NULL;                     // タスクアイコン用ビットマップハンドル
}

/// @brief デストラクタ
/// @param
/// @return 
/// @note
CTaskObj::~CTaskObj()
{
    delete inf.hBmp;
}

/// @brief 
/// @param
/// @return 
/// @note
void CTaskObj::init_task(void *pobj)
{
    return;
}

/// @brief 
/// @param
/// @return 
/// @note
unsigned CTaskObj::run(void *param)
{
    while (this->inf.thread_com != TERMINATE_THREAD)
    {
        //SIGNAL状態待ち
        inf.event_triggered = WaitForMultipleObjects(inf.n_active_events,               // 対象オブジェクト数
                                                     ((CTaskObj*)param)->inf.hevents,   // 対象オブジェクトハンドルの配列 
                                                     FALSE,                             // TRUE：ALL　Object　FALSE：Any of Object 
                                                     INFINITE);                         // SIGNAL待ち時間　msec

        //処理周期確認用
        DWORD   start_time = timeGetTime();
        inf.period     = start_time - inf.start_time;
        inf.start_time = start_time;

        routine_work(param);

        inf.act_time = timeGetTime() - start_time;  // 実処理時間測定用
        if (this->inf.thread_com != REPEAT_INFINIT) {break;}    // 逐次処理の場合は抜ける
    }
    return 0;
}

/// @brief タスクスレッドでの処理関数
/// @param
/// @return 
/// @note
void CTaskObj::routine_work(void *param)
{
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();
    return;
}

/// @brief メインウィンドウへのショートメッセージ表示 string
/// @param
/// @return 
/// @note
void CTaskObj::tweet2owner(const std::string &src)
{
    wstring srcw;
    CHelper::Str2Wstr(src, srcw);
    if (inf.hWnd_msgStatics != NULL) SetWindowText(inf.hWnd_msgStatics, srcw.c_str()); {return;}
}

/// @brief メインウィンドウへのショートメッセージ表示 wstring
/// @param
/// @return 
/// @note
void CTaskObj::tweet2owner(const std::wstring &srcw)
{
    if (inf.hWnd_msgStatics != NULL) SetWindowText(inf.hWnd_msgStatics, srcw.c_str()); {return;}
}

/// @brief 
/// @param
/// @return 
/// @note
void CTaskObj::txout2msg_listbox(std::wstring str)
{
    const wchar_t *pwc; pwc = str.c_str();

    inf.hWnd_msgList = GetDlgItem(inf.hWnd_opepane, IDC_LIST_TASK);
    LVITEM item;

    item.mask     = LVIF_TEXT;
    item.pszText  = (wchar_t *)pwc; // テキスト
    item.iItem    = inf.cnt_PNLlist_msg % MSG_LIST_MAX; // 番号
    item.iSubItem = 1;                                  // サブアイテムの番号
    ListView_SetItem(inf.hWnd_msgList, &item);

    SYSTEMTIME st; TCHAR tbuf[32];
    ::GetLocalTime(&st);
    wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

    item.pszText  = tbuf;   // テキスト
    item.iSubItem = 0;      // サブアイテムの番号
    ListView_SetItem(inf.hWnd_msgList, &item);

    //InvalidateRect(inf.hWnd_msgList, NULL, TRUE);
    inf.cnt_PNLlist_msg++;
}

/// @brief 
/// @param
/// @return 
/// @note
void CTaskObj::txout2msg_listbox(std::string str)
{
    wstring wstr; CHelper::Str2Wstr(str, wstr);
    txout2msg_listbox(wstr);
    return;
}

/// @brief パネルウィンドウのコールバック関数
/// @param
/// @return 
/// @note
LRESULT CALLBACK CTaskObj::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
        case IDC_RADIO_TASK_FUNC1:
        case IDC_RADIO_TASK_FUNC2:
        case IDC_RADIO_TASK_FUNC3:
        case IDC_RADIO_TASK_FUNC4:
        case IDC_RADIO_TASK_FUNC5:
        case IDC_RADIO_TASK_FUNC6:
            inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
            break;
        case IDC_RADIO_TASK_ITEM1:
        case IDC_RADIO_TASK_ITEM2:
        case IDC_RADIO_TASK_ITEM3:
        case IDC_RADIO_TASK_ITEM4:
        case IDC_RADIO_TASK_ITEM5:
        case IDC_RADIO_TASK_ITEM6:
            inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL1));
            break;
        case IDC_BUTTON_SET:
            {
                wstring wstr, wstr_tmp;

                //サンプルとしていろいろな型で読み込んで表示している
                wstr += L"Param 1(d):";
                int n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL1, (LPTSTR)wstr_tmp.c_str(), 128);
                if (n) wstr_tmp = to_wstring(stod(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

                wstr += L",  Param 2(i):";
                n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL2, (LPTSTR)wstr_tmp.c_str(), 128);
                if (n) wstr_tmp = to_wstring(stoi(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

                wstr += L",  Param 3(f):";
                n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL3, (LPTSTR)wstr_tmp.c_str(), 128);
                if (n) wstr_tmp = to_wstring(stof(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

                wstr += L",  Param 4(l):";
                n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL4, (LPTSTR)wstr_tmp.c_str(), 128);
                if (n) wstr_tmp = to_wstring(stol(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

                wstr += L",  Param 5(c):";
                n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL5, (LPTSTR)wstr_tmp.c_str(), 128);
                wstr += wstr_tmp.c_str();

                wstr += L",   Param 6(c):";
                n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL6, (LPTSTR)wstr_tmp.c_str(), 128);
                wstr += wstr_tmp.c_str();

                txout2msg_listbox(wstr);
            }
            break;
        case IDC_BUTTON_RESET:
            set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
            break;
        case IDC_CHECK_TASK_OPTION1:
            SendMessage(GetDlgItem(hDlg, IDC_CHECK_TASK_OPTION2), BM_SETCHECK, BST_UNCHECKED, 0L);
            if (IsDlgButtonChecked(hDlg, IDC_CHECK_TASK_OPTION1) == BST_CHECKED) {inf.work_select = THREAD_WORK_OPTION1;}
            else                                                                 {inf.work_select = THREAD_WORK_ROUTINE;}
            break;

        case IDC_CHECK_TASK_OPTION2:
            SendMessage(GetDlgItem(hDlg, IDC_CHECK_TASK_OPTION1), BM_SETCHECK, BST_UNCHECKED, 0L);
            if (IsDlgButtonChecked(hDlg, IDC_CHECK_TASK_OPTION2) == BST_CHECKED) {inf.work_select = THREAD_WORK_OPTION2;}
            else                                                                 {inf.work_select = THREAD_WORK_ROUTINE;}
            break;
        }
    }
    return 0;
}

/// @brief パネルウィンドウのパラメータ設定用エディットBOXデータセット関数
/// @param
/// @return 
/// @note
void CTaskObj::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6)
{
    wstring wstr;
    wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL1), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL2), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL3), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL4), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL5), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL6), wstr.c_str());
}

/// @brief パネルウィンドウの説明表示用テキスト表示設定関数
/// @param
/// @return 
/// @note
void CTaskObj::set_panel_tip_txt()
{
    wstring wstr_type; wstring wstr;
    switch (inf.panel_func_id)
    {
    case IDC_RADIO_TASK_FUNC1:
        {
            wstr = L"Type for Func1 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
            switch (inf.panel_type_id)
            {
            case IDC_RADIO_TASK_ITEM1:  wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM2:  wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM3:  wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM4:  wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM5:  wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM6:  wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            default:
                break;
            }
        }
        break;
    case IDC_RADIO_TASK_FUNC2:
        {
            wstr = L"Type for Func2 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
            switch (inf.panel_type_id)
            {
            case IDC_RADIO_TASK_ITEM1:  wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM2:  wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM3:  wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM4:  wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM5:  wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM6:  wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            default:
                break;
            }
        }
        break;
    case IDC_RADIO_TASK_FUNC3:
        {
            wstr = L"Type for Func3 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
            switch (inf.panel_type_id)
            {
            case IDC_RADIO_TASK_ITEM1:  wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM2:  wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM3:  wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM4:  wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM5:  wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM6:  wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            default:
                break;
            }
        }
        break;
    case IDC_RADIO_TASK_FUNC4:
        {
            wstr = L"Type for Func4 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
            switch (inf.panel_type_id)
            {
            case IDC_RADIO_TASK_ITEM1:  wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM2:  wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM3:  wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM4:  wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM5:  wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM6:  wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            default:
                break;
            }
        }
        break;
    case IDC_RADIO_TASK_FUNC5:
        {
            wstr = L"Type for Func5 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
            switch (inf.panel_type_id)
            {
            case IDC_RADIO_TASK_ITEM1:  wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM2:  wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM3:  wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM4:  wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM5:  wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM6:  wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            default:
                break;
            }
        }
        break;
    case IDC_RADIO_TASK_FUNC6:
        {
            wstr = L"Type for Func6 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
            switch (inf.panel_type_id)
            {
            case IDC_RADIO_TASK_ITEM1:  wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM2:  wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM3:  wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM4:  wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM5:  wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM6:  wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            default:
                break;
            }
        }
        break;
    default:
        {
            wstr = L"Type for Func? \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
            wstr_type += L"(Param of type?) \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
        }
        break;
    }

    SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASK_ITEM), wstr.c_str());
    SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASK_VAL), wstr_type.c_str());
}
