#include "CAnalyst.h"

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

    UINT32  exptime;
    g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &exptime);
    for (int ii = 0; ii < IMGPROC_ID_MAX; ii++)
    {
        m_stProcInfo.data[ii].posx       = 0.0;
        m_stProcInfo.data[ii].posy       = 0.0;
        m_stProcInfo.data[ii].roiSize    = 0;
        m_stProcInfo.data[ii].roi.x      = 0;
        m_stProcInfo.data[ii].roi.y      = 0;
        m_stProcInfo.data[ii].roi.width  = 0;
        m_stProcInfo.data[ii].roi.height = 0;
        m_stProcInfo.data[ii].valid      = FALSE;
    }
    m_stProcInfo.exposureTime = exptime;    // �I������[us]
    m_stProcInfo.procTime     = 0.0;        // �揈������[ms]
    g_pSharedObject->SetProcInfo(m_stProcInfo);
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
    m_stProcInfo.procTime = (DOUBLE)usec / 1000.0;        // �揈������[ms]

    g_pSharedObject->SetProcInfo(m_stProcInfo);

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
    UINT32          maskimage;
    UINT32          roienable, roisize;
    UINT32          filter;
    UINT32          filterval;
    std::vector<std::vector<cv::Point>> contours;
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

    //----------------------------------------------------------------------------
    // ���o����
