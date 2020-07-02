#include "CAnalyst.h"
#include "CSharedObject.h"

extern CSharedObject*   g_pSharedObject;    // �^�X�N�ԋ��L�f�[�^�̃|�C���^

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
    m_iBufferImgMask1 = IMAGE_ID_MASK1_A;
    m_iBufferImgMask2 = IMAGE_ID_MASK2_A;
    m_iBufferImgProc  = IMAGE_ID_PROC_A;

    stImageProcData stImgProcData[IMGPROC_ID_MAX];
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        stImgProcData[ii].posx       = 0.0;
        stImgProcData[ii].posy       = 0.0;
        stImgProcData[ii].roi.x      = 0;
        stImgProcData[ii].roi.y      = 0;
        stImgProcData[ii].roi.width  = IMAGE_ROI_SIZE;
        stImgProcData[ii].roi.height = IMAGE_ROI_SIZE;
        stImgProcData[ii].roisize    = IMAGE_ROI_SIZE;
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
    // �������Ԍv��(�J�n���Ԏ擾)
    LARGE_INTEGER   frequency;
    LARGE_INTEGER   start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    //----------------------------------------------------------------------------
    ImageProc();

    //----------------------------------------------------------------------------
    InclinationProc();

    //----------------------------------------------------------------------------
    // �������Ԍv��(�I�����Ԏ擾)
    LARGE_INTEGER   end;
    LONGLONG        span, usec;
    QueryPerformanceCounter(&end);
    span = end.QuadPart - start.QuadPart;
    usec = (span * 1000000L) / frequency.QuadPart;
    g_pSharedObject->SetParam(PARAM_ID_DOUBLE_PROC_TIME, (DOUBLE)usec / 1000.0);

    return;
}

