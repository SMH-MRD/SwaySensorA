#pragma once
#include "CTaskObj.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define S_MAX           255
#define S_MIN           50
#define V_MAX           255
#define V_MIN           50

#define BEVEL_MAX_ANGLE 30.0

class CAnalyst : public CTaskObj
{
public:
    CAnalyst();
    ~CAnalyst();

    void routine_work(void* param);
    void init_task(void* pobj);

private:
    BOOL m_bSaveImageArea;

    void ImageProc(void);
    void InclinationProc(void);
    void CalcCenterOfGravity1(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
    void CalcCenterOfGravity2(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
};
