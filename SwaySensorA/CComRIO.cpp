#include "CComRIO.h"

extern CSharedObject* g_pSharedObject;  // タスク間共有データのポインタ

/// @brief コンストラクタ
/// @param
/// @return 
/// @note
CComRIO::CComRIO()
{
}

/// @brief デストラクタ
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

/// @brief タスクスレッドでの処理関数
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
    // ModbusTCP設定値
    stRIOParamData  param;
    g_pSharedObject->GetParam(&param);

	char* cstr = (char*)malloc(sizeof(param.ipaddrs));
	if (cstr != NULL)
    {
		size_t size;
		wcstombs_s(&size, cstr, sizeof(param.ipaddrs), param.ipaddrs, sizeof(param.ipaddrs));
		string str = cstr;
	    memcpy(m_riocnfg.modbusInitCnfg.ipaddrs, str.c_str(), sizeof(m_riocnfg.modbusInitCnfg.ipaddrs));
		free(cstr);
	}
    else
    {
        memcpy(m_riocnfg.modbusInitCnfg.ipaddrs, "192.168.0.1", sizeof(m_riocnfg.modbusInitCnfg.ipaddrs));
    }
    m_riocnfg.modbusInitCnfg.portnum = param.tcpport;   // ポート番号
    m_riocnfg.modbusInitCnfg.timeout = param.timeout;   // 通信タイムアウト

	int32_t slaveaddrs;
    UINT32  portx, porty;
    slaveaddrs = param.slaveaddrs;                      // スレーブアドレス
    portx      = param.portx;                           // 傾斜計Xデータ接続ポート番号(1～8)
    porty      = param.porty;                           // 傾斜計Yデータ接続ポート番号(1～8)

    //--------------------------------------------------------------------------
    // Command書き込み
    m_riocnfg.stModbusTcpSetCmndReq.slaveAddr = slaveaddrs;
    m_riocnfg.stModbusTcpSetCmndReq.funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_riocnfg.stModbusTcpSetCmndReq.regAddr   = RIO_COMMAND_REGISTER;
    m_riocnfg.stModbusTcpSetCmndReq.dataCnt   = 1;
    m_riocnfg.stModbusTcpSetCmndReq.option    = 0;

    //--------------------------------------------------------------------------
    // IO-Link書き込み
    // PORT1
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].slaveAddr = slaveaddrs;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].regAddr   = m_stRegTable[portx].portMode;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].dataCnt   = 1;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_1].option    = 0;
    // PORT2
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].slaveAddr = slaveaddrs;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].regAddr   = m_stRegTable[porty].portMode;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].dataCnt   = 1;
    m_riocnfg.stModbusTcpSetIOLinkReq[RIO_PORT_2].option    = 0;

    //--------------------------------------------------------------------------
    // ポート書き込み
    // PORT1
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_1].slaveAddr = slaveaddrs;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_1].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_1].regAddr   = m_stRegTable[portx].outAddr;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_1].dataCnt   = 1;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_1].option    = 0;
    // PORT2
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_2].slaveAddr = slaveaddrs;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_2].funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_2].regAddr   = m_stRegTable[porty].outAddr;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_2].dataCnt   = 1;
    m_riocnfg.stModbusTcpSetPortReq[RIO_PORT_2].option    = 0;

   //--------------------------------------------------------------------------
    // ポート読み出し
    // PORT1
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_1].slaveAddr = slaveaddrs;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_1].funcCode  = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_1].regAddr   = m_stRegTable[portx].inAddr;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_1].dataCnt   = 1;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_1].option    = 0;
    // PORT2
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_2].slaveAddr = slaveaddrs;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_2].funcCode  = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_2].regAddr   = m_stRegTable[porty].inAddr;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_2].dataCnt   = 1;
    m_riocnfg.stModbusTcpGetPortReq[RIO_PORT_2].option    = 0;

    //--------------------------------------------------------------------------
    // RIO情報初期化
    m_rioinfo.error = RIO_ERR_INIT_INCOMPLETE;      // RIO初期化未完了
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        m_rioinfo.incldata[ii].dig = 0;             // 入力データ
        m_rioinfo.incldata[ii].cur = 0.0;           // 入力データ変換値(mA)
        m_rioinfo.incldata[ii].deg = 0.0;           // 入力データ変換値(deg.)    
    }
    g_pSharedObject->SetInfo(m_rioinfo);

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
void CComRIO::Close(void)
{
    modtClose(m_riocnfg.modbusDesc);
}

