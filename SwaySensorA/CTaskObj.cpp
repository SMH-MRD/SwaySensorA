#include "CTaskObj.h"
#include "CHelper.h"
#include "Resource.h"


/// @brief �R���X�g���N�^
/// @param
/// @return 
/// @note
CTaskObj::CTaskObj()
{
    // �^�X�N�֘A�̏����l�ݒ�
    inf.hndl       = NULL;                      // �X���b�h�n���h��
    inf.cycle_ms   = DEFAUT_TASK_CYCLE;         // �X���b�h�̌Ăяo������
    inf.thread_com = REPEAT_INFINIT;            // �X���b�h�̌Ăяo���^�C�v
    wsprintf(inf.name, L"Who am I?");           // �^�X�N��
    inf.act_count   = inf.act_time = 0;         // �X���b�h��������p�J�E���^���Z�b�g
    inf.priority    = THREAD_PRIORITY_NORMAL;   // �^�X�N�v���C�I���e�B�ݒ�i�����p�j
    inf.work_select = THREAD_WORK_IDLE;         // �X���b�h���s���̏����֐�
    inf.hWnd_parent = inf.hWnd_work = inf.hWnd_opepane = inf.hWnd_msgStatics = NULL;    // �e��\���֘A�n���h��
    inf.hBmp        = NULL;                     // �^�X�N�A�C�R���p�r�b�g�}�b�v�n���h��
}

/// @brief �f�X�g���N�^
/// @param
/// @return 
/// @note
CTaskObj::~CTaskObj()
{
	inf.thread_com = TERMINATE_THREAD;
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
        //SIGNAL��ԑ҂�
        inf.event_triggered = WaitForMultipleObjects(inf.n_active_events,               // �ΏۃI�u�W�F�N�g��
                                                     ((CTaskObj*)param)->inf.hevents,   // �ΏۃI�u�W�F�N�g�n���h���̔z�� 
                                                     FALSE,                             // TRUE�FALL�@Object�@FALSE�FAny of Object 
                                                     INFINITE);                         // SIGNAL�҂����ԁ@msec

        //���������m�F�p
        DWORD   start_time = timeGetTime();
        inf.period     = start_time - inf.start_time;
        inf.start_time = start_time;

        routine_work(param);

        inf.act_time = timeGetTime() - start_time;  // ���������ԑ���p
        if (this->inf.thread_com != REPEAT_INFINIT) {break;}    // ���������̏ꍇ�͔�����
    }
    return 0;
}

/// @brief �^�X�N�X���b�h�ł̏����֐�
/// @param
/// @return 
/// @note
void CTaskObj::routine_work(void *param)
{
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();
    return;
}

/// @brief ���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\�� string
/// @param
/// @return 
/// @note
void CTaskObj::tweet2owner(const std::string &src)
{
    wstring srcw;
    CHelper::Str2Wstr(src, srcw);
    if (inf.hWnd_msgStatics != NULL) SetWindowText(inf.hWnd_msgStatics, srcw.c_str()); {return;}
}

/// @brief ���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\�� wstring
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
    item.pszText  = (wchar_t *)pwc; // �e�L�X�g
    item.iItem    = inf.cnt_PNLlist_msg % MSG_LIST_MAX; // �ԍ�
    item.iSubItem = 1;                                  // �T�u�A�C�e���̔ԍ�
    ListView_SetItem(inf.hWnd_msgList, &item);

    SYSTEMTIME st; TCHAR tbuf[32];
    ::GetLocalTime(&st);
    wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

    item.pszText  = tbuf;   // �e�L�X�g
    item.iSubItem = 0;      // �T�u�A�C�e���̔ԍ�
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

/// @brief �p�l���E�B���h�E�̃R�[���o�b�N�֐�
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

                //�T���v���Ƃ��Ă��낢��Ȍ^�œǂݍ���ŕ\�����Ă���
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

/// @brief �p�l���E�B���h�E�̃p�����[�^�ݒ�p�G�f�B�b�gBOX�f�[�^�Z�b�g�֐�
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

/// @brief �p�l���E�B���h�E�̐����\���p�e�L�X�g�\���ݒ�֐�
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
