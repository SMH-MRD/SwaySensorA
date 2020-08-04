#include "CAnalyst.h"

extern CSharedObject* g_pSharedObject;  // �^�X�N�ԋ��L�f�[�^�̃|�C���^

/// @brief �R���X�g���N�^
/// @param
/// @return 
/// @note
CAnalyst::CAnalyst()
{
}

/// @brief �f�X�g���N�^
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
    m_proninfo.exposureTime = 0.0;              // �I������[us]
    m_proninfo.procTime     = 0.0;              // �揈������[ms]
    g_pSharedObject->SetInfo(m_proninfo);

    g_pSharedObject->GetParam(&m_camparam);     // �J�����ݒ�f�[�^
    g_pSharedObject->GetParam(&m_cmmnparam);    // ���ʐݒ�f�[�^
    g_pSharedObject->GetParam(&m_imgprocparam); // �摜�����ݒ�f�[�^
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
    // �������Ԍv��(�J�n���Ԏ擾)
    LARGE_INTEGER   frequency;
    LARGE_INTEGER   start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    //----------------------------------------------------------------------------
    ImageProc();

    //----------------------------------------------------------------------------
    // �������Ԍv��(�I�����Ԏ擾)
    LARGE_INTEGER   end;
    LONGLONG        span, usec;
    QueryPerformanceCounter(&end);
    span = end.QuadPart - start.QuadPart;
    usec = (span * 1000000L) / frequency.QuadPart;
    m_proninfo.procTime = (DOUBLE)usec / 1000.0;    // �揈������[ms]

    g_pSharedObject->SetInfo(m_proninfo);

    return;
}

