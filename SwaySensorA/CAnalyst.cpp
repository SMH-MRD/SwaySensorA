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

    stImageProcData stImgProcData[IMGPROC_ID_MAX];
    UINT32          val;
    g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_SIZE, &val);
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        stImgProcData[ii].posx       = 0.0;
        stImgProcData[ii].posy       = 0.0;
        stImgProcData[ii].roi.x      = 0;
        stImgProcData[ii].roi.y      = 0;
        stImgProcData[ii].roi.width  = val;
        stImgProcData[ii].roi.height = val;
        stImgProcData[ii].roisize    = val;
        stImgProcData[ii].enable     = FALSE;
        g_pSharedObject->SetProcData(ii, stImgProcData[ii]);
    }
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
    cv::Mat         imgHSVBin;
    cv::Mat         imgROI; // 切抜き画像
    cv::Mat         imgMask1, imgMask2;
    cv::Mat         lut;
    BOOL            bImgEnable = FALSE;
    UINT32          width = 0, height = 0;
    UINT            maskLow[3], maskUpp[3];
    std::vector<cv::Mat> planes;
    UINT32          roienable, roisize;
    UINT32          filter;
    UINT32          filterval;
    std::vector<std::vector<cv::Point>> contours;
    stImageProcData stImgProcData[IMGPROC_ID_MAX];
    double          posX, posY;
    UINT32          algo;
    BOOL            ret = FALSE;

    //----------------------------------------------------------------------------
    // 元画像読込み
