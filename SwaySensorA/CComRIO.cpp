#include "CComRIO.h"

extern CSharedObject*   g_pSharedObject;    // �^�X�N�ԋ��L�f�[�^�̃|�C���^

/// @brief �R���X�g���N�^
/// @param
/// @return 
/// @note
CComRIO::CComRIO()
{
}

/// @brief �f�X�g���N�^
/// @param
/// @return 
/// @note
CComRIO::~CComRIO()
{
}

/// @brief 
/// @param
/// @return 
/// @note
void CComRIO::init_task(void *pobj)
{
    Initialize();
}

/// @brief �^�X�N�X���b�h�ł̏����֐�
/// @param
/// @return 
/// @note
void CComRIO::routine_work(void *param)
{
//  if (g_pSharedObject == NULL) {return;}
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();

    //----------------------------------------------------------------------------
    ProcRIO();

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComRIO::Initialize(void)
{
    //--------------------------------------------------------------------------
    // ModbusTCP�ݒ�l
    string ipAddr;
    g_pSharedObject->GetParam(PARAM_ID_STR_RIO_IPADDR, &ipAddr);    memcpy(m_stRIOCnfg.modbusInitCnfg.ipaddrs, ipAddr.c_str(), ipAddr.length());
    g_pSharedObject->GetParam(PARAM_ID_RIO_TCPPORT,    (UINT32*)&m_stRIOCnfg.modbusInitCnfg.portnum);
    g_pSharedObject->GetParam(PARAM_ID_RIO_TIMEOUT,    (UINT32*)&m_stRIOCnfg.modbusInitCnfg.timeout);

	int32_t slaveaddrs;
    UINT32  portX, portY;
    g_pSharedObject->GetParam(PARAM_ID_RIO_SLAVEADDR, (UINT32*)&slaveaddrs);
    g_pSharedObject->GetParam(PARAM_ID_RIO_XPORT,     &portX);
    g_pSharedObject->GetParam(PARAM_ID_RIO_YPORT,     &portY);

    //--------------------------------------------------------------------------
    // Command��������
    m_stRIOCnfg.stModbusTcpSetCmndReq.slaveAddr = slaveaddrs;
    m_stRIOCnfg.stModbusTcpSetCmndReq.funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_stRIOCnfg.stModbusTcpSetCmndReq.regAddr   = RIO_COMMAND_REGISTER;
    m_stRIOCnfg.stModbusTcpSetCmndReq.dataCnt   = 1;
    m_stRIOCnfg.stModbusTcpSetCmndReq.option    = 0;

    //--------------------------------------------------------------------------
    // IO-Link��������
    // PORT1
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].slaveAddr = slaveaddrs;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].regAddr   = m_stRegTable[portX].portMode;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].dataCnt   = 1;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].option    = 0;
    // PORT2
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].slaveAddr = slaveaddrs;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].regAddr   = m_stRegTable[portY].portMode;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].dataCnt   = 1;
    m_stRIOCnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].option    = 0;

    //--------------------------------------------------------------------------
    // �|�[�g��������
    // PORT1
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_1].slaveAddr = slaveaddrs;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_1].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_1].regAddr   = m_stRegTable[portX].outAddr;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_1].dataCnt   = 1;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_1].option    = 0;
    // PORT2
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_2].slaveAddr = slaveaddrs;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_2].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_2].regAddr   = m_stRegTable[portY].outAddr;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_2].dataCnt   = 1;
    m_stRIOCnfg.stModbusTcpSetPortReq[RIO_PORT_2].option    = 0;

   //--------------------------------------------------------------------------
    // �|�[�g�ǂݏo��
    // PORT1
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_1].slaveAddr = slaveaddrs;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_1].funcCode  = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_1].regAddr   = m_stRegTable[portX].inAddr;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_1].dataCnt   = 1;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_1].option    = 0;
    // PORT2
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_2].slaveAddr = slaveaddrs;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_2].funcCode  = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_2].regAddr   = m_stRegTable[portY].inAddr;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_2].dataCnt   = 1;
    m_stRIOCnfg.stModbusTcpGetPortReq[RIO_PORT_2].option    = 0;

    //--------------------------------------------------------------------------
    // RIO��񏉊���
    m_stRIOInfo.error = RIO_ERR_INIT_INCOMPLETE;    // RIO������������
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        m_stRIOInfo.data[ii].dig = 0;   // ���̓f�[�^
        m_stRIOInfo.data[ii].cur = 0.0; // ���̓f�[�^�ϊ��l(mA)
        m_stRIOInfo.data[ii].deg = 0.0; // ���̓f�[�^�ϊ��l(deg.)    
    }
    g_pSharedObject->SetRIOInfo(m_stRIOInfo);

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComRIO::Close(void)
{
    modtClose(m_stRIOCnfg.modbusDesc);
}

