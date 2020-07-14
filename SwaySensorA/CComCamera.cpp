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
    if (m_camera.IsGrabbing())  {m_camera.StopGrabbing();}
    if (m_camera.IsOpen())      {m_camera.Close();}    
    PylonTerminate();
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::init_task(void *pobj)
{
    PylonInitialize();
    m_bOpenCmare = FALSE;
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

    if (camProc)
    {
        //----------------------------------------------------------------------------
        GrabImage();
    }
    else if (imageProc)
    {
        //----------------------------------------------------------------------------
        ImageProcStart();
    }
    else ;

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages)
{
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
{
    // NOTE: 
    // This function is called from the CInstantCamera grab thread.
    // You shouldn't perform lengthy operations here.
    // Also, you shouldn't access any UI objects, since we are not in the GUI thread.

    //----------------------------------------------------------------------------
    // 処理時間計測(終了時間取得)
    static LARGE_INTEGER   start;
    LARGE_INTEGER   frequency;

    LARGE_INTEGER   end;
    LONGLONG        span, usec;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&end);
    span = end.QuadPart - start.QuadPart;
    usec = (span * 1000000L) / frequency.QuadPart;
    g_pSharedObject->SetParam(PARAM_ID_DOUBLE_IMG_GRAB_TIME, (DOUBLE)usec / 1000.0);
    start = end;

    if (g_pSharedObject == NULL) {return;}
    try
    {
        Pylon::CImageFormatConverter    converter;      // First the image format converter class must be created.
        Pylon::CPylonImage              pylonImage;     // Create a PylonImage that will be used to reate OpenCV images later.
        cv::Mat                         opencvImage;    // Create an OpenCV image.

        // Image grabbed successfully?
        if (ptrGrabResult->GrabSucceeded())
        {
            // Convert the grabbed buffer to a pylon image.
            converter.OutputPixelFormat = PixelType_BGR8packed;   // Specify the output pixcel format.
            converter.Convert(pylonImage, ptrGrabResult);

            // Create an OpenCV image from a pylon image.
            opencvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
            if (m_iBufferImg == IMAGE_ID_RAW_A)
            {
                g_pSharedObject->SetImage(IMAGE_ID_RAW_A, opencvImage);
                m_iBufferImg = IMAGE_ID_RAW_B;
            }
            else
            {
                g_pSharedObject->SetImage(IMAGE_ID_RAW_B, opencvImage);
                m_iBufferImg = IMAGE_ID_RAW_A;
            }
        }   // if (ptrGrabResult->GrabSucceeded())
    }
    catch (GenICam::GenericException & e)   // エラーハンドリング
    {
        str = e.GetDescription();
        // 異常時はインスタンスを再生成
        if (m_camera.IsOpen()) {m_camera.Close();}
    }
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnAttach(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnAttach");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnAttached(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnAttached");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnDetach(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnDetach");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnDetached(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnDetached");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnDestroy(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnDestroy");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnDestroyed(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnDestroyed");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnOpen(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnOpen");
    // Remember the camera properties that allow detecting the same camera again.
    m_info.SetDeviceClass(camera.GetDeviceInfo().GetDeviceClass());
    m_info.SetSerialNumber(camera.GetDeviceInfo().GetSerialNumber());
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnOpened(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnOpened");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnClose(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnClose");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnClosed(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnClosed");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnGrabStart(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnGrabStart");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnGrabStarted(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnGrabStarted");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnGrabStop(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnGrabStop");
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnGrabStopped(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnGrabStopped");
    // Deregister all configurations.
    m_camera.DeregisterConfiguration(&m_continousConfiguration);
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnGrabError(Pylon::CInstantCamera& camera, const char* errorMessage)
{
    _RPT1(_CRT_WARN, "%s\n", "OnGrabError");
    m_bOpenCmare = FALSE;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::OnCameraDeviceRemoved(Pylon::CInstantCamera& camera)
{
    _RPT1(_CRT_WARN, "%s\n", "OnCameraDeviceRemoved");
    try
    {
        // Destroy the Pylon Device representing the detached camera device.
        // It can't be used anymore.
        m_camera.DestroyDevice();
        m_bOpenCmare = FALSE;
    }
    catch (Pylon::GenericException)
    {
    }
}

/// @brief 
/// @param
/// @return 
/// @note
void CComCamera::GrabImage(void)
{
    try
    {
        if (!m_camera.IsOpen())
        {
            // Get the transport layer factory.
            Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();

            // Get all attached devices and return function if no device is found.
            Pylon::DeviceInfoList_t devices;
            if (tlFactory.EnumerateDevices(devices) == 0)   {return;}

            // Create an instant camera object with the camera device found first.
            Pylon::IPylonDevice* pDevice = Pylon::CTlFactory::GetInstance().CreateFirstDevice();
            m_camera.Attach(tlFactory.CreateFirstDevice(), Pylon::Cleanup_Delete);

            // Register this object as an image event handler, so we will be notified of new new images
            // See Pylon::CImageEventHandler for details
            m_camera.RegisterImageEventHandler(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);
            // Register this object as a configuration event handler, so we will be notified of camera state changes.
            // See Pylon::CConfigurationEventHandler for details
            m_camera.RegisterConfiguration(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);

            // Open the camera. Camera device removal is only detected while the camera is open.
            m_camera.Open();
 
            // Width, Height
            UINT32 width, height;
            g_pSharedObject->GetParam(PARAM_ID_CAM_WIDTH, &width);
            m_camera.Width.SetValue(width);
            g_pSharedObject->GetParam(PARAM_ID_CAM_HEIGHT, &height);
            m_camera.Height.SetValue(height);

            // Set the pixel data format.
            m_camera.PixelFormat.SetValue(PixelFormat_BGR8);

            // Acquisition Frame Rate
            UINT32 setFrameRate;
            g_pSharedObject->GetParam(PARAM_ID_CAM_FRAMERATE, &setFrameRate);
            m_camera.AcquisitionFrameRateEnable.SetValue(true);
            m_camera.AcquisitionFrameRate.SetValue((double)setFrameRate);

            // Exposure
            UINT32 exposure;
            g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &exposure);
            m_camera.ExposureTime.SetValue((double)exposure);
        
            // Reconfigure the camera to use continuous acquisition.
            m_continousConfiguration.OnOpened(m_camera);

            // Start grabbing until StopGrabbing() is called.
//          m_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
            m_camera.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly, Pylon::GrabLoop_ProvidedByInstantCamera);

            m_bOpenCmare = TRUE;
        }
        else
        {
            // Exposure
            UINT32 exposure;
            g_pSharedObject->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &exposure);
            m_camera.ExposureTime.SetValue((double)exposure);
        }
        return;
    }
    catch (GenICam::GenericException)   // エラーハンドリング
    {
        return ;
    }
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
