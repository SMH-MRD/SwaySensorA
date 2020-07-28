#include "CAnalyst.h"

extern CSharedObject* g_pSharedObject;  // タスク間共有データのポインタ

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
    m_iBufferImgMask[IMGPROC_ID_IMG_1] = IMAGE_ID_MASK1_A;
    m_iBufferImgMask[IMGPROC_ID_IMG_2] = IMAGE_ID_MASK2_A;
    m_iBufferImgProc                   = IMAGE_ID_PROC_A;

    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_proninfo.imgprocdata[ii].posx       = 0.0;
        m_proninfo.imgprocdata[ii].posy       = 0.0;
        m_proninfo.imgprocdata[ii].tgtsize    = 0;
        m_proninfo.imgprocdata[ii].roi.x      = 0;
        m_proninfo.imgprocdata[ii].roi.y      = 0;
        m_proninfo.imgprocdata[ii].roi.width  = 0;
        m_proninfo.imgprocdata[ii].roi.height = 0;
        m_proninfo.imgprocdata[ii].valid      = FALSE;
    }
    m_proninfo.exposureTime = 0.0;              // 露光時間[us]
    m_proninfo.procTime     = 0.0;              // 画処理時間[ms]
    g_pSharedObject->SetInfo(m_proninfo);

    g_pSharedObject->GetParam(&m_camparam);     // カメラ設定データ
    g_pSharedObject->GetParam(&m_cnfgparam);    // 構造設定データ
    g_pSharedObject->GetParam(&m_imgprocparam); // 画像処理設定データ
}