/// @brief �摜����
/// @param
/// @return 
/// @note
void CAnalyst::ImageProc(void)
{
    cv::Mat imgSrc;
    cv::Mat imgHSV;
    cv::Mat imgHSVBin;
    cv::Mat imgROI; // �ؔ����摜
    cv::Mat imgMask[IMGPROC_ID_MAX];
    cv::Mat lut;
    BOOL    bImgEnable = FALSE;
    UINT32  width = 0, height = 0;
    UINT    maskLow[3], maskUpp[3];
    std::vector<cv::Mat> planes;
    std::vector<std::vector<cv::Point>> contours;
    BOOL    ret = FALSE;

    //----------------------------------------------------------------------------
   // �摜�����ݒ�Ǎ���
    g_pSharedObject->GetParam(&m_imgprocparam);

    //----------------------------------------------------------------------------
    // ���摜�Ǎ���
#pragma region GetSrcImage
    if      (g_pSharedObject->GetImage(IMAGE_ID_RAW_A, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else if (g_pSharedObject->GetImage(IMAGE_ID_RAW_B, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else ;
#pragma endregion GetSrcImage

    //----------------------------------------------------------------------------
    // ���o����
#pragma region ProcTarget
    if (bImgEnable)
    {
        //----------------------------------------------------------------------------
        // �摜�F��BGR��HSV�ɕϊ�
#pragma region ConvBGRToHSV
        if (m_imgprocparam.roi.valid == 0) {cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);}
#pragma endregion ConvBGRToHSV

        //----------------------------------------------------------------------------
        // �e�`�����l�����Ƃ�2�l��(LUT�ϊ�)���A3�`�����l���S�Ă�AND�����A�}�X�N�摜���쐬����
#pragma region CreateMaskImage
        lut = cv::Mat(256, 1, CV_8UC3); // LUT
        for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
        {
            if (m_imgprocparam.maskvalid[ii])
            {
                if (m_imgprocparam.roi.valid > 0)
                {
                    // ROI�͈̔�(�����`)��ݒ肷��
                    // * (x, y, width, height)�Ŏw��
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

                    // �����摜�𐶐�
                    // * �����摜�Ƃ��̌��摜�͋��ʂ̉摜�f�[�^���Q�Ƃ��邽�߁A
                    //   �����摜�ɕύX��������ƁA���摜���ύX�����B
                    imgROI = imgSrc(m_proninfo.imgprocdata[ii].roi);
                    // �摜�F��BGR��HSV�ɕϊ�
                    cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
                }
                else
                {
                    m_proninfo.imgprocdata[ii].roi.x      = 0;
                    m_proninfo.imgprocdata[ii].roi.y      = 0;
                    m_proninfo.imgprocdata[ii].roi.width  = imgSrc.cols;
                    m_proninfo.imgprocdata[ii].roi.height = imgSrc.rows;
                }
                // 3�`�����l����LUT�쐬
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
                // �`�����l�����Ƃ�LUT�ϊ�(�e�`�����l�����Ƃ�2�l������)
                cv::LUT(imgHSV, lut, imgHSVBin);
                // �}�X�N�摜�̍쐬
                cv::split(imgHSVBin, planes);   // �`�����l�����Ƃ�2�l�����ꂽ�摜�����ꂼ��̃`�����l���ɕ�������
                cv::bitwise_and(planes[0],   planes[1], imgMask[ii]);
                cv::bitwise_and(imgMask[ii], planes[2], imgMask[ii]);
            }   // if (m_imgprocparam.maskvalid[ii])
        }   // for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
#pragma endregion CreateMaskImage

        //----------------------------------------------------------------------------
        // �}�X�N���ꂽ�摜�̍���
#pragma region MaskImageCombined
// �e�}�X�N�摜�͌ʂɏ���
#if 0
        cv::Mat imgMask;
//      imgMask = imgMask[IMGPROC_ID_IMG_1] + imgMask[IMGPROC_ID_IMG_2];
        cv::add(imgMask[IMGPROC_ID_IMG_1], imgMask[IMGPROC_ID_IMG_2], imgMask);

//      // �o��
//      cv::Mat imgMasked;
//      imgSrc.copyTo(imgMasked, imgMask);
#endif
#pragma endregion MaskImageCombined

        //----------------------------------------------------------------------------
        // �m�C�Y����
        // �S�}��
#pragma region NoiseCut1
        switch (m_imgprocparam.filter1.type)
        {
        case NOISEFILTER1_MEDIAN:    // �����l�t�B���^
            for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
            {
                if (m_imgprocparam.maskvalid[ii]) {cv::medianBlur(imgMask[ii], imgMask[ii], m_imgprocparam.filter1.val);}
            }
            break;
        case NOISEFILTER1_OPENNING:  // �I�[�v�j���O����(�k�����g��)
            for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
            {
                if (m_imgprocparam.maskvalid[ii])
                {
//                  cv::morphologyEx(imgMask[ii], imgMask[ii], MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), m_imgprocparam.filter1.val);
                    cv::erode(imgMask[ii],  imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter1.val); // ���k
                    cv::dilate(imgMask[ii], imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter1.val); // �c��
                }
            }
            break;
        default:
            break;
        }
#pragma endregion NoiseCut1

        // ������
#pragma region NoiseCut2
        switch (m_imgprocparam.filter2.type)
        {
        case NOISEFILTER2_CLOSING:  // �N���[�W���O����(�g�偨�k��)
            for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
            {
                if (m_imgprocparam.maskvalid[ii])
                {
//                  cv::morphologyEx(imgMask[ii], imgMask[ii], MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), m_imgprocparam.filter2.val);
                    cv::dilate(imgMask[ii], imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter2.val); // �c��
                    cv::erode(imgMask[ii],  imgMask[ii], cv::Mat(), cv::Point(-1, -1), m_imgprocparam.filter2.val); // ���k
                }
            }
            break;
        default:
            break;
        }
#pragma endregion NoiseCut2

        //----------------------------------------------------------------------------
        // �摜����
#pragma region ImageProc
        double  posx, posy;
        int     tgtsize;
        for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
        {
            if (m_imgprocparam.maskvalid[ii])
            {
                // �֊s���o(��ԊO���̔��̗֊s�݂̂��擾)
                cv::findContours(imgMask[ii], contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

                // �d�S���o
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
        }   // for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
#pragma endregion ImageProc

        //----------------------------------------------------------------------------
        // �摜�ۑ�
#pragma region PutImage
        // �}�X�N�摜1
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
        // �}�X�N�摜2
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
        // �����摜
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
    // �U�ꌟ�o����
#pragma region SwayProc
    SwayProc();
#pragma endregion SwayProc
}

/// @brief �d�S���o
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
    // �d�S�ʒu�Z�o�A���S���Y��(�S�֊s�_)
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
    // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s�ʐ�)
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
    // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s��)
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

/// @brief �U�ꌟ�o����
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
    // �U��ʒu���o
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
    // �U��p, �U��p���x���o
#pragma region SWAY_ANG
    double anglci;                  // BOX�ݓ_�`�J�������S�p�x��ci
    double lx, ly;                  // �݋�ݓ_�`�J�������S����Lx,Ly
    double l;                       // �J�������S�`�^�[�Q�b�g�ԋ���L
    double anglsensor;              // ��sensor
    double angl;                    // ��sensor + ��inc + ��0
    double deg, rad, spd;
    double dt;

    dt = inf.cycle_ms * 0.001;
    for (UINT ii = 0; ii < AXIS_MAX; ii++)
    {
        if (m_proninfo.valid)
        {
            anglci = m_cmmnparam.cnfg[ii].camoffsetAC + rioinfo.incldata[ii].deg;       // BOX�ݓ_�`�J�������S�p�x��ci:��c + ��inc
            lx     = m_cmmnparam.cnfg[ii].camoffsetLX0
                   - ((m_cmmnparam.cnfg[ii].camoffsetL0 * sin(extninfo.boxangle * CONV_DEG_RAD))
                   +  (m_cmmnparam.cnfg[ii].camoffsetLC * sin(anglci * CONV_DEG_RAD))); // �݋�ݓ_�`�J�������S����Lx:Lx0 - (l0sin�� + lcsin��)
            ly     = m_cmmnparam.cnfg[ii].camoffsetLY0
                   - ((m_cmmnparam.cnfg[ii].camoffsetL0 * cos(extninfo.boxangle * CONV_DEG_RAD))
                   +  (m_cmmnparam.cnfg[ii].camoffsetLC * cos(anglci * CONV_DEG_RAD))); // �݋�ݓ_�`�J�������S����Ly:Ly0 - (l0cos�� + lccos��)
            l      = extninfo.ropelen - ly;                                             // �J�������S�`�^�[�Q�b�g�ԋ���L:Lr - Ly

            anglsensor = (m_proninfo.swaydata[AXIS_X].pos - (double)m_camparam.size[ii] * 0.5) * (m_cmmnparam.cnfg[ii].camviewangl / (double)m_camparam.size[ii]);  // ��sensor
            angl       = anglsensor + rioinfo.incldata[ii].deg + m_cmmnparam.cnfg[ii].camoffsetA0;  // ��sensor + ��inc + ��0

            rad = atan((l * tan(angl * CONV_DEG_RAD) - lx) / extninfo.ropelen);
            deg = rad / CONV_DEG_RAD;
            spd = (rad - m_proninfo.swaydata[ii].rad) / dt;
            spd = ((dt * spd) + (m_cmmnparam.filter * m_proninfo.swaydata[ii].spd)) / (m_cmmnparam.filter + dt);

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
