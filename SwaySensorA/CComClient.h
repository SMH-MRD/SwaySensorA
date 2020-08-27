#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"
#include "SockUDP.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

class CComClient : public CTaskObj
{
public:
    CComClient();
    ~CComClient();

    void init_task(void* pobj);
    void routine_work(void* param);

private:
    CSockUDPLib* m_socksend;
    CSockUDPLib* m_sockrecv;
    int          m_socksendstat;
    int          m_sockrecvstat;

private:
    void Initialize(void);
    void Close(void);
    void Proc(void);
};