/// @brief 
/// @param
/// @return 
/// @note
void CAnalyst::routine_work(void* param)
{
//  if (g_pSharedObject == NULL) {return;}
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
    // 処理時間計測(終了時間取得)
    LARGE_INTEGER   end;
    LONGLONG        span, usec;
    QueryPerformanceCounter(&end);
    span = end.QuadPart - start.QuadPart;
    usec = (span * 1000000L) / frequency.QuadPart;
    m_proninfo.procTime = (DOUBLE)usec / 1000.0;    // 画処理時間[ms]

    g_pSharedObject->SetInfo(m_proninfo);

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
    cv::Mat         imgMask[IMGPROC_ID_MAX];
    cv::Mat         lut;
    BOOL            bImgEnable = FALSE;
    UINT32          width = 0, height = 0;
    UINT            maskLow[3], maskUpp[3];
    std::vector<cv::Mat> planes;
    std::vector<std::vector<cv::Point>> contours;
    BOOL            ret = FALSE;

    //----------------------------------------------------------------------------
   // 画像処理設定読込み
    g_pSharedObject->GetParam(&m_imgprocparam);

    //----------------------------------------------------------------------------
    // 元画像読込み
#pragma region GetSrcImage
    if      (g_pSharedObject->GetImage(IMAGE_ID_RAW_A, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else if (g_pSharedObject->GetImage(IMAGE_ID_RAW_B, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else ;
#pragma endregion GetSrcImage

    //----------------------------------------------------------------------------
    // 検出処理
#pragma region ProcTarget
    if (bImgEnable)
    {
        //----------------------------------------------------------------------------
        // 画像色をBGR→HSVに変換
#pragma region ConvBGRToHSV
        if (m_imgprocparam.roi.valid == 0) {cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);}
#pragma endregion ConvBGRToHSV

        //----------------------------------------------------------------------------
        // 各チャンネルごとに2値化(LUT変換)し、3チャンネル全てのANDを取り、マスク画像を作成する
#pragma region CreateMaskImage
        lut = cv::Mat(256, 1, CV_8UC3); // LUT
        for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
        {
            if (m_imgprocparam.maskvalid[ii])
            {
                if (m_imgprocparam.roi.valid > 0)
                {
                    // ROIの範囲(長方形)を設定する
                    // * (x, y, width, height)で指定
                    if (m_proninfo.imgprocdata[ii].valid)
                    {
                        int roisize = (int)((double)m_proninfo.imgprocdata[ii].tgtsize * m_imgprocparam.roi.scale);
                        if (roisize <= 0)           {roisize = imgSrc.cols;}
                        if (roisize >  imgSrc.cols) {roisize = imgSrc.cols;}
                        if (roisize >  imgSrc.rows) {roisize = imgSrc.rows;}

                        int tmpval = (int)(((double)roisize / 2.0) + 0.5);
                        if      (((int)m_proninfo.imgprocdata[ii].posx - tmpval) < 0)           {m_proninfo.imgprocdata[ii].roi.x = 0;}
                        else if (((int)m_proninfo.imgprocdata[ii].posx + tmpval) > imgSrc.cols) {m_proninfo.imgprocdata[ii].roi.x = imgSrc.cols - roisize;}
                        else                                                                    {m_proninfo.imgprocdata[ii].roi.x = (int)m_proninfo.imgprocdata[ii].posx - tmpval;}
                        if      (((int)m_proninfo.imgprocdata[ii].posy - tmpval) < 0)           {m_proninfo.imgprocdata[ii].roi.y = 0;}
                        else if (((int)m_proninfo.imgprocdata[ii].posy + tmpval) > imgSrc.rows) {m_proninfo.imgprocdata[ii].roi.y = imgSrc.rows - roisize;}
                        else                                                                    {m_proninfo.imgprocdata[ii].roi.y = (int)m_proninfo.imgprocdata[ii].posy - tmpval;}
                        m_proninfo.imgprocdata[ii].roi.width  = roisize;
                        m_proninfo.imgprocdata[ii].roi.height = roisize;
                    }
                    else
                    {
                        m_proninfo.imgprocdata[ii].roi.x      = 0;
                        m_proninfo.imgprocdata[ii].roi.y      = 0;
                        m_proninfo.imgprocdata[ii].roi.width  = imgSrc.cols;
                        m_proninfo.imgprocdata[ii].roi.height = imgSrc.rows;
                    }

                    // 部分画像を生成
                    // * 部分画像とその元画像は共通の画像データを参照するため、
                    //   部分画像に変更を加えると、元画像も変更される。
                    imgROI = imgSrc(m_proninfo.imgprocdata[ii].roi);
                    // 画像色をBGR→HSVに変換
                    cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
                }
                else
                {
                    m_proninfo.imgprocdata[ii].roi.x      = 0;
                    m_proninfo.imgprocdata[ii].roi.y      = 0;
                    m_proninfo.imgprocdata[ii].roi.width  = imgSrc.cols;
                    m_proninfo.imgprocdata[ii].roi.height = imgSrc.rows;
                }
                // 3チャンネルのLUT作成
                maskLow[0] = m_imgprocparam.hsvl[ii].h;
                maskUpp[0] = m_imgprocparam.hsvu[ii].h;
                maskLow[1] = m_imgprocparam.hsvl[ii].s;
                maskUpp[1] = m_imgprocparam.hsvu[ii].s;
                maskLow[2] = m_imgprocparam.hsvl[ii].v;
                maskUpp[2] = m_imgprocparam.hsvu[ii].v;
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
                cv::bitwise_and(planes[0],   planes[1], imgMask[ii]);
                cv::bitwise_and(imgMask[ii], planes[2], imgMask[ii]);
            }   // if (m_imgprocparam.maskvalid[ii])
        }
#pragma endregion CreateMaskImage

        //----------------------------------------------------------------------------
        // マスクされた画像の合成
#pragma region MaskImageCombined
// 各マスク画像は個別に処理
#if 0
        cv::Mat imgMask;
//      imgMask = imgMask[IMGPROC_ID_IMG_1] + imgMask[IMGPROC_ID_IMG_2];
        cv::add(imgMask[IMGPROC_ID_IMG_1], imgMask[IMGPROC_ID_IMG_2], imgMask);

//      // 出力
//      cv::Mat imgMasked;
//      imgSrc.copyTo(imgMasked, imgMask);
#endif
#pragma endregion MaskImageCombined

        //----------------------------------------------------------------------------
        // ノイズ除去
        // ゴマ塩
#pragma region NoiseCut1
        switch (m_imgprocparam.filter1.type)
        {
        case NOISEFILTER1_MEDIAN:    // 中央値フィルタ
            for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
            {
                if (m_imgprocparam.maskvalid[ii]) {cv::medianBlur(imgMask[ii], imgMask[IMGPROC_ID_IMG_1], m_imgprocparam.filter1.val);}
            }
            break;
        case NOISEFILTER1_OPENNING:  // オープニング処理(縮小→拡大)
            for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
            {
                if (m_imgprocparam.maskvalid[ii])
                {
//                  cv::morphologyEx(imgMask[ii], imgMask[ii], MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), m_imgprocparam.filter1.val);
                    cv::erode(imgMask[ii],  imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter1.val); // 収縮
                    cv::dilate(imgMask[ii], imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter1.val); // 膨張
                }
            }
            break;
        default:
            break;
        }
#pragma endregion NoiseCut1

        // 穴埋め
#pragma region NoiseCut2
        switch (m_imgprocparam.filter2.type)
        {
        case NOISEFILTER2_CLOSING:  // クロージング処理(拡大→縮小)
            for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
            {
                if (m_imgprocparam.maskvalid[ii])
                {
//                  cv::morphologyEx(imgMask[ii], imgMask[ii], MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), m_imgprocparam.filter2.val);
                    cv::dilate(imgMask[ii], imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter2.val); // 膨張
                    cv::erode(imgMask[ii],  imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter2.val); // 収縮
                }
            }
            break;
        default:
            break;
        }
#pragma endregion NoiseCut2

        //----------------------------------------------------------------------------
        // 画像処理
#pragma region ImageProc
        double  posx, posy;
        int     tgtsize;
        for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
        {
            if (m_imgprocparam.maskvalid[ii])
            {
                // 輪郭抽出(一番外側の白の輪郭のみを取得)
                cv::findContours(imgMask[ii], contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

                // 重心検出
                posx    = 0.0;
                posy    = 0.0;
                tgtsize = 0;
                ret  = CalcCenterOfGravity(contours, &posx, &posy, &tgtsize, m_imgprocparam.algorithm);
                m_proninfo.imgprocdata[ii].posx    = posx + m_proninfo.imgprocdata[ii].roi.x;
                m_proninfo.imgprocdata[ii].posy    = posy + m_proninfo.imgprocdata[ii].roi.y;
                m_proninfo.imgprocdata[ii].tgtsize = tgtsize;
                m_proninfo.imgprocdata[ii].valid   = ret;
            }
            else
            {
                m_proninfo.imgprocdata[ii].posx    = 0.0;
                m_proninfo.imgprocdata[ii].posy    = 0.0;
                m_proninfo.imgprocdata[ii].tgtsize = 0;
                m_proninfo.imgprocdata[ii].valid   = FALSE;
            }
        }
#pragma endregion ImageProc

        //----------------------------------------------------------------------------
        // 画像保存
#pragma region PutImage
        // マスク画像1
        if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_1])
        {
            if (m_iBufferImgMask[IMGPROC_ID_IMG_1] == IMAGE_ID_MASK1_A)
            {
                g_pSharedObject->SetImage(IMAGE_ID_MASK1_A, imgMask[IMGPROC_ID_IMG_1]);
                m_iBufferImgMask[IMGPROC_ID_IMG_1] = IMAGE_ID_MASK1_B;
            }
            else
            {
                g_pSharedObject->SetImage(IMAGE_ID_MASK1_B, imgMask[IMGPROC_ID_IMG_1]);
                m_iBufferImgMask[IMGPROC_ID_IMG_1] = IMAGE_ID_MASK1_A;
            }
        }
        // マスク画像2
        if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_2])
        {
            if (m_iBufferImgMask[IMGPROC_ID_IMG_2] == IMAGE_ID_MASK2_A)
            {
                g_pSharedObject->SetImage(IMAGE_ID_MASK2_A, imgMask[IMGPROC_ID_IMG_2]);
                m_iBufferImgMask[IMGPROC_ID_IMG_2] = IMAGE_ID_MASK2_B;
            }
            else
            {
                g_pSharedObject->SetImage(IMAGE_ID_MASK2_B, imgMask[IMGPROC_ID_IMG_2]);
                m_iBufferImgMask[IMGPROC_ID_IMG_2] = IMAGE_ID_MASK2_A;
            }
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
            m_proninfo.imgprocdata[ii].posx       = 0.0;
            m_proninfo.imgprocdata[ii].posy       = 0.0;
            m_proninfo.imgprocdata[ii].tgtsize    = 0;
            m_proninfo.imgprocdata[ii].roi.x      = 0;
            m_proninfo.imgprocdata[ii].roi.y      = 0;
            m_proninfo.imgprocdata[ii].roi.width  = 0;
            m_proninfo.imgprocdata[ii].roi.height = 0;
            m_proninfo.imgprocdata[ii].valid      = FALSE;
        }
    }
#pragma endregion ProcTarget

    //----------------------------------------------------------------------------
    // 振れ検出処理
#pragma region SwayProc
    SwayProc();
#pragma endregion SwayProc
}