#pragma region ProcTarget
    if (bImgEnable)
    {
        g_pSharedObject->GetParam(PARAM_ID_IMG_MASK_TYPE, &maskimage);  // �}�X�N�摜�I��

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
        // �摜1
#pragma region Image1
        if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE1))
        {
            if (roienable > 0)
            {
                // ROI�͈̔�(�����`)��ݒ肷��
                // * (x, y, width, height)�Ŏw��
                if (m_stProcInfo.data[IMGPROC_ID_IMG_1].valid)
                {
                    int tmpval = (int)(((double)m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize / 2.0) + 0.5);
                    if      (((int)m_stProcInfo.data[IMGPROC_ID_IMG_1].posx - tmpval) < 0)           {m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.x  = 0;}
                    else if (((int)m_stProcInfo.data[IMGPROC_ID_IMG_1].posx + tmpval) > imgSrc.cols) {m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.x  = imgSrc.cols - m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize;}
                    else                                                                             {m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.x  = (int)m_stProcInfo.data[IMGPROC_ID_IMG_1].posx - tmpval;}
                    if      (((int)m_stProcInfo.data[IMGPROC_ID_IMG_1].posy - tmpval) < 0)           {m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.y  = 0;}
                    else if (((int)m_stProcInfo.data[IMGPROC_ID_IMG_1].posy + tmpval) > imgSrc.rows) {m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.y  = imgSrc.rows - m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize;}
                    else                                                                             {m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.y  = (int)m_stProcInfo.data[IMGPROC_ID_IMG_1].posy - tmpval;}
                    m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.width  = m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize;
                    m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.height = m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize;
                }
                else
                {
                    m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.x      = 0;
                    m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.y      = 0;
                    m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.width  = imgSrc.cols;
                    m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.height = imgSrc.rows;
                }
                // �����摜�𐶐�
                // * �����摜�Ƃ��̌��摜�͋��ʂ̉摜�f�[�^���Q�Ƃ��邽�߁A
                //   �����摜�ɕύX��������ƁA���摜���ύX�����B
                imgROI = imgSrc(m_stProcInfo.data[IMGPROC_ID_IMG_1].roi);
                // �摜�F��BGR��HSV�ɕϊ�
                cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
            }
            else
            {
                m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.x      = 0;
                m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.y      = 0;
                m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.width  = imgSrc.cols;
                m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.height = imgSrc.rows;
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
            cv::bitwise_and(planes[0], planes[1], imgMask1);
            cv::bitwise_and(imgMask1,  planes[2], imgMask1);
        }   // if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE1))
#pragma endregion Image1

        // �摜2
#pragma region Image2
        if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE2))
        {
            if (roienable > 0)
            {
                // ROI�͈̔́i�����`�j��ݒ肷��
                // * (x, y, width, height)�Ŏw��
                if (m_stProcInfo.data[IMGPROC_ID_IMG_2].valid)
                {
                    int tmpval = (int)(((double)m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize / 2.0) + 0.5);
                    if      (((int)m_stProcInfo.data[IMGPROC_ID_IMG_2].posx - tmpval) < 0)           {m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.x  = 0;}
                    else if (((int)m_stProcInfo.data[IMGPROC_ID_IMG_2].posx + tmpval) > imgSrc.cols) {m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.x  = imgSrc.cols - m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize;}
                    else                                                                             {m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.x  = (int)m_stProcInfo.data[IMGPROC_ID_IMG_2].posx - tmpval;}
                    if      (((int)m_stProcInfo.data[IMGPROC_ID_IMG_2].posy - tmpval) < 0)           {m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.y  = 0;}
                    else if (((int)m_stProcInfo.data[IMGPROC_ID_IMG_2].posy + tmpval) > imgSrc.rows) {m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.y  = imgSrc.rows - m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize;}
                    else                                                                             {m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.y  = (int)m_stProcInfo.data[IMGPROC_ID_IMG_2].posy - tmpval;}
                    m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.width  = m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize;
                    m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.height = m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize;
                }
                else
                {
                    m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.x      = 0;
                    m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.y      = 0;
                    m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.width  = imgSrc.cols;
                    m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.height = imgSrc.rows;
                }
                // �����摜�𐶐�
                // * �����摜�Ƃ��̌��摜�͋��ʂ̉摜�f�[�^���Q�Ƃ��邽�߁A
                //   �����摜�ɕύX��������ƁA���摜���ύX�����B
                imgROI = imgSrc(m_stProcInfo.data[IMGPROC_ID_IMG_2].roi);
                // �摜�F��BGR��HSV�ɕϊ�
                cv::cvtColor(imgROI, imgHSV, COLOR_BGR2HSV);
            }
            else
            {
                m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.x      = 0;
                m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.y      = 0;
                m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.width  = imgSrc.cols;
                m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.height = imgSrc.rows;
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
        }   // if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE2))
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
            if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE1)) {cv::medianBlur(imgMask1, imgMask1, filterval);}
            // �摜2
            if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE2)) {cv::medianBlur(imgMask2, imgMask2, filterval);}
            break;
        case NOISEFILTER1_OPENNING:  // �I�[�v�j���O����(�k�����g��)
            // �摜1
            if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE1))
            {
//              cv::morphologyEx(imgMask1, imgMask1, MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), filterval);
                cv::erode(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
                cv::dilate(imgMask1, imgMask1, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            }
            // �摜2
            if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE2))
            {
//              cv::morphologyEx(imgMask2, imgMask2, MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), filterval);
                cv::erode(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);     // ���k
                cv::dilate(imgMask2, imgMask2, cv::Mat(), cv::Point(-1, -1), filterval);    // �c��
            }
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
        if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE1))
        {
            // �֊s���o(��ԊO���̔��̗֊s�݂̂��擾)
            cv::findContours(imgMask1, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

            // �d�S���o
            posX = 0.0;
            posY = 0.0;
            ret  = CalcCenterOfGravity(contours, &posX, &posY, &roisize, algo);
            m_stProcInfo.data[IMGPROC_ID_IMG_1].posx  = posX + m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.x;
            m_stProcInfo.data[IMGPROC_ID_IMG_1].posy  = posY + m_stProcInfo.data[IMGPROC_ID_IMG_1].roi.y;
            m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize = (int)roisize;
            if (m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize > imgSrc.cols) {m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize = imgSrc.cols;}
            if (m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize > imgSrc.rows) {m_stProcInfo.data[IMGPROC_ID_IMG_1].roiSize = imgSrc.rows;}
            m_stProcInfo.data[IMGPROC_ID_IMG_1].valid = ret;
        }
        else
        {
            m_stProcInfo.data[IMGPROC_ID_IMG_1].posx  = 0.0;
            m_stProcInfo.data[IMGPROC_ID_IMG_1].posy  = 0.0;
            m_stProcInfo.data[IMGPROC_ID_IMG_1].valid = FALSE;
        }
#pragma endregion Image1

        // �摜2
#pragma region Image2
        if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE2))
        {
            // �֊s���o(��ԊO���̔��̗֊s�݂̂��擾)
            cv::findContours(imgMask2, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

            // �d�S���o
            posX = 0.0;
            posY = 0.0;
            ret  = CalcCenterOfGravity(contours, &posX, &posY, &roisize, algo);
            m_stProcInfo.data[IMGPROC_ID_IMG_2].posx  = posX + m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.x;
            m_stProcInfo.data[IMGPROC_ID_IMG_2].posy  = posY + m_stProcInfo.data[IMGPROC_ID_IMG_2].roi.y;
            m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize = (int)roisize;
            if (m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize > imgSrc.cols) {m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize = imgSrc.cols;}
            if (m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize > imgSrc.rows) {m_stProcInfo.data[IMGPROC_ID_IMG_2].roiSize = imgSrc.rows;}
            m_stProcInfo.data[IMGPROC_ID_IMG_2].valid = ret;
        }
        else
        {
            m_stProcInfo.data[IMGPROC_ID_IMG_2].posx  = 0.0;
            m_stProcInfo.data[IMGPROC_ID_IMG_2].posy  = 0.0;
            m_stProcInfo.data[IMGPROC_ID_IMG_2].valid = FALSE;
        }
#pragma endregion Image2
#pragma endregion ImageProc

        //----------------------------------------------------------------------------
        // �摜�ۑ�
#pragma region PutImage
        // �}�X�N�摜1
        if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE1))
        {
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
        }
        // �}�X�N�摜2
        if ((maskimage == MASK_IMG_ALL) || (maskimage == MASK_IMG_IMAGE2))
        {
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
            m_stProcInfo.data[ii].posx       = 0.0;
            m_stProcInfo.data[ii].posy       = 0.0;
            m_stProcInfo.data[ii].roiSize    = 0;
            m_stProcInfo.data[ii].roi.x      = 0;
            m_stProcInfo.data[ii].roi.y      = 0;
            m_stProcInfo.data[ii].roi.width  = 0;
            m_stProcInfo.data[ii].roi.height = 0;
            m_stProcInfo.data[ii].valid      = FALSE;
        }
    }
