#include "CPublicRelation.h"
#include "resource.h"
#include "CSharedObject.h"
#include <Commdlg.h>

extern CSharedObject*	g_pSharedObject;//タスク間共有データのポインタ

CPublicRelation::CPublicRelation() {}
CPublicRelation::~CPublicRelation() {}

///######  メンテナンスパネル表示
HWND CPublicRelation::OpenCameraPanel() {
	return nullptr;
}

///######メンテナンスパネルWnd用コールバック関数
LRESULT CALLBACK DispWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	INT wmId = LOWORD(wp);
#if 0
	switch (msg) {
	case WM_INITDIALOG:

		break;
	case WM_COMMAND:
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDC_BUTTON_CAM_START:
			m_cSharedData->SetParam(PARAM_ID_CAM_PROC, (UINT32)TRUE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PIC_PROC), FALSE);
			break;
		case IDC_BUTTON_CAM_STOP:
			m_cSharedData->SetParam(PARAM_ID_CAM_PROC, (UINT32)FALSE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PIC_PROC), TRUE);
			break;
		case IDC_BUTTON_SAVE:
			if (m_saveImage.empty() != TRUE) {
				imwrite("procImage.bmp", m_saveImage);
			}
			else {
				MessageBox(m_hDlg, L"保存する画像がありません。", L"エラー", MB_OK);
			}
			break;
		case IDC_BUTTON_OPEN_SET:
			DispSetDialog();
			break;
		case IDC_BUTTON_PIC_PROC: {
			static OPENFILENAME ofn;
			static TCHAR strFile[MAX_PATH], strCustom[256] = TEXT("Before files\0*.*\0\0");

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hDlg;
			ofn.lpstrFilter = TEXT("Image files {*.jpg;*.bmp;*.png}\0*.jpg;*.bmp;*.png\0")
				TEXT("All files {*.*}\0*.*\0\0");
			ofn.lpstrCustomFilter = strCustom;
			ofn.nMaxCustFilter = 256;
			ofn.nFilterIndex = 0;
			ofn.lpstrFile = strFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn) == TRUE) {
				char* cstr = (char*)malloc(sizeof(strFile));
				if (cstr != NULL) {
					size_t size;
					wcstombs_s(&size, cstr, sizeof(strFile), strFile, sizeof(strFile));
					string str = cstr;
					m_cSharedData->SetParam(PARAM_ID_STR_PROC_FILENAME, str);
					free(cstr);
					m_cSharedData->SetParam(PARAM_ID_PIC_PROC_FLAG, (UINT32)TRUE);
				}
			}

			break;
		}
		case IDCANCEL:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_PAINT:

		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
#endif
	return FALSE;
}
