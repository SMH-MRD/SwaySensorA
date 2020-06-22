#include "CAnalyst.h"
#include "CSharedObject.h"

extern CSharedObject*   g_pSharedObject;    // タスク間共有データのポインタ

/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CAnalyst::CAnalyst()
{
}

/// @brief デストラクタ
/// @param
/// @return 
/// @note
CAnalyst::~CAnalyst()
{
}

/// @brief 
/// @param
/// @return 
/// @note
void CAnalyst::init_task(void* pobj)
{
}

/// @brief 
/// @param
/// @return 
/// @note
void CAnalyst::routine_work(void* param)
{
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();

    ImageProc();
    InclinationProc();

    return;
}

/// @brief 画像処理
/// @param
/// @return 
/// @note
void CAnalyst::ImageProc(void)
{
    // 画像変換処理
    Mat     image;
    UINT32  width = 0, height = 0;

    // 処理時間計測(開始時間取得)
    LARGE_INTEGER    frequency;
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER    start;
    QueryPerformanceCounter(&start);

    if (g_pSharedObject->GetImage(IMAGE_ID_CAM_A, &image) != RESULT_OK)
    {
        if (g_pSharedObject->GetImage(IMAGE_ID_CAM_B, &image) != RESULT_OK)
        {
            return; // 成功以外のため、終了
        }
    }

    Mat hsvImage;
    // ①画像色をBGR→HSVに変換
    cvtColor(image, hsvImage, COLOR_BGR2HSV);
#ifdef DEBUG
    namedWindow("hsvImage", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO
    imshow("hsvImage", hsvImage);
#endif

    // ②色相でマスク画像作成
    UINT32  maskEn, maskMin, maskMax;
    Mat     maskImage1, maskImage2, maskImage3;
    g_pSharedObject->GetParam(PARAM_ID_PIC_HUE1_EN, &maskEn);
    if (maskEn == TRUE)
    {
        // マスク1生成
        g_pSharedObject->GetParam(PARAM_ID_PIC_HUE1_MIN, &maskMin);
        g_pSharedObject->GetParam(PARAM_ID_PIC_HUE1_MAX, &maskMax);
        Scalar mask1Min = Scalar(maskMin, S_MIN, V_MIN);
        Scalar mask1Max = Scalar(maskMax, S_MAX, V_MAX);
        inRange(hsvImage, mask1Min, mask1Max, maskImage1);
    }
    else
    {
        Scalar mask1Min = Scalar(0, S_MIN, V_MIN);
        Scalar mask1Max = Scalar(0, S_MAX, V_MAX);
        inRange(hsvImage, mask1Min, mask1Max, maskImage1);
    }

    g_pSharedObject->GetParam(PARAM_ID_PIC_HUE2_EN, &maskEn);
    if (maskEn == TRUE)
    {
        // マスク2生成
        g_pSharedObject->GetParam(PARAM_ID_PIC_HUE2_MIN, &maskMin);
        g_pSharedObject->GetParam(PARAM_ID_PIC_HUE2_MAX, &maskMax);
        Scalar mask2Min = Scalar(maskMin, S_MIN, V_MIN);
        Scalar mask2Max = Scalar(maskMax, S_MAX, V_MAX);
        inRange(hsvImage, mask2Min, mask2Max, maskImage2);
    }
    else
    {
        Scalar mask2Min = Scalar(0, S_MIN, V_MIN);
        Scalar mask2Max = Scalar(0, S_MAX, V_MAX);
        inRange(hsvImage, mask2Min, mask2Max, maskImage2);
    }

    g_pSharedObject->GetParam(PARAM_ID_PIC_HUE3_EN, &maskEn);
    if (maskEn == TRUE)
    {
        // マスク3生成
        g_pSharedObject->GetParam(PARAM_ID_PIC_HUE3_MIN, &maskMin);
        g_pSharedObject->GetParam(PARAM_ID_PIC_HUE3_MAX, &maskMax);
        Scalar mask3Min = Scalar(maskMin, S_MIN, V_MIN);
        Scalar mask3Max = Scalar(maskMax, S_MAX, V_MAX);
        inRange(hsvImage, mask3Min, mask3Max, maskImage3);
    }
    else
    {
        Scalar mask3Min = Scalar(0, S_MIN, V_MIN);
        Scalar mask3Max = Scalar(0, S_MAX, V_MAX);
        inRange(hsvImage, mask3Min, mask3Max, maskImage3);
    }

    // 色相マスク生成
    Mat maskImage;
    maskImage = maskImage1 + maskImage2 + maskImage3;

    // ③V画像にマスクを掛けた後、2値化
    Mat splitImage[3];
    Mat maskedImg;
    split(hsvImage, splitImage);
    bitwise_and(splitImage[2], splitImage[2], maskedImg, maskImage);

    Mat binarizeImage;
    threshold(maskedImg, binarizeImage, 100, 255, THRESH_BINARY);
    if (m_bSaveImageArea == FALSE)
    {
        g_pSharedObject->SetImage(IMAGE_ID_MASK_A, binarizeImage);
        m_bSaveImageArea = TRUE;
    }
    else
    {
        g_pSharedObject->SetImage(IMAGE_ID_MASK_B, binarizeImage);
        m_bSaveImageArea = FALSE;
    }
#ifdef DEBUG
    namedWindow("maskImage", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO
    imshow("maskImage", binarizeImage);
#endif
    // ④領域抽出(ROI)でターゲット認識
    // 輪郭抽出
    vector<vector<Point>> contours;
    findContours(binarizeImage, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // ⑤候補ターゲットの信頼性評価

    // ⑥重心位置計算
    double posX = 0;
    double posY = 0;

    UINT32 algo;
    g_pSharedObject->GetParam(PARAM_ID_PIC_COG_ALGO, &algo);
    if (algo == COG_ALGO_KIND_ALL) {CalcCenterOfGravity1(image, contours, &posX, &posY);}
    else                           {CalcCenterOfGravity2(image, contours, &posX, &posY);}
#ifdef DEBUG
    namedWindow("procImage", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO
    imshow("procImage", image);
    waitKey(0);
#endif
    STProcData stProcData;
    stProcData.image = image;
    stProcData.posx  = posX;
    stProcData.posy  = posY;
    if (m_bSaveImageArea == FALSE)
    {
        g_pSharedObject->SetProcImage(IMAGE_ID_PROC_A, stProcData);
        m_bSaveImageArea = TRUE;
    }
    else
    {
        g_pSharedObject->SetProcImage(IMAGE_ID_PROC_B, stProcData);
        m_bSaveImageArea = FALSE;
    }

    // 処理時間計測(終了時間取得)
    LARGE_INTEGER    end;
    QueryPerformanceCounter(&end);
    LONGLONG span = end.QuadPart - start.QuadPart;
    LONGLONG usec = (span * 1000000L) / frequency.QuadPart;

    g_pSharedObject->SetParam(PARAM_ID_DOUBLE_PROC_TIME, (DOUBLE)usec / 1000.0);
}

/// @brief 傾斜計データ処理
/// @param
/// @return 
/// @note
void CAnalyst::InclinationProc(void)
{
    DOUBLE port1AnaData, port2AnaData;
    DOUBLE port1AngleData, port2AngleData;

    if (g_pSharedObject->GetInclinoData(INCLINO_ID_PORT_1_MA, &port1AnaData) == RESULT_OK)
    {
        if (isnan(port1AnaData))
        {
            port1AngleData = (DOUBLE)NAN;
        }
        else if (port1AnaData < 12.0)
        {
            // 12.0mA未満ならば角度は-方向に倒れている(4mAで-30度)
            port1AngleData = ((port1AnaData - 4.0) - 8.0) / 8.0 * BEVEL_MAX_ANGLE;
        }
        else
        {
            // 12.0mA以上ならば角度は+方向に倒れている(20mAで30度)
            port1AngleData = (port1AnaData - 12.0) / 8.0 * BEVEL_MAX_ANGLE;
        }
        g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_1_RAD, port1AngleData);
    }

    if (g_pSharedObject->GetInclinoData(INCLINO_ID_PORT_2_MA, &port2AnaData) == RESULT_OK)
    {
        if (isnan(port2AnaData))
        {
            port2AngleData = (DOUBLE)NAN;
        }
        else if (port2AnaData < 12.0)
        {
            // 12.0mA未満ならば角度は-方向に倒れている(4mAで-30度)
            port2AngleData = ((port2AnaData - 4.0) - 8.0) / 8.0 * BEVEL_MAX_ANGLE;
        }
        else
        {
            // 12.0mA以上ならば角度は+方向に倒れている(20mAで30度)
            port2AngleData = (port2AnaData - 12.0) / 8.0 * BEVEL_MAX_ANGLE;
        }
        g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_RAD, port2AngleData);
    }
}

/// @brief 重心位置算出アルゴリズム1
/// @param
/// @return 
/// @note
void CAnalyst::CalcCenterOfGravity1(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY)
{
    double posX  = 0;
    double posY  = 0;
    UINT32 count = 0;

    // 輪郭全点から重心位置を算出
    for (UINT ii = 0; ii < contours.size(); ii++)
    {
        UINT tempCount = contours.at(ii).size();
        count += tempCount;
        for (UINT jj = 0; jj < tempCount; jj++)
        {
            posX += contours.at(ii).at(jj).x;
            posY += contours.at(ii).at(jj).y;
        }
    }

    posX /= count;
    posY /= count;

    circle(image, Point((INT)posX, (INT)posY), 30, Scalar(0, 0, 255), 3, 4);
    *outPosX = posX;
    *outPosY = posY;
}

/// @brief 重心位置算出アルゴリズム2
/// @param
/// @return 
/// @note
void CAnalyst::CalcCenterOfGravity2(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY)
{
    double  max_size  = 0;
    double  max2_size = 0;
    int     max_id    = -1;
    int     max2_id   = -1;

    if (contours.size() == 0)
    {
        // 対象輪郭が存在しない場合
        *outPosX = log(-1.0);
        *outPosY = log(-1.0);
        return;
    }

    // 輪郭長から上位2つの輪郭を持つ対象を抽出する
    for (size_t ii = 0; ii < contours.size(); ii++)
    {
        if (contours[ii].size() > max_size)
        {
            max_size = contours[ii].size();
            max_id   = ii;
        }
        else if (contours[ii].size() > max2_size)
        {
            max2_size = contours[ii].size();
            max2_id   = ii;
        }
        else ;
    }
    if ((max_id <= 0) || (max2_id <= 0))
    {
        // 対象輪郭が存在しない場合
        *outPosX = log(-1.0);
        *outPosY = log(-1.0);
        return;
    }

    Moments mu1 = moments(contours[max_id]);
    Point2f mc1 = Point2f((float)mu1.m10 / (float)mu1.m00, (float)mu1.m01 / (float)mu1.m00);
    Moments mu2 = moments(contours[max2_id]);
    Point2f mc2 = Point2f((float)mu2.m10 / (float)mu2.m00, (float)mu2.m01 / (float)mu2.m00);

    circle(image, Point((INT)((mc1.x + mc2.x) / 2), (INT)((mc1.y + mc2.y) / 2)), 30, Scalar(0, 0, 255), 3, 4);
    *outPosX = ((DOUBLE)mc1.x + mc2.x) / 2.0;
    *outPosY = ((DOUBLE)mc1.y + mc2.y) / 2.0;
}
