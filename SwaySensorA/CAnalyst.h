#pragma once
#include "CTaskObj.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define IMAGE_ROI_SIZE      400
#define INCLINATION_MAX     30.0

class CAnalyst : public CTaskObj
{
public:
    CAnalyst();
    ~CAnalyst();

    void routine_work(void* param);
    void init_task(void* pobj);

private:
    UINT8 m_iBufferImgMask1;
    UINT8 m_iBufferImgMask2;
    UINT8 m_iBufferImgProc;

    void ImageProc(void);
    void InclinationProc(void);
    BOOL CalcCenterOfGravity1(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
    BOOL CalcCenterOfGravity2(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
};
