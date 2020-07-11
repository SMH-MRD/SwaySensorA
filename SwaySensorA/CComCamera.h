#pragma once
#include "CTaskObj.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define saveImages  0
#define recordVideo 0

class CComCamera : public CTaskObj
{
public:
    CComCamera();
    ~CComCamera();

    void init_task(void* pobj);
    void routine_work(void* param);

private:
    CBaslerUniversalInstantCamera*  m_pCamera;
    const UINT32                    c_countOfImagesToGrab = 1;
    VideoWriter                     cvVideoCreator;
    UINT8                           m_iBufferImg;

    BOOL OpenCamera(void);
    void StartGrabImage(void);

    void CaptureImage(void);
    void ImageProcStart(void);
};
