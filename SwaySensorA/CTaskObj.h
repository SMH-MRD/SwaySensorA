#pragma once
#include "framework.h"
#include "AppCommon.h"
#include <process.h>    // threads and processes�֐��w�b�_

using namespace std;

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask         g_itask;

// #�^�X�N�����ݒ蕔
#define TARGET_RESOLUTION       1       // �}���`���f�B�A�^�C�}�[���x
#define SYSTEM_TICK_ms          25      // �^�X�N�X���b�h�Ăяo�������P�ʁ@���̔{���̎����ƂȂ�
#define MAX_APL_TASK            9       // �o�^�\�^�X�N�ő吔
#define INITIAL_TASK_STACK_SIZE 16384
#define DEFAUT_TASK_CYCLE       50      // �^�X�N�X���b�h�Ăяo���f�t�H���g����
#define TASK_EVENT_MAX          8       // �e�^�X�N�X���b�h�Ăяo���C�x���g�ő吔

// inf.thread_com�p�@�X���b�h���[�v����p
#define REPEAT_INFINIT      0   // �i�v���[�v
#define TERMINATE_THREAD    1   // �X���b�h���f
#define REPEAT_ONCE         -1  // ��������

// ������N���X���b�h�Ŏ��s����֐��̓��e�I��
#define THREAD_WORK_IDLE    0   // �A�C�h�������i���������j
#define THREAD_WORK_ROUTINE 1   // �ʏ폈��
#define THREAD_WORK_OPTION1 2   // �I�v�V��������
#define THREAD_WORK_OPTION2 3   // �I�v�V��������

#define ID_TIMER_EVENT  0   // �^�C�}�[�p�C�x���g�z��̈ʒu
#define ID_OPT1_EVENT   1   // �I�v�V�����C�x���g�z��̈ʒu

#define MSG_LIST_MAX    14  // ���C���E�B���h�E���X�g�{�b�N�X�\���s��

// �^�X�N�I�u�W�F�N�g�̌ʊǗ����\����
typedef struct
{
    // -�I�u�W�F�N�g���ʏ��
    TCHAR   name[24];   // �I�u�W�F�N�g��
    TCHAR   sname[8];   // �I�u�W�F�N�g����
    HBITMAP hBmp;       // �I�u�W�F�N�g���ʗp�r�b�g�}�b�v

    // -�X���b�h�ݒ���e
    int             index;                      // �X���b�hIndex
    unsigned int    ID;                         // �X���b�hID
    HANDLE          hndl;                       // �X���b�h�n���h��
    HANDLE          hevents[TASK_EVENT_MAX];    // �C�x���g�n���h��
    int             n_active_events = 1;        // �L���ȃC�x���g��
    int             event_triggered;            // ���������C�x���g�̔ԍ�
    unsigned int    cycle_ms;                   // �X���b�h���s�ݒ����
    unsigned int    cycle_count;                // �X���b�h���s�ݒ�����@Tick count�ims/system tick)
    int             trigger_type;               // �X���b�h�N�������@�����or�C�x���g
    int             priority;                   // �X���b�h�̃v���C�I���e�B
    int             thread_com;                 // �X���b�h����t���O �@0:�J��Ԃ��@1:���f�@����ȊO�F��������:

    // -�X���b�h���j�^���
    DWORD           start_time;         // ���X�L�����̃X���b�h�J�n����
    DWORD           act_time;           // 1�X�L�����̃X���b�h����������
    DWORD           period;             // �X���b�h���[�v��������
    DWORD           act_count;          // �X���b�h���[�v�J�E���^
    DWORD           total_act;          // �N���ώZ�J�E���^
    unsigned int    time_over_count;    // �\��������I�[�o�[������

    // -�֘A�E�B���h�E�n���h��
    HWND        hWnd_parent;        // �e�E�B���h�E�̃n���h��
    HWND        hWnd_msgStatics;    // �e�E�B���h�E���b�Z�[�W�\���p�E�B���h�E�ւ̃n���h��
    HWND        hWnd_opepane;       // �����C���E�B���h�E�̃n���h���i���C���t���[����ɔz�u�j
    HWND        hWnd_msgList;       // �����C���E�B���h�E�̃��b�Z�[�W�\���p���X�g�R���g���[���ւ̃n���h��
    HWND        hWnd_work;          // ����p��Ɨp�E�B���h�E�̃n���h��
    HINSTANCE   hInstance;          // �A�v���P�[�V�����̃C���X�^���X

    // -����p�l���֘A
    int cnt_PNLlist_msg = 0;    // �p�l�����b�Z�[�W���X�g�̃J�E���g��
    int panel_func_id = 1;      // �p�l��function�{�^���̑I����e
    int panel_type_id = 1;      // �p�l��type�{�^���̑I����e

    // -�O���C���^�[�t�F�[�X
    unsigned long*  psys_counter;       // ���C���V�X�e���J�E���^�[�̎Q�Ɛ�|�C���^
    unsigned        work_select = 0;    // �X���b�h���s�̊֐��̎�ނ��w��
} ST_THREAD_INFO, *PST_THREAD_INFO;

// �^�X�N�X���b�h�I�u�W�F�N�g�x�[�X�N���X
class CTaskObj
{
public:
    ST_THREAD_INFO inf;

    CTaskObj();
    virtual ~CTaskObj();

    unsigned __stdcall run(void *param);    // �X���b�h���s�Ώۊ֐�

    virtual void init_task(void *pobj);
    virtual void routine_work(void *param);

    //�^�X�N�ݒ�E�B���h�E�p���\�b�h
    virtual LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    virtual void set_panel_tip_txt();   // �^�u�p�l���̐����pStatic�e�L�X�g��ݒ�
    virtual void set_panel_pb_txt() {return;}   // �^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6);    // �p�����[�^�����\���l�ݒ�

protected:
    ostringstream   s;
    wostringstream  ws;
    wstring         wstr;
    string          str;

    void tweet2owner(const std::string &src);       // ���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\��
    void tweet2owner(const std::wstring &srcw);     // ���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\��
    void txout2msg_listbox(const std::wstring str); // ���X�g�R���g���[���ւ̃V���[�g���b�Z�[�W�\��
    void txout2msg_listbox(const std::string str);  // ���X�g�R���g���[���ւ̃V���[�g���b�Z�[�W�\��
};
