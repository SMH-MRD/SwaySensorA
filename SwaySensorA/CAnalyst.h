#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

#define PI              3.14159                 // π
#define CONV_DEG_RAD    PI/180.0                // deg→rad

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

    stCameraParamData   m_camparam;         // カメラ設定データ
    stConfigParamData   m_cnfgparam;        // 構造設定データ
    stImgProcParamData  m_imgprocparam;     // 画像処理設定データ
    stProcInfoData      m_proninfo;         // 画像処理結果

private:
    void ImageProc(void);
    BOOL CalcCenterOfGravity(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY, int* outTgtSize, UINT32 sel);
    void SwayProc(void);
};