#pragma region GetSrcImage
    if      (g_pSharedObject->GetImage(IMAGE_ID_RAW_A, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else if (g_pSharedObject->GetImage(IMAGE_ID_RAW_B, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else ;
#pragma endregion GetSrcImage

    if (bImgEnable)
    {
        //----------------------------------------------------------------------------
        // 画像色をBGR→HSVに変換
#pragma region ConvBGRToHSV
        g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_ENABLE, &roienable);
        if (roienable == 0) {cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);}
#pragma endregion ConvBGRToHSV

        //----------------------------------------------------------------------------
        // 各チャンネルごとに2値化(LUT変換)し、3チャンネル全てのANDを取り、マスク画像を作成する
#pragma region CreateMaskImage
        lut = cv::Mat(256, 1, CV_8UC3); // LUT
        g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_SIZE, &roisize);
        if ((int)roisize >= imgSrc.cols) roisize = imgSrc.cols;
        if ((int)roisize >= imgSrc.rows) roisize = imgSrc.rows;
        // 画像1
#pragma region Image1
        if (roienable > 0)
        {
            // ROIの範囲(長方形)を設定する
            // * (x, y, width, height)で指定
            if (g_pSharedObject->GetProcData(IMGPROC_ID_IMG_1, &stImgProcData[IMGPROC_ID_IMG_1]) != RESULT_OK)
            {
                stImgProcData[IMGPROC_ID_IMG_1].posx = 0.0;
                stImgProcData[IMGPROC_ID_IMG_1].posy = 0.0;
                stImgProcData[IMGPROC_ID_IMG_1].enable = FALSE;
            } 
            if (stImgProcData[IMGPROC_ID_IMG_1].enable)
            {
//@@@roisizeは自動計算するようにする必要あり
                stImgProcData[IMGPROC_ID_IMG_1].roisize = roisize;
                int tmpval = (int)(((double)stImgProcData[IMGPROC_ID_IMG_1].roisize / 2.0) + 0.5);
                if      (((int)stImgProcData[IMGPROC_ID_IMG_1].posx - tmpval) < 0)           {stImgProcData[IMGPROC_ID_IMG_1].roi.x  = 0;}
                else if (((int)stImgProcData[IMGPROC_ID_IMG_1].posx + tmpval) > imgSrc.cols) {stImgProcData[IMGPROC_ID_IMG_1].roi.x  = imgSrc.cols - stImgProcData[IMGPROC_ID_IMG_1].roisize;}
                else                                                                         {stImgProcData[IMGPROC_ID_IMG_1].roi.x  = (int)stImgProcData[IMGPROC_ID_IMG_1].posx - tmpval;}
                if      (((int)stImgProcData[IMGPROC_ID_IMG_1].posy - tmpval) < 0)           {stImgProcData[IMGPROC_ID_IMG_1].roi.y  = 0;}
                else if (((int)stImgProcData[IMGPROC_ID_IMG_1].posy + tmpval) > imgSrc.rows) {stImgProcData[IMGPROC_ID_IMG_1].roi.y  = imgSrc.rows - stImgProcData[IMGPROC_ID_IMG_1].roisize;}
                else                                                                         {stImgProcData[IMGPROC_ID_IMG_1].roi.y  = (int)stImgProcData[IMGPROC_ID_IMG_1].posy - tmpval;}
                stImgProcData[IMGPROC_ID_IMG_1].roi.width  = stImgProcData[IMGPROC_ID_IMG_1].roisize;
                stImgProcData[IMGPROC_ID_IMG_1].roi.height = stImgProcData[IMGPROC_ID_IMG_1].roisize;
            }
            else
            {
                stImgProcData[IMGPROC_ID_IMG_1].roisize    = roisize;
                stImgProcData[IMGPROC_ID_IMG_1].roi.x      = 0;
                stImgProcData[IMGPROC_ID_IMG_1].roi.y      = 0;
                stImgProcData[IMGPROC_ID_IMG_1].roi.width  = imgSrc.cols;
                stImgProcData[IMGPROC_ID_IMG_1].roi.height = imgSrc.rows;
            }
            // 部分画像を生成
            // * 部分画像とその元画像は共通の画像データを参照するため、
            //   部分画像に変更を加えると、元画像も変更される。
            imgROI = imgSrc(stImgProcData[IMGPROC_ID_IMG_1].roi);
            // 画像色をBGR→HSVに変換
            cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
        }
        else
        {
            stImgProcData[IMGPROC_ID_IMG_1].roisize    = roisize;
            stImgProcData[IMGPROC_ID_IMG_1].roi.x      = 0;
            stImgProcData[IMGPROC_ID_IMG_1].roi.y      = 0;
            stImgProcData[IMGPROC_ID_IMG_1].roi.width  = imgSrc.cols;
            stImgProcData[IMGPROC_ID_IMG_1].roi.height = imgSrc.rows;
        }
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_HLOW, &maskLow[0]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_HUPP, &maskUpp[0]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_SLOW, &maskLow[1]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_SUPP, &maskUpp[1]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_VLOW, &maskLow[2]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK1_VUPP, &maskUpp[2]);
        for (int i = 0; i < 256; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                if ((int)maskLow[k] <= (int)maskUpp[k])
                {
                    if (((int)maskLow[k] <= i) && (i <= (int)maskUpp[k])) { lut.data[i*lut.step + k] = 255; }
                    else { lut.data[i*lut.step + k] = 0; }
                }
                else
                {
                    if ((i <= (int)maskUpp[k]) || ((int)maskLow[k] <= i)) { lut.data[i*lut.step + k] = 255; }
                    else { lut.data[i*lut.step + k] = 0; }
                }
            }
        }
        // チャンネルごとのLUT変換(各チャンネルごとに2値化処理)
        cv::LUT(imgHSV, lut, imgHSVBin);
        // マスク画像の作成
        cv::split(imgHSVBin, planes);   // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する
        cv::bitwise_and(planes[0], planes[1], imgMask1);
        cv::bitwise_and(imgMask1,  planes[2], imgMask1);
#pragma endregion Image1

        // 画像2
