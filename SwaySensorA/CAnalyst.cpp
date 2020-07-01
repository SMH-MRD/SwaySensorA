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
    m_iBufferImgMask1 = IMAGE_ID_MASK1_A;
    m_iBufferImgMask2 = IMAGE_ID_MASK2_A;
    m_iBufferImgProc  = IMAGE_ID_PROC_A;
}

/// @brief 
/// @param
/// @return 
/// @note
void CAnalyst::routine_work(void* param)
{
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();

    //----------------------------------------------------------------------------
    // 処理時間計測(開始時間取得)
    LARGE_INTEGER   frequency;
    LARGE_INTEGER   start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    //----------------------------------------------------------------------------
    ImageProc();

    //----------------------------------------------------------------------------
    InclinationProc();

    //----------------------------------------------------------------------------
    // 処理時間計測(終了時間取得)
    LARGE_INTEGER   end;
    LONGLONG        span, usec;
    QueryPerformanceCounter(&end);
    span = end.QuadPart - start.QuadPart;
    usec = (span * 1000000L) / frequency.QuadPart;
    g_pSharedObject->SetParam(PARAM_ID_DOUBLE_PROC_TIME, (DOUBLE)usec / 1000.0);

    return;
}

/// @brief 画像処理
/// @param
/// @return 
/// @note
void CAnalyst::ImageProc(void)
{
    cv::Mat         imgSrc;
    cv::Mat         imgHSV;
    cv::Mat         imgHSVBin1, imgHSVBin2;
    cv::Mat         imgMask1, imgMask2;
    cv::Mat         lut;
    BOOL            bImgEnable = FALSE;
    UINT32          width = 0, height = 0;
    UINT            maskLow[3], maskUpp[3];
    std::vector<cv::Mat> planes;
    UINT32          filter;
    UINT32          filterval;
    std::vector<std::vector<cv::Point>> contours;
    stMngProcData   stProcData;
    double          posX, posY;
    UINT32          algo;
    BOOL            ret = FALSE;

    //----------------------------------------------------------------------------
    // 元画像読込み
    if (g_pSharedObject->GetImage(IMAGE_ID_RAW_A, &imgSrc) == RESULT_OK)
    {
        bImgEnable = TRUE;
    }
    else
    {
        if (g_pSharedObject->GetImage(IMAGE_ID_RAW_B, &imgSrc) == RESULT_OK)
        {
            bImgEnable = TRUE;
        }
    }

    if (bImgEnable)
    {
        //----------------------------------------------------------------------------
        // 画像色をBGR→HSVに変換
        cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);

        //----------------------------------------------------------------------------
        // 各チャンネルごとに2値化(LUT変換)
        lut = cv::Mat(256, 1, CV_8UC3);
        // 画像1
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HLOW, &maskLow[0]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_HUPP, &maskUpp[0]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SLOW, &maskLow[1]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_SUPP, &maskUpp[1]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VLOW, &maskLow[2]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK1_VUPP, &maskUpp[2]);
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
        cv::LUT(imgHSV, lut, imgHSVBin1);

        // 画像2
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HLOW, &maskLow[0]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_HUPP, &maskUpp[0]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SLOW, &maskLow[1]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_SUPP, &maskUpp[1]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VLOW, &maskLow[2]);
        g_pSharedObject->GetParam(PARAM_ID_PIC_MASK2_VUPP, &maskUpp[2]);
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
        cv::LUT(imgHSV, lut, imgHSVBin2);

        //----------------------------------------------------------------------------
        // 3チャンネル全てのANDを取り、マスク画像を作成する
        // 画像1
        cv::split(imgHSVBin1, planes);  // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する
        cv::bitwise_and(planes[0], planes[1], imgMask1);
        cv::bitwise_and(imgMask1,  planes[2], imgMask1);

        // 画像2
        cv::split(imgHSVBin2, planes);  // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する
        cv::bitwise_and(planes[0], planes[1], imgMask2);
        cv::bitwise_and(imgMask2,  planes[2], imgMask2);

#if 0   // 各マスク画像は個別に処理
        //----------------------------------------------------------------------------
        // マスクされた画像の合成
        cv::Mat imgMask;
//      imgMask = imgMask1 + imgMask2;
        cv::add(imgMask1, imgMask2, imgMask);

