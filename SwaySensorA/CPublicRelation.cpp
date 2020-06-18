#include "CPublicRelation.h"
#include "resource.h"
#include "CSharedObject.h"
#include <Commdlg.h>
#include <commctrl.h>

extern CSharedObject*   g_pSharedObject;    // �^�X�N�ԋ��L�f�[�^�̃|�C���^

HWND CPublicRelation::m_hCamDlg;
Mat CPublicRelation::m_mtSaveImage;

/// @brief �R���X�g���N�^
/// @param
/// @return 
/// @note
CPublicRelation::CPublicRelation()
{
    m_hCamDlg = NULL;
}

/// @brief �f�X�g���N�^
/// @param
/// @return 
/// @note
CPublicRelation::~CPublicRelation()
{
}

/// @brief 
/// @param
/// @return 
/// @note
void CPublicRelation::init_task(void *pobj)
{
    // �^�O�p�l��PB�\���ݒ�
    set_panel_pb_txt();
    set_panel_tip_txt();
    return;
};

/// @brief �^�X�N�X���b�h�ł̏����֐�
/// @param
/// @return 
/// @note
void CPublicRelation::routine_work(void *param)
{
    if (g_pSharedObject == NULL) {return;}
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();

    DOUBLE  port1Ma, port2Ma, port1Angle, port2Angle;
    TCHAR   msg[10];

    //----------------------------------------------------------------------------
    // �X�Όv�f�[�^�X�V
    if (g_pSharedObject->GetInclinoData(INCLINO_ID_PORT_1_ANALOG, &port1Ma) == RESULT_OK)
    {
        if (!isnan(port1Ma))    {_stprintf_s(msg, TEXT("%.3f"), port1Ma);       SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_X), msg);}
        else                    {_stprintf_s(msg, TEXT("-"));                   SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_X), msg);}
    }
    if (g_pSharedObject->GetInclinoData(INCLINO_ID_PORT_2_ANALOG, &port2Ma) == RESULT_OK)
    {
        if (!isnan(port2Ma))    {_stprintf_s(msg, TEXT("%.3f"), port2Ma);       SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_Y), msg);}
        else                    {_stprintf_s(msg, TEXT("-"));                   SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_Y), msg);}
    }
    if (g_pSharedObject->GetInclinoData(INCLINO_ID_PORT_1_RAD, &port1Angle) == RESULT_OK)
    {
        if (!isnan(port1Angle)) {_stprintf_s(msg, TEXT("%.3f"), port1Angle);    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_X_DEG), msg);}
        else                    {_stprintf_s(msg, TEXT("-"));                   SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_X_DEG), msg);}
    }
    if (g_pSharedObject->GetInclinoData(INCLINO_ID_PORT_1_RAD, &port2Angle) == RESULT_OK)
    {
        if (!isnan(port2Angle)) {_stprintf_s(msg, TEXT("%.3f"), port2Angle);    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_Y_DEG), msg);}
        else                    {_stprintf_s(msg, TEXT("-"));                   SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_INC_Y_DEG), msg);}
    }

    //----------------------------------------------------------------------------
    // �t���[�����[�g�X�V
    UINT32 frameRate;
    if (g_pSharedObject->GetParam(PARAM_ID_CAM_READ_FRAMERATE, &frameRate) == RESULT_OK)
    {
        _stprintf_s(msg, TEXT("%d"), frameRate);
        SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_FPS), msg);
    }

    //----------------------------------------------------------------------------
    // �摜�X�V
    HBITMAP     bmp;        // �摜(bitmap�t�@�C��)
    STProcData  stProcData;
    Mat         dispImage;  // �摜�ǂݍ���

    if (g_pSharedObject->GetProcImage(IMAGE_ID_PROC_A, &stProcData) != RESULT_OK)
    {
        if (g_pSharedObject->GetProcImage(IMAGE_ID_PROC_B, &stProcData) != RESULT_OK) {return;} // �����ȊO�̂��߁A�I��
    }

    // �ۑ��p�ɍŐV�摜��ێ����Ă���
    stProcData.image.copyTo(m_mtSaveImage);

    resize(stProcData.image, dispImage, cv::Size(), DISP_IMG_WIDTH / stProcData.image.cols, DISP_IMG_HEIGHT / stProcData.image.rows);

    char* ColorBuf = (char*)calloc(dispImage.cols * dispImage.rows * 4, sizeof(RGBQUAD));

    for (int y = 0; y < dispImage.rows; y++)
    {
        for (int x = 0; x < dispImage.cols; x++)
        {
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 0] = dispImage.data[y * dispImage.step + x * 3 + 0];  // Blue
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 1] = dispImage.data[y * dispImage.step + x * 3 + 1];  // Green
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 2] = dispImage.data[y * dispImage.step + x * 3 + 2];  // Red
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 3] = 0;                                               // Reserved
        }
    }

    bmp = CreateBitmap(dispImage.cols, dispImage.rows, 1, 32, ColorBuf);
    free(ColorBuf);

    SendMessage(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_RAW), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
    DeleteObject(bmp);

    if (!isnan(stProcData.posx)) {_stprintf_s(msg, TEXT("%.3f"), stProcData.posx);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_X), msg);}
    else                         {_stprintf_s(msg, TEXT("-"));                      SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_X), msg);}
    if (!isnan(stProcData.posy)) {_stprintf_s(msg, TEXT("%.3f"), stProcData.posy);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_Y), msg);}
    else                         {_stprintf_s(msg, TEXT("-"));                      SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_Y), msg);}

    Mat maskImage;
    if (g_pSharedObject->GetImage(IMAGE_ID_MASK_A, &maskImage) != RESULT_OK)
    {
        if (g_pSharedObject->GetImage(IMAGE_ID_MASK_B, &maskImage) != RESULT_OK) {return;}  // �����ȊO�̂��߁A�I��
    }
    resize(maskImage, dispImage, cv::Size(), DISP_IMG_WIDTH / maskImage.cols, DISP_IMG_HEIGHT / maskImage.rows);

    ColorBuf = (char*)calloc(dispImage.cols * dispImage.rows * 4, sizeof(RGBQUAD));

    for (int y = 0; y < dispImage.rows; y++)
    {
        for (int x = 0; x < dispImage.cols; x++)
        {
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 0] = dispImage.data[y * dispImage.step + x];  // Blue
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 1] = dispImage.data[y * dispImage.step + x];  // Green
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 2] = dispImage.data[y * dispImage.step + x];  // Red
            ColorBuf[y * dispImage.cols * 4 + x * 4 + 3] = 0;                                       // Reserved
        }
    }

    bmp = CreateBitmap(dispImage.cols, dispImage.rows, 1, 32, ColorBuf);
    free(ColorBuf);

    SendMessage(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_MASK), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
    DeleteObject(bmp);

    DOUBLE procTime;
    g_pSharedObject->GetParam(PARAM_ID_DOUBLE_PROC_TIME, &procTime);
    if (!isnan(procTime))
    {
        _stprintf_s(msg, TEXT("%.3f"), procTime);
        SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_PROC_TIME), msg);
    }

    return;
}