#pragma region Image2
        if (roienable > 0)
        {
            // ROIの範囲（長方形）を設定する
            // * (x, y, width, height)で指定
            if (g_pSharedObject->GetProcData(IMGPROC_ID_IMG_2, &stImgProcData[IMGPROC_ID_IMG_2]) != RESULT_OK)
            {
                stImgProcData[IMGPROC_ID_IMG_2].posx   = 0.0;
                stImgProcData[IMGPROC_ID_IMG_2].posy   = 0.0;
                stImgProcData[IMGPROC_ID_IMG_2].enable = FALSE;
            }
            if (stImgProcData[IMGPROC_ID_IMG_2].enable)
            {
//@@@roisizeは自動計算するようにする必要あり
                stImgProcData[IMGPROC_ID_IMG_2].roisize = roisize;
                int tmpval = (int)(((double)stImgProcData[IMGPROC_ID_IMG_2].roisize / 2.0) + 0.5);
                if      (((int)stImgProcData[IMGPROC_ID_IMG_2].posx - tmpval) < 0)           {stImgProcData[IMGPROC_ID_IMG_2].roi.x  = 0;}
                else if (((int)stImgProcData[IMGPROC_ID_IMG_2].posx + tmpval) > imgSrc.cols) {stImgProcData[IMGPROC_ID_IMG_2].roi.x  = imgSrc.cols - stImgProcData[IMGPROC_ID_IMG_2].roisize;}
                else                                                                         {stImgProcData[IMGPROC_ID_IMG_2].roi.x  = (int)stImgProcData[IMGPROC_ID_IMG_2].posx - tmpval;}
                if      (((int)stImgProcData[IMGPROC_ID_IMG_2].posy - tmpval) < 0)           {stImgProcData[IMGPROC_ID_IMG_2].roi.y  = 0;}
                else if (((int)stImgProcData[IMGPROC_ID_IMG_2].posy + tmpval) > imgSrc.rows) {stImgProcData[IMGPROC_ID_IMG_2].roi.y  = imgSrc.rows - stImgProcData[IMGPROC_ID_IMG_2].roisize;}
                else                                                                         {stImgProcData[IMGPROC_ID_IMG_2].roi.y  = (int)stImgProcData[IMGPROC_ID_IMG_2].posy - tmpval;}
                stImgProcData[IMGPROC_ID_IMG_2].roi.width  = stImgProcData[IMGPROC_ID_IMG_2].roisize;
                stImgProcData[IMGPROC_ID_IMG_2].roi.height = stImgProcData[IMGPROC_ID_IMG_2].roisize;
            }
            else
            {
                stImgProcData[IMGPROC_ID_IMG_2].roisize    = roisize;
                stImgProcData[IMGPROC_ID_IMG_2].roi.x      = 0;
                stImgProcData[IMGPROC_ID_IMG_2].roi.y      = 0;
                stImgProcData[IMGPROC_ID_IMG_2].roi.width  = imgSrc.cols;
                stImgProcData[IMGPROC_ID_IMG_2].roi.height = imgSrc.rows;
            }
            // 部分画像を生成
            // * 部分画像とその元画像は共通の画像データを参照するため、
            //   部分画像に変更を加えると、元画像も変更される。
            imgROI = imgSrc(stImgProcData[IMGPROC_ID_IMG_2].roi);
            // 画像色をBGR→HSVに変換
            cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
        }
        else
        {
            stImgProcData[IMGPROC_ID_IMG_2].roisize    = roisize;
            stImgProcData[IMGPROC_ID_IMG_2].roi.x      = 0;
            stImgProcData[IMGPROC_ID_IMG_2].roi.y      = 0;
            stImgProcData[IMGPROC_ID_IMG_2].roi.width  = imgSrc.cols;
            stImgProcData[IMGPROC_ID_IMG_2].roi.height = imgSrc.rows;
        }
        // 3チャンネルのLUT作成
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_HLOW, &maskLow[0]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_HUPP, &maskUpp[0]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_SLOW, &maskLow[1]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_SUPP, &maskUpp[1]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_VLOW, &maskLow[2]);
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK2_VUPP, &maskUpp[2]);
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
        cv::LUT(imgHSV, lut, imgHSVBin);
        // マスク画像の作成
        cv::split(imgHSVBin, planes);   // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する
        cv::bitwise_and(planes[0], planes[1], imgMask2);
        cv::bitwise_and(imgMask2,  planes[2], imgMask2);
