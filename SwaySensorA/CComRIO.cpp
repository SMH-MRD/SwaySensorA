#include "CComRIO.h"
#include "CSharedObject.h"

extern CSharedObject*   g_pSharedObject;    // タスク間共有データのポインタ

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
    stRIO_ph.bRIO_init_ok = false;

    if (g_pSharedObject == NULL) {return;}
    g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_1_ANALOG, (DOUBLE)NAN);
    g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_1_MA,     (DOUBLE)NAN);
    g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_ANALOG, (DOUBLE)NAN);
    g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_MA,     (DOUBLE)NAN);

    return;
};

/// @brief タスクスレッドでの処理関数
/// @param
/// @return 
/// @note
void CComRIO::routine_work(void *param)
{
    if (g_pSharedObject == NULL) {return;}
    ws << L"Act: " << std::setw(2) << *(inf.psys_counter) % 100;
    tweet2owner(ws.str()); ws.str(L""); ws.clear();

    if (!stRIO_ph.bRIO_init_ok)
    {
		if (inf.act_count % 100){ //初期化未完時の初期化周期100スキャン毎
			if (Initialize() >= 0)
			{
				stRIO_ph.bRIO_init_ok = true;
				stRIO_ph.error_status = 0x0000;
			}
		}
    }
    else
    {
        // PORT1データ読み込み
        stRIO_ph.error_code = modtGetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq_p1read, (uint8_t *)stRIO_ph.RIO_ai_port1.uint8);

        if (stRIO_ph.error_code)
        {
            stRIO_ph.error_status = RIO_ERR_TYPE_AI_READ1;
            stRIO_ph.bRIO_init_ok = false;
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_1_ANALOG, (DOUBLE)NAN);
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_1_MA,     (DOUBLE)NAN);
        }
        else
        {
            UINT temp = (stRIO_ph.RIO_ai_port1.uint8[0] << 8) | stRIO_ph.RIO_ai_port1.uint8[1];
            // PORT1読み込みデータ mA変換
            if      (temp == 0x7FFF) {stRIO_ph.RIO_ai_p1_mA = 22.81;}
            else if (temp == 0x8000) {stRIO_ph.RIO_ai_p1_mA = 1.186;}
			else if (temp |  0x8000) {stRIO_ph.RIO_ai_p1_mA = 0.0;} //ERROR
            else                     {stRIO_ph.RIO_ai_p1_mA = 4.0 + 16.0 / 30000.0 * (double)(temp);}
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_1_ANALOG, (double)temp);
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_1_MA,     stRIO_ph.RIO_ai_p1_mA);
        }

        // PORT2データ読み込み
        stRIO_ph.error_code = modtGetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq_p2read, (uint8_t *)stRIO_ph.RIO_ai_port2.uint8);

        if (stRIO_ph.error_code)
        {
            stRIO_ph.error_status = RIO_ERR_TYPE_AI_READ2;
            stRIO_ph.bRIO_init_ok = false;
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_ANALOG, (DOUBLE)NAN);
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_MA,     (DOUBLE)NAN);
        }
        else
        {
            // PORT2読み込みデータ mA変換
            UINT temp = (stRIO_ph.RIO_ai_port2.uint8[0] << 8) | stRIO_ph.RIO_ai_port2.uint8[1];
            if      (temp == 0x7FFF) {stRIO_ph.RIO_ai_p2_mA = 22.81;}
            else if (temp == 0x8000) {stRIO_ph.RIO_ai_p2_mA = 1.186;}
			else if (temp |  0x8000) {stRIO_ph.RIO_ai_p1_mA = 0.0;} //ERROR
            else                     {stRIO_ph.RIO_ai_p2_mA = 4.0 + 16.0 / 30000.0 * (double)(temp);}
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_ANALOG, (double)temp);
            g_pSharedObject->SetInclinoData(INCLINO_ID_PORT_2_MA,     stRIO_ph.RIO_ai_p2_mA);
        }
    }

    return;
}

