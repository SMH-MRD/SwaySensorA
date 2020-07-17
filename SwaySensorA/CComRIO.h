#pragma once
#include "CTaskObj.h"
#include "CSharedObject.h"
#include "ModbusTCPLibMain.h"

extern vector<void*>    VectpCTaskObj;  // �^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask         g_itask;

// RIO�Z�b�g�p�����[�^
#define RIO_COMMAND_REGISTER            2006    // COMMAND REGISTER 
#define RIO_COMMAND_AI_PORT_ACTIVE      0x8000  // 2006�֏������݂�2100-2279�ւ̐ݒ�l��L���ɂ���
#define RIO_PORT_REGISTER_PORT1_MODE    2120    // PORT1��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT2_MODE    2140    // PORT2��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT3_MODE    2160    // PORT3��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT4_MODE    2180    // PORT4��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT5_MODE    2200    // PORT5��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT6_MODE    2220    // PORT6��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT7_MODE    2240    // PORT7��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT8_MODE    2260    // PORT8��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_MODE_IOLINK   0x0004  // 2120,2140�ւ̐ݒ�l�@�A�i���O���j�b�g��IO LINK���[�h04�Ƃ���
#define RIO_PORT1_IN_ADDRESS            8002    // PORT1�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT2_IN_ADDRESS            8018    // PORT2�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT3_IN_ADDRESS            8034    // PORT3�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT4_IN_ADDRESS            8050    // PORT4�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT5_IN_ADDRESS            8066    // PORT5�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT6_IN_ADDRESS            8082    // PORT6�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT7_IN_ADDRESS            8098    // PORT7�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT8_IN_ADDRESS            8114    // PORT8�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT1_OUT_ADDRESS           9002    // PORT1�������݃f�[�^�A�h���X
#define RIO_PORT2_OUT_ADDRESS           9018    // PORT2�������݃f�[�^�A�h���X
#define RIO_PORT3_OUT_ADDRESS           9034    // PORT3�������݃f�[�^�A�h���X
#define RIO_PORT4_OUT_ADDRESS           9050    // PORT4�������݃f�[�^�A�h���X
#define RIO_PORT5_OUT_ADDRESS           9066    // PORT5�������݃f�[�^�A�h���X
#define RIO_PORT6_OUT_ADDRESS           9082    // PORT6�������݃f�[�^�A�h���X
#define RIO_PORT7_OUT_ADDRESS           9098    // PORT7�������݃f�[�^�A�h���X
#define RIO_PORT8_OUT_ADDRESS           9114    // PORT8�������݃f�[�^�A�h���X
//#define RIO_COMMAND_AI_PARA_SET         0x8010  // AI: 15bit set command  8,9bit filter 4,5 format->0 sampling filter, S7 format
#define RIO_COMMAND_AI_PARA_SET         0x8000  // AI: 15bit set command  8,9bit filter 4,5 format->0 sampling filter, S7 format

#define RIO_PORT_NUM                    8

#define ADDR_HIGH                       1
#define ADDR_LOW                        0

#define CUR_MIN                         4.0                             // Cur. min.
#define CUR_MAX                         20.0                            // Cur. max.
#define DEG_RANGE                       60.0                            // Deg. range (�}30deg)
#define CONV_DIG_CUR                    (0.00463/8.0)                   // Resolution 1mA/1dig
#define CONV_CUR_DEG                    (DEG_RANGE/(CUR_MAX-CUR_MIN))   // Resolution 1deg/1mA
#define CUR_OVERRANGE                   22.81                           // Cur. overrange
#define CUR_UNDERRANGE                  1.185                           // Cur. underrange

union UNION_WandB   // 1WORD/2BYTE
{
    uint16_t    uint16;
    uint8_t     uint8[2];
};

typedef struct _stModbusInitCnfg    // Modbus�������ݒ�
{
    char    ipaddrs[20U];   // IP�A�h���X
    int32_t portnum;        // �|�[�g�ԍ�
    int32_t timeout;        // �ʐM�^�C���A�E�g
} stModbusInitCnfg;

typedef struct _stRIOCnfg           // RIO�ݒ�
{
    ModbusTCPDesc       modbusDesc;                         // ModbusTCP�f�B�X�N���v�^

    stModbusInitCnfg    modbusInitCnfg;                     // Modbus�������ݒ�

    ST_MODBUS_TCP_REQ   stModbusTcpSetCmndReq;                  // Command��������
    ST_MODBUS_TCP_REQ   stModbusTcpSetIOLinkReq[RIO_PORT_MAX];  // IO-Link��������
    ST_MODBUS_TCP_REQ   stModbusTcpSetPortReq[RIO_PORT_MAX];    // �|�[�g��������
    ST_MODBUS_TCP_REQ   stModbusTcpGetPortReq[RIO_PORT_MAX];    // �|�[�g�ǂݏo��


    UNION_WandB setdata;
    UNION_WandB getdata;
} ST_RIOCnfg, *LPST_RIOCnfg;

typedef struct _stRIORegTable
{
    UINT    portMode;   // mode�ݒ背�W�X�^
    UINT    inAddr;     // �ǂݍ��݃f�[�^�A�h���X
    UINT    outAddr;    // �������݃f�[�^�A�h���X
} stRIORegTable;

const stRIORegTable m_stRegTable[RIO_PORT_NUM + 1] =
{
    {0, 0, 0},  // 0�͌��Ԉ����Ƃ��Ă���
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
    ST_RIOCnfg  m_stRIOCnfg;
    stRIOInfo   m_stRIOInfo;

private:
    void    Initialize(void);
    void    Close(void);    
    int32_t InitializeRIO(void);
    void    ProcRIO(void);
};
