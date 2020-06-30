#include "CPublicRelation.h"
#include "resource.h"
#include "CSharedObject.h"
#include <Commdlg.h>
#include <commctrl.h>

extern CSharedObject*   g_pSharedObject;    // タスク間共有データのポインタ

HWND CPublicRelation::m_hCamDlg;
Mat CPublicRelation::m_mtSaveImage;
BOOL CPublicRelation::m_bCursor;
POINT CPublicRelation::m_pntCursor;
UINT CPublicRelation::m_iSelImg;

/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CPublicRelation::CPublicRelation()
{
    m_hCamDlg = NULL;
}

/// @brief デストラクタ
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
    // タグパネルPB表示設定
    set_panel_pb_txt();
    set_panel_tip_txt();

    m_iSelImg = 0;
    m_bCursor = FALSE;
    m_pntCursor.x = 0;
    m_pntCursor.y = 0;

    return;
};

/// @brief タスクスレッドでの処理関数
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
    // 傾斜計データ更新
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
    // フレームレート更新
    UINT32 frameRate;
    if (g_pSharedObject->GetParam(PARAM_ID_CAM_READ_FRAMERATE, &frameRate) == RESULT_OK)
    {
        _stprintf_s(msg, TEXT("%d"), frameRate);
        SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_FPS), msg);
    }

    //----------------------------------------------------------------------------
    // 画像更新
    HBITMAP     bmp;        // 画像(bitmapファイル)
    STProcData  stProcData;
    Mat         dispImage;  // 画像読み込み

    //----------------------------------------------------------------------------
    if (m_iSelImg == 0)
    {
        if (g_pSharedObject->GetProcImage(IMAGE_ID_PROC1_A, &stProcData) != RESULT_OK)
        {
            if (g_pSharedObject->GetProcImage(IMAGE_ID_PROC1_B, &stProcData) != RESULT_OK) {return;}    // 成功以外のため、終了
        }
    }
    else
    {
        if (g_pSharedObject->GetProcImage(IMAGE_ID_PROC2_A, &stProcData) != RESULT_OK)
        {
            if (g_pSharedObject->GetProcImage(IMAGE_ID_PROC2_B, &stProcData) != RESULT_OK) {return;}    // 成功以外のため、終了
        }
    }

    //----------------------------------------------------------------------------
    // 保存用に最新画像を保持しておく
    stProcData.image.copyTo(m_mtSaveImage);

    //----------------------------------------------------------------------------
    // 検出位置表示
    INT x0, y0, x1, y1;
    if (stProcData.enable)
    {
        x0 = (INT)stProcData.posx - 10 * (INT)(stProcData.image.cols / DISP_IMG_WIDTH);  y0 = (INT)stProcData.posy;
        x1 = (INT)stProcData.posx + 10 * (INT)(stProcData.image.cols / DISP_IMG_WIDTH);  y1 = (INT)stProcData.posy;
        cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(255, 0, 255), (INT)(stProcData.image.cols / DISP_IMG_WIDTH), 4);

        x0 = (INT)stProcData.posx;  y0 = (INT)stProcData.posy - 10 * (INT)(stProcData.image.rows / DISP_IMG_HEIGHT);
        x1 = (INT)stProcData.posx;  y1 = (INT)stProcData.posy + 10 * (INT)(stProcData.image.rows / DISP_IMG_HEIGHT);
        cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(255, 0, 255), (INT)(stProcData.image.cols / DISP_IMG_WIDTH), 4);
    }
   
    if (m_bCursor)
    {
        x0 = (INT)m_pntCursor.x - 10 * (INT)(stProcData.image.cols / DISP_IMG_WIDTH);  y0 = (INT)m_pntCursor.y;
        x1 = (INT)m_pntCursor.x + 10 * (INT)(stProcData.image.cols / DISP_IMG_WIDTH);  y1 = (INT)m_pntCursor.y;
        cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(255, 255, 0), (INT)(stProcData.image.cols / DISP_IMG_WIDTH), 4);

        x0 = (INT)m_pntCursor.x;  y0 = (INT)m_pntCursor.y - 10 * (INT)(stProcData.image.rows / DISP_IMG_HEIGHT);
        x1 = (INT)m_pntCursor.x;  y1 = (INT)m_pntCursor.y + 10 * (INT)(stProcData.image.rows / DISP_IMG_HEIGHT);
        cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(255, 255, 0), (INT)(stProcData.image.cols / DISP_IMG_WIDTH), 4);
    }

    resize(m_mtSaveImage, dispImage, cv::Size(), DISP_IMG_WIDTH / stProcData.image.cols, DISP_IMG_HEIGHT / stProcData.image.rows);

    if (m_bCursor)
    {
        cv::Mat imgHSV;
        cv::cvtColor(stProcData.image, imgHSV, COLOR_BGR2HSV);
        int H = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 0];
        int S = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 1];
        int V = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 2];
        cv::putText(dispImage, cv::format("(%d,%d)H:%03d S:%03d V:%03d", m_pntCursor.x, m_pntCursor.y, H, S, V), cv::Point(5, DISP_IMG_HEIGHT - 5), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
    }

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

    //----------------------------------------------------------------------------
    Mat maskImage;
    if (m_iSelImg == 0)
    {
        if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_A, &maskImage) != RESULT_OK)
        {
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_B, &maskImage) != RESULT_OK) {return;}  // 成功以外のため、終了
        }
    }
    else
    {
        if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_A, &maskImage) != RESULT_OK)
        {
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_B, &maskImage) != RESULT_OK) {return;}  // 成功以外のため、終了
        }
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

    //----------------------------------------------------------------------------
    if (stProcData.enable)
    {
        _stprintf_s(msg, TEXT("%.3f"), stProcData.posx);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_X), msg);
        _stprintf_s(msg, TEXT("%.3f"), stProcData.posy);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_Y), msg);
    }
    else
    {
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_X), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_Y), msg);
    }

    //----------------------------------------------------------------------------
    DOUBLE procTime;
    g_pSharedObject->GetParam(PARAM_ID_DOUBLE_PROC_TIME, &procTime);
    if (!isnan(procTime))
    {
        _stprintf_s(msg, TEXT("%.3f"), procTime);
        SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_PROC_TIME), msg);
    }

    return;
}

