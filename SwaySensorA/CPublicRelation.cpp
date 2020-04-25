#include "CPublicRelation.h"
#include "resource.h"
#include "CSharedObject.h"
#include <Commdlg.h>

extern CSharedObject*	g_pSharedObject;//タスク間共有データのポインタ

CPublicRelation::CPublicRelation() {
	hCamDlg = NULL;
}
CPublicRelation::~CPublicRelation() {}

///######  センサパネル表示
HWND CPublicRelation::OpenCameraPanel() {
	

	return hCamDlg;
}
///######　センサパネルWnd用コールバック関数
LRESULT CALLBACK CPublicRelation::DispWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	INT wmId = LOWORD(wp);

	switch (msg) {
	case WM_INITDIALOG:

		break;
#if 0
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
#endif
	case WM_PAINT:

		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	}

	return FALSE;
}
///######　タスクパネルWnd用コールバック関数
LRESULT CALLBACK CPublicRelation::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
			//機能選択IDセット
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		case IDC_TASK_ITEM_RADIO1:
		case IDC_TASK_ITEM_RADIO2:
		case IDC_TASK_ITEM_RADIO3:
		case IDC_TASK_ITEM_RADIO4:
		case IDC_TASK_ITEM_RADIO5:
		case IDC_TASK_ITEM_RADIO6:
			//機能入力IDセット
			inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));

			//### 各FUNCTION PB処理
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
				if (inf.panel_type_id == IDC_TASK_ITEM_RADIO1) {

					if (hCamDlg == NULL) {
						hCamDlg = CreateDialog(inf.hInstance, MAKEINTRESOURCE(IDD_DIALOG_CAMMAIN), nullptr, (DLGPROC)DispWndProc);
						ShowWindow(hCamDlg, SW_SHOW);

						TCHAR msg[10];
						_stprintf_s(msg, TEXT("-"));
						SetWindowText(GetDlgItem(hCamDlg, IDC_EDIT_GRA_POSX), msg);
						_stprintf_s(msg, TEXT("-"));
						SetWindowText(GetDlgItem(hCamDlg, IDC_EDIT_GRA_POSY), msg);
					}
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO2) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO3) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO4) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO5) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO6) {
					;
				}
				else;
			} 
			else if (inf.panel_func_id == IDC_TASK_FUNC_RADIO2) {

				if (inf.panel_type_id == IDC_TASK_ITEM_RADIO1) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO2) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO3) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO4) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO5) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO6) {
					;
				}
				else;
			}
			else if (inf.panel_func_id == IDC_TASK_FUNC_RADIO4) {
				if (inf.panel_type_id == IDC_TASK_ITEM_RADIO1);
			}
			else if (inf.panel_func_id == IDC_TASK_FUNC_RADIO6) {
				if (inf.panel_type_id == IDC_TASK_ITEM_RADIO1) ;
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO2);
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO3) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO4) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO5) {
					;
				}
				else if (inf.panel_type_id == IDC_TASK_ITEM_RADIO6) {
					;
				}
				else;
			}
			else;
			break;

		case ID_TASK_SET: {
			wstring wstr, wstr_tmp;

			//サンプルとしていろいろな型で読み込んで表示している
			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stod(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 2(i):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT2, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stoi(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 3(f):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT3, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stof(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 4(l):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT4, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stol(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 5(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT5, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			wstr += L",   Param 6(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT6, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			txout2msg_listbox(wstr);


		}break;
		case ID_TASK_RESET: {
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		}break;
		case IDC_TASK_OPTION_CHECK1:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK1) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION1;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;

		case IDC_TASK_OPTION_CHECK2:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK2) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION2;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;
		}
		break;
	}
	return 0;
};

void CPublicRelation::set_panel_pb_txt() {
	wstring wstr;
	wstr = L"PANEL";
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO1), wstr.c_str()); wstr.clear();
	wstr = L"-";
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO2), wstr.c_str()); wstr.clear();
	wstr = L"-";
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO3), wstr.c_str()); wstr.clear();
	wstr = L"-";
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO4), wstr.c_str()); wstr.clear();
	wstr = L"-";
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO5), wstr.c_str()); wstr.clear();
	wstr = L"-";
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO6), wstr.c_str()); wstr.clear();

	return; 
}

void CPublicRelation::set_panel_tip_txt()
{
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"Type for Func1 \n\r 1:OPEN 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO2: {
		wstr = L"Type of Func2 \n\r  1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO3: {
		wstr = L"Type for Func3 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:AS_ACT 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"Type for Func4 \n\r  1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO5: {
		wstr = L"Type for Func5 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO6: {
		wstr = L"Type for Func6 \n\r  1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	default: {
		wstr = L"Type for Func? \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		wstr_type += L"(Param of type?) \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
	}break;
	}

	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET3), wstr.c_str());
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET4), wstr_type.c_str());
}

//# パネルウィンドウのパラメータ設定用エディットBOXデータセット関数
void CPublicRelation::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}

void CPublicRelation::init_task(void *pobj) {

	//タグパネルPB表示設定

	set_panel_pb_txt();
	set_panel_tip_txt();
	return;
};