//      // 出力
//      cv::Mat imgMasked;
//      imgSrc.copyTo(imgMasked, imgMask);
#endif

        //----------------------------------------------------------------------------
        // ノイズ除去
        g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTER,    &filter);
        g_pSharedObject->GetParam(PARAM_ID_PIC_NOISEFILTERVAL, &filterval);
        switch (filter)
        {
        case NOISEFILTER_MEDIAN:    // 中央値フィルタ
            // 画像1
            cv::medianBlur(imgMask1, imgMask1, filterval);
            // 画像2
            cv::medianBlur(imgMask2, imgMask2, filterval);
            break;
        case NOISEFILTER_OPENNING:  // オープニング処理
            // 画像1
            cv::erode(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);     // 収縮
            cv::dilate(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);    // 膨張
            // 画像2
            cv::erode(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);     // 収縮
            cv::dilate(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);    // 膨張
            break;
        default:
            break;
        }

        //----------------------------------------------------------------------------
        // 画像処理
        g_pSharedObject->GetParam(PARAM_ID_PIC_ALGORITHM, &algo);
        // 画像1
        //----------------------------------------------------------------------------
        // 領域抽出(ROI)でターゲット認識
        // 輪郭抽出
        cv::findContours(imgMask1, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        //----------------------------------------------------------------------------
        // 候補ターゲットの信頼性評価

        //----------------------------------------------------------------------------
        // 重心位置計算
        posX = 0.0;
        posY = 0.0;
        if (algo == COG_ALGORITHM_ALL) {ret = CalcCenterOfGravity1(contours, &posX, &posY);}
        else                           {ret = CalcCenterOfGravity2(contours, &posX, &posY);}

        stProcData.posx   = posX;
        stProcData.posy   = posY;
        stProcData.enable = ret;
        g_pSharedObject->SetProcData(IMGPROC_ID_IMG_1, stProcData);

        // 画像2
        //----------------------------------------------------------------------------
        // 領域抽出(ROI)でターゲット認識
        // 輪郭抽出
        cv::findContours(imgMask2, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        //----------------------------------------------------------------------------
        // 候補ターゲットの信頼性評価

        //----------------------------------------------------------------------------
        // 重心位置計算
        posX = 0.0;
        posY = 0.0;
        if (algo == COG_ALGORITHM_ALL) {ret = CalcCenterOfGravity1(contours, &posX, &posY);}
        else                           {ret = CalcCenterOfGravity2(contours, &posX, &posY);}
        stProcData.posx   = posX;
        stProcData.posy   = posY;
        stProcData.enable = ret;

        g_pSharedObject->SetProcData(IMGPROC_ID_IMG_2, stProcData);

        //----------------------------------------------------------------------------
        // 画像保存
        // マスク画像1
        if (m_iBufferImgMask1 == IMAGE_ID_MASK1_A)
        {
            g_pSharedObject->SetImage(IMAGE_ID_MASK1_A, imgMask1);
            m_iBufferImgMask1 = IMAGE_ID_MASK1_B;
        }
        else
        {
            g_pSharedObject->SetImage(IMAGE_ID_MASK1_B, imgMask1);
            m_iBufferImgMask1 = IMAGE_ID_MASK1_A;
        }

        // マスク画像2
        if (m_iBufferImgMask2 == IMAGE_ID_MASK2_A)
        {
            g_pSharedObject->SetImage(IMAGE_ID_MASK2_A, imgMask2);
            m_iBufferImgMask2 = IMAGE_ID_MASK2_B;
        }
        else
        {
            g_pSharedObject->SetImage(IMAGE_ID_MASK2_B, imgMask2);
            m_iBufferImgMask2 = IMAGE_ID_MASK2_A;
        }

        // 処理画像
        if (m_iBufferImgProc == IMAGE_ID_PROC_A)
        {
            g_pSharedObject->SetImage(IMAGE_ID_PROC_A, imgSrc);
            m_iBufferImgProc = IMAGE_ID_PROC_B;
        }
        else
        {
            g_pSharedObject->SetImage(IMAGE_ID_PROC_B, imgSrc);
            m_iBufferImgProc = IMAGE_ID_PROC_A;
        }
    }   // if (bImgEnable)
    else
    {
        stProcData.posx   = 0.0;
        stProcData.posy   = 0.0;
        stProcData.enable = FALSE;
        g_pSharedObject->SetProcData(IMGPROC_ID_IMG_1, stProcData);
        g_pSharedObject->SetProcData(IMGPROC_ID_IMG_2, stProcData);
    }
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
BOOL CAnalyst::CalcCenterOfGravity1(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY)
{
    BOOL    ret   = FALSE;
    double  posX  = 0;
    double  posY  = 0;
    UINT32  count = 0;

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
    
    if (count > 0)
    {
        posX /= count;
        posY /= count;
        ret = TRUE;
    }

    *outPosX = posX;
    *outPosY = posY;
    return ret;
}

/// @brief 重心位置算出アルゴリズム2
/// @param
/// @return 
/// @note
BOOL CAnalyst::CalcCenterOfGravity2(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY)
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
        return FALSE;
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
        return FALSE;
    }

    Moments mu1 = moments(contours[max_id]);
    Point2f mc1 = Point2f((float)mu1.m10 / (float)mu1.m00, (float)mu1.m01 / (float)mu1.m00);
    Moments mu2 = moments(contours[max2_id]);
    Point2f mc2 = Point2f((float)mu2.m10 / (float)mu2.m00, (float)mu2.m01 / (float)mu2.m00);

    *outPosX = ((DOUBLE)mc1.x + mc2.x) / 2.0;
    *outPosY = ((DOUBLE)mc1.y + mc2.y) / 2.0;
    return TRUE;
}
