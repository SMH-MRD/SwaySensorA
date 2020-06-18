#pragma once
#include "CTaskObj.h"

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
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
    BOOL m_bSaveImageArea;

    void ImageProc(void);
    void InclinationProc(void);
    void CalcCenterOfGravity1(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
    void CalcCenterOfGravity2(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
};
