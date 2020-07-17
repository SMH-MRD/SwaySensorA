#include "CPublicRelation.h"
#include "resource.h"
#include <Commdlg.h>
#include <commctrl.h>

extern CSharedObject*   g_pSharedObject;    // �^�X�N�ԋ��L�f�[�^�̃|�C���^

HWND    CPublicRelation::m_hCamDlg;
cv::Mat CPublicRelation::m_mtSaveImage;
BOOL    CPublicRelation::m_bCursor;
POINT   CPublicRelation::m_pntCursor;
UINT    CPublicRelation::m_iSelImg;

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

    m_iSelImg = 0;
    m_bCursor = FALSE;
    m_pntCursor.x = 0;
    m_pntCursor.y = 0;

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

    HBITMAP         bmp;                            // �摜(bitmap�t�@�C��)
    stCameraInfo    stCameraInfo;                   // �J�������
    stRIOInfo       stRIOInfo;                      // RIO���
    stProcInfo      stProcInfo;                     // �������
    cv::Mat         imgProc;                        // �����摜
    cv::Mat         imgMask;                        // �}�X�N�摜
    cv::Mat         imgDisp;                        // �\���摜
    BOOL            bImgEnable = FALSE;
    INT             x0, y0, x1, y1;
    TCHAR           msg[16];
    
    //----------------------------------------------------------------------------
    // ���L�f�[�^�擾
    g_pSharedObject->GetCameraInfo(&stCameraInfo);  // �J�������
    g_pSharedObject->GetRIOInfo(&stRIOInfo);        // RIO���
    g_pSharedObject->GetProcInfo(&stProcInfo);      // �������

    //----------------------------------------------------------------------------
    // �J�������
    // �J�������͏��
    if (stCameraInfo.valid) {_stprintf_s(msg, TEXT("����"));}
    else                    {_stprintf_s(msg, TEXT("�ُ�"));}
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_CAMERA_STAT), msg);

    // �摜�捞����
    _stprintf_s(msg, TEXT("%d"), (int)stCameraInfo.cycleTime);
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_IMG_GRAB_TIME), msg);

    //----------------------------------------------------------------------------
    // RIO���
    // RIO���
    if (stRIOInfo.error == RIO_ERR_NONE) {_stprintf_s(msg, TEXT("����"));}
    else                                 {_stprintf_s(msg, TEXT("�ُ�"));}
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_STAT), msg);

    // �X�Όv�f�[�^
    if (stRIOInfo.error == RIO_ERR_NONE)
    {
        _stprintf_s(msg, TEXT("%d"),   stRIOInfo.data[RIO_PORT_1].dig); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL),     msg);
        _stprintf_s(msg, TEXT("%d"),   stRIOInfo.data[RIO_PORT_2].dig); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL),     msg);
        _stprintf_s(msg, TEXT("%.1f"), stRIOInfo.data[RIO_PORT_1].cur); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_CUR), msg);
        _stprintf_s(msg, TEXT("%.1f"), stRIOInfo.data[RIO_PORT_2].cur); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_CUR), msg);
        _stprintf_s(msg, TEXT("%.1f"), stRIOInfo.data[RIO_PORT_1].deg); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_DEG), msg);
        _stprintf_s(msg, TEXT("%.1f"), stRIOInfo.data[RIO_PORT_2].deg); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_DEG), msg);
    }
    else
    {
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL),     msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL),     msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_CUR), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_CUR), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_DEG), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_DEG), msg);
    }

    //----------------------------------------------------------------------------
    // �������
    // �d�S�ʒu
    if (stProcInfo.data[IMGPROC_ID_IMG_1].valid)
    {
        _stprintf_s(msg, TEXT("%.1f"), stProcInfo.data[IMGPROC_ID_IMG_1].posx);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X1), msg);
        _stprintf_s(msg, TEXT("%.1f"), stProcInfo.data[IMGPROC_ID_IMG_1].posy);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y1), msg);
    }
    else
    {
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X1), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y1), msg);
    }
    if (stProcInfo.data[IMGPROC_ID_IMG_2].valid)
    {
        _stprintf_s(msg, TEXT("%.1f"), stProcInfo.data[IMGPROC_ID_IMG_2].posx);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X2), msg);
        _stprintf_s(msg, TEXT("%.1f"), stProcInfo.data[IMGPROC_ID_IMG_2].posy);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y2), msg);
    }
    else
    {
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X2), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y2), msg);
    }

    // ��������
    _stprintf_s(msg, TEXT("%d"), (int)stProcInfo.procTime);
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_PROC_TIME), msg);

    //----------------------------------------------------------------------------
    // �����摜�Ǎ���
    if      (g_pSharedObject->GetImage(IMAGE_ID_PROC_A, &imgProc) == RESULT_OK) {bImgEnable = TRUE;}
    else if (g_pSharedObject->GetImage(IMAGE_ID_PROC_B, &imgProc) == RESULT_OK) {bImgEnable = TRUE;}
    else ;
    //----------------------------------------------------------------------------
    if (bImgEnable)
    {
        //----------------------------------------------------------------------------
        // �ۑ��p�ɍŐV�摜��ێ����Ă���
        imgProc.copyTo(m_mtSaveImage);

        //----------------------------------------------------------------------------
        // �J�[�\���\��
        if (m_bCursor)
        {
            cv::line(m_mtSaveImage, Point(0, m_pntCursor.y), Point((imgProc.cols - 1), m_pntCursor.y), Scalar(255, 255, 255), (INT)(imgProc.rows / DISP_IMG_HEIGHT), cv::LINE_4);   // ����
            cv::line(m_mtSaveImage, Point(m_pntCursor.x, 0), Point(m_pntCursor.x, (imgProc.rows - 1)), Scalar(255, 255, 255), (INT)(imgProc.cols / DISP_IMG_WIDTH),  cv::LINE_4);   // �c��
        }

        //----------------------------------------------------------------------------
        // ROI�\��
        cv::rectangle(m_mtSaveImage, stProcInfo.data[IMGPROC_ID_IMG_1].roi, cv::Scalar(0, 255, 0), (INT)(imgProc.cols / DISP_IMG_WIDTH), cv::LINE_AA);
        cv::rectangle(m_mtSaveImage, stProcInfo.data[IMGPROC_ID_IMG_2].roi, cv::Scalar(0, 255, 0), (INT)(imgProc.cols / DISP_IMG_WIDTH), cv::LINE_AA);

        //----------------------------------------------------------------------------
        // ���o�ʒu�\��
        // �摜1
        if (stProcInfo.data[IMGPROC_ID_IMG_1].valid)
        {
            x0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posx - 10 * (INT)(imgProc.cols / DISP_IMG_WIDTH);  y0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posy;
            x1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posx + 10 * (INT)(imgProc.cols / DISP_IMG_WIDTH);  y1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posy;
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgProc.rows / DISP_IMG_HEIGHT), cv::LINE_4);  // ����

            x0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posx;  y0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posy - 10 * (INT)(imgProc.rows / DISP_IMG_HEIGHT);
            x1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posx;  y1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_1].posy + 10 * (INT)(imgProc.rows / DISP_IMG_HEIGHT);
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgProc.cols / DISP_IMG_WIDTH), cv::LINE_4);   // �c��
        }
        // �摜2
        if (stProcInfo.data[IMGPROC_ID_IMG_2].valid)
        {
            x0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posx - 10 * (INT)(imgProc.cols / DISP_IMG_WIDTH);  y0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posy;
            x1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posx + 10 * (INT)(imgProc.cols / DISP_IMG_WIDTH);  y1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posy;
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgProc.rows / DISP_IMG_HEIGHT), cv::LINE_4);  // ����

            x0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posx;  y0 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posy - 10 * (INT)(imgProc.rows / DISP_IMG_HEIGHT);
            x1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posx;  y1 = (INT)stProcInfo.data[IMGPROC_ID_IMG_2].posy + 10 * (INT)(imgProc.rows / DISP_IMG_HEIGHT);
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgProc.cols / DISP_IMG_WIDTH), cv::LINE_4);   // �c��
        }
        cv::resize(m_mtSaveImage, imgDisp, cv::Size(), DISP_IMG_WIDTH / imgProc.cols, DISP_IMG_HEIGHT / imgProc.rows);

        //----------------------------------------------------------------------------
        // H,S,V�\��
        if (m_bCursor)
        {
            cv::Mat imgHSV;
            cv::cvtColor(imgProc, imgHSV, COLOR_BGR2HSV);
            int H = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 0];
            int S = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 1];
            int V = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 2];
            cv::putText(imgDisp, cv::format("(%d,%d)H:%03d S:%03d V:%03d", m_pntCursor.x, m_pntCursor.y, H, S, V), cv::Point(5, (int)DISP_IMG_HEIGHT - 5), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
        }

        //----------------------------------------------------------------------------
        // ���摜�\��
        char* ColorBuf = (char*)calloc(imgDisp.cols * imgDisp.rows * 4, sizeof(RGBQUAD));
        for (int y = 0; y < imgDisp.rows; y++)
        {
            for (int x = 0; x < imgDisp.cols; x++)
            {
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 0] = imgDisp.data[y * imgDisp.step + x * 3 + 0];  // Blue
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 1] = imgDisp.data[y * imgDisp.step + x * 3 + 1];  // Green
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 2] = imgDisp.data[y * imgDisp.step + x * 3 + 2];  // Red
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 3] = 0;                                           // Reserved
            }
        }
        bmp = CreateBitmap(imgDisp.cols, imgDisp.rows, 1, 32, ColorBuf);
        free(ColorBuf);

        SendMessage(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_RAW), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
        DeleteObject(bmp);

        //----------------------------------------------------------------------------
        // �}�X�N�摜�\��
        UINT32 maskimage;
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK_TYPE, &maskimage);  // �}�X�N�摜�I��
        switch (maskimage)
        {
        case MASK_IMG_ALL:
            if (m_iSelImg == 0)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_A, &imgMask) != RESULT_OK)
                {
                    if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_B, &imgMask) != RESULT_OK) {return;}  // �����ȊO�̂��߁A�I��
                }
            }
            else
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_A, &imgMask) != RESULT_OK)
                {
                    if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_B, &imgMask) != RESULT_OK) {return;}  // �����ȊO�̂��߁A�I��
                }
            }
            break;
        case MASK_IMG_IMAGE1:
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_A, &imgMask) != RESULT_OK)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_B, &imgMask) != RESULT_OK) {return;}  // �����ȊO�̂��߁A�I��
            }
            break;
        case MASK_IMG_IMAGE2:
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_A, &imgMask) != RESULT_OK)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_B, &imgMask) != RESULT_OK) {return;}  // �����ȊO�̂��߁A�I��
            }
            break;
        default:
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_A, &imgMask) != RESULT_OK)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_B, &imgMask) != RESULT_OK) {return;}  // �����ȊO�̂��߁A�I��
            }
            break;
        }
        cv::resize(imgMask, imgDisp, cv::Size(), DISP_IMG_WIDTH / imgMask.cols, DISP_IMG_HEIGHT / imgMask.rows);

        ColorBuf = (char*)calloc(imgDisp.cols * imgDisp.rows * 4, sizeof(RGBQUAD));
        for (int y = 0; y < imgDisp.rows; y++)
        {
            for (int x = 0; x < imgDisp.cols; x++)
            {
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 0] = imgDisp.data[y * imgDisp.step + x];  // Blue
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 1] = imgDisp.data[y * imgDisp.step + x];  // Green
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 2] = imgDisp.data[y * imgDisp.step + x];  // Red
                ColorBuf[y * imgDisp.cols * 4 + x * 4 + 3] = 0;                                   // Reserved
            }
        }
        bmp = CreateBitmap(imgDisp.cols, imgDisp.rows, 1, 32, ColorBuf);
        free(ColorBuf);

        SendMessage(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_MASK), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
        DeleteObject(bmp);
    }   // if (bImgEnable)

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
    wstr = L"IMAGE";    SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_RADIO_TASK_FUNC1), wstr.c_str()); wstr.clear();
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
        m_hCamDlg = CreateDialog(inf.hInstance, MAKEINTRESOURCE(IDD_DIALOG_IMAGE_PROC), nullptr, (DLGPROC)CameraWndProc);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_RAW),  DISP_IMG_RAW_X0, DISP_IMG_RAW_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_MASK), DISP_IMG_MSK_X0, DISP_IMG_MSK_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        ShowWindow(m_hCamDlg, SW_SHOW);

        //----------------------------------------------------------------------------
        // ����
        g_pSharedObject->SetParam(PARAM_ID_IMG_GRAB_CAMERA, (UINT32)TRUE);
        EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  FALSE);
        EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   TRUE);
        EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), FALSE);
        EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_STOP),  FALSE);

        //----------------------------------------------------------------------------
        // �摜1
        {
            HWND    wndhdl;
            UINT32  val;
            TCHAR   msg[10];

            // �F��H(Low)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_HLOW, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // �F��H(Upp)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_HUPP, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // �ʓxS(Low)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_SLOW, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // �ʓxS(Upp)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_SUPP, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,   TRUE,  MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // ���xV(Low)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_VLOW, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // ���xV(Upp)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_VUPP, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // �摜2
        {
            HWND    wndhdl;
            UINT32  val;
            TCHAR   msg[10];

            // �F��H(Low)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_HLOW, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // �F��H(Upp)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_HUPP, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // �ʓxS(Low)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_SLOW, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // �ʓxS(Upp)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_SUPP, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // ���xV(Low)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_VLOW, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
            // ���xV(Upp)
            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_VUPP, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, val);                // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                  // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // �摜�I��
        {
            LPCTSTR strImgItem[] = {TEXT("�摜1"), TEXT("�摜2")};
            HWND    wndhdl;
            UINT32  maskimage;

            g_pSharedObject->GetParam(PARAM_ID_IMG_MASK_TYPE, &maskimage);  // �}�X�N�摜�I��
            wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_IMG);
            switch (maskimage)
            {
            case MASK_IMG_ALL:
                for (int i = 0; i < 2; i++)   {SendMessage(wndhdl, CB_ADDSTRING, i, (LPARAM)strImgItem[i]);}
                SendMessage(wndhdl, CB_SETCURSEL, m_iSelImg, 0);
                break;
            case MASK_IMG_IMAGE1:
                SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strImgItem[0]);
                SendMessage(wndhdl, CB_SETCURSEL, m_iSelImg, 0);
                break;
            case MASK_IMG_IMAGE2:
                SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strImgItem[1]);
                SendMessage(wndhdl, CB_SETCURSEL, m_iSelImg, 0);
                break;
            default:
                break;
            }
        }

        //----------------------------------------------------------------------------
        // �m�C�Y�t�B���^
        // �S�}��
        {
            // ���
            UINT32  pgsize = 1;
            LPCTSTR strFilterItem[] = {TEXT("�Ȃ�"), TEXT("�����l�t�B���^"), TEXT("�I�[�v�j���O����")};
            HWND    wndhdl;
            UINT32  val;
            TCHAR   msg[10];
 
            wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER1);
            for (int i = 0; i < NOIZEFILTER1_MAX; i++) {SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strFilterItem[i]);}
            g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER1, &val);
            SendMessage(wndhdl, CB_SETCURSEL, val, 0);
            if ((val == NOISEFILTER1_MEDIAN) || (val == NOISEFILTER1_OPENNING))
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1),     SW_SHOW);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), SW_SHOW);
                if (val == NOISEFILTER1_MEDIAN) {pgsize = 2;}
            }
            else
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1),     SW_HIDE);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), SW_HIDE);
            }
            // �t�B���^�l
            g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTERVAL1, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1);
            SendMessage(wndhdl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ, 5, 0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS, TRUE, val);                 // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, pgsize);            // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }
        // ������
        {
            // ���
            UINT32  pgsize = 1;
            LPCTSTR strFilterItem[] = {TEXT("�Ȃ�"), TEXT("�N���[�W���O����")};
            HWND    wndhdl;
            UINT32  val;
            TCHAR   msg[10];

            wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER2);
            for (int i = 0; i < NOIZEFILTER2_MAX; i++) {SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strFilterItem[i]);}
            g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER2, &val);
            SendMessage(wndhdl, CB_SETCURSEL, val, 0);
            if (val == NOISEFILTER2_CLOSING)
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2),     SW_SHOW);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), SW_SHOW);
            }
            else
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2),     SW_HIDE);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), SW_HIDE);
            }
            // �t�B���^�l
            g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTERVAL2, &val);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2);
            SendMessage(wndhdl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30)); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ, 5, 0);                  // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS, TRUE, val);                 // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, pgsize);            // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2);
            _stprintf_s(msg, 10, TEXT("%d"), val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // �J�����I������
        {
            HWND    wndhdl;
            UINT32  exptimemin, exptimemax, exptime;
            TCHAR   msg[10];

            g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME_MIN, &exptimemin);
            g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME_MAX, &exptimemax);
            g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME,     &exptime);
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE);
            SendMessage(wndhdl, TBM_SETRANGEMIN, TRUE, exptimemin); // �����W���w��
            SendMessage(wndhdl, TBM_SETRANGEMAX, TRUE, exptimemax); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ, 1000, 0);           // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS, TRUE, exptime);         // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, 1);             // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_CAMERA_EXPOSURE);
            _stprintf_s(msg, 10, TEXT("%d"), exptime);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // ROI�L��
        {
            HWND    wndhdl;
            UINT32  roienable, roisize, roisizemax;
            UINT32  imgw, imgh;
            TCHAR   msg[10];

            g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_ENABLE, &roienable);
            g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_SIZE,   &roisize);
            g_pSharedObject->GetParam(PARAM_ID_CAM_WIDTH,      &imgw);
            g_pSharedObject->GetParam(PARAM_ID_CAM_HEIGHT,     &imgh);
            if (imgh > imgw) {roisizemax = imgw;}
            else             {roisizemax = imgh;}
            wndhdl = GetDlgItem(m_hCamDlg, IDC_CHECK_ROI);
            if (roienable > 0) {SendMessage(wndhdl, BM_SETCHECK, BST_CHECKED,   0);}
            else               {SendMessage(wndhdl, BM_SETCHECK, BST_UNCHECKED, 0);}
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_ROI);
            SendMessage(wndhdl, TBM_SETRANGEMIN, TRUE, 10);         // �����W���w��
            SendMessage(wndhdl, TBM_SETRANGEMAX, TRUE, roisizemax); // �����W���w��
            SendMessage(wndhdl, TBM_SETTICFREQ, 1000, 0);           // �ڐ���̑���
            SendMessage(wndhdl, TBM_SETPOS, TRUE, roisize);         // �ʒu�̐ݒ�
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, 1);             // �N���b�N���̈ړ���
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_ROI);
            _stprintf_s(msg, 10, TEXT("%d"), roisize);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // �J�������
        {
            TCHAR   msg[10];
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_CAMERA_STAT),   msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_IMG_GRAB_TIME), msg);
        }

        //----------------------------------------------------------------------------
        // RIO���
        {
            TCHAR   msg[10];
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_STAT),       msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_CUR), msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_CUR), msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_DEG), msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_DEG), msg);
        }

        //----------------------------------------------------------------------------
        // �������
        {
            TCHAR   msg[10];
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X1),    msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y1),    msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X2),    msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y2),    msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_PROC_TIME), msg);
        }
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
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_HLOW, (UINT32)pos);
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_UPP), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_HUPP, (UINT32)pos);
            }
            else ;
            // �ʓxS
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_LOW), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_SLOW, (UINT32)pos);
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_UPP), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_SUPP, (UINT32)pos);
            }
            else ;
            // ���xV
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_LOW), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_VLOW, (UINT32)pos);
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_UPP), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK1_VUPP, (UINT32)pos);
            }
            else ;

            //----------------------------------------------------------------------------
            // �摜2
            // �F��H
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_LOW), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_HLOW, (UINT32)pos);
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_UPP), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_HUPP, (UINT32)pos);
            }
            else ;
            // �ʓxS
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_LOW), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_SLOW, (UINT32)pos);
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_UPP), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_SUPP, (UINT32)pos);
            }
            else ;
            // ���xV
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_LOW), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_VLOW, (UINT32)pos);
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_UPP), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_MASK2_VUPP, (UINT32)pos);
            }
            else ;

            //----------------------------------------------------------------------------
            // �m�C�Y�t�B���^
            // �S�}��
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1) == (HWND)lp)
            {
                UINT32  val;
                g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER1, &val);
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_GETPOS, 0, 0);
                if (val == NOISEFILTER1_MEDIAN)
                {
                    if ((pos % 2) == 0) {pos = pos + 1;}
                    SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPOS, TRUE, pos);  // �ʒu�̐ݒ�
                }
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTERVAL1, (UINT32)pos);
            }
            // ������
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2) == (HWND)lp)
            {
                UINT32  val;
                g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER2, &val);
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTERVAL2, (UINT32)pos);
            }

            //----------------------------------------------------------------------------
            // �J�����I������
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_CAMERA_EXPOSURE), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_CAM_EXPOSURE_TIME, (UINT32)pos);
            }

            //----------------------------------------------------------------------------
            // ROI
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_ROI) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_ROI), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_ROI), (LPCTSTR)str);
                g_pSharedObject->SetParam(PARAM_ID_IMG_ROI_SIZE, (UINT32)pos);
            }
        }
        break;
    case WM_COMMAND:
        // �I�����ꂽ���j���[�̉��:
        switch (wmId)
        {
        case IDC_BUTTON_CAM_START:
            g_pSharedObject->SetParam(PARAM_ID_IMG_GRAB_CAMERA, (UINT32)TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_STOP),  FALSE);
            break;
        case IDC_BUTTON_CAM_STOP:
            g_pSharedObject->SetParam(PARAM_ID_IMG_GRAB_CAMERA, (UINT32)FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_STOP),  FALSE);
            break;
        case IDC_BUTTON_SAVE:
            if (m_mtSaveImage.empty() != TRUE) {imwrite("procImage.bmp", m_mtSaveImage);}
            else                               {MessageBox(m_hCamDlg, L"�ۑ�����摜������܂���B", L"�G���[", MB_OK);}
            break;
        case IDC_BUTTON_FILE_START:
            {
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  FALSE);
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   FALSE);
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), FALSE);
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_STOP),  TRUE);

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
                            g_pSharedObject->SetParam(PARAM_ID_IMG_GRAB_FILE,     (UINT32)TRUE);
                        }
                        free(cstr);
                    }
                }
            }
            break;
        case IDC_BUTTON_FILE_STOP:
            g_pSharedObject->SetParam(PARAM_ID_IMG_GRAB_FILE, (UINT32)FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_STOP),  FALSE);
            break;
        case IDC_CHECK_ROI:
            if (BST_CHECKED == SendMessage(GetDlgItem(m_hCamDlg, IDC_CHECK_ROI), BM_GETCHECK, 0, 0)) {g_pSharedObject->SetParam(PARAM_ID_IMG_ROI_ENABLE, (UINT32)1);}
            else                                                                                     {g_pSharedObject->SetParam(PARAM_ID_IMG_ROI_ENABLE, (UINT32)0);}
            break;
        case IDC_CHECK_CURSOR:
            if (BST_CHECKED == SendMessage(GetDlgItem(m_hCamDlg, IDC_CHECK_CURSOR), BM_GETCHECK, 0, 0)) {m_bCursor = TRUE;}
            else                                                                                        {m_bCursor = FALSE;}
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
                if (pos >= (LONG)width) {pos = 0;}
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
                if (pos >= (LONG)height) {pos = 0;}
                m_pntCursor.y = pos;
            }
            break;
        case IDC_COMBO_IMG:
            if (HIWORD(wp) == CBN_SELCHANGE)
            {
                m_iSelImg = (UINT)SendMessage(GetDlgItem(m_hCamDlg, IDC_COMBO_IMG), CB_GETCURSEL, 0, 0);
            }
            break;
        case IDC_COMBO_NOISEFILTER1:
            if (HIWORD(wp) == CBN_SELCHANGE)
            {
                UINT    sel;
                TCHAR   str[10];
                int     pos;
                sel = (UINT)SendMessage(GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER1), CB_GETCURSEL, 0, 0);
                if ((sel == NOISEFILTER1_MEDIAN) || (sel == NOISEFILTER1_OPENNING))
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1),     SW_SHOW);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), SW_SHOW);
                    if (sel == NOISEFILTER1_MEDIAN)
                    {
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPAGESIZE, 0, 2);  // �N���b�N���̈ړ���
                        pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_GETPOS, 0, 0);
                        if ((pos % 2) == 0) {pos = pos + 1;}
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPOS, TRUE, pos);  // �ʒu�̐ݒ�
                        _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), (LPCTSTR)str);
                        g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTERVAL1, (UINT32)pos);
                    }
                    else
                    {
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPAGESIZE, 0, 1);  // �N���b�N���̈ړ���
                    }
                }
                else
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1),     SW_HIDE);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), SW_HIDE);
                }
                g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTER1, (UINT32)sel);
            }
            break;
        case IDC_COMBO_NOISEFILTER2:
            if (HIWORD(wp) == CBN_SELCHANGE)
            {
                UINT    sel;
                sel = (UINT)SendMessage(GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER2), CB_GETCURSEL, 0, 0);
                if (sel == NOISEFILTER2_CLOSING)
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2),     SW_SHOW);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), SW_SHOW);
                    SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2), TBM_SETPAGESIZE, 0, 1);  // �N���b�N���̈ړ���
                }
                else
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2),     SW_HIDE);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), SW_HIDE);
                }
                g_pSharedObject->SetParam(PARAM_ID_IMG_NOISEFILTER2, (UINT32)sel);
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
