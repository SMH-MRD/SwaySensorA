#pragma once

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

//ini�t�@�C���֐��p�R�[�h
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

	static void GetIniInf(	LPCWSTR file_name,		//file�� 
							LPCWSTR section_name,	//Section��
							LPCWSTR key_name,		//Key��
							LPCWSTR str_default,	//�f�t�H���g�l 
							int value_type,			//�ϊ���̌^�R�[�h CHelper.henum��`
							void* p_param			//�ǂݍ��ݒl�Z�b�g��|�C���^
						);

};