#pragma endregion ProcTarget
}

/// @brief �d�S���o
/// @param
/// @return 
/// @note 
BOOL CAnalyst::CalcCenterOfGravity(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY, UINT32* outroisize, UINT32 sel)
{
    BOOL    ret     = FALSE;
    double  posX    = 0.0;
    double  posY    = 0.0;
    UINT32  roisize = 0;    
    UINT32  scale   = 0;

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
                    posX += x;
                    posY += y;
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
                posX /= count;
                posY /= count;
                width  = (xmax - xmin);
                height = (ymax - ymin);
                if (width > height) {roisize = (UINT32)width  + 1;}
                else                {roisize = (UINT32)height + 1;}
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
                    posX += contours.at(max_area_contour).at(ii).x;
                    posY += contours.at(max_area_contour).at(ii).y;
                }
                if (count > 0)
                {
                    posX /= count;
                    posY /= count;
                    roi = cv::boundingRect(contours[max_area_contour]);
                    if (roi.width > roi.height) {roisize = (UINT32)roi.width  + 1;}
                    else                        {roisize = (UINT32)roi.height + 1;}
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
	                    posX = mu.m10 / mu.m00;
	                    posY = mu.m01 / mu.m00;
                        roi = cv::boundingRect(contours[max_id]);
                        if (roi.width > roi.height) {roisize = (UINT32)roi.width  + 1;}
                        else                        {roisize = (UINT32)roi.height + 1;}
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
        ret  = FALSE;
    }
    g_pSharedObject->GetParam(PARAM_ID_IMG_ROI_SIZE, &scale);
    *outPosX    = posX;
    *outPosY    = posY;
    *outroisize = (UINT32)((double)roisize * (double)scale/10.0);

    return ret;
}
