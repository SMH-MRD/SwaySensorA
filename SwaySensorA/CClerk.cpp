#include "CClerk.h"
#include "CSharedObject.h"
#include "resource.h"
#include "CMKlog.h"
#include "CMKChart.h"

extern CSharedObject* g_pSharedObject;  // タスク間共有データのポインタ
using namespace MKlog;
static CMKlog* plogobj = nullptr;

/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CClerk::CClerk()
{
}

/// @brief デストラクタ
/// @param
/// @return 
/// @note
CClerk::~CClerk()
{
}

/// @brief 
/// @param
/// @return 
/// @note
void CClerk::init_task(void *pobj)
{
    set_panel_tip_txt();

    // LOG関連設定
    plogobj = new CMKlog;
    plogobj->init_logfunc();
/*
    plogobj->set_double_data(&(pIO_Table->physics.PhPlane_n.x), MK_LOGSET_1, 0);
    plogobj->set_double_data(&(pIO_Table->physics.PhPlane_n.y), MK_LOGSET_1, 1);
*/
    for (int i = 0; i < CLERK_LOG_MAX; i++) {b_logact[i] = false;}

    return;
};

/// @brief タスクスレッドでの処理関数
/// @param
/// @return 
/// @note
void CClerk::routine_work(void *param)
{
//  ws << L"Act:" << *(inf.psys_counter) % 100;
    stRIOInfoData   rioinfo;
    g_pSharedObject->GetInfo(&rioinfo);
	ws << L"Port1:" << rioinfo.incldata[RIO_PORT_1].dig << L" Port2:" << rioinfo.incldata[RIO_PORT_2].dig;
	tweet2owner(ws.str()); ws.str(L""); ws.clear();
};

/// @brief タスクパネルWnd用コールバック関数
/// @param
/// @return 
/// @note
LRESULT CALLBACK CClerk::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
//  CPlayer* pPly = (CPlayer*)VectpCTaskObj[g_itask.ply];

    switch (msg)
    {
    case WM_COMMAND:
        {
		    switch (LOWORD(wp))
            {
		    case IDC_RADIO_TASK_FUNC1:
		    case IDC_RADIO_TASK_FUNC2:
		    case IDC_RADIO_TASK_FUNC3:
		    case IDC_RADIO_TASK_FUNC4:
		    case IDC_RADIO_TASK_FUNC5:
		    case IDC_RADIO_TASK_FUNC6:
                {
                    inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
                }
                break;
		    case IDC_RADIO_TASK_ITEM1:
		    case IDC_RADIO_TASK_ITEM2:
		    case IDC_RADIO_TASK_ITEM3:
		    case IDC_RADIO_TASK_ITEM4:
		    case IDC_RADIO_TASK_ITEM5:
		    case IDC_RADIO_TASK_ITEM6: 
                {
                    inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL1));
                    if (inf.panel_func_id == IDC_RADIO_TASK_FUNC1)
                    {
                        MKChart::CMKChart::init_chartfunc();
                        if (inf.panel_type_id == IDC_RADIO_TASK_ITEM1)
                        {
                            double          val;
                            stRIOInfoData   rioinfo;
                            g_pSharedObject->GetInfo(&rioinfo);
                            // PORT1 生データ
                            val = (double)rioinfo.incldata[RIO_PORT_1].dig;
                            MKChart::CMKChart::set_double_data(&val, MK_CHART1, 0, 0, 20.0, false);
                            // PORT2 生データ
                            val = (double)rioinfo.incldata[RIO_PORT_2].dig;
                            MKChart::CMKChart::set_double_data(&val, MK_CHART1, 0, 1, 20.0, false);

                            MKChart::CMKChart::open_chart(MK_CHART1, hDlg);
                            MKChart::CMKChart::set_chart_spd(MK_CHART1, 30000);
                        }
                        else if (inf.panel_type_id == IDC_RADIO_TASK_ITEM2)
                        {
/*
                            MKChart::CMKChart::set_double_data(&(pIO_Table->physics.PhPlane_n.x), MK_CHART2, 0, 0, 0.05, true);
                            MKChart::CMKChart::set_double_data(&(pIO_Table->physics.PhPlane_n.y), MK_CHART2, 0, 0, 0.05, false);
                            MKChart::CMKChart::set_double_data(&(pIO_Table->physics.PhPlane_t.x), MK_CHART2, 1, 0, 0.05, true);
                            MKChart::CMKChart::set_double_data(&(pIO_Table->physics.PhPlane_t.y), MK_CHART2, 1, 0, 0.05, false);
                            MKChart::CMKChart::set_double_data(&(pIO_Table->physics.PhPlane_r.x), MK_CHART2, 2, 0, 0.05, true);
                            MKChart::CMKChart::set_double_data(&(pIO_Table->physics.PhPlane_r.y), MK_CHART2, 2, 0, 0.05, false);
*/
                            MKChart::CMKChart::open_chart(MK_CHART2, hDlg);
                            MKChart::CMKChart::set_reflesh_time(MK_CHART2, 60000);
                        }
                        else if (inf.panel_type_id == IDC_RADIO_TASK_ITEM3)
                        {
                            ;
                        }
                        else if (inf.panel_type_id == IDC_RADIO_TASK_ITEM4)
                        {
                            ;
                        }
                        else if (inf.panel_type_id == IDC_RADIO_TASK_ITEM5)
                        {
                            ;
                        }
                        else if (inf.panel_type_id == IDC_RADIO_TASK_ITEM6)
                        {
                            ;
                        }
                        else;
			        }
                    else if (inf.panel_func_id == IDC_RADIO_TASK_FUNC2)
                    {
                        if (b_logact[0] == false)
                        {
                            b_logact[0] = true;
//                          plogobj->start_record(MK_LOGSET_1, NULL);
                            MKlog::CMKlog::start_record(MK_LOGSET_1, NULL);
                        }
                        else
                        {
                            b_logact[0] = false;
//                          plogobj->end_record(MK_LOGSET_1);
                            MKlog::CMKlog::end_record(MK_LOGSET_1);
                        }
                    }
                    else ;
		        }
                break;
            case IDC_BUTTON_SET:
                {
                    wstring wstr, wstr_tmp;
                    // サンプルとしていろいろな型で読み込んで表示している
                    wstr += L"Param 1(d):";
                    int n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL1, (LPTSTR)wstr_tmp.c_str(), 128);
                    if (n) {wstr_tmp = to_wstring(stod(wstr_tmp));}
                    wstr = wstr + wstr_tmp.c_str();

                    wstr += L",  Param 2(i):";
                    n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL2, (LPTSTR)wstr_tmp.c_str(), 128);
                    if (n) {wstr_tmp = to_wstring(stoi(wstr_tmp));}
                    wstr = wstr + wstr_tmp.c_str();

                    wstr += L",  Param 3(f):";
                    n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL3, (LPTSTR)wstr_tmp.c_str(), 128);
                    if (n) {wstr_tmp = to_wstring(stof(wstr_tmp));}
                    wstr = wstr + wstr_tmp.c_str();

                    wstr += L",  Param 4(l):";
                    n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL4, (LPTSTR)wstr_tmp.c_str(), 128);
                    if (n) {wstr_tmp = to_wstring(stol(wstr_tmp));}
                    wstr = wstr + wstr_tmp.c_str();

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
                {
                    set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
                }
                break;
            case IDC_CHECK_TASK_OPTION1:
                {
                    SendMessage(GetDlgItem(hDlg, IDC_CHECK_TASK_OPTION2), BM_SETCHECK, BST_UNCHECKED, 0L);
                    if (IsDlgButtonChecked(hDlg, IDC_CHECK_TASK_OPTION1) == BST_CHECKED) {inf.work_select = THREAD_WORK_OPTION1;}
                    else                                                                 {inf.work_select = THREAD_WORK_ROUTINE;}
                }
                break;
            case IDC_CHECK_TASK_OPTION2:
                {
                    SendMessage(GetDlgItem(hDlg, IDC_CHECK_TASK_OPTION1), BM_SETCHECK, BST_UNCHECKED, 0L);
                    if (IsDlgButtonChecked(hDlg, IDC_CHECK_TASK_OPTION2) == BST_CHECKED) {inf.work_select = THREAD_WORK_OPTION2;}
                    else                                                                 {inf.work_select = THREAD_WORK_ROUTINE;}
                }
                break;
		    default:
                break;
		    }
	    }
        break;
    default:
        break;
    }

    return 0;
};