/// @brief タスクパネルWnd用コールバック関数
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
            // 機能選択IDセット
            inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        case IDC_RADIO_TASK_ITEM1:
        case IDC_RADIO_TASK_ITEM2:
        case IDC_RADIO_TASK_ITEM3:
        case IDC_RADIO_TASK_ITEM4:
        case IDC_RADIO_TASK_ITEM5:
        case IDC_RADIO_TASK_ITEM6:
            // 機能入力IDセット
            inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_EDIT_TASK_VAL1));

            // ###各FUNCTION PB処理
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

            // サンプルとしていろいろな型で読み込んで表示している
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

/// @brief パネルウィンドウのパラメータ設定用エディットBOXデータセット関数
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

/// @brief センサパネル表示
/// @param
/// @return 
/// @note
HWND CPublicRelation::OpenCameraPanel()
{
    if (m_hCamDlg == NULL)
    {
        m_hCamDlg = CreateDialog(inf.hInstance, MAKEINTRESOURCE(IDD_DIALOG_CAMMAIN), nullptr, (DLGPROC)CameraWndProc);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_RAW),  DISP_IMG_RAW_X0, DISP_IMG_RAW_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_MASK), DISP_IMG_MSK_X0, DISP_IMG_MSK_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        ShowWindow(m_hCamDlg, SW_SHOW);

        TCHAR msg[10];
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_X), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_EDIT_GRV_Y), msg);

        HWND    wndhdl;
        UINT32  val;
        //----------------------------------------------------------------------------
        // 画像1
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VALID, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_CHECK_MASK1);
        if(val) {SendMessage(wndhdl, BM_SETCHECK, BST_CHECKED,   0);}
        else    {SendMessage(wndhdl, BM_SETCHECK, BST_UNCHECKED, 0);}
        // 色相H(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 179)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 色相H(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 170)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 彩度S(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 彩度S(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,   TRUE,  MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 明度V(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 明度V(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);

        //----------------------------------------------------------------------------
        // 画像2
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VALID, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_CHECK_MASK2);
        if (val) {SendMessage(wndhdl, BM_SETCHECK, BST_CHECKED,   0);}
        else     {SendMessage(wndhdl, BM_SETCHECK, BST_UNCHECKED, 0);}
        // 色相H(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 179)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 色相H(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 179)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 彩度S(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 彩度S(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 明度V(Low)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VLOW, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_LOW);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        // 明度V(Upp)
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VUPP, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP);
        SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(0, 255)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_UPP);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);

        //----------------------------------------------------------------------------
        // 画像選択
        LPCTSTR strImgItem[] = {TEXT("画像1"), TEXT("画像2")};
        wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_IMG);
        for (int i = 0; i < 2; i++)   {SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strImgItem[i]); }
        SendMessage(wndhdl, CB_SETCURSEL, m_iSelImg, 0);

        //----------------------------------------------------------------------------
        // ノイズフィルタ
        // 種類
        UINT32  pgsize = 1;
        LPCTSTR strFilterItem[] = {TEXT("なし"), TEXT("中央値フィルタ"), TEXT("オープニング処理")};
        wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER);
        for (int i = 0; i < NOIZEFILTER; i++) {SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strFilterItem[i]);}
        g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTER, &val);
        SendMessage(wndhdl, CB_SETCURSEL, val, 0);
        if ((val == NOISEFILTER_MEDIAN) || (val == NOISEFILTER_OPENNING))
        {
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER),     SW_SHOW);
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), SW_SHOW);
            if (val == NOISEFILTER_MEDIAN) {pgsize = 2;}
        }
        else
        {
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER),     SW_HIDE);
            ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), SW_HIDE);
        }
        // フィルタ値
        g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTERVAL, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER);
        SendMessage(wndhdl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30)); // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ, 5, 0);                  // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS, TRUE, val);                 // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0, pgsize);            // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);

        //----------------------------------------------------------------------------
        // カメラ露光時間
        g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &val);
        wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE);
        SendMessage(wndhdl, TBM_SETRANGEMIN, TRUE, 81);         // レンジを指定
        SendMessage(wndhdl, TBM_SETRANGEMAX, TRUE, 1000000);    // レンジを指定
        SendMessage(wndhdl, TBM_SETTICFREQ, 1000, 0);           // 目盛りの増分
        SendMessage(wndhdl, TBM_SETPOS, TRUE, val);             // 位置の設定
        SendMessage(wndhdl, TBM_SETPAGESIZE, 0, 1);             // クリック時の移動量
        wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_CAMERA_EXPOSURE);
        _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
    }
    return m_hCamDlg;
}

