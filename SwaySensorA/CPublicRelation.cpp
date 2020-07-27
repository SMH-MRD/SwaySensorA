#include "CPublicRelation.h"
#include "resource.h"
#include <Commdlg.h>
#include <commctrl.h>

extern CSharedObject* g_pSharedObject;  // タスク間共有データのポインタ

HWND                CPublicRelation::m_hCamDlg;
cv::Mat             CPublicRelation::m_mtSaveImage;
BOOL                CPublicRelation::m_bCursor;
POINT               CPublicRelation::m_pntCursor;
UINT                CPublicRelation::m_iSelImg;
stCameraParamData   CPublicRelation::m_camparam;
stImgProcParamData  CPublicRelation::m_imgprocparam;
stExtnInfoData      CPublicRelation::m_extninfo;

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

    g_pSharedObject->GetParam(&m_camparam);     // カメラ設定データ
    g_pSharedObject->GetParam(&m_imgprocparam); // 画像処理設定データ

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

    HBITMAP             bmp;                    // 画像(bitmapファイル)
    stCameraInfoData    caminfo;                // カメラ情報データ
    stRIOInfoData       rioinfo;                // RIO情報データ
    stProcInfoData      prcinfo;                // 処理情報データ
    cv::Mat             imgproc;                // 処理画像
    cv::Mat             imgmask;                // マスク画像
    cv::Mat             imgdisp;                // 表示画像
    BOOL                bImgEnable = FALSE;
    INT                 x0, y0, x1, y1;
    TCHAR               msg[16];
    
    //----------------------------------------------------------------------------
    // 共有データ読み出し
    g_pSharedObject->GetInfo(&caminfo);         // カメラ情報データ
    g_pSharedObject->GetInfo(&rioinfo);         // RIO情報データ
    g_pSharedObject->GetInfo(&prcinfo);         // 処理情報データ

    //----------------------------------------------------------------------------
    // 共有データ書き込み
    g_pSharedObject->SetParam(m_camparam);      // カメラ設定データ
    g_pSharedObject->SetParam(m_imgprocparam);  // 画像処理設定データ
    g_pSharedObject->SetInfo(m_extninfo);       // 外部入力データ

    //----------------------------------------------------------------------------
    // カメラ情報
    // カメラ入力状態
    if (caminfo.valid) {_stprintf_s(msg, TEXT("正常"));}
    else               {_stprintf_s(msg, TEXT("異常"));}
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_CAMERA_STAT), msg);

    // 画像取込時間
    if (caminfo.valid) {_stprintf_s(msg, TEXT("%d"), (int)caminfo.cycleTime);}
    else               {_stprintf_s(msg, TEXT("-"));}
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_IMG_GRAB_TIME), msg);

    //----------------------------------------------------------------------------
    // RIO情報
    // RIO状態
    if (rioinfo.error == RIO_ERR_NONE) {_stprintf_s(msg, TEXT("正常"));}
    else                               {_stprintf_s(msg, TEXT("異常"));}
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_STAT), msg);

    // 傾斜計データ
    if (rioinfo.error == RIO_ERR_NONE)
    {
        _stprintf_s(msg, TEXT("%d"),   rioinfo.incldata[AXIS_X].dig); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL),     msg);
        _stprintf_s(msg, TEXT("%d"),   rioinfo.incldata[RIO_PORT_2].dig); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL),     msg);
        _stprintf_s(msg, TEXT("%.1f"), rioinfo.incldata[AXIS_X].cur); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_CUR), msg);
        _stprintf_s(msg, TEXT("%.1f"), rioinfo.incldata[RIO_PORT_2].cur); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_CUR), msg);
        _stprintf_s(msg, TEXT("%.1f"), rioinfo.incldata[AXIS_X].deg); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P1_VAL_DEG), msg);
        _stprintf_s(msg, TEXT("%.1f"), rioinfo.incldata[RIO_PORT_2].deg); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_RIO_P2_VAL_DEG), msg);
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
    // 処理情報
    // 重心位置
    if (prcinfo.imgprocdata[IMGPROC_ID_IMG_1].valid)
    {
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posx);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X1), msg);
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posy);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y1), msg);
    }
    else
    {
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X1), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y1), msg);
    }
    if (prcinfo.imgprocdata[IMGPROC_ID_IMG_2].valid)
    {
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posx);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X2), msg);
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posy);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y2), msg);
    }
    else
    {
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X2), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y2), msg);
    }

    // 振れ
    if (prcinfo.valid)
    {
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.swaydata[AXIS_X].deg); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_X),     msg);
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.swaydata[AXIS_Y].deg); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_Y),     msg);
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.swaydata[AXIS_X].spd); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_SPD_X), msg);
        _stprintf_s(msg, TEXT("%.1f"), prcinfo.swaydata[AXIS_Y].spd); SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_SPD_Y), msg);
    }
    else
    {
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_X),     msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_Y),     msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_SPD_X), msg);
        _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_SPD_Y), msg);
    }

    // 処理時間
    _stprintf_s(msg, TEXT("%d"), (int)prcinfo.procTime);
    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_PROC_TIME), msg);

    //----------------------------------------------------------------------------
    // 処理画像読込み
    if      (g_pSharedObject->GetImage(IMAGE_ID_PROC_A, &imgproc) == RESULT_OK) {bImgEnable = TRUE;}
    else if (g_pSharedObject->GetImage(IMAGE_ID_PROC_B, &imgproc) == RESULT_OK) {bImgEnable = TRUE;}
    else ;
    //----------------------------------------------------------------------------
    if (bImgEnable)
    {
        //----------------------------------------------------------------------------
        // 保存用に最新画像を保持しておく
        imgproc.copyTo(m_mtSaveImage);

        //----------------------------------------------------------------------------
        // カーソル表示
        if (m_bCursor)
        {
            cv::line(m_mtSaveImage, Point(0, m_pntCursor.y), Point((imgproc.cols - 1), m_pntCursor.y), Scalar(255, 255, 255), (INT)(imgproc.rows / DISP_IMG_HEIGHT), cv::LINE_4);   // 横線
            cv::line(m_mtSaveImage, Point(m_pntCursor.x, 0), Point(m_pntCursor.x, (imgproc.rows - 1)), Scalar(255, 255, 255), (INT)(imgproc.cols / DISP_IMG_WIDTH),  cv::LINE_4);   // 縦線
        }

        //----------------------------------------------------------------------------
        // ROI表示
        cv::rectangle(m_mtSaveImage, prcinfo.imgprocdata[IMGPROC_ID_IMG_1].roi, cv::Scalar(0, 255, 0), (INT)(imgproc.cols / DISP_IMG_WIDTH), cv::LINE_AA);
        cv::rectangle(m_mtSaveImage, prcinfo.imgprocdata[IMGPROC_ID_IMG_2].roi, cv::Scalar(0, 255, 0), (INT)(imgproc.cols / DISP_IMG_WIDTH), cv::LINE_AA);

        //----------------------------------------------------------------------------
        // 検出位置表示
        // 画像1
        if (prcinfo.imgprocdata[IMGPROC_ID_IMG_1].valid)
        {
            x0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posx - 10 * (INT)(imgproc.cols / DISP_IMG_WIDTH);  y0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posy;
            x1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posx + 10 * (INT)(imgproc.cols / DISP_IMG_WIDTH);  y1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posy;
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgproc.rows / DISP_IMG_HEIGHT), cv::LINE_4);  // 横線

            x0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posx;  y0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posy - 10 * (INT)(imgproc.rows / DISP_IMG_HEIGHT);
            x1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posx;  y1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_1].posy + 10 * (INT)(imgproc.rows / DISP_IMG_HEIGHT);
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgproc.cols / DISP_IMG_WIDTH), cv::LINE_4);   // 縦線
        }
        // 画像2
        if (prcinfo.imgprocdata[IMGPROC_ID_IMG_2].valid)
        {
            x0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posx - 10 * (INT)(imgproc.cols / DISP_IMG_WIDTH);  y0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posy;
            x1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posx + 10 * (INT)(imgproc.cols / DISP_IMG_WIDTH);  y1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posy;
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgproc.rows / DISP_IMG_HEIGHT), cv::LINE_4);  // 横線

            x0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posx;  y0 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posy - 10 * (INT)(imgproc.rows / DISP_IMG_HEIGHT);
            x1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posx;  y1 = (INT)prcinfo.imgprocdata[IMGPROC_ID_IMG_2].posy + 10 * (INT)(imgproc.rows / DISP_IMG_HEIGHT);
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 255), 2 * (INT)(imgproc.cols / DISP_IMG_WIDTH), cv::LINE_4);   // 縦線
        }
        if (prcinfo.valid)
        {
            x0 = (INT)prcinfo.swaydata[AXIS_X].pos - 10 * (INT)(imgproc.cols / DISP_IMG_WIDTH);  y0 = (INT)prcinfo.swaydata[AXIS_Y].pos;
            x1 = (INT)prcinfo.swaydata[AXIS_X].pos + 10 * (INT)(imgproc.cols / DISP_IMG_WIDTH);  y1 = (INT)prcinfo.swaydata[AXIS_Y].pos;
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 0, 255), 2 * (INT)(imgproc.rows / DISP_IMG_HEIGHT), cv::LINE_4);    // 横線

            x0 = (INT)prcinfo.swaydata[AXIS_X].pos;  y0 = (INT)prcinfo.swaydata[AXIS_Y].pos - 10 * (INT)(imgproc.rows / DISP_IMG_HEIGHT);
            x1 = (INT)prcinfo.swaydata[AXIS_X].pos;  y1 = (INT)prcinfo.swaydata[AXIS_Y].pos + 10 * (INT)(imgproc.rows / DISP_IMG_HEIGHT);
            cv::line(m_mtSaveImage, Point(x0, y0), Point(x1, y1), Scalar(0, 0, 255), 2 * (INT)(imgproc.cols / DISP_IMG_WIDTH), cv::LINE_4);     // 縦線
        }
        cv::resize(m_mtSaveImage, imgdisp, cv::Size(), DISP_IMG_WIDTH / imgproc.cols, DISP_IMG_HEIGHT / imgproc.rows);

        //----------------------------------------------------------------------------
        // H,S,V表示
        if (m_bCursor)
        {
            cv::Mat imgHSV;
            cv::cvtColor(imgproc, imgHSV, COLOR_BGR2HSV);
            int H = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 0];
            int S = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 1];
            int V = imgHSV.data[m_pntCursor.y * imgHSV.step + m_pntCursor.x * imgHSV.elemSize() + 2];
            cv::putText(imgdisp, cv::format("(%d,%d)H:%03d S:%03d V:%03d", m_pntCursor.x, m_pntCursor.y, H, S, V), cv::Point(5, (int)DISP_IMG_HEIGHT - 5), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
        }

        //----------------------------------------------------------------------------
        // 元画像表示
        char* ColorBuf = (char*)calloc(imgdisp.cols * imgdisp.rows * 4, sizeof(RGBQUAD));
        for (int y = 0; y < imgdisp.rows; y++)
        {
            for (int x = 0; x < imgdisp.cols; x++)
            {
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 0] = imgdisp.data[y * imgdisp.step + x * 3 + 0];  // Blue
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 1] = imgdisp.data[y * imgdisp.step + x * 3 + 1];  // Green
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 2] = imgdisp.data[y * imgdisp.step + x * 3 + 2];  // Red
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 3] = 0;                                           // Reserved
            }
        }
        bmp = CreateBitmap(imgdisp.cols, imgdisp.rows, 1, 32, ColorBuf);
        free(ColorBuf);

        SendMessage(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_RAW), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
        DeleteObject(bmp);

        //----------------------------------------------------------------------------
        // マスク画像表示
        if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_1] && m_imgprocparam.maskvalid[IMGPROC_ID_IMG_2])
        {
            if (m_iSelImg == 0)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_A, &imgmask) != RESULT_OK)
                {
                    if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_B, &imgmask) != RESULT_OK) {return;}  // 成功以外のため、終了
                }
            }
            else
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_A, &imgmask) != RESULT_OK)
                {
                    if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_B, &imgmask) != RESULT_OK) {return;}  // 成功以外のため、終了
                }
            }
        }
        else if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_1])
        {
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_A, &imgmask) != RESULT_OK)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_B, &imgmask) != RESULT_OK) {return;}  // 成功以外のため、終了
            }
        }
        else if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_2])
        {
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_A, &imgmask) != RESULT_OK)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK2_B, &imgmask) != RESULT_OK) {return;}  // 成功以外のため、終了
            }
        }
        else
        {
            if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_A, &imgmask) != RESULT_OK)
            {
                if (g_pSharedObject->GetImage(IMAGE_ID_MASK1_B, &imgmask) != RESULT_OK) {return;}  // 成功以外のため、終了
            }
        }
        cv::resize(imgmask, imgdisp, cv::Size(), DISP_IMG_WIDTH / imgmask.cols, DISP_IMG_HEIGHT / imgmask.rows);

        ColorBuf = (char*)calloc(imgdisp.cols * imgdisp.rows * 4, sizeof(RGBQUAD));
        for (int y = 0; y < imgdisp.rows; y++)
        {
            for (int x = 0; x < imgdisp.cols; x++)
            {
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 0] = imgdisp.data[y * imgdisp.step + x];  // Blue
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 1] = imgdisp.data[y * imgdisp.step + x];  // Green
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 2] = imgdisp.data[y * imgdisp.step + x];  // Red
                ColorBuf[y * imgdisp.cols * 4 + x * 4 + 3] = 0;                                   // Reserved
            }
        }
        bmp = CreateBitmap(imgdisp.cols, imgdisp.rows, 1, 32, ColorBuf);
        free(ColorBuf);

        SendMessage(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_MASK), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
        DeleteObject(bmp);
    }   // if (bImgEnable)

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
        m_hCamDlg = CreateDialog(inf.hInstance, MAKEINTRESOURCE(IDD_DIALOG_IMAGE_PROC), nullptr, (DLGPROC)CameraWndProc);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_RAW),  DISP_IMG_RAW_X0, DISP_IMG_RAW_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        MoveWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_IMAGE_MASK), DISP_IMG_MSK_X0, DISP_IMG_MSK_Y0, (int)DISP_IMG_WIDTH, (int)DISP_IMG_HEIGHT, false);
        ShowWindow(m_hCamDlg, SW_SHOW);

        //----------------------------------------------------------------------------
        // 操作
        {
            if (m_camparam.imgsource == GRAB_IMG_GRAB_CAMERA)
            {
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  FALSE);
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   TRUE);
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), FALSE);
            }
            else
            {
                if (m_camparam.imgsource == GRAB_IMG_GRAB_FILE)
                {
                    EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  FALSE);
                    EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   TRUE);
                    EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), FALSE);
                }
                else
                {
                    EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  TRUE);
                    EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   FALSE);
                    EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), TRUE);
                }
            }
        }

        //----------------------------------------------------------------------------
        // 画像1
        {
            HWND    wndhdl;
            TCHAR   msg[10];
            // 色相H(Low)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].h);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].h); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 色相H(Upp)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].h);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].h); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 彩度S(Low)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].s);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].s); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 彩度S(Upp)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,   TRUE,  MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].s);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].s); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 明度V(Low)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].v);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].v); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 明度V(Upp)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].v);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].v); SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // 画像2
        {
            HWND    wndhdl;
            TCHAR   msg[10];

            // 色相H(Low)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].h);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].h); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 色相H(Upp)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  20,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].h);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].h); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 彩度S(Low)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].s);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].s); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 彩度S(Upp)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].s);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].s); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 明度V(Low)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].v);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_LOW);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].v); SetWindowText(wndhdl, (LPCTSTR)msg);
            // 明度V(Upp)
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP);
            SendMessage(wndhdl, TBM_SETRANGE,    TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX));   // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ,  25,   0);                                              // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS,      TRUE, m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].v);        // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0,    1);                                              // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_UPP);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].v); SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // 画像選択
        {
            LPCTSTR strImgItem[] = {TEXT("画像1"), TEXT("画像2")};
            HWND    wndhdl;
            wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_IMG);
            if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_1] && m_imgprocparam.maskvalid[IMGPROC_ID_IMG_2])
            {
                for (int i = 0; i < 2; i++)   {SendMessage(wndhdl, CB_ADDSTRING, i, (LPARAM)strImgItem[i]);}
                SendMessage(wndhdl, CB_SETCURSEL, m_iSelImg, 0);
            }
            else if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_1])
            {
                SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strImgItem[0]);
                SendMessage(wndhdl, CB_SETCURSEL, m_iSelImg, 0);
            }
            else if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_2])
            {
                SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strImgItem[1]);
                SendMessage(wndhdl, CB_SETCURSEL, m_iSelImg, 0);
            }
            else ;
        }

        //----------------------------------------------------------------------------
        // ノイズフィルタ
        // ゴマ塩
        {
            // 種類
            UINT32  pgsize = 1;
            LPCTSTR strFilterItem[] = {TEXT("なし"), TEXT("中央値フィルタ"), TEXT("オープニング処理")};
            HWND    wndhdl;
            TCHAR   msg[10];
            wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER1);
            for (int i = 0; i < NOIZEFILTER1_MAX; i++) {SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strFilterItem[i]);}
            SendMessage(wndhdl, CB_SETCURSEL, m_imgprocparam.filter1.type, 0);
            if ((m_imgprocparam.filter1.type == NOISEFILTER1_MEDIAN) || (m_imgprocparam.filter1.type == NOISEFILTER1_OPENNING))
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1),     SW_SHOW);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), SW_SHOW);
                if (m_imgprocparam.filter1.type == NOISEFILTER1_MEDIAN) {pgsize = 2;}
            }
            else
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1),     SW_HIDE);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), SW_HIDE);
            }
            // フィルタ値
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1);
            SendMessage(wndhdl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30));         // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ, 5, 0);                          // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS, TRUE, m_imgprocparam.filter1.val);  // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, pgsize);                    // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.filter1.val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }
        // 穴埋め
        {
            // 種類
            UINT32  pgsize = 1;
            LPCTSTR strFilterItem[] = {TEXT("なし"), TEXT("クロージング処理")};
            HWND    wndhdl;
            TCHAR   msg[10];
            wndhdl = GetDlgItem(m_hCamDlg, IDC_COMBO_NOISEFILTER2);
            for (int i = 0; i < NOIZEFILTER2_MAX; i++) {SendMessage(wndhdl, CB_ADDSTRING, 0, (LPARAM)strFilterItem[i]);}
            SendMessage(wndhdl, CB_SETCURSEL, m_imgprocparam.filter2.type, 0);
            if (m_imgprocparam.filter2.type == NOISEFILTER2_CLOSING)
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2),     SW_SHOW);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), SW_SHOW);
            }
            else
            {
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2),     SW_HIDE);
                ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), SW_HIDE);
            }
            // フィルタ値
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2);
            SendMessage(wndhdl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30));         // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ, 5, 0);                          // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS, TRUE, m_imgprocparam.filter2.val);  // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, pgsize);                    // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2);
            _stprintf_s(msg, 10, TEXT("%d"), m_imgprocparam.filter2.val);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // カメラ露光時間
        {
            HWND    wndhdl;
            TCHAR   msg[10];

            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE);
            SendMessage(wndhdl, TBM_SETRANGEMIN, TRUE, (int)m_camparam.exptimemin);         // レンジを指定
            SendMessage(wndhdl, TBM_SETRANGEMAX, TRUE, (int)m_camparam.exptimemax);         // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ, 1000, 0);                                   // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS, TRUE, (int)m_camparam.exptime);                 // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, 1);                                     // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_CAMERA_EXPOSURE);
            _stprintf_s(msg, 10, TEXT("%d"), (int)m_camparam.exptime);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // ROI有効
        {
            HWND    wndhdl;
            wndhdl = GetDlgItem(m_hCamDlg, IDC_CHECK_ROI);
            if (m_imgprocparam.roi.valid > 0) {SendMessage(wndhdl, BM_SETCHECK, BST_CHECKED,   0);}
            else                              {SendMessage(wndhdl, BM_SETCHECK, BST_UNCHECKED, 0);}
        }

        //----------------------------------------------------------------------------
        // ロープ長
        {
            HWND    wndhdl;
            TCHAR   msg[10];

            m_extninfo.ropelen = EXTN_ROPELEN_MIN;
            wndhdl = GetDlgItem(m_hCamDlg, IDC_SLIDER_ROPELEN);
            SendMessage(wndhdl, TBM_SETRANGEMIN, TRUE, (UINT)EXTN_ROPELEN_MIN);             // レンジを指定
            SendMessage(wndhdl, TBM_SETRANGEMAX, TRUE, (UINT)EXTN_ROPELEN_MAX);             // レンジを指定
            SendMessage(wndhdl, TBM_SETTICFREQ, 1000, 0);                                   // 目盛りの増分
            SendMessage(wndhdl, TBM_SETPOS, TRUE, (UINT)m_extninfo.ropelen);                // 位置の設定
            SendMessage(wndhdl, TBM_SETPAGESIZE, 0, 1);                                     // クリック時の移動量
            wndhdl = GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_ROPELEN);
            _stprintf_s(msg, 10, TEXT("%d"), (UINT)m_extninfo.ropelen);  SetWindowText(wndhdl, (LPCTSTR)msg);
        }

        //----------------------------------------------------------------------------
        // カメラ情報
        {
            TCHAR   msg[10];
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_CAMERA_STAT),   msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_IMG_GRAB_TIME), msg);
        }

        //----------------------------------------------------------------------------
        // RIO情報
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
        // 処理情報
        {
            TCHAR   msg[10];
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X1),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y1),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_X2),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_GRV_Y2),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_X),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_Y),     msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_SPD_X), msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_SWAY_SPD_Y), msg);
            _stprintf_s(msg, TEXT("-"));    SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_PROC_TIME),  msg);
        }
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
                m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].h = pos;
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H1_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H1_UPP), (LPCTSTR)str);
                m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].h = pos;
            }
            else ;
            // 彩度S
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_LOW), (LPCTSTR)str);
                m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].s = pos;
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S1_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S1_UPP), (LPCTSTR)str);
                m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].s = pos;
            }
            else ;
            // 明度V
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_LOW), (LPCTSTR)str);
                m_imgprocparam.hsvl[IMGPROC_ID_IMG_1].v = pos;
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V1_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V1_UPP), (LPCTSTR)str);
                m_imgprocparam.hsvu[IMGPROC_ID_IMG_1].v = pos;
            }
            else ;

            //----------------------------------------------------------------------------
            // 画像2
            // 色相H
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_LOW), (LPCTSTR)str);
                m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].h = pos;
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_H2_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_H2_UPP), (LPCTSTR)str);
                m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].h = pos;
            }
            else ;
            // 彩度S
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_LOW), (LPCTSTR)str);
                m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].s = pos;
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_S2_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_S2_UPP), (LPCTSTR)str);
                m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].s = pos;
            }
            else ;
            // 明度V
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_LOW), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_LOW), (LPCTSTR)str);
                m_imgprocparam.hsvl[IMGPROC_ID_IMG_2].v = pos;
            }
            else if (GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_V2_UPP), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_V2_UPP), (LPCTSTR)str);
                m_imgprocparam.hsvu[IMGPROC_ID_IMG_2].v = pos;
            }
            else ;

            //----------------------------------------------------------------------------
            // ノイズフィルタ
            // ゴマ塩
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_GETPOS, 0, 0);
                if (m_imgprocparam.filter1.type == NOISEFILTER1_MEDIAN)
                {
                    if ((pos % 2) == 0) {pos = pos + 1;}
                    SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPOS, TRUE, pos);  // 位置の設定
                }
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), (LPCTSTR)str);
                m_imgprocparam.filter1.val = pos;
            }
            // 穴埋め
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), (LPCTSTR)str);
                m_imgprocparam.filter2.val = pos;
            }

            //----------------------------------------------------------------------------
            // カメラ露光時間
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_CAMERA_EXPOSURE), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_CAMERA_EXPOSURE), (LPCTSTR)str);
                m_camparam.exptime = (double)pos;
            }

            //----------------------------------------------------------------------------
            // ロープ長
            if (GetDlgItem(m_hCamDlg, IDC_SLIDER_ROPELEN) == (HWND)lp)
            {
                pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_ROPELEN), TBM_GETPOS, 0, 0);
                _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_ROPELEN), (LPCTSTR)str);
                m_extninfo.ropelen = (double)pos;
            }
        }
        break;
    case WM_COMMAND:
        // 選択されたメニューの解析:
        switch (wmId)
        {
        case IDC_BUTTON_CAM_START:
            m_camparam.imgsource = GRAB_IMG_GRAB_CAMERA;
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), FALSE);
            break;
        case IDC_BUTTON_CAM_STOP:
            m_camparam.imgsource = GRAB_IMG_STOP;
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  TRUE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   FALSE);
            EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), TRUE);
            break;
        case IDC_BUTTON_SAVE:
            if (m_mtSaveImage.empty() != TRUE) {imwrite("procImage.bmp", m_mtSaveImage);}
            else                               {MessageBox(m_hCamDlg, L"保存する画像がありません。", L"エラー", MB_OK);}
            break;
        case IDC_BUTTON_FILE_START:
            {
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_START),  FALSE);
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_CAM_STOP),   TRUE);
                EnableWindow(GetDlgItem(m_hCamDlg, IDC_BUTTON_FILE_START), FALSE);

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
                            m_camparam.imgfname  = str;
                            m_camparam.imgsource = GRAB_IMG_GRAB_FILE;
                        }
                        free(cstr);
                    }
                }
            }
            break;
        case IDC_CHECK_ROI:
            if (BST_CHECKED == SendMessage(GetDlgItem(m_hCamDlg, IDC_CHECK_ROI), BM_GETCHECK, 0, 0)) {m_imgprocparam.roi.valid = 1;}
            else                                                                                     {m_imgprocparam.roi.valid = 0;}
            break;
        case IDC_CHECK_CURSOR:
            if (BST_CHECKED == SendMessage(GetDlgItem(m_hCamDlg, IDC_CHECK_CURSOR), BM_GETCHECK, 0, 0)) {m_bCursor = TRUE;}
            else                                                                                        {m_bCursor = FALSE;}
            break;
        case IDC_BUTTON_CURSOR_L:
            {
                LONG    pos;
                pos = m_pntCursor.x;
                if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos -= 50;}
                else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos -= 10;}
                else                                            {pos -= 1;}
                if (pos < 0) {pos = m_camparam.size[AXIS_X] - 1;}
                m_pntCursor.x = pos;
            }
            break;
        case IDC_BUTTON_CURSOR_R:
            {
                LONG    pos;
                pos = m_pntCursor.x;
                if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos += 50;}
                else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos += 10;}
                else                                            {pos += 1;}
                if (pos >= (LONG)m_camparam.size[AXIS_X]) {pos = 0;}
                m_pntCursor.x = pos;
            }
            break;
        case IDC_BUTTON_CURSOR_U:
            {
                LONG    pos;
                pos = m_pntCursor.y;
                if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos -= 50;}
                else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos -= 10;}
                else                                            {pos -= 1;}
                if (pos < 0) {pos = m_camparam.size[AXIS_Y] - 1;}
                m_pntCursor.y = pos;
            }
            break;
        case IDC_BUTTON_CURSOR_D:
            {
                LONG    pos;
                pos = m_pntCursor.y;
                if      (GetAsyncKeyState(VK_SHIFT)   & 0x8000) {pos += 50;}
                else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {pos += 10;}
                else                                            {pos += 1;}
                if (pos >= (LONG)m_camparam.size[AXIS_Y]) {pos = 0;}
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
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPAGESIZE, 0, 2);  // クリック時の移動量
                        pos = SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_GETPOS, 0, 0);
                        if ((pos % 2) == 0) {pos = pos + 1;}
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPOS, TRUE, pos);  // 位置の設定
                        _stprintf_s(str, 10, TEXT("%d"), pos);  SetWindowText(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), (LPCTSTR)str);
                        m_imgprocparam.filter1.val = pos;
                    }
                    else
                    {
                        SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1), TBM_SETPAGESIZE, 0, 1);  // クリック時の移動量
                    }
                }
                else
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER1),     SW_HIDE);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER1), SW_HIDE);
                }
                m_imgprocparam.filter1.type = sel;
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
                    SendMessage(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2), TBM_SETPAGESIZE, 0, 1);  // クリック時の移動量
                }
                else
                {
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_SLIDER_NOISEFILTER2),     SW_HIDE);
                    ShowWindow(GetDlgItem(m_hCamDlg, IDC_STATIC_VAL_NOISEFILTER2), SW_HIDE);
                }
                m_imgprocparam.filter2.type = sel;
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