/// @brief 重心検出
/// @param
/// @return 
/// @note 
BOOL CAnalyst::CalcCenterOfGravity(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY, int* outTgtSize, UINT32 sel)
{
    BOOL    ret     = FALSE;
    double  posx    = 0.0;
    double  posy    = 0.0;
    int     tgtsize = 0;

    switch (sel)
    {
#pragma region COG_ALGORITHM1
    case COG_ALGORITHM_ALL:
    //----------------------------------------------------------------------------
    // 重心位置算出アルゴリズム(全輪郭点)
        {
            UINT32  count = 0;
            double  x = 0.0, xmin = 3000.0, xmax = 0.0, width  = 0.0;
            double  y = 0.0, ymin = 3000.0, ymax = 0.0, height = 0.0;
            for (UINT ii = 0; ii < contours.size(); ii++)
            {
                UINT tempCount = contours.at(ii).size();
                count += tempCount;
                for (UINT jj = 0; jj < tempCount; jj++)
                {
                    x = contours.at(ii).at(jj).x;
                    y = contours.at(ii).at(jj).y;
                    posx += x;
                    posy += y;
                    if      (xmin > x) {xmin = x;}
                    else if (xmax < x) {xmax = x;}
                    else ;
                    if      (ymin > y) {ymin = y;}
                    else if (ymax < y) {ymax = y;}
                    else ;
                }
            }
            if (count > 0)
            {
                posx /= count;
                posy /= count;
                width  = (xmax - xmin);
                height = (ymax - ymin);
                if (width > height) {tgtsize = (int)width  + 1;}
                else                {tgtsize = (int)height + 1;}
                ret = TRUE;
            }
        }
        break;
#pragma endregion COG_ALGORITHM1

#pragma region COG_ALGORITHM2
    case COG_ALGORITHM_AREA:
    //----------------------------------------------------------------------------
    // 重心位置算出アルゴリズム(最大輪郭面積)
        {
            UINT32      count            = 0;
            double      max_area         = 0;
            int         max_area_contour = -1;
            cv::Rect    roi;
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
                    posx += contours.at(max_area_contour).at(ii).x;
                    posy += contours.at(max_area_contour).at(ii).y;
                }
                if (count > 0)
                {
                    posx /= count;
                    posy /= count;
                    roi = cv::boundingRect(contours[max_area_contour]);
                    if (roi.width > roi.height) {tgtsize = roi.width  + 1;}
                    else                        {tgtsize = roi.height + 1;}
                    ret = TRUE;
                }
            }
        }
        break;