/// @brief 
/// @param
/// @return 
/// @note
int32_t CComRIO::InitializeRIO(void)
{
    int32_t err;

    //--------------------------------------------------------------------------
    // ModbusTCP������
    m_stRIOCnfg.modbusDesc = modtInit((const int8_t*)m_stRIOCnfg.modbusInitCnfg.ipaddrs, m_stRIOCnfg.modbusInitCnfg.portnum, m_stRIOCnfg.modbusInitCnfg.timeout);
    if (m_stRIOCnfg.modbusDesc == NULL) {return RIO_ERR_INIT_INCOMPLETE;}

	int32_t slaveaddrs;
    UINT32  xPort, yPort;
    g_pSharedObject->GetParam(PARAM_ID_RIO_SLAVEADDR, (UINT32*)&slaveaddrs);
    g_pSharedObject->GetParam(PARAM_ID_RIO_XPORT,     &xPort);
    g_pSharedObject->GetParam(PARAM_ID_RIO_YPORT,     &yPort);
    //--------------------------------------------------------------------------
    // IO LINK MODE�ݒ�
    m_stRIOCnfg.setdata.uint16 = RIO_PORT_REGISTER_MODE_IOLINK;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        if ((err = modtSetdata(m_stRIOCnfg.modbusDesc, m_stRIOCnfg.stModbusTcpSetIOLinkReq[ii], m_stRIOCnfg.setdata.uint8)) != 0)
        {
            return RIO_ERR_SET_IOLINKMODE;
        }
    }

    //--------------------------------------------------------------------------
    // �p�����[�^�ݒ�l�L����
    m_stRIOCnfg.setdata.uint16 = RIO_COMMAND_AI_PORT_ACTIVE;
    if ((err = modtSetdata(m_stRIOCnfg.modbusDesc, m_stRIOCnfg.stModbusTcpSetCmndReq, m_stRIOCnfg.setdata.uint8)))
    {
        return RIO_ERR_SET_PARAM_VALID;
    }

    //--------------------------------------------------------------------------
    // AI�p�����[�^�ݒ�
    m_stRIOCnfg.setdata.uint16 = RIO_COMMAND_AI_PARA_SET;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        if ((err = modtSetdata(m_stRIOCnfg.modbusDesc, m_stRIOCnfg.stModbusTcpSetPortReq[ii], m_stRIOCnfg.setdata.uint8)))
        {
            return RIO_ERR_SET_PARAM_AI;
        }
    }

    return RIO_ERR_NONE;
}

/// @brief �X�Όv�f�[�^����
/// @param
/// @return 
/// @note
void CComRIO::ProcRIO(void)
{
    //--------------------------------------------------------------------------
    // ������
    if (m_stRIOInfo.error != RIO_ERR_NONE) {m_stRIOInfo.error = InitializeRIO();}

    //--------------------------------------------------------------------------
    // �f�[�^�ǂݏo��
    if (m_stRIOInfo.error == RIO_ERR_NONE)
    {
        int32_t err = 0;
        for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
        {
            // �f�[�^�ǂݏo��
            if ((err =  modtGetdata(m_stRIOCnfg.modbusDesc, m_stRIOCnfg.stModbusTcpGetPortReq[ii], (uint8_t *)m_stRIOCnfg.getdata.uint8)))
            {
                m_stRIOInfo.error |= (RIO_ERR_GET_AI_READ << ii);
                m_stRIOInfo.data[ii].dig = 0;       // ���̓f�[�^
                m_stRIOInfo.data[ii].cur = 0.0;     // ���̓f�[�^�ϊ��l(mA)
                m_stRIOInfo.data[ii].deg = 0.0;     // ���̓f�[�^�ϊ��l(deg.)
            }
            else
            {
                int16_t val = (m_stRIOCnfg.getdata.uint8[ADDR_LOW] << 8) | m_stRIOCnfg.getdata.uint8[ADDR_HIGH];

                // ���̓f�[�^
                m_stRIOInfo.data[ii].dig = val;
                // ���̓f�[�^�ϊ��l(mA)
                if      (m_stRIOInfo.data[ii].dig == S7CMPTBL_FORMAT_OVERRANGE)  {m_stRIOInfo.data[ii].cur = CUR_OVERRANGE;}
                else if (m_stRIOInfo.data[ii].dig == S7CMPTBL_FORMAT_UNDERRANGE) {m_stRIOInfo.data[ii].cur = CUR_UNDERRANGE;}
                else                                                             {m_stRIOInfo.data[ii].cur = CUR_MIN +  ((double)val * CONV_DIG_CUR);}
                // ���̓f�[�^�ϊ��l(deg.)
                m_stRIOInfo.data[ii].deg = ((m_stRIOInfo.data[ii].cur - CUR_MIN) * CONV_CUR_DEG) - (DEG_RANGE / 2.0);
            }
        }
    }
    else
    {
        m_stRIOInfo.data[RIO_PORT_1].dig = 0;       // ���̓f�[�^
        m_stRIOInfo.data[RIO_PORT_1].cur = 0.0;     // ���̓f�[�^�ϊ��l(mA)
        m_stRIOInfo.data[RIO_PORT_1].deg = 0.0;     // ���̓f�[�^�ϊ��l(deg.)
    }
    g_pSharedObject->SetRIOInfo(m_stRIOInfo);

    return;
}
