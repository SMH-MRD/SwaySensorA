#include "framework.h"
#include "CHelper.h"

///# 関数: INIファイル読出し ***************
void CHelper::GetIniInf(LPCWSTR file_name, LPCWSTR section_name, LPCWSTR key_name, LPCWSTR str_default, int value_type, void* p_param)
{
	DWORD	state;
	WCHAR	buf[256];

	state = GetPrivateProfileString(section_name, key_name, str_default, buf, sizeof(buf) / 2, file_name);
	if (state <= 0)
	{
		_stprintf_s(buf, L"%s", str_default);
	}

	switch (value_type)
	{
	case INITYPE_CHAR:		_stprintf_s((LPWSTR)p_param, 256, buf);	break;
	case INITYPE_INT:		*((int*)p_param) = _wtoi(buf);			break;
	case INITYPE_SHORT:		*((short*)p_param) = (short)_wtoi(buf);	break;
	case INITYPE_LONG:		*((long*)p_param) = _wtoi(buf);			break;
	case INITYPE_FLOAT:		*((float*)p_param) = (float)_wtof(buf);	break;
	case INITYPE_DOUBLE:	*((double*)p_param) = _wtof(buf);		break;
	default:														break;
	}
}


///# 関数: string型をwstring型に変換 ***************
void CHelper::Str2Wstr(const std::string &src, std::wstring &dest) {

	setlocale(LC_ALL, "");

	wchar_t *wcs = new wchar_t[src.length() + 1];

	size_t ret_val;
	mbstowcs_s(&ret_val, wcs, src.length() + 1, src.c_str(), _TRUNCATE);

	dest = wcs; delete[] wcs;
	return;
}