#pragma endregion COG_ALGORITHM2

#pragma region COG_ALGORITHM3
    case COG_ALGORITHM_LEN:
    //----------------------------------------------------------------------------
    // 重心位置算出アルゴリズム(最大輪郭長)
        {
	        double      max_size = 0;
	        int         max_id   = -1;
            cv::Rect    roi;
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
	                    posx = mu.m10 / mu.m00;
	                    posy = mu.m01 / mu.m00;
                        roi = cv::boundingRect(contours[max_id]);
                        if (roi.width > roi.height) {tgtsize = roi.width  + 1;}
                        else                        {tgtsize = roi.height + 1;}
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

    if (isnan(posx) || isnan(posy))
    {
        posx = 0.0;
        posy = 0.0;
        ret  = FALSE;
    }
    *outPosX    = posx;
    *outPosY    = posy;
    *outTgtSize = tgtsize;

    return ret;
}

/// @brief 振れ検出処理
/// @param
/// @return 
/// @note
void CAnalyst::SwayProc(void)
{
    stExtnInfoData  extninfo;
    stRIOInfoData   rioinfo;

    g_pSharedObject->GetInfo(&extninfo);
    g_pSharedObject->GetInfo(&rioinfo);

    //----------------------------------------------------------------------------
    // 振れ位置検出
#pragma region SWAY_POS
    if ((m_imgprocparam.maskvalid[IMGPROC_ID_IMG_1]) && (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_2]))
    {
        if ((m_proninfo.imgprocdata[IMGPROC_ID_IMG_1].valid) && (m_proninfo.imgprocdata[IMGPROC_ID_IMG_2].valid))
        {
            m_proninfo.swaydata[AXIS_X].pos = (m_proninfo.imgprocdata[IMGPROC_ID_IMG_1].posx + m_proninfo.imgprocdata[IMGPROC_ID_IMG_2].posx) * 0.5;
            m_proninfo.swaydata[AXIS_Y].pos = (m_proninfo.imgprocdata[IMGPROC_ID_IMG_1].posy + m_proninfo.imgprocdata[IMGPROC_ID_IMG_2].posy) * 0.5;
            m_proninfo.valid = TRUE;
        }
        else
        {
            m_proninfo.swaydata[AXIS_X].pos = 0.0;
            m_proninfo.swaydata[AXIS_Y].pos = 0.0;
            m_proninfo.valid = FALSE;
        }
    }
    else
    {
        if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_1])
        {
            if (m_proninfo.imgprocdata[IMGPROC_ID_IMG_1].valid)
            {
                m_proninfo.swaydata[AXIS_X].pos = m_proninfo.imgprocdata[IMGPROC_ID_IMG_1].posx;
                m_proninfo.swaydata[AXIS_Y].pos = m_proninfo.imgprocdata[IMGPROC_ID_IMG_1].posy;
                m_proninfo.valid = TRUE;
            }
            else
            {
                m_proninfo.swaydata[AXIS_X].pos = 0.0;
                m_proninfo.swaydata[AXIS_Y].pos = 0.0;
                m_proninfo.valid = FALSE;
            }
        }
        else if (m_imgprocparam.maskvalid[IMGPROC_ID_IMG_2])
        {
            if (m_proninfo.imgprocdata[IMGPROC_ID_IMG_2].valid)
            {
                m_proninfo.swaydata[AXIS_X].pos = m_proninfo.imgprocdata[IMGPROC_ID_IMG_2].posx;
                m_proninfo.swaydata[AXIS_Y].pos = m_proninfo.imgprocdata[IMGPROC_ID_IMG_2].posy;
                m_proninfo.valid = TRUE;
            }
            else
            {
                m_proninfo.swaydata[AXIS_X].pos = 0.0;
                m_proninfo.swaydata[AXIS_Y].pos = 0.0;
                m_proninfo.valid = FALSE;
            }
        }
        else
        {
            m_proninfo.swaydata[AXIS_X].pos = 0.0;
            m_proninfo.swaydata[AXIS_Y].pos = 0.0;
            m_proninfo.valid = FALSE;
        }
    }