/// @brief 
/// @param
/// @return 
/// @note
int CComRIO::Initialize()
{
    string ipAddr;
    g_pSharedObject->GetParam(PARAM_ID_STR_RIO_IPADDR, &ipAddr);    memcpy(stRIO_ph.ip_string, ipAddr.c_str(), ipAddr.length());
    g_pSharedObject->GetParam(PARAM_ID_RIO_TCPPORT,    (UINT32*)&stRIO_ph.port_num);
    g_pSharedObject->GetParam(PARAM_ID_RIO_SLAVEADDR,  (UINT32*)&stRIO_ph.slave_addr);
    g_pSharedObject->GetParam(PARAM_ID_RIO_TIMEOUT,    (UINT32*)&stRIO_ph.timeOut);

    UINT32 xPort, yPort;
    g_pSharedObject->GetParam(PARAM_ID_RIO_XPORT, &xPort);
    g_pSharedObject->GetParam(PARAM_ID_RIO_YPORT, &yPort);

    stRIO_ph.modbusDesc = modtInit((const int8_t*)stRIO_ph.ip_string, stRIO_ph.port_num, stRIO_ph.timeOut);
    if (stRIO_ph.modbusDesc == NULL)
    {
        stRIO_ph.error_status = RIO_ERR_ITEM_INIT_FAIL;
        stRIO_ph.bRIO_init_ok = false;
        return -1;
    }
    // PORT1をIO LINK MODEに設定
    stRIO_ph.stModbusTcpReq.slaveAddr = stRIO_ph.slave_addr;
    stRIO_ph.stModbusTcpReq.funcCode  = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    stRIO_ph.stModbusTcpReq.regAddr   = m_stRegTable[xPort].portMode;
    stRIO_ph.stModbusTcpReq.dataCnt   = 1;
    stRIO_ph.stModbusTcpReq.option    = 0;
    stRIO_ph.setData[0].uint16        = RIO_PORT_REGISTER_MODE_IOLINK;

    stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
    if (stRIO_ph.error_code)
    {
        stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
        stRIO_ph.bRIO_init_ok = false;
        return -1;
    }

    // PORT2をIO LINK MODEに設定
    stRIO_ph.stModbusTcpReq.regAddr = m_stRegTable[yPort].portMode;
    stRIO_ph.error_code             = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
    if (stRIO_ph.error_code)
    {
        stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
        stRIO_ph.bRIO_init_ok = false;
        return -1;
    }

    // パラメータ設定値有効化
    stRIO_ph.stModbusTcpReq.regAddr = RIO_COMMAND_REGISTER;
    stRIO_ph.stModbusTcpReq.dataCnt = 2;
    stRIO_ph.stModbusTcpReq.option  = 0;
    stRIO_ph.setData[0].uint16      = RIO_COMMAND_AI_PORT_ACTIVE;

    stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
    if (stRIO_ph.error_code)
    {
        stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
        stRIO_ph.bRIO_init_ok = false;
        return -1;
    }

    // PORT1 AIのパラメータ設定
    stRIO_ph.stModbusTcpReq.funcCode = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
    stRIO_ph.stModbusTcpReq.regAddr  = m_stRegTable[xPort].outAddr;
    stRIO_ph.stModbusTcpReq.dataCnt  = 1;
    stRIO_ph.stModbusTcpReq.option   = 0;
    stRIO_ph.setData[0].uint16       = RIO_COMMAND_AI_PARA_SET;

    stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
    if (stRIO_ph.error_code)
    {
        stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
        stRIO_ph.bRIO_init_ok = false;
        return -1;
    }

    // PORT2 AIのパラメータ設定
    stRIO_ph.stModbusTcpReq.regAddr = m_stRegTable[yPort].outAddr;
    stRIO_ph.error_code             = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
    if (stRIO_ph.error_code)
    {
        stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
        stRIO_ph.bRIO_init_ok = false;
        return -1;
    }

    // PORT1読み込み設定登録
    stRIO_ph.stModbusTcpReq_p1read.slaveAddr = stRIO_ph.slave_addr;
    stRIO_ph.stModbusTcpReq_p1read.funcCode  = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
    stRIO_ph.stModbusTcpReq_p1read.regAddr   = m_stRegTable[xPort].inAddr;
    stRIO_ph.stModbusTcpReq_p1read.dataCnt   = 1;
    stRIO_ph.stModbusTcpReq_p1read.option    = 0;

    // PORT2読み込み設定登録
    stRIO_ph.stModbusTcpReq_p2read.slaveAddr = stRIO_ph.slave_addr;
    stRIO_ph.stModbusTcpReq_p2read.funcCode  = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
    stRIO_ph.stModbusTcpReq_p2read.regAddr   = m_stRegTable[yPort].inAddr;
    stRIO_ph.stModbusTcpReq_p2read.dataCnt   = 1;
    stRIO_ph.stModbusTcpReq_p2read.option    = 0;

    return 0;
}