/// @brief �^�X�N�p�l��Wnd�p�R�[���o�b�N�֐�
/// @param
/// @return 
/// @note
LRESULT CALLBACK CPublicRelation::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
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
            // �@�\�I��ID�Z�b�g
            inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        case IDC_RADIO_TASK_ITEM1:
        case IDC_RADIO_TASK_ITEM2:
        case IDC_RADIO_TASK_ITEM3:
        case IDC_RADIO_TASK_ITEM4:
        case IDC_RADIO_TASK_ITEM5:
        case IDC_RADIO_TASK_ITEM6:
            // �@�\����ID�Z�b�g
            inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL1));

            // ###�eFUNCTION PB����
            switch (inf.panel_func_id)
            {
            case IDC_RADIO_TASK_FUNC1:
                switch (inf.panel_type_id)
                {
                case IDC_RADIO_TASK_ITEM1:  OpenCameraPanel();  break;
                case IDC_RADIO_TASK_ITEM2:                      break;
                case IDC_RADIO_TASK_ITEM3:                      break;
                case IDC_RADIO_TASK_ITEM4:                      break;
                case IDC_RADIO_TASK_ITEM5:                      break;
                case IDC_RADIO_TASK_ITEM6:                      break;
                default:                                        break;
                }
                break;
            case IDC_RADIO_TASK_FUNC2:
                switch (inf.panel_type_id)
                {
                case IDC_RADIO_TASK_ITEM1:  break;
                case IDC_RADIO_TASK_ITEM2:  break;
                case IDC_RADIO_TASK_ITEM3:  break;
                case IDC_RADIO_TASK_ITEM4:  break;
                case IDC_RADIO_TASK_ITEM5:  break;
                case IDC_RADIO_TASK_ITEM6:  break;
                default:                    break;
                }
                break;
            case IDC_RADIO_TASK_FUNC3:
                switch (inf.panel_type_id)
                {
                case IDC_RADIO_TASK_ITEM1:  break;
                case IDC_RADIO_TASK_ITEM2:  break;
                case IDC_RADIO_TASK_ITEM3:  break;
                case IDC_RADIO_TASK_ITEM4:  break;
                case IDC_RADIO_TASK_ITEM5:  break;
                case IDC_RADIO_TASK_ITEM6:  break;
                default:                    break;
                }
                break;
            case IDC_RADIO_TASK_FUNC4:
                switch (inf.panel_type_id)
                {
                case IDC_RADIO_TASK_ITEM1:  break;
                case IDC_RADIO_TASK_ITEM2:  break;
                case IDC_RADIO_TASK_ITEM3:  break;
                case IDC_RADIO_TASK_ITEM4:  break;
                case IDC_RADIO_TASK_ITEM5:  break;
                case IDC_RADIO_TASK_ITEM6:  break;
                default:                    break;
                }
                break;
            case IDC_RADIO_TASK_FUNC5:
                switch (inf.panel_type_id)
                {
                case IDC_RADIO_TASK_ITEM1:  break;
                case IDC_RADIO_TASK_ITEM2:  break;
                case IDC_RADIO_TASK_ITEM3:  break;
                case IDC_RADIO_TASK_ITEM4:  break;
                case IDC_RADIO_TASK_ITEM5:  break;
                case IDC_RADIO_TASK_ITEM6:  break;
                default:                    break;
                }
                break;
            case IDC_RADIO_TASK_FUNC6:
                switch (inf.panel_type_id)
                {
                case IDC_RADIO_TASK_ITEM1:  break;
                case IDC_RADIO_TASK_ITEM2:  break;
                case IDC_RADIO_TASK_ITEM3:  break;
                case IDC_RADIO_TASK_ITEM4:  break;
                case IDC_RADIO_TASK_ITEM5:  break;
                case IDC_RADIO_TASK_ITEM6:  break;
                default:                    break;
                }
                break;
            default:
                break;
            }
            break;
        case IDC_BUTTON_SET:
        {
            wstring wstr, wstr_tmp;

            // �T���v���Ƃ��Ă��낢��Ȍ^�œǂݍ���ŕ\�����Ă���
            wstr += L"Param 1(d):";
            int n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL1, (LPTSTR)wstr_tmp.c_str(), 128);
            if (n) { wstr_tmp = to_wstring(stod(wstr_tmp)); }
            wstr = wstr + wstr_tmp.c_str();

            wstr += L",  Param 2(i):";
            n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL2, (LPTSTR)wstr_tmp.c_str(), 128);
            if (n) { wstr_tmp = to_wstring(stoi(wstr_tmp)); }
            wstr = wstr + wstr_tmp.c_str();

            wstr += L",  Param 3(f):";
            n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL3, (LPTSTR)wstr_tmp.c_str(), 128);
            if (n) { wstr_tmp = to_wstring(stof(wstr_tmp)); }
            wstr = wstr + wstr_tmp.c_str();

            wstr += L",  Param 4(l):";
            n = GetDlgItemText(hDlg, IDC_EDIT_TASK_VAL4, (LPTSTR)wstr_tmp.c_str(), 128);
            if (n) { wstr_tmp = to_wstring(stol(wstr_tmp)); }
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
            SendMessage(GetDlgItem(hDlg, IDC_CHECK_TASK_OPTION2), BM_SETCHECK, BST_UNCHECKED, 0L);
            if (IsDlgButtonChecked(hDlg, IDC_CHECK_TASK_OPTION1) == BST_CHECKED) { inf.work_select = THREAD_WORK_OPTION1; }
            else { inf.work_select = THREAD_WORK_ROUTINE; }
            break;
        case IDC_CHECK_TASK_OPTION2:
            SendMessage(GetDlgItem(hDlg, IDC_CHECK_TASK_OPTION1), BM_SETCHECK, BST_UNCHECKED, 0L);
            if (IsDlgButtonChecked(hDlg, IDC_CHECK_TASK_OPTION2) == BST_CHECKED) { inf.work_select = THREAD_WORK_OPTION2; }
            else { inf.work_select = THREAD_WORK_ROUTINE; }
            break;
        }
        break;
    }
    return 0;
}