#pragma endregion SWAY_POS
    
    //----------------------------------------------------------------------------
    // 振れ角, 振れ角速度検出
#pragma region SWAY_ANG
    double th00;                    // Box吊点～カメラ中心角度
    double lc;                      // カメラ中心～ターゲット距離
    double fi0;                     // φ0
    double fisnensor;               // φsensor
    double deg, rad, spd;
    double dt;

    dt = inf.cycle_ms * 0.001;
    for (UINT ii = 0; ii < AXIS_MAX; ii++)
    {
        if (m_proninfo.valid)
        {
            th00 = m_cnfgparam.camoffsetTHC[ii] + rioinfo.incldata[ii].deg;                                         // θc + θ1
            lc   = extninfo.ropelen - m_cnfgparam.camboxoffsetLH0[ii] - (m_cnfgparam.camoffsetL0[ii] * cos(th00 * CONV_DEG_RAD)); // L0 - lh0 - l0 * cosθ00
            fi0  = ((m_cnfgparam.camboxoffsetD0[ii] - m_cnfgparam.camoffsetL0[ii] * sin(th00 * CONV_DEG_RAD)) / lc / CONV_DEG_RAD)
                 - (rioinfo.incldata[ii].deg + m_cnfgparam.camoffsetTH0[ii]);                                       // (D0 - l0 * sinθ00) / Lc - (θ1 + θ0)

            fisnensor = (m_proninfo.swaydata[AXIS_X].pos - (double)m_camparam.size[ii] * 0.5) * (m_cnfgparam.camviewAngle[ii] / (double)m_camparam.size[ii]);
            deg       = fisnensor - fi0;
            rad       = deg * CONV_DEG_RAD;
            spd       = (rad - m_proninfo.swaydata[ii].rad) / dt;
            spd       = ((dt * spd) + (m_cnfgparam.filter * m_proninfo.swaydata[ii].spd)) / (m_cnfgparam.filter + dt);

            m_proninfo.swaydata[ii].deg = deg;
            m_proninfo.swaydata[ii].rad = rad;
            m_proninfo.swaydata[ii].spd = spd;
        }
        else
        {
            m_proninfo.swaydata[ii].deg = 0.0;
            m_proninfo.swaydata[ii].rad = 0.0;
            m_proninfo.swaydata[ii].spd = 0.0;
        }
    }
#pragma endregion SWAY_ANG
}
