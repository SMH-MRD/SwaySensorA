#include "CTaskObj.h"

//# �}���`�X���b�h��{�N���X �����o�[�֐� 

CTaskObj::CTaskObj() {//�R���X�g���N�^
	//�^�X�N�֘A�̏����l�ݒ�
	inf.hndl = NULL;						//�X���b�h�n���h��
	inf.cycle_ms = DEFAUT_TASK_CYCLE;		//�X���b�h�̌Ăяo������
	inf.thread_com = REPEAT_INFINIT;		//�X���b�h�̌Ăяo���^�C�v
	wsprintf(inf.name, L"Who am I?");		//�^�X�N��
	inf.act_count = inf.act_time = 0;		//�X���b�h��������p�J�E���^���Z�b�g
	inf.priority = THREAD_PRIORITY_NORMAL;	//�^�X�N�v���C�I���e�B�ݒ�i�����p�j
	inf.work_select = THREAD_WORK_IDLE;		//�X���b�h���s���̏����֐�
	inf.hWnd_parent = inf.hWnd_work = inf.hWnd_opepane = inf.hWnd_msgStatics = NULL;//�e��\���֘A�n���h��
	inf.hBmp = NULL;						//�^�X�N�A�C�R���p�r�b�g�}�b�v�n���h��
};

CTaskObj::~CTaskObj() {

	delete inf.hBmp;
};//�f�X�g���N�^

void CTaskObj::init_task(void *pobj) {
	return;
};

//# �X���b�h���C���֐�
unsigned CTaskObj::run(void *param) {

	while (this->inf.thread_com != TERMINATE_THREAD) {
		inf.event_triggered = WaitForMultipleObjects(inf.n_active_events, ((CTaskObj*)param)->inf.hevents, FALSE, INFINITE);//���C���X���b�h�����SIGNAL��ԑ҂�

		//���������m�F�p
		DWORD start_time = timeGetTime();
		inf.period = start_time - inf.start_time;
		inf.start_time = start_time;

		routine_work(param);

		inf.act_time = timeGetTime() - start_time;//���������ԑ���p
		if (this->inf.thread_com != REPEAT_INFINIT) break;//���������̏ꍇ�͔�����
	}
	return 0;
}

//# �^�X�N�X���b�h�ł̏����֐�
void CTaskObj::routine_work(void *param) {
	return;
};