/// @brief 
/// @param
/// @return 
/// @note
void CPublicRelation::set_panel_pb_txt()
{
    wstring wstr;
    wstr = L"PANEL";    SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC1), wstr.c_str()); wstr.clear();
    wstr = L"-";        SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC2), wstr.c_str()); wstr.clear();
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
void CPublicRelation::set_panel_tip_txt()
{
    wstring wstr_type; wstring wstr;
    switch (inf.panel_func_id)
    {
    case IDC_RADIO_TASK_FUNC1:
    {
        wstr = L"Type for Func1 \n\r 1:OPEN 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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

/// @brief �p�l���E�B���h�E�̃p�����[�^�ݒ�p�G�f�B�b�gBOX�f�[�^�Z�b�g�֐�
/// @param
/// @return 
/// @note
void CPublicRelation::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6)
{
    wstring wstr;
    wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL1), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL2), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL3), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL4), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL5), wstr.c_str()); wstr.clear();
    wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL6), wstr.c_str());
}

/// @brief �Z���T�p�l���\��
/// @param
/// @return 
/// @note
HWND CPublicRelation::OpenCameraPanel()
{
    if (m_hCamDlg == NULL)
    {
        m_hCamDlg = CreateDialog(inf.hInstance, MAKEINTRESOURCE(IDD_DIALOG_CAMMAIN), nullptr, (DLGPROC)CameraWndProc);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_RAW),  DISP_IMG_X0,                            DISP_IMG_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_MASK), DISP_IMG_X0 + (int)DISP_IMG_WIDTH + 10, DISP_IMG_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        ShowWindow(m_hCamDlg, SW_SHOW);

        TCHAR msg[10];
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_X), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_Y), msg);

        HWND    wndhdl;
        UINT32  val;
        //----------------------------------------------------------------------------
        // �摜1
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VALID, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_CHECK_MASK1);
        if(val) {SendMessage(wndhdl, BM_SETCHECK, BST_CHECKED,   0);}
        else    {SendMessage(wndhdl, BM_SETCHECK, BST_UNCHECKED, 0);}
        // �F��H(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 179)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // �F��H(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 170)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // �ʓxS(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // �ʓxS(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,   TRUE,  MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // ���xV(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // ���xV(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);

        //----------------------------------------------------------------------------
        // �摜2
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VALID, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_CHECK_MASK2);
        if (val) {SendMessage(wndhdl, BM_SETCHECK, BST_CHECKED,   0);}
        else     {SendMessage(wndhdl, BM_SETCHECK, BST_UNCHECKED, 0);}
        // �F��H(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 179)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // �F��H(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 179)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // �ʓxS(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // �ʓxS(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // ���xV(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // ���xV(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);

        //----------------------------------------------------------------------------
        // �m�C�Y�t�B���^
        // ���
        LPCTSTR strItem[] = { TEXT("�Ȃ�"), TEXT("�����l�t�B���^"), TEXT("�I�[�v�j���O����") };
        wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER);
        for (int i = 0; i < NOIZEFILTER; i++) {SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strItem[i]);}
        g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTER, &val);
        SendMessage(wndhdl, CB_SETCURSEL, val, 0);
        if ((val == NOISEFILTER_MEDIAN) || (val == NOISEFILTER_OPENNING))
        {
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER),     SW_SHOW);
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), SW_SHOW);
        }
        else
        {
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER),     SW_HIDE);
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), SW_HIDE);
        }
        // �t�B���^�l
        g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTERVAL, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER);
        SendMessage(wndhdl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 15)); // �����W���w��
        SendMessage(wndhdl, TBM_SETTICFREQ, 5, 0);                  // �ڐ���̑���
        SendMessage(wndhdl, TBM_SETPOS, TRUE, val);                 // �ʒu�̐ݒ�
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0, 2);                 // �N���b�N���̈ړ���
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
    }
    return m_hCamDlg;
}

