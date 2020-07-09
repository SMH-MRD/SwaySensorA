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
    UINT32          roienable, roisize;
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
        g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_ENABLE, &roienable);
        if (roienable == 0) {cv::cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);}
#pragma endregion ConvBGRToHSV

        //----------------------------------------------------------------------------
        // �e�`�����l�����Ƃ�2�l��(LUT�ϊ�)���A3�`�����l���S�Ă�AND�����A�}�X�N�摜���쐬����
#pragma region CreateMaskImage
        lut = cv::Mat(256, 1, CV_8UC3); // LUT
        g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_SIZE, &roisize);
        if ((int)roisize >= imgSrc.cols) roisize = imgSrc.cols;
        if ((int)roisize >= imgSrc.rows) roisize = imgSrc.rows;
        // �摜1
#pragma region Image1
        if (roienable > 0)
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
            // �����摜�𐶐�
            // * �����摜�Ƃ��̌��摜�͋��ʂ̉摜�f�[�^���Q�Ƃ��邽�߁A
            //   �����摜�ɕύX��������ƁA���摜���ύX�����B
            imgROI = imgSrc(stImgProcData[IMGPROC_ID_IMG_1].roi);
            // �摜�F��BGR��HSV�ɕϊ�
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
#pragma endregion Image1

        // �摜2
#pragma region Image2
        if (roienable > 0)
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
            // �����摜�𐶐�
            // * �����摜�Ƃ��̌��摜�͋��ʂ̉摜�f�[�^���Q�Ƃ��邽�߁A
            //   �����摜�ɕύX��������ƁA���摜���ύX�����B
            imgROI = imgSrc(stImgProcData[IMGPROC_ID_IMG_2].roi);
            // �摜�F��BGR��HSV�ɕϊ�
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
#pragma endregion Image2
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
        // �S�}��
#pragma region NoiseCut1
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER1,    &filter);
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTERVAL1, &filterval);
        switch (filter)
        {
        case NOISEFILTER1_MEDIAN:    // �����l�t�B���^
            // �摜1
            cv::medianBlur(imgMask1, imgMask1, filterval);
            // �摜2
            cv::medianBlur(imgMask2, imgMask2, filterval);
            break;
        case NOISEFILTER1_OPENNING:  // �I�[�v�j���O����(�k�����g��)
            // �摜1
//          cv::morphologyEx(imgMask1, imgMask1, MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::erode(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
            cv::dilate(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            // �摜2
//          cv::morphologyEx(imgMask2, imgMask2, MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::erode(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
            cv::dilate(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            break;
        default:
            break;
        }
#pragma endregion NoiseCut1

        // ������
#pragma region NoiseCut2
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTER2,    &filter);
        g_pSharedObject->GetParam(PARAM_ID_IMG_NOISEFILTERVAL2, &filterval);
        // �摜1
        switch (filter)
        {
        case NOISEFILTER2_CLOSING:  // �N���[�W���O����(�g�偨�k��)
//          cv::morphologyEx(imgMask1, imgMask1, MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::dilate(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            cv::erode(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
            // �摜2
//          cv::morphologyEx(imgMask2, imgMask2, MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), filterval);
            cv::dilate(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            cv::erode(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
            break;
        default:
            break;
        }
#pragma endregion NoiseCut2

        //----------------------------------------------------------------------------
        // �摜����
#pragma region ImageProc
        g_pSharedObject->GetParam(PARAM_ID_IMG_ALGORITHM, &algo);
        // �摜1
#pragma region Image1
        // �֊s���o(��ԊO���̔��̗֊s�݂̂��擾)
        cv::findContours(imgMask1, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        // �d�S���o
        posX = 0.0;
        posY = 0.0;
        ret  = CalcCenterOfGravity(contours, &posX, &posY, algo);
        stImgProcData[IMGPROC_ID_IMG_1].posx   = posX + stImgProcData[IMGPROC_ID_IMG_1].roi.x;
        stImgProcData[IMGPROC_ID_IMG_1].posy   = posY + stImgProcData[IMGPROC_ID_IMG_1].roi.y;
        stImgProcData[IMGPROC_ID_IMG_1].enable = ret;
        g_pSharedObject->SetProcData(IMGPROC_ID_IMG_1, stImgProcData[IMGPROC_ID_IMG_1]);
#pragma endregion Image1

        // �摜2
#pragma region Image2
        // �֊s���o
        cv::findContours(imgMask2, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        // �d�S���o
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

/// @brief �d�S���o
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
    case COG_ALGORITHM_ALL:     // �d�S�ʒu�Z�o�A���S���Y��(�S�֊s�_)
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
    case COG_ALGORITHM_AREA:    // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s�ʐ�)
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
    case COG_ALGORITHM_LEN:     // �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s��)
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