#pragma endregion Image2
#pragma endregion CreateMaskImage

        //----------------------------------------------------------------------------
        // マスクされた画像の合成
#pragma region MaskImageCombined
// 各マスク画像は個別に処理
#if 0
        cv::Mat imgMask;
//      imgMask = imgMask1 + imgMask2;
        cv::add(imgMask1, imgMask2, imgMask);

//      // 出力
//      cv::Mat imgMasked;
//      imgSrc.copyTo(imgMasked, imgMask);
#endif
#pragma endregion MaskImageCombined

        //----------------------------------------------------------------------------
        // ノイズ除去
        // ゴマ塩
#pragma region NoiseCut1
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER1,    &filter);
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTERVAL1, &filterval);
        switch (filter)
        {
        case NOISEFILTER1_MEDIAN:    // 中央値フィルタ
            // 画像1
            cv::medianBlur(imgMask1, imgMask1, filterval);
            // 画像2
            cv::medianBlur(imgMask2, imgMask2, filterval);
            break;
        case NOISEFILTER1_OPENNING:  // オープニング処理(縮小→拡大)
            // 画像1
//          cv::morphologyEx(imgMask1, imgMask1, MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::erode(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);     // 収縮
            cv::dilate(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);    // 膨張
            // 画像2
//          cv::morphologyEx(imgMask2, imgMask2, MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::erode(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);     // 収縮
            cv::dilate(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);    // 膨張
            break;
        default:
            break;
        }
#pragma endregion NoiseCut1

        // 穴埋め
#pragma region NoiseCut2
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER2,    &filter);
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTERVAL2, &filterval);
        // 画像1
        switch (filter)
        {
        case NOISEFILTER2_CLOSING:  // クロージング処理(拡大→縮小)
//          cv::morphologyEx(imgMask1, imgMask1, MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::dilate(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);    // 膨張
            cv::erode(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);     // 収縮
            // 画像2
//          cv::morphologyEx(imgMask2, imgMask2, MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::dilate(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);    // 膨張
            cv::erode(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);     // 収縮
            break;
        default:
            break;
        }
#pragma endregion NoiseCut2

        //----------------------------------------------------------------------------
        // 画像処理
#pragma region ImageProc
        g_pSharedObject->GetParam(PARAM_ID_IMG_ALGORITHM, &algo);
        // 画像1
#pragma region Image1
        // 輪郭抽出(一番外側の白の輪郭のみを取得)
        cv::findContours(imgMask1, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        // 重心検出
        posX = 0.0;
        posY = 0.0;
        ret  = CalcCenterOfGravity(contours, &posX, &posY, algo);
        stImgProcData[IMGPROC_ID_IMG_1].posx   = posX + stImgProcData[IMGPROC_ID_IMG_1].roi.x;
        stImgProcData[IMGPROC_ID_IMG_1].posy   = posY + stImgProcData[IMGPROC_ID_IMG_1].roi.y;
        stImgProcData[IMGPROC_ID_IMG_1].enable = ret;
        g_pSharedObject->SetProcData(IMGPROC_ID_IMG_1, stImgProcData[IMGPROC_ID_IMG_1]);
#pragma endregion Image1

        // 画像2
#pragma region Image2
        // 輪郭抽出
        cv::findContours(imgMask2, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        // 重心検出
        posX = 0.0;
        posY = 0.0;
        ret  = CalcCenterOfGravity(contours, &posX, &posY, algo);
        stImgProcData[IMGPROC_ID_IMG_2].posx   = posX + stImgProcData[IMGPROC_ID_IMG_2].roi.x;
        stImgProcData[IMGPROC_ID_IMG_2].posy   = posY + stImgProcData[IMGPROC_ID_IMG_2].roi.y;
        stImgProcData[IMGPROC_ID_IMG_2].enable = ret;
        g_pSharedObject->SetProcData(IMGPROC_ID_IMG_2, stImgProcData[IMGPROC_ID_IMG_2]);
#pragma endregion Image2
#pragma endregion ImageProc

        //----------------------------------------------------------------------------
        // 画像保存
#pragma region PutImage
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
#pragma endregion PutImage
    }   // if (bImgEnable)
    else
    {
        for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
        {
            stImgProcData[ii].posx       = 0.0;
            stImgProcData[ii].posy       = 0.0;
            stImgProcData[ii].roisize    = 0;
            stImgProcData[ii].roi.x      = 0;
            stImgProcData[ii].roi.y      = 0;
            stImgProcData[ii].roi.width  = 0;
            stImgProcData[ii].roi.height = 0;
            stImgProcData[ii].enable     = FALSE;
            g_pSharedObject->SetProcData(ii, stImgProcData[ii]);
        }
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
            port1AngleData = ((port1AnaData - 4.0) - 8.0) / 8.0 * INCLINATION_MAX;
        }
        else
        {
            // 12.0mA以上ならば角度は+方向に倒れている(20mAで30度)
            port1AngleData = (port1AnaData - 12.0) / 8.0 * INCLINATION_MAX;
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
            port2AngleData = ((port2AnaData - 4.0) - 8.0) / 8.0 * INCLINATION_MAX;
        }
        else
        {
            // 12.0mA以上ならば角度は+方向に倒れている(20mAで30度)
            port2AngleData = (port2AnaData - 12.0) / 8.0 * INCLINATION_MAX;
        }
        g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_RAD, port2AngleData);
    }
}