/// @brief �摜����
/// @param
/// @return 
/// @note
void CAnalyst::ImageProc(void)
{
    cv::Mat         imgSrc;
    cv::Mat         imgHSV;
    cv::Mat         imgHSVBin;
    cv::Mat         imgROI; // �ؔ����摜
    cv::Mat         imgMask1, imgMask2;
    cv::Mat         lut;
    BOOL            bImgEnable = FALSE;
    UINT32          width = 0, height = 0;
    UINT            maskLow[3], maskUpp[3];
    std::vector<cv::Mat> planes;
    UINT32          roiEnable;
    UINT32          filter;
    UINT32          filterval;
    std::vector<std::vector<cv::Point>> contours;
    stImageProcData stImgProcData[IMGPROC_ID_MAX];
    double          posX, posY;
    UINT32          algo;
    BOOL            ret = FALSE;

    //----------------------------------------------------------------------------
    // ���摜�Ǎ���
#pragma region GetSrcImage
    if      (g_pSharedObject->GetImage(IMAGE_ID_RAW_A, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else if (g_pSharedObject->GetImage(IMAGE_ID_RAW_B, &imgSrc) == RESULT_OK) {bImgEnable = TRUE;}
    else ;
#pragma endregion GetSrcImage

    if (bImgEnable)
    {
        //----------------------------------------------------------------------------
        // �摜�F��BGR��HSV�ɕϊ�
#pragma region ConvBGRToHSV
        g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_ENABLE, &roiEnable);
        if (roiEnable == 0) {cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);}
#pragma endregion ConvBGRToHSV

        //----------------------------------------------------------------------------
        // �e�`�����l�����Ƃ�2�l��(LUT�ϊ�)���A3�`�����l���S�Ă�AND�����A�}�X�N�摜���쐬����
#pragma region CreateMaskImage
        lut = cv::Mat(256, 1, CV_8UC3); // LUT
        // �摜1
        {
            if (roiEnable > 0)
            {
                // ROI�͈̔�(�����`)��ݒ肷��
                // * (x, y, width, height)�Ŏw��
                if (g_pSharedObject->GetProcData(IMGPROC_ID_IMG_1, &stImgProcData[IMGPROC_ID_IMG_1]) != RESULT_OK)
                {
                    stImgProcData[IMGPROC_ID_IMG_1].posx = 0.0;
                    stImgProcData[IMGPROC_ID_IMG_1].posy = 0.0;
                    stImgProcData[IMGPROC_ID_IMG_1].enable = FALSE;
                } 
                if (stImgProcData[IMGPROC_ID_IMG_1].enable)
                {
//@@@roisize�͎����v�Z����悤�ɂ���K�v����
                    if      (((int)stImgProcData[IMGPROC_ID_IMG_1].posx - (stImgProcData[IMGPROC_ID_IMG_1].roisize / 2)) < 0)           {stImgProcData[IMGPROC_ID_IMG_1].roi.x  = 0;}
                    else if (((int)stImgProcData[IMGPROC_ID_IMG_1].posx + (stImgProcData[IMGPROC_ID_IMG_1].roisize / 2)) > imgSrc.cols) {stImgProcData[IMGPROC_ID_IMG_1].roi.x  = imgSrc.cols - stImgProcData[IMGPROC_ID_IMG_1].roisize;}
                    else                                                                                                                {stImgProcData[IMGPROC_ID_IMG_1].roi.x  = (int)stImgProcData[IMGPROC_ID_IMG_1].posx - (stImgProcData[IMGPROC_ID_IMG_1].roisize / 2);}
                    if      (((int)stImgProcData[IMGPROC_ID_IMG_1].posy - (stImgProcData[IMGPROC_ID_IMG_1].roisize / 2)) < 0)           {stImgProcData[IMGPROC_ID_IMG_1].roi.y  = 0;}
                    else if (((int)stImgProcData[IMGPROC_ID_IMG_1].posy + (stImgProcData[IMGPROC_ID_IMG_1].roisize / 2)) > imgSrc.rows) {stImgProcData[IMGPROC_ID_IMG_1].roi.y  = imgSrc.rows - stImgProcData[IMGPROC_ID_IMG_1].roisize;}
                    else                                                                                                                {stImgProcData[IMGPROC_ID_IMG_1].roi.y  = (int)stImgProcData[IMGPROC_ID_IMG_1].posy - (stImgProcData[IMGPROC_ID_IMG_1].roisize / 2);}
                    stImgProcData[IMGPROC_ID_IMG_1].roi.width  = stImgProcData[IMGPROC_ID_IMG_1].roisize;
                    stImgProcData[IMGPROC_ID_IMG_1].roi.height = stImgProcData[IMGPROC_ID_IMG_1].roisize;
                }
                else
                {
                    stImgProcData[IMGPROC_ID_IMG_1].roi.x      = 0;
                    stImgProcData[IMGPROC_ID_IMG_1].roi.y      = 0;
                    stImgProcData[IMGPROC_ID_IMG_1].roi.width  = imgSrc.cols;
                    stImgProcData[IMGPROC_ID_IMG_1].roi.height = imgSrc.rows;
                }
                // �����摜�𐶐�
                // * �����摜�Ƃ��̌��摜�͋��ʂ̉摜�f�[�^���Q�Ƃ��邽�߁A
                //   �����摜�ɕύX��������ƁA���摜���ύX�����B
                imgROI = imgSrc(stImgProcData[IMGPROC_ID_IMG_1].roi);
                // �摜�F��BGR��HSV�ɕϊ�
                cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
            }
            else
            {
                stImgProcData[IMGPROC_ID_IMG_1].roi.x      = 0;
                stImgProcData[IMGPROC_ID_IMG_1].roi.y      = 0;
                stImgProcData[IMGPROC_ID_IMG_1].roi.width  = imgSrc.cols;
                stImgProcData[IMGPROC_ID_IMG_1].roi.height = imgSrc.rows;
                stImgProcData[IMGPROC_ID_IMG_1].roisize    = IMAGE_ROI_SIZE;
            }
            // 3�`�����l����LUT�쐬
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
            // �`�����l�����Ƃ�LUT�ϊ�(�e�`�����l�����Ƃ�2�l������)
            cv::LUT(imgHSV, lut, imgHSVBin);
            // �}�X�N�摜�̍쐬
            cv::split(imgHSVBin, planes);   // �`�����l�����Ƃ�2�l�����ꂽ�摜�����ꂼ��̃`�����l���ɕ�������
            cv::bitwise_and(planes[0], planes[1], imgMask1);
            cv::bitwise_and(imgMask1,  planes[2], imgMask1);
        }

        // �摜2
        {
            if (roiEnable > 0)
            {
                // ROI�͈̔́i�����`�j��ݒ肷��
                // * (x, y, width, height)�Ŏw��
                if (g_pSharedObject->GetProcData(IMGPROC_ID_IMG_2, &stImgProcData[IMGPROC_ID_IMG_2]) != RESULT_OK)
                {
                    stImgProcData[IMGPROC_ID_IMG_2].posx   = 0.0;
                    stImgProcData[IMGPROC_ID_IMG_2].posy   = 0.0;
                    stImgProcData[IMGPROC_ID_IMG_2].enable = FALSE;
                }
                if (stImgProcData[IMGPROC_ID_IMG_2].enable)
                {
//@@@roisize�͎����v�Z����悤�ɂ���K�v����
                    if      (((int)stImgProcData[IMGPROC_ID_IMG_2].posx - (stImgProcData[IMGPROC_ID_IMG_2].roisize / 2)) < 0)           {stImgProcData[IMGPROC_ID_IMG_2].roi.x  = 0;}
                    else if (((int)stImgProcData[IMGPROC_ID_IMG_2].posx + (stImgProcData[IMGPROC_ID_IMG_2].roisize / 2)) > imgSrc.cols) {stImgProcData[IMGPROC_ID_IMG_2].roi.x  = imgSrc.cols - stImgProcData[IMGPROC_ID_IMG_2].roisize;}
                    else                                                                                                                {stImgProcData[IMGPROC_ID_IMG_2].roi.x  = (int)stImgProcData[IMGPROC_ID_IMG_2].posx - (stImgProcData[IMGPROC_ID_IMG_2].roisize / 2);}
                    if      (((int)stImgProcData[IMGPROC_ID_IMG_2].posy - (stImgProcData[IMGPROC_ID_IMG_2].roisize / 2)) < 0)           {stImgProcData[IMGPROC_ID_IMG_2].roi.y  = 0;}
                    else if (((int)stImgProcData[IMGPROC_ID_IMG_2].posy + (stImgProcData[IMGPROC_ID_IMG_2].roisize / 2)) > imgSrc.rows) {stImgProcData[IMGPROC_ID_IMG_2].roi.y  = imgSrc.rows - stImgProcData[IMGPROC_ID_IMG_2].roisize;}
                    else                                                                                                                {stImgProcData[IMGPROC_ID_IMG_2].roi.y  = (int)stImgProcData[IMGPROC_ID_IMG_2].posy - (stImgProcData[IMGPROC_ID_IMG_2].roisize / 2);}
                    stImgProcData[IMGPROC_ID_IMG_2].roi.width  = stImgProcData[IMGPROC_ID_IMG_2].roisize;
                    stImgProcData[IMGPROC_ID_IMG_2].roi.height = stImgProcData[IMGPROC_ID_IMG_2].roisize;
                }
                else
                {
                    stImgProcData[IMGPROC_ID_IMG_2].roi.x      = 0;
                    stImgProcData[IMGPROC_ID_IMG_2].roi.y      = 0;
                    stImgProcData[IMGPROC_ID_IMG_2].roi.width  = imgSrc.cols;
                    stImgProcData[IMGPROC_ID_IMG_2].roi.height = imgSrc.rows;
                    stImgProcData[IMGPROC_ID_IMG_2].roisize    = IMAGE_ROI_SIZE;
                }
                // �����摜�𐶐�
                // * �����摜�Ƃ��̌��摜�͋��ʂ̉摜�f�[�^���Q�Ƃ��邽�߁A
                //   �����摜�ɕύX��������ƁA���摜���ύX�����B
                imgROI = imgSrc(stImgProcData[IMGPROC_ID_IMG_2].roi);
                // �摜�F��BGR��HSV�ɕϊ�
                cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
            }
            else
            {
                stImgProcData[IMGPROC_ID_IMG_2].roi.x      = 0;
                stImgProcData[IMGPROC_ID_IMG_2].roi.y      = 0;
                stImgProcData[IMGPROC_ID_IMG_2].roi.width  = imgSrc.cols;
                stImgProcData[IMGPROC_ID_IMG_2].roi.height = imgSrc.rows;
                stImgProcData[IMGPROC_ID_IMG_2].roisize    = IMAGE_ROI_SIZE;
            }
            // 3�`�����l����LUT�쐬
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
            // �`�����l�����Ƃ�LUT�ϊ�(�e�`�����l�����Ƃ�2�l������)
            cv::LUT(imgHSV, lut, imgHSVBin);
            // �}�X�N�摜�̍쐬
            cv::split(imgHSVBin, planes);   // �`�����l�����Ƃ�2�l�����ꂽ�摜�����ꂼ��̃`�����l���ɕ�������
            cv::bitwise_and(planes[0], planes[1], imgMask2);
            cv::bitwise_and(imgMask2,  planes[2], imgMask2);
        }
#pragma endregion CreateMaskImage

        //----------------------------------------------------------------------------
        // �}�X�N���ꂽ�摜�̍���
#pragma region MaskImageCombined
// �e�}�X�N�摜�͌ʂɏ���
#if 0
        cv::Mat imgMask;
//      imgMask = imgMask1 + imgMask2;
        cv::add(imgMask1, imgMask2, imgMask);

//      // �o��
//      cv::Mat imgMasked;
//      imgSrc.copyTo(imgMasked, imgMask);
#endif
#pragma endregion MaskImageCombined

        //----------------------------------------------------------------------------
        // �m�C�Y����
#pragma region NoiseCut
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER,    &filter);
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTERVAL, &filterval);
        switch (filter)
        {
        case NOISEFILTER_MEDIAN:    // �����l�t�B���^
            // �摜1
            cv::medianBlur(imgMask1, imgMask1, filterval);
            // �摜2
            cv::medianBlur(imgMask2, imgMask2, filterval);
            break;
        case NOISEFILTER_OPENNING:  // �I�[�v�j���O����(�k�����g��)
            // �摜1
            cv::erode(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
            cv::dilate(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            // �摜2
            cv::erode(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
            cv::dilate(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            break;
        default:
            break;
        }
#pragma endregion NoiseCut

        //----------------------------------------------------------------------------
        // �摜����
#pragma region ImageProc
        g_pSharedObject->GetParam(PARAM_ID_IMG_ALGORITHM, &algo);

        // �摜1
        {
            // �֊s���o
            cv::findContours(imgMask1, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

            // ���^�[�Q�b�g�̐M�����]��

            // �d�S�ʒu�v�Z
            posX = 0.0;
            posY = 0.0;
            if (algo == COG_ALGORITHM_ALL) {ret = CalcCenterOfGravity1(contours, &posX, &posY);}
            else                           {ret = CalcCenterOfGravity2(contours, &posX, &posY);}
            stImgProcData[IMGPROC_ID_IMG_1].posx   = posX + stImgProcData[IMGPROC_ID_IMG_1].roi.x;
            stImgProcData[IMGPROC_ID_IMG_1].posy   = posY + stImgProcData[IMGPROC_ID_IMG_1].roi.y;
            stImgProcData[IMGPROC_ID_IMG_1].enable = ret;
            g_pSharedObject->SetProcData(IMGPROC_ID_IMG_1, stImgProcData[IMGPROC_ID_IMG_1]);
        }

        // �摜2
        {
            // �֊s���o
            cv::findContours(imgMask2, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

            // ���^�[�Q�b�g�̐M�����]��

            // �d�S�ʒu�v�Z
            posX = 0.0;
            posY = 0.0;
            if (algo == COG_ALGORITHM_ALL) {ret = CalcCenterOfGravity1(contours, &posX, &posY);}
            else                           {ret = CalcCenterOfGravity2(contours, &posX, &posY);}
            stImgProcData[IMGPROC_ID_IMG_2].posx   = posX + stImgProcData[IMGPROC_ID_IMG_2].roi.x;
            stImgProcData[IMGPROC_ID_IMG_2].posy   = posY + stImgProcData[IMGPROC_ID_IMG_2].roi.y;
            stImgProcData[IMGPROC_ID_IMG_2].enable = ret;
            g_pSharedObject->SetProcData(IMGPROC_ID_IMG_2, stImgProcData[IMGPROC_ID_IMG_2]);
        }
#pragma endregion ImageProc

        //----------------------------------------------------------------------------
        // �摜�ۑ�
#pragma region PutImage
        // �}�X�N�摜1
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
        // �}�X�N�摜2
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
            stImgProcData[ii].posx       = 0.0;
            stImgProcData[ii].posy       = 0.0;
            stImgProcData[ii].roi.x      = 0;
            stImgProcData[ii].roi.y      = 0;
            stImgProcData[ii].roi.width  = IMAGE_ROI_SIZE;
            stImgProcData[ii].roi.height = IMAGE_ROI_SIZE;
            stImgProcData[ii].roisize    = IMAGE_ROI_SIZE;
            stImgProcData[ii].enable     = FALSE;
            g_pSharedObject->SetProcData(ii, stImgProcData[ii]);
        }
    }
}

/// @brief �X�Όv�f�[�^����
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
            // 12.0mA�����Ȃ�Ίp�x��-�����ɓ|��Ă���(4mA��-30�x)
            port1AngleData = ((port1AnaData - 4.0) - 8.0) / 8.0 * INCLINATION_MAX;
        }
        else
        {
            // 12.0mA�ȏ�Ȃ�Ίp�x��+�����ɓ|��Ă���(20mA��30�x)
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
            // 12.0mA�����Ȃ�Ίp�x��-�����ɓ|��Ă���(4mA��-30�x)
            port2AngleData = ((port2AnaData - 4.0) - 8.0) / 8.0 * INCLINATION_MAX;
        }
        else
        {
            // 12.0mA�ȏ�Ȃ�Ίp�x��+�����ɓ|��Ă���(20mA��30�x)
            port2AngleData = (port2AnaData - 12.0) / 8.0 * INCLINATION_MAX;
        }
        g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_RAD, port2AngleData);
    }
}

/// @brief �d�S�ʒu�Z�o�A���S���Y��1
/// @param
/// @return 
/// @note
BOOL CAnalyst::CalcCenterOfGravity1(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY)
{
    BOOL    ret   = FALSE;
    double  posX  = 0.0;
    double  posY  = 0.0;
    UINT32  count = 0;

    // �֊s�S�_����d�S�ʒu���Z�o
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

/// @brief �d�S�ʒu�Z�o�A���S���Y��2
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
        // �Ώۗ֊s�����݂��Ȃ��ꍇ
        *outPosX = log(-1.0);
        *outPosY = log(-1.0);
        return FALSE;
    }

    // �֊s��������2�̗֊s�����Ώۂ𒊏o����
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
        // �Ώۗ֊s�����݂��Ȃ��ꍇ
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