/// @brief 
/// @param
/// @return 
/// @note
void CClerk::set_panel_tip_txt()
{
    wstring wstr_type; wstring wstr;
    switch (inf.panel_func_id)
    {
    case IDC_RADIO_TASK_FUNC1:
        {
            wstr = L"Type for Func1 \n\r 1:No1 OPEN 2:No2 OPEN 3:?? \n\r 4:?? 5:?? 6:??";
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
            wstr = L"Type of Func2 \n\r  1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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
            case IDC_RADIO_TASK_ITEM1:  wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";        break;
            case IDC_RADIO_TASK_ITEM2:  wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";        break;
            case IDC_RADIO_TASK_ITEM3:  wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";        break;
            case IDC_RADIO_TASK_ITEM4:  wstr_type += L"Param of type4 \n\r 1:AS_ACT 2:??  3:?? \n\r 4:?? 5:?? 6:??";    break;
            case IDC_RADIO_TASK_ITEM5:  wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";        break;
            case IDC_RADIO_TASK_ITEM6:  wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";        break;
            default:
                break;
            }
        }
        break;
	case IDC_RADIO_TASK_FUNC4:
        {
            wstr = L"Type for Func4 \n\r  1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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
            wstr = L"Type for Func6 \n\r  1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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

/// @brief 
/// @param
/// @return 
/// @note
void CClerk::set_panel_pb_txt()
{
    wstring wstr;
    wstr = L"CHART";    SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC1), wstr.c_str()); wstr.clear();
    wstr = L"LOG";      SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC2), wstr.c_str()); wstr.clear();
    wstr = L"-";        SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC3), wstr.c_str()); wstr.clear();
    wstr = L"-";        SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC4), wstr.c_str()); wstr.clear();
    wstr = L"-";        SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC5), wstr.c_str()); wstr.clear();
    wstr = L"-";        SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC6), wstr.c_str()); wstr.clear();

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
HWND CClerk::CreateOwnWindow(HWND h_parent_wnd)
{
    return inf.hWnd_work;
};