/// @brief �Z���T�p�l��Wnd�p�R�[���o�b�N�֐�
/// @param
/// @return 
/// @note
LRESULT CALLBACK CPublicRelation::CameraWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    INT wmId = LOWORD(wp);
    
    switch (msg)
    {
    case WM_INITDIALOG:
        break;
    case WM_HSCROLL:
        {
        TCHAR   str[10];
        int     pos;
        //----------------------------------------------------------------------------
        // �摜1
        // �F��H
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_LOW) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_LOW), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_LOW), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_HLOW, (UINT32)pos);
        }
        else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_UPP), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_HUPP, (UINT32)pos);
        }
        else ;
        // �ʓxS
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_LOW), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_SLOW, (UINT32)pos);
        }
        else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_UPP), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_SUPP, (UINT32)pos);
        }
        else ;
        // ���xV
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_LOW), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_VLOW, (UINT32)pos);
        }
        else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_UPP), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_VUPP, (UINT32)pos);
        }
        else ;

        //----------------------------------------------------------------------------
        // �摜2
        // �F��H
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_LOW), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_HLOW, (UINT32)pos);
        }
        else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_UPP), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_HUPP, (UINT32)pos);
        }
        else ;
        // �ʓxS
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_LOW), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_SLOW, (UINT32)pos);
        }
        else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_UPP), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_SUPP, (UINT32)pos);
        }
        else ;
        // ���xV
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_LOW), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_VLOW, (UINT32)pos);
        }
        else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_UPP), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_VUPP, (UINT32)pos);
        }
        else ;

        //----------------------------------------------------------------------------
        // �m�C�Y�t�B���^
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER) == (HWND)lp)
        {
            UINT32  val;
            g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTER, &val);
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_GETPOS, 0, 0);
            if (val == NOISEFILTER_MEDIAN)
            {
                if ((pos % 2) == 0) {pos = pos + 1;}
                SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_SETPOS, TRUE, pos);  // �ʒu�̐ݒ�
            }
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_NOISEFILTERVAL, (UINT32)pos);
        }
        }
        break;
    case WM_COMMAND:
        // �I�����ꂽ���j���[�̉��:
        switch (wmId)
        {
        case IDC_BUTTON_CAM_START:
            g_pSharedObject->SetParam(PARAM_ID_CAM_PROC, (UINT32)TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_IMG_PROC), FALSE);
            break;
        case IDC_BUTTON_CAM_STOP:
            g_pSharedObject->SetParam(PARAM_ID_CAM_PROC, (UINT32)FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_IMG_PROC), TRUE);
            break;
        case IDC_BUTTON_SAVE:
            if (m_mtSaveImage.empty() != TRUE) {imwrite("procImage.bmp", m_mtSaveImage);}
            else                               {MessageBox(m_hCamDlg, L"�ۑ�����摜������܂���B", L"�G���[", MB_OK);}
            break;
        case IDC_BUTTON_IMG_PROC:
            {
            static OPENFILENAME ofn;
            static TCHAR strFile[MAX_PATH], strCustom[256] = TEXT("Before files\0*.*\0\0");

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner   = m_hCamDlg;
            ofn.lpstrFilter = TEXT("Image files {*.jpg;*.bmp;*.png}\0*.jpg;*.bmp;*.png\0")
            TEXT("All files {*.*}\0*.*\0\0");
            ofn.lpstrCustomFilter = strCustom;
            ofn.nMaxCustFilter    = 256;
            ofn.nFilterIndex      = 0;
            ofn.lpstrFile         = strFile;
            ofn.nMaxFile          = MAX_PATH;
            ofn.Flags             = OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&ofn) == TRUE)
            {
                char* cstr = (char*)malloc(sizeof(strFile));
                if (cstr != NULL)
                {
                    size_t size;
                    wcstombs_s(&size, cstr, sizeof(strFile), strFile, sizeof(strFile));
                    string str = cstr;
                    g_pSharedObject->SetParam(PARAM_ID_STR_PROC_FILENAME, str);
                    free(cstr);
                    g_pSharedObject->SetParam(PARAM_ID_PIC_PROC_FLAG, (UINT32)TRUE);
                }
            }
            }
            break;
        case IDC_CHECK_MASK1:
            if (BST_CHECKED == SendMessage(GetDlgItem(m_hCamDlg, IDC_CHECK_MASK1), BM_GETCHECK, 0, 0))
            {
                g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_VALID, (UINT32)TRUE);
            }
            else
            {
                g_pSharedObject->SetParam(PARAM_ID_PIC_MASK1_VALID, (UINT32)FALSE);
            }
            break;
        case IDC_CHECK_MASK2:
            if (BST_CHECKED == SendMessage(GetDlgItem(m_hCamDlg, IDC_CHECK_MASK2), BM_GETCHECK, 0, 0))
            {
                g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_VALID, (UINT32)TRUE);
            }
            else
            {
                g_pSharedObject->SetParam(PARAM_ID_PIC_MASK2_VALID, (UINT32)FALSE);
            }
            break;
        case IDC_COMBO_NOISEFILTER:
            if (HIWORD(wp) == CBN_SELCHANGE)
            {
                UINT    sel;
                TCHAR   str[10];
                int     pos;
                sel = (UINT)SendMessage(GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER), CB_GETCURSEL, 0, 0);
                if ((sel == NOISEFILTER_MEDIAN) || (sel == NOISEFILTER_OPENNING))
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER),     SW_SHOW);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), SW_SHOW);
                    if (sel == NOISEFILTER_MEDIAN)
                    {
                        pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_GETPOS, 0, 0);
                        if ((pos % 2) == 0) {pos = pos + 1;}
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_SETPOS, TRUE, pos);  // �ʒu�̐ݒ�
                        _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), (LPCTSTR)str);
                        g_pSharedObject->SetParam(PARAM_ID_PIC_NOISEFILTERVAL, (UINT32)pos);
                    }
                }
                else
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER),     SW_HIDE);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), SW_HIDE);
                }
                g_pSharedObject->SetParam(PARAM_ID_PIC_NOISEFILTER, (UINT32)sel);
            }
            break;
        case IDCANCEL:
//          PostQuitMessage(0);
            SendMessage(m_hCamDlg, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case WM_PAINT:
        break;
    case WM_CLOSE:
        EndDialog(hwnd, LOWORD(wp));
        m_hCamDlg = NULL;
        return (INT_PTR)TRUE;
//      PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }

    return FALSE;
}
