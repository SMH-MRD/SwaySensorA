#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"
#include "ModbusTCPLibMain.h"

extern vector<void*>    VectpCTaskObj;  // タスクオブジェクトのポインタ
extern ST_iTask         g_itask;

// RIOセットパラメータ
#define RIO_COMMAND_REGISTER            2006    // COMMAND REGISTER 
#define RIO_COMMAND_AI_PORT_ACTIVE      0x8000  // 2006へ書き込みで2100-2279への設定値を有効にする
#define RIO_PORT_REGISTER_PORT1_MODE    2120    // PORT1のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT2_MODE    2140    // PORT2のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT3_MODE    2160    // PORT3のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT4_MODE    2180    // PORT4のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT5_MODE    2200    // PORT5のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT6_MODE    2220    // PORT6のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT7_MODE    2240    // PORT7のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT8_MODE    2260    // PORT8のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_MODE_IOLINK   0x0004  // 2120,2140への設定値　アナログユニットはIO LINKモード04とする
#define RIO_PORT1_IN_ADDRESS            8002    // PORT1読み込みデータアドレス
#define RIO_PORT2_IN_ADDRESS            8018    // PORT2読み込みデータアドレス
#define RIO_PORT3_IN_ADDRESS            8034    // PORT3読み込みデータアドレス
#define RIO_PORT4_IN_ADDRESS            8050    // PORT4読み込みデータアドレス
#define RIO_PORT5_IN_ADDRESS            8066    // PORT5読み込みデータアドレス
#define RIO_PORT6_IN_ADDRESS            8082    // PORT6読み込みデータアドレス
#define RIO_PORT7_IN_ADDRESS            8098    // PORT7読み込みデータアドレス
#define RIO_PORT8_IN_ADDRESS            8114    // PORT8読み込みデータアドレス
#define RIO_PORT1_OUT_ADDRESS           9002    // PORT1書き込みデータアドレス
#define RIO_PORT2_OUT_ADDRESS           9018    // PORT2書き込みデータアドレス
#define RIO_PORT3_OUT_ADDRESS           9034    // PORT3書き込みデータアドレス
#define RIO_PORT4_OUT_ADDRESS           9050    // PORT4書き込みデータアドレス
#define RIO_PORT5_OUT_ADDRESS           9066    // PORT5書き込みデータアドレス
#define RIO_PORT6_OUT_ADDRESS           9082    // PORT6書き込みデータアドレス
#define RIO_PORT7_OUT_ADDRESS           9098    // PORT7書き込みデータアドレス
#define RIO_PORT8_OUT_ADDRESS           9114    // PORT8書き込みデータアドレス
//#define RIO_COMMAND_AI_PARA_SET         0x8010  // AI: 15bit set command  8,9bit filter 4,5 format->0 sampling filter, S7 format
#define RIO_COMMAND_AI_PARA_SET         0x8000  // AI: 15bit set command  8,9bit filter 4,5 format->0 sampling filter, S7 format

#define RIO_PORT_NUM                    8

#define ADDR_HIGH                       1
#define ADDR_LOW                        0

#define CUR_MIN                         4.0                             // Cur. min.
#define CUR_MAX                         20.0                            // Cur. max.
#define DEG_RANGE                       60.0                            // Deg. range (±30deg)
#define CONV_DIG_CUR                    (0.00463/8.0)                   // Resolution 1mA/1dig
#define CONV_CUR_DEG                    (DEG_RANGE/(CUR_MAX-CUR_MIN))   // Resolution 1deg/1mA
#define CUR_OVERRANGE                   22.81                           // Cur. overrange
#define CUR_UNDERRANGE                  1.185                           // Cur. underrange

union UNION_WandB                   // 1WORD/2BYTE
{
    uint16_t    uint16;
    uint8_t     uint8[2];
};

typedef struct _stModbusInitCnfg                                // Modbus初期化設定
{
    char    ipaddrs[20U];                                       // IPアドレス
    int32_t portnum;                                            // ポート番号
    int32_t timeout;                                            // 通信タイムアウト
} stModbusInitCnfg;

typedef struct _stRIOCnfg                                       // RIO設定
{
    ModbusTCPDesc       modbusDesc;                             // ModbusTCPディスクリプタ

    stModbusInitCnfg    modbusInitCnfg;                         // Modbus初期化設定

    ST_MODBUS_TCP_REQ   stModbusTcpSetCmndReq;                  // Command書き込み
    ST_MODBUS_TCP_REQ   stModbusTcpSetIOLinkReq[RIO_PORT_MAX];  // IO-Link書き込み
    ST_MODBUS_TCP_REQ   stModbusTcpSetPortReq[RIO_PORT_MAX];    // ポート書き込み
    ST_MODBUS_TCP_REQ   stModbusTcpGetPortReq[RIO_PORT_MAX];    // ポート読み出し


    UNION_WandB setdata;
    UNION_WandB getdata;
} ST_RIOCnfg, *LPST_RIOCnfg;

typedef struct _stRIORegTable
{
    UINT    portMode;   // mode設定レジスタ
    UINT    inAddr;     // 読み込みデータアドレス
    UINT    outAddr;    // 書き込みデータアドレス
} stRIORegTable;

const stRIORegTable m_stRegTable[RIO_PORT_NUM + 1] =
{
    {0, 0, 0},  // 0は欠番扱いとしておく
    {RIO_PORT_REGISTER_PORT1_MODE, RIO_PORT1_IN_ADDRESS, RIO_PORT1_OUT_ADDRESS},
    {RIO_PORT_REGISTER_PORT2_MODE, RIO_PORT2_IN_ADDRESS, RIO_PORT2_OUT_ADDRESS},
    {RIO_PORT_REGISTER_PORT3_MODE, RIO_PORT3_IN_ADDRESS, RIO_PORT3_OUT_ADDRESS},
    {RIO_PORT_REGISTER_PORT4_MODE, RIO_PORT4_IN_ADDRESS, RIO_PORT4_OUT_ADDRESS},
    {RIO_PORT_REGISTER_PORT5_MODE, RIO_PORT5_IN_ADDRESS, RIO_PORT5_OUT_ADDRESS},
    {RIO_PORT_REGISTER_PORT6_MODE, RIO_PORT6_IN_ADDRESS, RIO_PORT6_OUT_ADDRESS},
    {RIO_PORT_REGISTER_PORT7_MODE, RIO_PORT7_IN_ADDRESS, RIO_PORT7_OUT_ADDRESS},
    {RIO_PORT_REGISTER_PORT8_MODE, RIO_PORT8_IN_ADDRESS, RIO_PORT8_OUT_ADDRESS},
};

class CComRIO : public CTaskObj
{
public:
    CComRIO();
    ~CComRIO();

    void init_task(void* pobj);
    void routine_work(void* param);

private:
    ST_RIOCnfg      m_riocnfg;
    stRIOInfoData   m_rioinfo;

private:
    void    Initialize(void);
    void    Close(void);    
    int32_t InitializeRIO(void);
    void    ProcRIO(void);
};
