#pragma once

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

//iniファイル関数用コード
enum {
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
	CHelper() {};
	~CHelper() {};

	static void GetIniInf(	LPCWSTR file_name,		//file名 
		LPCWSTR section_name,	//Section名
		LPCWSTR key_name,		//Key名
		LPCWSTR str_default,	//デフォルト値 
		int value_type,			//変換後の型コード CHelper.henum定義
		void* p_param			//読み込み値セット先ポインタ
	);

	static void Str2Wstr(const std::string &src, std::wstring &dest);

};

