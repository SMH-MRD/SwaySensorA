#include "CSharedObject.h"

/************************************/
/* �ϐ���`							*/
/************************************/
static STMngImageData m_stImage[IMAGE_ID_CAM_MAX];
static STMngProcData m_stProcImage[IMAGE_ID_PROC_MAX];
static STMngInclinoData m_stInclinoData[INCLINO_ID_MAX];
static UINT32 m_u32Param[PARAM_ID_MAX];
static string m_strParam[PARAM_ID_STR_MAX];
static DOUBLE m_dParam[PARAM_ID_DOUBLE_MAX];

static CRITICAL_SECTION csImage[IMAGE_ID_CAM_MAX];
static CRITICAL_SECTION csProcImage[IMAGE_ID_PROC_MAX];
static CRITICAL_SECTION csInclino[INCLINO_ID_MAX];
static CRITICAL_SECTION csParam[PARAM_ID_MAX];
static CRITICAL_SECTION csStrParam[PARAM_ID_STR_MAX];
static CRITICAL_SECTION csDoubleParam[PARAM_ID_DOUBLE_MAX];

/************************************/
/* �֐��v���g�^�C�v					*/
/************************************/
CSharedObject::CSharedObject() {}
CSharedObject::~CSharedObject() {}

void CSharedObject::InitSharedObject(void) {
	//���L�f�[�^�A�N�Z�X�p�N���e�B�J���Z�N�V�����̏�����
	for (UINT ii = 0; ii < IMAGE_ID_CAM_MAX; ii++) 	InitializeCriticalSection(&csImage[ii]);
	for (UINT ii = 0; ii < IMAGE_ID_PROC_MAX; ii++) InitializeCriticalSection(&csProcImage[ii]);
	for (UINT ii = 0; ii < INCLINO_ID_MAX; ii++)	InitializeCriticalSection(&csInclino[ii]);
	for (UINT ii = 0; ii < PARAM_ID_MAX; ii++)		InitializeCriticalSection(&csParam[ii]);
	for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++) 	InitializeCriticalSection(&csStrParam[ii]);
	for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX;ii++)InitializeCriticalSection(&csDoubleParam[ii]);
}

///# �֐�: �摜�f�[�^�ݒ菈�� ***************
INT CSharedObject::SetImage(UINT8 id, Mat image) {
	
	if (id >= IMAGE_ID_CAM_MAX) return RESULT_NG_INVALID;

	EnterCriticalSection(&csImage[id]);
		image.copyTo(m_stImage[id].image);
		m_stImage[id].update = TRUE;
	LeaveCriticalSection(&csImage[id]);

	return RESULT_OK;
}

///# �֐�: �摜�f�[�^�擾���� ***************
INT CSharedObject::GetImage(UINT8 id, Mat* image) {
	if (id >= IMAGE_ID_CAM_MAX) return RESULT_NG_INVALID;
	if (image == NULL)			return RESULT_NG_INVALID;
	/* �X�V�L���̊m�F */
	if (m_stImage[id].update == FALSE) 	return RESULT_NG_SEQUENCE;

	EnterCriticalSection(&csImage[id]);
		m_stImage[id].image.copyTo(*image);
		m_stImage[id].update = FALSE;
	LeaveCriticalSection(&csImage[id]);

	return RESULT_OK;
}

///# �֐�: �����摜�f�[�^�ݒ菈�� ***************
INT CSharedObject::SetProcImage(UINT8 id, STProcData data) {
	if (id >= IMAGE_ID_PROC_MAX) 	return RESULT_NG_INVALID;

	EnterCriticalSection(&csProcImage[id]);
		data.image.copyTo(m_stProcImage[id].data.image);
		m_stProcImage[id].data.posx = data.posx;
		m_stProcImage[id].data.posy = data.posy;
		m_stProcImage[id].update = TRUE;
	LeaveCriticalSection(&csProcImage[id]);

	return RESULT_OK;
}

