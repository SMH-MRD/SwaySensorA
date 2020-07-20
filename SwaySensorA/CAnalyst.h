#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask         g_itask;

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
    
    stProcInfo m_stProcInfo;

private:
    void ImageProc(void);
    BOOL CalcCenterOfGravity(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY, UINT32* outroisize, UINT32 sel);
};