/// @brief センサパネルWnd用コールバック関数
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
        // 画像1
        // 色相H
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
        // 彩度S
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
        // 明度V
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
        // 画像2
        // 色相H
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
        // 彩度S
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
        // 明度V
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
        // ノイズフィルタ
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER) == (HWND)lp)
        {
            UINT32  val;
            g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTER, &val);
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_GETPOS, 0, 0);
            if (val == NOISEFILTER_MEDIAN)
            {
                if ((pos % 2) == 0) {pos = pos + 1;}
                SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_SETPOS, TRUE, pos);  // 位置の設定
            }
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_PIC_NOISEFILTERVAL, (UINT32)pos);
        }

        //----------------------------------------------------------------------------
        // カメラ露光時間
        if (GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE) == (HWND)lp)
        {
            pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE), TBM_GETPOS, 0, 0);
            _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_CAMERA_EXPOSURE), (LPCTSTR)str);
            g_pSharedObject->SetParam(PARAM_ID_CAM_EXPOSURE_TIME, (UINT32)pos);
        }
        }
        break;
    case WM_COMMAND:
        // 選択されたメニューの解析:
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
            else                               {MessageBox(m_hCamDlg, L"保存する画像がありません。", L"エラー", MB_OK);}
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
                    _tsetlocale(LC_ALL, _T(""));
                    errno_t ret = wcstombs_s(&size, cstr, sizeof(strFile), strFile, sizeof(strFile));
                    string  str = cstr;
                    if (ret == 0)
                    {
                        g_pSharedObject->SetParam(PARAM_ID_STR_PROC_FILENAME, str);
                        g_pSharedObject->SetParam(PARAM_ID_PIC_PROC_FLAG,     (UINT32)TRUE);
                    }
                    free(cstr);
                }
            }
            }
            break;
        case IDC_CHECK_CURSOR:
            if (BST_CHECKED == SendMessage(GetDlgItem(m_hCamDlg, IDC_CHECK_CURSOR), BM_GETCHECK, 0, 0))
            {
                m_bCursor = TRUE;
            }
            else
            {
                m_bCursor = FALSE;
            }
            break;
        case IDC_BUTTON_CURSOR_L:
            {
            UINT32  width;
            LONG    pos;
            g_pSharedObject->GetParam(PARAM_ID_CAM_WIDTH, &width);
            pos = m_pntCursor.x;
            if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos -= 50;}
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos -= 10;}
            else                                            {pos -= 1;}
            if (pos < 0) {pos = width - 1;}
            m_pntCursor.x = pos;
            }
            break;
        case IDC_BUTTON_CURSOR_R:
            {
            UINT32  width;
            LONG    pos;
            g_pSharedObject->GetParam(PARAM_ID_CAM_WIDTH, &width);
            pos = m_pntCursor.x;
            if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos += 50;}
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos += 10;}
            else                                            {pos += 1;}
            if (pos >= width) {pos = 0;}
            m_pntCursor.x = pos;
            }
            break;
        case IDC_BUTTON_CURSOR_U:
            {
            UINT32  height;
            LONG    pos;
            g_pSharedObject->GetParam(PARAM_ID_CAM_HEIGHT, &height);
            pos = m_pntCursor.y;
            if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos -= 50;}
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos -= 10;}
            else                                            {pos -= 1;}
            if (pos < 0) {pos = height - 1;}
            m_pntCursor.y = pos;
            }
            break;
        case IDC_BUTTON_CURSOR_D:
            {
            UINT32  height;
            LONG    pos;
            g_pSharedObject->GetParam(PARAM_ID_CAM_HEIGHT, &height);
            pos = m_pntCursor.y;
            if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos += 50;}
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos += 10;}
            else                                            {pos += 1;}
            if (pos >= height) {pos = 0;}
            m_pntCursor.y = pos;
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
        case IDC_COMBO_IMG:
            if (HIWORD(wp) == CBN_SELCHANGE)
            {
                m_iSelImg = (UINT)SendMessage(GetDlgItem(m_hCamDlg, IDC_COMBO_IMG), CB_GETCURSEL, 0, 0);
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
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_SETPAGESIZE, 0, 2);  // クリック時の移動量
                        pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_GETPOS, 0, 0);
                        if ((pos % 2) == 0) {pos = pos + 1;}
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_SETPOS, TRUE, pos);  // 位置の設定
                        _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER), (LPCTSTR)str);
                        g_pSharedObject->SetParam(PARAM_ID_PIC_NOISEFILTERVAL, (UINT32)pos);
                    }
                    else
                    {
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER), TBM_SETPAGESIZE, 0, 1);  // クリック時の移動量
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
