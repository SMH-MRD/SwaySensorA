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
    cv::Mat imgSrc;
    UINT32  width = 0, height = 0;

    // 処理時間計測(開始時間取得)
    LARGE_INTEGER    frequency;
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER    start;
    QueryPerformanceCounter(&start);

    if (g_pSharedObject->GetImage(IMAGE_ID_CAM_A, &imgSrc) != RESULT_OK)
    {
        if (g_pSharedObject->GetImage(IMAGE_ID_CAM_B, &imgSrc) != RESULT_OK) {return;}  // 成功以外のため、終了
    }

//////////////////////////////////////////////////////////////////////////////
#if 0
    //----------------------------------------------------------------------------
    // ①画像色をBGR→HSVに変換
    cv::Mat imgHSV;
    cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);

    //----------------------------------------------------------------------------
    // ②色相でマスク画像作成
    UINT32  maskValid, maskLow[3], maskUpp[3];
    cv::Mat imgBinHSV, imgBinHSV1, imgBinHSV2;
    // マスク1生成
    {
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VALID, &maskValid);
        if (maskValid)
        {
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HLOW, &maskLow[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HUPP, &maskUpp[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SLOW, &maskLow[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SUPP, &maskUpp[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VLOW, &maskLow[2]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VUPP, &maskUpp[2]);
        }
        else
        {
            maskLow[0] = 0; maskUpp[0] = 0;
            maskLow[1] = 0; maskUpp[1] = 255;
            maskLow[2] = 0; maskUpp[2] = 255;
        }
        Scalar maskMin = Scalar(maskLow[0], maskLow[1], maskLow[2]);
        Scalar maskMax = Scalar(maskUpp[0], maskUpp[1], maskUpp[2]);
        inRange(imgHSV, maskMin, maskMax, imgBinHSV1);
    }

    // マスク2生成
    {
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VALID, &maskValid);
        if (maskValid)
        {
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HLOW, &maskLow[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HUPP, &maskUpp[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SLOW, &maskLow[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SUPP, &maskUpp[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VLOW, &maskLow[2]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VUPP, &maskUpp[2]);
        }
        else
        {
            maskLow[0] = 0; maskUpp[0] = 0;
            maskLow[1] = 0; maskUpp[1] = 255;
            maskLow[2] = 0; maskUpp[2] = 255;
        }
        Scalar maskMin = Scalar(maskLow[0], maskLow[1], maskLow[2]);
        Scalar maskMax = Scalar(maskUpp[0], maskUpp[1], maskUpp[2]);
        inRange(imgHSV, maskMin, maskMax, imgBinHSV2);
    }

    // 各チャンネルごとに2値化された画像の合成
//  imgBinHSV = imgBinHSV1 + imgBinHSV2;
    cv::add(imgBinHSV1, imgBinHSV2, imgBinHSV);

    //----------------------------------------------------------------------------
    // ③3チャンネル全てのANDを取り、マスク画像を作成する
    // チャンネルごとに二値化された画像をそれぞれのチャンネルに分解する
    std::vector<cv::Mat> planes;
    cv::split(imgHSV, planes); // チャンネルごとに分解

    cv::Mat imgMask;
    cv::bitwise_and(planes[2], planes[2], imgMask, imgBinHSV);
    imwrite("./imgMask.jpg", imgMask);

    cv::Mat imgBin;
    cv::threshold(imgMask, imgBin, 100, 255, THRESH_BINARY);
    if (m_bSaveImageArea == FALSE)
    {
        g_pSharedObject->SetImage(IMAGE_ID_MASK_A, imgBin);
        m_bSaveImageArea = TRUE;
    }
    else
    {
        g_pSharedObject->SetImage(IMAGE_ID_MASK_B, imgBin);
        m_bSaveImageArea = FALSE;
    }

    //----------------------------------------------------------------------------
    // ④領域抽出(ROI)でターゲット認識
    // 輪郭抽出
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(imgMask, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // ⑤候補ターゲットの信頼性評価

    // ⑥重心位置計算
    double posX = 0;
    double posY = 0;

    UINT32 algo;
    g_pSharedObject->GetParam(PARAM_ID_PIC_ALGORITHM, &algo);
    if (algo == COG_ALGORITHM_ALL) {CalcCenterOfGravity1(imgSrc, contours, &posX, &posY);}
    else                           {CalcCenterOfGravity2(imgSrc, contours, &posX, &posY);}

    STProcData stProcData;
    stProcData.image = imgSrc;
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
//////////////////////////////////////////////////////////////////////////////
#else
    //----------------------------------------------------------------------------
    // ①画像色をBGR→HSVに変換
    cv::Mat imgHSV;
    cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);

    //----------------------------------------------------------------------------
    // ②各チャンネルごとに2値化(LUT変換)
    UINT32  maskValid, maskLow[3], maskUpp[3];
    cv::Mat imgBinHSV1, imgBinHSV2;
    // 画像1
    {
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VALID, &maskValid);
        if (maskValid)
        {
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HLOW, &maskLow[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HUPP, &maskUpp[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SLOW, &maskLow[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SUPP, &maskUpp[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VLOW, &maskLow[2]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VUPP, &maskUpp[2]);
            cv::Mat lut = cv::Mat(256, 1, CV_8UC3);
            for (int i = 0; i < 256; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    if ((int)maskLow[k] <= (int)maskUpp[k])
                    {
                        if (((int)maskLow[k] <= i) && (i <= (int)maskUpp[k])) {lut.data[i*lut.step + k] = 255;}
                        else                                                  {lut.data[i*lut.step + k] = 0;}
                    }
                    else
                    {
                        if ((i <= (int)maskUpp[k]) || ((int)maskLow[k] <= i)) {lut.data[i*lut.step + k] = 255;}
                        else                                                  {lut.data[i*lut.step + k] = 0;}
                    }
                }
            }
            // チャンネルごとのLUT変換(各チャンネルごとに2値化処理)
            cv::LUT(imgHSV, lut, imgBinHSV1);
        }
        else
        {
            imgBinHSV1 = cv::Mat::zeros(imgHSV.rows, imgHSV.cols, CV_8UC3);
        }
    }

    // 画像2
    {
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VALID, &maskValid);
        if (maskValid)
        {
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HLOW, &maskLow[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HUPP, &maskUpp[0]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SLOW, &maskLow[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SUPP, &maskUpp[1]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VLOW, &maskLow[2]);
            g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VUPP, &maskUpp[2]);
            cv::Mat lut = cv::Mat(256, 1, CV_8UC3);
            for (int i = 0; i < 256; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    if ((int)maskLow[k] <= (int)maskUpp[k])
                    {
                        if (((int)maskLow[k] <= i) && (i <= (int)maskUpp[k])) {lut.data[i*lut.step + k] = 255;}
                        else                                                  {lut.data[i*lut.step + k] = 0;}
                    }
                    else
                    {
                        if ((i <= (int)maskUpp[k]) || ((int)maskLow[k] <= i)) {lut.data[i*lut.step + k] = 255;}
                        else                                                  {lut.data[i*lut.step + k] = 0;}
                    }
                }
            }
            // チャンネルごとのLUT変換(各チャンネルごとに2値化処理)
            cv::LUT(imgHSV, lut, imgBinHSV2);
        }
        else
        {
            imgBinHSV2 = cv::Mat::zeros(imgHSV.rows, imgHSV.cols, CV_8UC3);
        }
    }

    //----------------------------------------------------------------------------
    // ③3チャンネル全てのANDを取り、マスク画像を作成する
    cv::Mat imgMask1, imgMask2;
    std::vector<cv::Mat> planes;
    // 画像1
    cv::split(imgBinHSV1, planes);  // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する
    cv::bitwise_and(planes[0], planes[1], imgMask1);
    cv::bitwise_and(imgMask1,  planes[2], imgMask1);
    // 画像2
    cv::split(imgBinHSV2, planes);  // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する
    cv::bitwise_and(planes[0], planes[1], imgMask2);
    cv::bitwise_and(imgMask2,  planes[2], imgMask2);

    // マスクされた画像の合成
    cv::Mat imgMask;
//  imgMask = imgMask1 + imgMask2;
    cv::add(imgMask1, imgMask2, imgMask);

//  // 出力
//  cv::Mat imgMasked;
//  imgSrc.copyTo(imgMasked, imgMask);

    // ノイズ除去
    UINT32 filter;
    UINT32 filterval;
    g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTER,    &filter);
    g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTERVAL, &filterval);
    switch (filter)
    {
    case NOISEFILTER_MEDIAN:    // 中央値フィルタ
        cv::medianBlur(imgMask, imgMask, filterval);
        break;
    case NOISEFILTER_OPENNING:  // オープニング処理
        cv::erode(imgMask,  imgMask, cv::Mat(), cv::Point(-1, -1), filterval);  // 収縮
        cv::dilate(imgMask, imgMask, cv::Mat(), cv::Point(-1, -1), filterval);  // 膨張
        break;
    default:
        break;
    }

    if (m_bSaveImageArea == FALSE)
    {
        g_pSharedObject->SetImage(IMAGE_ID_MASK_A, imgMask);
        m_bSaveImageArea = TRUE;
    }
    else
    {
        g_pSharedObject->SetImage(IMAGE_ID_MASK_B, imgMask);
        m_bSaveImageArea = FALSE;
    }

    //----------------------------------------------------------------------------
    // ④領域抽出(ROI)でターゲット認識
    // 輪郭抽出
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(imgMask, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // ⑤候補ターゲットの信頼性評価

    // ⑥重心位置計算
    double posX = 0;
    double posY = 0;

    UINT32 algo;
    g_pSharedObject->GetParam(PARAM_ID_PIC_ALGORITHM, &algo);
    if (algo == COG_ALGORITHM_ALL) {CalcCenterOfGravity1(imgSrc, contours, &posX, &posY);}
    else                           {CalcCenterOfGravity2(imgSrc, contours, &posX, &posY);}

    STProcData stProcData;
    stProcData.image = imgSrc;
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
#endif
//////////////////////////////////////////////////////////////////

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
