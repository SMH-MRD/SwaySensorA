#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask         g_itask;

#define PI              3.14159                 // ��
#define CONV_DEG_RAD    (PI/180.0)              // deg��rad

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

    stCameraParamData   m_camparam;         // �J�����ݒ�f�[�^
    stCommonParamData   m_cmmnparam;        // �\���ݒ�f�[�^
    stImgProcParamData  m_imgprocparam;     // �摜�����ݒ�f�[�^
    stProcInfoData      m_proninfo;         // �摜��������

private:
    void ImageProc(void);
    BOOL CalcCenterOfGravity(vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY, int* outTgtSize, UINT32 sel);
    void SwayProc(void);
};