///# �֐�: �����摜�f�[�^�擾���� ***************
INT CSharedObject::GetProcImage(UINT8 id, STProcData* data) {
	if (id >= IMAGE_ID_PROC_MAX)	return RESULT_NG_INVALID;
	if (data == NULL)				return RESULT_NG_INVALID;
	/* �X�V�L���̊m�F */
	if (m_stProcImage[id].update == FALSE)	return RESULT_NG_SEQUENCE;

	EnterCriticalSection(&csProcImage[id]);
		m_stProcImage[id].data.image.copyTo(data->image);
		data->posx = m_stProcImage[id].data.posx;
		data->posy = m_stProcImage[id].data.posy;
		m_stProcImage[id].update = FALSE;
	LeaveCriticalSection(&csProcImage[id]);

	return RESULT_OK;
}

///# �֐�: �X�Όv�f�[�^�ݒ菈�� ***************
INT CSharedObject::SetInclinoData(UINT8 id, DOUBLE data) {
	if (id >= INCLINO_ID_MAX) return RESULT_NG_INVALID;

	EnterCriticalSection(&csInclino[id]);
		m_stInclinoData[id].data = data;
	LeaveCriticalSection(&csInclino[id]);

	return RESULT_OK;
}

///# �֐�: �X�Όv�f�[�^�擾���� ***************
INT CSharedObject::GetInclinoData(UINT8 id, DOUBLE* data) {
	if (id >= INCLINO_ID_MAX)	return RESULT_NG_INVALID;
	if (data == NULL)			return RESULT_NG_INVALID;

	EnterCriticalSection(&csInclino[id]);
		*data = m_stInclinoData[id].data;
	LeaveCriticalSection(&csInclino[id]);

	return RESULT_OK;
}

///# �֐�:  UINT32�p�����[�^�f�[�^�ݒ菈�� ***************
INT CSharedObject::SetParam(UINT8 id, UINT32 data) {
	if (id >= PARAM_ID_MAX) return RESULT_NG_INVALID;

	EnterCriticalSection(&csParam[id]);
		m_u32Param[id] = data;
	LeaveCriticalSection(&csParam[id]);

	return RESULT_OK;
}

///# �֐�:  UINT32�p�����[�^�f�[�^�擾���� ***************
INT CSharedObject::GetParam(UINT8 id, UINT32* data) {
	if (id >= PARAM_ID_MAX) return RESULT_NG_INVALID;
	if (data == NULL)		return RESULT_NG_INVALID;

	EnterCriticalSection(&csParam[id]);
		*data = m_u32Param[id];
	LeaveCriticalSection(&csParam[id]);

	return RESULT_OK;
}

///# �֐�: string�p�����[�^�f�[�^�ݒ菈�� ***************
INT CSharedObject::SetParam(UINT8 id, string str) {
	if (id >= PARAM_ID_MAX) return RESULT_NG_INVALID;

	EnterCriticalSection(&csStrParam[id]);
		m_strParam[id] = str;
	LeaveCriticalSection(&csStrParam[id]);

	return RESULT_OK;
}

///# �֐�: string�p�����[�^�f�[�^�擾���� ***************
INT CSharedObject::GetParam(UINT8 id, string* str) {
	if (id >= PARAM_ID_MAX) return RESULT_NG_INVALID;
	if (str == NULL) 		return RESULT_NG_INVALID;

	EnterCriticalSection(&csStrParam[id]);
		*str = m_strParam[id];
	LeaveCriticalSection(&csStrParam[id]);

	return RESULT_OK;
}

///# �֐�: DOUBLE�p�����[�^�f�[�^�ݒ菈�� ***************
INT CSharedObject::SetParam(UINT8 id, DOUBLE data) {
	if (id >= PARAM_ID_MAX) return RESULT_NG_INVALID;

	EnterCriticalSection(&csDoubleParam[id]);
		m_dParam[id] = data;
	LeaveCriticalSection(&csDoubleParam[id]);

	return RESULT_OK;
}

///# �֐�: DOUBLE�p�����[�^�f�[�^�擾���� ***************
INT CSharedObject::GetParam(UINT8 id, DOUBLE* data) {
	if (id >= PARAM_ID_MAX) return RESULT_NG_INVALID;
	if (data == NULL) 		return RESULT_NG_INVALID;

	EnterCriticalSection(&csDoubleParam[id]);
		*data = m_dParam[id];
	LeaveCriticalSection(&csDoubleParam[id]);

	return RESULT_OK;
}
