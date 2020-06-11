#pragma once
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

// iniファイル関数用コード
enum
{
    INITYPE_CHAR = 0,
    INITYPE_INT,
    INITYPE_SHORT,
    INITYPE_LONG,
    INITYPE_FLOAT,
    INITYPE_DOUBLE
};

class CHelper
{
public:
    CHelper();
    ~CHelper();
    static void GetIniInf(LPCWSTR file_name, LPCWSTR section_name, LPCWSTR key_name, LPCWSTR str_default, int value_type, void* p_param);
    static void Str2Wstr(const std::string &src, std::wstring &dest);
};
