#include "CComCamera.h"
#include "CSharedObject.h"

extern CSharedObject*   g_pSharedObject;    // タスク間共有データのポインタ

/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CComCamera::CComCamera()
{
}

/// @brief デストラクタ
/// @param
/// @return 
/// @note
CComCamera::~CComCamera()
{
    if (m_pCamera != NULL)
    {
        if (m_pCamera->IsOpen()) {m_pCamera->Close();}
        m_pCamera = NULL;
    }
    PylonTerminate();
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::init_task(void *pobj)
{
    PylonInitialize();
    m_pCamera = NULL;
    m_iBufferImg = IMAGE_ID_RAW_A;
    return;
};

/// @brief タスクスレッドでの処理関数
/// @param
/// @return 
/// @note
void CComCamera::routine_work(void *param)
{
    if (g_pSharedObject == NULL) {return;}
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();

    UINT32 setFrameRate;
    g_pSharedObject->GetParam(PARAM_ID_CAM_FRAMERATE, &setFrameRate);
    inf.cycle_ms = 1000 / setFrameRate;
    // 起動周期カウント値
    if (inf.cycle_ms >= SYSTEM_TICK_ms) {inf.cycle_count = inf.cycle_ms / SYSTEM_TICK_ms;}
    else                                {inf.cycle_count = 1;}

    UINT32 camProc, imageProc;
    if (g_pSharedObject->GetParam(PARAM_ID_IMG_GRAB_CAMERA, &camProc)   != RESULT_OK) {return;}
    if (g_pSharedObject->GetParam(PARAM_ID_IMG_GRAB_FILE,   &imageProc) != RESULT_OK) {return;}

    //----------------------------------------------------------------------------
    // 処理時間計測(開始時間取得)
    LARGE_INTEGER   frequency;
    LARGE_INTEGER   start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    if (camProc)
    {
        //----------------------------------------------------------------------------
        // Open camera.
        if (!OpenCamera()) {return;}

        //----------------------------------------------------------------------------
        // Start the grabbing
        StartGrabImage();

        //----------------------------------------------------------------------------
        // Image grabbed
        CaptureImage();
    }
    else if (imageProc)
    {
        //----------------------------------------------------------------------------
        // Image grabbed
        ImageProcStart();
    }
    else ;

    //----------------------------------------------------------------------------
    // 処理時間計測(終了時間取得)
    LARGE_INTEGER   end;
    LONGLONG        span, usec;
    QueryPerformanceCounter(&end);
    span = end.QuadPart - start.QuadPart;
    usec = (span * 1000000L) / frequency.QuadPart;
    g_pSharedObject->SetParam(PARAM_ID_DOUBLE_IMG_GRAB_TIME, (DOUBLE)usec / 1000.0);

    return;
}

/// @brief カメラ撮影処理
/// @param
/// @return 
/// @note
BOOL CComCamera::OpenCamera(void)
{
    try
    {
        //----------------------------------------------------------------------------
        if (m_pCamera == NULL)
        {
            m_pCamera = new CBaslerUniversalInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
            if (m_pCamera == NULL) {return FALSE;}
        }

        if (m_pCamera->IsOpen()) {return TRUE;}

        //----------------------------------------------------------------------------
        // Open camera
        m_pCamera->Open();

        UINT32 setWidth;
        g_pSharedObject->GetParam(PARAM_ID_CAM_WIDTH, &setWidth);
        m_pCamera->Width.SetValue(setWidth);

        UINT32 setHeight;
        g_pSharedObject->GetParam(PARAM_ID_CAM_HEIGHT, &setHeight);
        m_pCamera->Height.SetValue(setHeight);

        UINT32 setExposure;
        g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &setExposure);
        m_pCamera->ExposureTime.SetValue(setExposure);

        UINT32 setFrameRate;
        g_pSharedObject->GetParam(PARAM_ID_CAM_FRAMERATE, &setFrameRate);
        UINT frameRate = (UINT)m_pCamera->ResultingFrameRate.GetValue();
        if (setFrameRate < frameRate)
        {
            g_pSharedObject->SetParam(PARAM_ID_CAM_READ_FRAMERATE, setFrameRate);
        }
        else
        {
            // 最小値に丸め込む
            if (frameRate == 0) { frameRate = 1; }
            g_pSharedObject->SetParam(PARAM_ID_CAM_READ_FRAMERATE, frameRate);
        }

        // 画像取り込み用のバッファ数を5に指定します。こちらに明確な指定がない場合には、デフォルトで10枚になります。
        m_pCamera->MaxNumBuffer = 5;

        return TRUE;
    }
    catch (GenICam::GenericException & e)   // エラーハンドリング
    {
        return FALSE;
    }
}

/// @brief カメラ撮影処理
/// @param
/// @return 
/// @note
void CComCamera::StartGrabImage(void)
{
    try
    {
        if (m_pCamera == NULL) {return;}
        if (!m_pCamera->IsOpen() || m_pCamera->IsGrabbing()) {return;}
        // Start the grabbing of c_countOfImagesToGrab images.
        // The camera device is parameterized with a default configuration which sets up free-running continuous acquisition.

        UINT32 setExposure;
        g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &setExposure);
        m_pCamera->ExposureTime.SetValue(setExposure);

        m_pCamera->StartGrabbing(c_countOfImagesToGrab, GrabStrategy_LatestImageOnly);
    }
    catch (Pylon::GenericException)
    {
        if (m_pCamera != NULL) {delete m_pCamera;  m_pCamera = NULL;}
    }
}