/// @brief 重心検出
/// @param
/// @return 
/// @note 
BOOL CAnalyst::CalcCenterOfGravity(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY, UINT32 sel)
{
    BOOL    ret  = FALSE;
    double  posX = 0.0;
    double  posY = 0.0;
    
    switch (sel)
    {
#pragma region COG_ALGORITHM1
    case COG_ALGORITHM_ALL:     // 重心位置算出アルゴリズム(全輪郭点)
        {
        UINT32  count = 0;

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
        }
        break;
#pragma endregion COG_ALGORITHM1

#pragma region COG_ALGORITHM2
    case COG_ALGORITHM_AREA:    // 重心位置算出アルゴリズム(最大輪郭面積)
        {
        UINT32  count            = 0;
        double  max_area         = 0;
        int     max_area_contour = -1;

        for(UINT ii = 0; ii < contours.size(); ii++)
        {
            double area = contourArea(contours.at(ii));
            if(max_area < area)
            {
                max_area = area;
                max_area_contour = ii;
            }
        }

        if (max_area_contour >= 0)
        {
            count = contours.at(max_area_contour).size();
            for(UINT ii = 0; ii < count; ii++)
            {
                posX += contours.at(max_area_contour).at(ii).x;
                posY += contours.at(max_area_contour).at(ii).y;
            }
            if (count > 0)
            {
                posX /= count;
                posY /= count;
                ret = TRUE;
            }
        }
        }
        break;
#pragma endregion COG_ALGORITHM2

#pragma region COG_ALGORITHM3
    case COG_ALGORITHM_LEN:     // 重心位置算出アルゴリズム(最大輪郭長)
        {
	    double  max_size = 0;
	    int     max_id   = -1;

	    if (contours.size() > 0)
        {
	        for (UINT ii = 0; ii < contours.size(); ii++)
            {
		        if (contours[ii].size() > max_size)
                {
			        max_size = contours[ii].size();
			        max_id   = ii;
		        }
	        }
            if (max_id >= 0)
            {
	            Moments mu = moments(contours[max_id]);
                if(mu.m00 > 0.0)
                {
	                posX = mu.m10 / mu.m00;
	                posY = mu.m01 / mu.m00;
                    ret = TRUE;
                }
            }
        }
        }
        break;
#pragma endregion COG_ALGORITHM3

    default:
        break;
    }   // switch (sel)

    if (isnan(posX) || isnan(posY))
    {
        posX = 0.0;
        posY = 0.0;
        ret = FALSE;
    }
    *outPosX = posX;
    *outPosY = posY;

    return ret;
}
