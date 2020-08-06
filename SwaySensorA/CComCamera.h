#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define saveImages  0
#define recordVideo 0

class CComCamera :
    public CTaskObj,
    public Pylon::CImageEventHandler,           // Allows you to get informed about received images and grab errors.
    public Pylon::CConfigurationEventHandler    // Allows you to get informed about device removal.
{
public:
    CComCamera();
    ~CComCamera();

    void init_task(void* pobj);
    void routine_work(void* param);

// Pylon::CImageEventHandler functions
    virtual void OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages);
    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult);

// Pylon::CConfigurationEventHandler functions
    virtual void OnAttach(Pylon::CInstantCamera& camera);
    virtual void OnAttached(Pylon::CInstantCamera& camera);
    virtual void OnDetach(Pylon::CInstantCamera& camera);
    virtual void OnDetached(Pylon::CInstantCamera& camera);
    virtual void OnDestroy(Pylon::CInstantCamera& camera);
    virtual void OnDestroyed(Pylon::CInstantCamera& camera);
    virtual void OnOpen(Pylon::CInstantCamera& camera);
    virtual void OnOpened(Pylon::CInstantCamera& camera);
    virtual void OnClose(Pylon::CInstantCamera& camera);
    virtual void OnClosed(Pylon::CInstantCamera& camera);
    virtual void OnGrabStart(Pylon::CInstantCamera& camera);
    virtual void OnGrabStarted(Pylon::CInstantCamera& camera);
    virtual void OnGrabStop(Pylon::CInstantCamera& camera);
    virtual void OnGrabStopped(Pylon::CInstantCamera& camera);
    virtual void OnGrabError(Pylon::CInstantCamera& camera, const char* errorMessage);
    virtual void OnCameraDeviceRemoved(Pylon::CInstantCamera& camera);

private:
    Pylon::CBaslerUniversalInstantCamera   m_camera;
    Pylon::CAcquireContinuousConfiguration m_continousConfiguration;
    Pylon::CDeviceInfo                     m_info;
    Pylon::CGrabResultPtr                  m_ptrGrabResult;

    stCameraParamData m_camparam;           // カメラ設定データ
    stCameraInfoData  m_caminfo;            // カメラ情報データ
    UINT8             m_iBufferImg;
    CRITICAL_SECTION  m_csImageGrab;

    LARGE_INTEGER     m_iGrabStartPC;

private:
    void Initialize(void);
    void Close(void);
    void GrabImage(void);
    void ProcImage(void);
};
