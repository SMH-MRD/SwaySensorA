#pragma once
#include "CTaskObj.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define BEVEL_MAX_ANGLE 30.0

class CAnalyst : public CTaskObj
{
public:
    CAnalyst();
    ~CAnalyst();

    void routine_work(void* param);
    void init_task(void* pobj);

private:
//    BOOL m_bSaveImgMask;
//    BOOL m_bSaveImgProc;
    UINT8 m_iBufferImgMask1;
    UINT8 m_iBufferImgProc1;
    UINT8 m_iBufferImgMask2;
    UINT8 m_iBufferImgProc2;

    void ImageProc(void);
    void InclinationProc(void);
    BOOL CalcCenterOfGravity1(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
    BOOL CalcCenterOfGravity2(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
};
