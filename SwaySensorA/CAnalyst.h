#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

class CAnalyst : public CTaskObj
{
public:
    CAnalyst();
    ~CAnalyst();

    void routine_work(void* param);
    void init_task(void* pobj);

private:
    UINT8 m_iBufferImgMask[IMGPROC_ID_MAX];
    UINT8 m_iBufferImgProc;

    stImgProcParamData  m_imgprocparam;     // 画像処理設定データ
    stProcInfoData      m_proninfo;         // 画像処理結果

private:
    void ImageProc(void);
    BOOL CalcCenterOfGravity(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY, int* outTgtSize, UINT32 sel);
};