/// @brief カメラ撮影処理
/// @param
/// @return 
/// @note
void CComCamera::CaptureImage(void)
{
    if (g_pSharedObject == NULL) {return;}

    wstring out_string;	wstring wstr; string str;

    // Declare an integer variable to count the number of grabbed images and create image file names with ascending number.
    static int grabbedImages = 0;
    try
    {
        if (m_pCamera == NULL) {return;}
//      CIntegerPtr width  = m_pCamera->GetNodeMap().GetNode("Width");
//      CIntegerPtr height = m_pCamera->GetNodeMap().GetNode("Height");

        CGrabResultPtr          ptrGrabResult;      // This smart pointer will receive the grab result data.
        CImageFormatConverter   formatConverter;    // First the image format converter class must be created.
        CPylonImage             pylonImage;         // Create a PylonImage that will be used to reate OpenCV images later.
        cv::Mat                 openCvImage;        // Create an OpenCV image.

        // Camera.StopGrabbing() is called automatically by the Retriveresult() method when c_countOfImagesToGrab images hav been retrived.
        while (m_pCamera->IsGrabbing())
        {
            // Wait for an image and then retrive it. A timeout of 5000 ms is used
            m_pCamera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
                // Access the image data.
                const uint8_t* pImageBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
                out_string.clear();
                out_string = out_string + L"Intensity of the first pixel:" + to_wstring((uint32_t)pImageBuffer[0])
                           + L"   SixeX:" + to_wstring(ptrGrabResult->GetWidth())
                           + L"   SixeY:" + to_wstring(ptrGrabResult->GetHeight());

                // Convert the grabbed buffer to a pylon image.
                formatConverter.OutputPixelFormat = PixelType_BGR8packed;   // Specify the output pixcel format.
                formatConverter.Convert(pylonImage, ptrGrabResult);

                // Create an OpenCV image from a pylon image.
                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
                if (m_iBufferImg == IMAGE_ID_RAW_A)
                {
                    g_pSharedObject->SetImage(IMAGE_ID_RAW_A, openCvImage);
                    m_iBufferImg = IMAGE_ID_RAW_B;
                }
                else
                {
                    g_pSharedObject->SetImage(IMAGE_ID_RAW_B, openCvImage);
                    m_iBufferImg = IMAGE_ID_RAW_A;
                }
                // Set saveImages to '1' to save images
                if (saveImages)
                {
                    // Create the current image name for saving.
                    ostringstream s;
                    // Create image name files with ascending grabbed image numbers.
                    s << "c:/work/image_" << grabbedImages << ".jpg";
                    std::string imageName(s.str());
                    // Save an OpenCV image.
                    imwrite(imageName, openCvImage);
                    grabbedImages++;
                }
                // Set recordVideo  to '1' to record AVI video file
                if (recordVideo)
                {
                    // Create an OpenCV video creater.
                    // VideoWriter cvVideoCreator;
                    // Define the video file name.
                    // string videoFileName = "c:/work/openCvVideo.avi";
                    // cvVideoCreator.open(videoFileName, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, frameSize, true);
                    // cvVideoCreator.open(videoFileName, CV_FOURCC('M', 'P', '4', '2'), 20, frameSize, true);
                    // cvVideoCreator.open(videoFileName, CV_FOURCC('M', 'J', 'P', 'G'), 20, frameSize, true);
                    if (cvVideoCreator.isOpened()) {cvVideoCreator << openCvImage;}
                }
                // Create an OpenCV display window.
//				namedWindow("OpenCV Display Window", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO

                // Display the current image in the OpenCV display window.
//				imshow("OpenCV Display Window", openCvImage);

//				waitKey(0);
            }
            else ;
        }
    }
    catch (GenICam::GenericException & e)   // エラーハンドリング
    {
        str = e.GetDescription();
        // 異常時はインスタンスを再生成
        if (m_pCamera->IsOpen()) {m_pCamera->Close();}
        if (m_pCamera != NULL) {delete m_pCamera; m_pCamera = NULL;}
    }

    return;
}

/// @brief 画像ファイル処理
/// @param
/// @return 
/// @note
void CComCamera::ImageProcStart(void)
{
//  g_pSharedObject->SetParam(PARAM_ID_IMG_GRAB_FILE, (UINT32)FALSE);

    string  fileName;
    Mat     fileData;
    g_pSharedObject->GetParam(PARAM_ID_STR_PROC_FILENAME, &fileName);

    fileData = imread(fileName);

    if (m_iBufferImg == IMAGE_ID_RAW_A)
    {
        g_pSharedObject->SetImage(IMAGE_ID_RAW_A, fileData);
        m_iBufferImg = IMAGE_ID_RAW_B;
    }
    else
    {
        g_pSharedObject->SetImage(IMAGE_ID_RAW_B, fileData);
        m_iBufferImg = IMAGE_ID_RAW_A;
    }
}
