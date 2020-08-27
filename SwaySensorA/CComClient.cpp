#include "CComClient.h"
#include "CSharedObject.h"

extern CSharedObject* g_pSharedObject;  // タスク間共有データのポインタ

/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CComClient::CComClient()
{
    m_socksend     = NULL;
    m_sockrecv     = NULL;
    m_socksendstat = SOCK_STATUS_CLOSE;
    m_sockrecvstat = SOCK_STATUS_CLOSE;
}

/// @brief デストラクタ
/// @param
/// @return 
/// @note
CComClient::~CComClient()
{
    Close();
}

/// @brief 
/// @param
/// @return 
/// @note
void CComClient::init_task(void *pobj)
{
    Initialize();
}

/// @brief タスクスレッドでの処理関数
/// @param
/// @return 
/// @note
void CComClient::routine_work(void *param)
{
//  if (g_pSharedObject == NULL) {return;}
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();

    //----------------------------------------------------------------------------
    Proc();

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComClient::Initialize(void)
{
    m_socksend = new CSockUDPLib();
    m_socksend->Initialize(SOCK_TYPE_SEND, (char*)"192.168.127.100", 12000);

    m_sockrecv = new CSockUDPLib();
    m_sockrecv->Initialize(SOCK_TYPE_RECV, (char*)"192.168.127.200", 12000);

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComClient::Close(void)
{
    if (m_socksend != NULL) {m_socksend->Close();}
    if (m_sockrecv != NULL) {m_sockrecv->Close();}
    return;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComClient::Proc(void)
{
    int  ret;
    char recvbuf[128];
    char sendbuf[128];
    int  size;
    int  err;

    //--------------------------------------------------------------------------
    // データ送信
    if (m_socksend != NULL)
    {
        if ((m_socksendstat =  m_socksend->GetSockStatus()) != SOCK_STATUS_OPEN)
        {
            m_socksend->Open();
        }
        if ((m_socksendstat =  m_socksend->GetSockStatus()) == SOCK_STATUS_OPEN)
        {
            sendbuf[0] = 1;
            size = m_socksend->SendData(sendbuf, sizeof(sendbuf), &err);
            if (err != 0)
            {
                m_socksend->Close();
            }
        }        
    }

    //--------------------------------------------------------------------------
    // データ受信
    if (m_sockrecv != NULL)
    {
        if ((m_sockrecvstat =  m_sockrecv->GetSockStatus()) != SOCK_STATUS_OPEN)
        {
            m_sockrecv->Open(SOCK_NONBLOCKING);
        }
        if ((m_socksendstat =  m_socksend->GetSockStatus()) == SOCK_STATUS_OPEN)
        {
            ZeroMemory(recvbuf, sizeof(recvbuf));
            size = m_sockrecv->RecvData(recvbuf, sizeof(recvbuf), &err);
            if (err != 0)
            {
                if (err != WSAEWOULDBLOCK) {m_sockrecv->Close();}
            }
            else
            {
                ;   // 受信処理
            }
        }        
    }

    return;
}