/// @brief 
/// @param
/// @return 
/// @note
int32_t CComRIO::InitializeRIO(void)
{
    int32_t err;

    //--------------------------------------------------------------------------
    // ModbusTCP初期化
    m_riocnfg.modbusDesc = modtInit((const int8_t*)m_riocnfg.modbusInitCnfg.ipaddrs, m_riocnfg.modbusInitCnfg.portnum, m_riocnfg.modbusInitCnfg.timeout);
    if (m_riocnfg.modbusDesc == NULL) {return RIO_ERR_INIT_INCOMPLETE;}

    //--------------------------------------------------------------------------
    // IO LINK MODE設定
    m_riocnfg.setdata.uint16 = RIO_PORT_REGISTER_MODE_IOLINK;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        if ((err = modtSetdata(m_riocnfg.modbusDesc, m_riocnfg.stModbusTcpSetIOLinkReq[ii], m_riocnfg.setdata.uint8)) != 0)
        {
            return RIO_ERR_SET_IOLINKMODE;
        }
    }

    //--------------------------------------------------------------------------
    // パラメータ設定値有効化
    m_riocnfg.setdata.uint16 = RIO_COMMAND_AI_PORT_ACTIVE;
    if ((err = modtSetdata(m_riocnfg.modbusDesc, m_riocnfg.stModbusTcpSetCmndReq, m_riocnfg.setdata.uint8)))
    {
        return RIO_ERR_SET_PARAM_VALID;
    }

    //--------------------------------------------------------------------------
    // AIパラメータ設定
    m_riocnfg.setdata.uint16 = RIO_COMMAND_AI_PARA_SET;
    for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
    {
        if ((err = modtSetdata(m_riocnfg.modbusDesc, m_riocnfg.stModbusTcpSetPortReq[ii], m_riocnfg.setdata.uint8)))
        {
            return RIO_ERR_SET_PARAM_AI;
        }
    }

    return RIO_ERR_NONE;
}

/// @brief 傾斜計データ処理
/// @param
/// @return 
/// @note
void CComRIO::ProcRIO(void)
{
    //--------------------------------------------------------------------------
    // 初期化
    if (m_rioinfo.error != RIO_ERR_NONE) {m_rioinfo.error = InitializeRIO();}

    //--------------------------------------------------------------------------
    // データ読み出し
    if (m_rioinfo.error == RIO_ERR_NONE)
    {
        int32_t err = 0;
        for (UINT ii = 0; ii < RIO_PORT_MAX; ii++)
        {
            // データ読み出し
            if ((err =  modtGetdata(m_riocnfg.modbusDesc, m_riocnfg.stModbusTcpGetPortReq[ii], (uint8_t *)m_riocnfg.getdata.uint8)))
            {
                m_rioinfo.error |= (RIO_ERR_GET_AI_READ << ii);
                m_rioinfo.incldata[ii].dig = 0;     // 入力データ
                m_rioinfo.incldata[ii].cur = 0.0;   // 入力データ変換値(mA)
                m_rioinfo.incldata[ii].deg = 0.0;   // 入力データ変換値(deg.)
            }
            else
            {
                int16_t val = (m_riocnfg.getdata.uint8[ADDR_LOW] << 8) | m_riocnfg.getdata.uint8[ADDR_HIGH];

                // 入力データ
                m_rioinfo.incldata[ii].dig = val;
                // 入力データ変換値(mA)
                if      (m_rioinfo.incldata[ii].dig == S7CMPTBL_FORMAT_OVERRANGE)  {m_rioinfo.incldata[ii].cur = CUR_OVERRANGE;}
                else if (m_rioinfo.incldata[ii].dig == S7CMPTBL_FORMAT_UNDERRANGE) {m_rioinfo.incldata[ii].cur = CUR_UNDERRANGE;}
                else                                                               {m_rioinfo.incldata[ii].cur = CUR_MIN +  ((double)val * CONV_DIG_CUR);}
                // 入力データ変換値(deg.)
                m_rioinfo.incldata[ii].deg = ((m_rioinfo.incldata[ii].cur - CUR_MIN) * CONV_CUR_DEG) - (DEG_RANGE / 2.0);
            }
        }
    }
    else
    {
        m_rioinfo.incldata[RIO_PORT_1].dig = 0;     // 入力データ
        m_rioinfo.incldata[RIO_PORT_1].cur = 0.0;   // 入力データ変換値(mA)
        m_rioinfo.incldata[RIO_PORT_1].deg = 0.0;   // 入力データ変換値(deg.)
    }
    g_pSharedObject->SetInfo(m_rioinfo);

    return;
}
