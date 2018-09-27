#include "stdafx.h"
#include "PublicRelation.h"


CPublicRelation::CPublicRelation()
{
}


CPublicRelation::~CPublicRelation()
{
}

CPublicRelation* CPublicRelation:: pPrInst;

HBITMAP CPublicRelation::hBmp_bg;
HBITMAP CPublicRelation::hBmp_mob;
HDC CPublicRelation::hdc_mem0;
HDC CPublicRelation::hdc_mem_bg;
HDC CPublicRelation::hdc_mem_mob;
HDC CPublicRelation::hdc_mem_inf;
HIMAGELIST CPublicRelation::hImgList_harai;

void CPublicRelation::init_task(void* pobj) {

	pPrInst = (CPublicRelation *)pobj;//スタティック変数にインスタンスポインタ登録
	
	///# 作業ウィンドウ生成
	InitWorkWnd(pPrInst->inf.hInstance, PrWndProc, TEXT("child"));
	pPrInst->inf.hWnd_work = CreateWindow(TEXT("child"),
		TEXT("Yard Simulator"),
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
		PR_WND_X,
		PR_WND_Y,
		PR_WND_W,
		PR_WND_H,
		pPrInst->inf.hWnd_parent,
		0,
		pPrInst->inf.hInstance,
		NULL);
	ShowWindow(pPrInst->inf.hWnd_work, SW_SHOW);
	UpdateWindow(pPrInst->inf.hWnd_work);

	///# INIファイル読み込み
	wchar_t tbuf[32];
//	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);

}

BOOL CPublicRelation::InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpszClassName) {
	
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = PrWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszClassName;
	wc.hIconSm = NULL;

	return RegisterClassExW(&wc);
}

LRESULT CPublicRelation::PrWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	BITMAP bmp_info;
	static int bgw,bgh,mobw,mobh,mobx,moby, i_img;
	int nFramX, nFramY, nCaption, scrw, scrh, winw, winh, winx, winy, mob_speed = 4;
	HDC hdc;

	switch (msg) {
	case WM_COMMAND: {
	}break;
	case WM_CREATE: {
		nFramX = GetSystemMetrics(SM_CXSIZEFRAME);//ウィンドウ周囲の幅
		nFramY = GetSystemMetrics(SM_CYSIZEFRAME);//ウィンドウ周囲の高さ
		nCaption = GetSystemMetrics(SM_CYCAPTION);//タイトルバーの高さ
		scrw = GetSystemMetrics(SM_CXSCREEN);//プライマモニタの幅
		scrh = GetSystemMetrics(SM_CYSCREEN);//プライマモニタの高さ

		hBmp_bg = (HBITMAP)LoadImage(pPrInst->inf.hInstance, TEXT("IDB_BACKGROUND"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		GetObject(hBmp_bg, (int)sizeof(bmp_info), &bmp_info);
		bgw = bmp_info.bmWidth; bgh = bmp_info.bmHeight;
		hdc_mem_bg = CreateCompatibleDC(NULL);
		SelectObject(hdc_mem_bg, hBmp_bg);

		hBmp_mob = (HBITMAP)LoadImage(pPrInst->inf.hInstance, TEXT("IDB_HARAI0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		GetObject(hBmp_mob, (int)sizeof(bmp_info), &bmp_info);
		mobw = bmp_info.bmWidth; mobh = bmp_info.bmHeight;
		hdc_mem_mob = CreateCompatibleDC(NULL);
		SelectObject(hdc_mem_mob, hBmp_mob);

		hdc_mem0 = CreateCompatibleDC(NULL);
		hdc = GetDC(hWnd);
		HBITMAP hDummy = CreateCompatibleBitmap(hdc, bgw, bgh);
		SelectObject(hdc_mem0, hDummy);
		ReleaseDC(hWnd, hdc);
		DeleteObject(hDummy);

		BitBlt(hdc_mem0, 0, 0, bgw, bgh, hdc_mem_bg, 0, 0, SRCCOPY);
		TransparentBlt(hdc_mem0, mobx, moby, 16, 16, hdc_mem_mob, 0, 0, 16, 16, RGB(255, 255, 255));

		winw = bgw + nFramX * 2; winh = bgh + nFramY * 2 + nCaption; winx = (scrw - winw) / 2; winy = (scrh - winh) / 2;
		MoveWindow(hWnd, winx-20, winy-10, winw+40, winh+20, TRUE);

		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 20, 0, bgw, bgh, hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_KEYDOWN: {
		if (wp == VK_LEFT) { mobx -= mob_speed; if (mobx < 0)mobx = 0; }
		else if(wp == VK_RIGHT) { mobx += mob_speed; if (mobx + 16 > bgw ) mobx = bgw-16; }
		else if (wp == VK_UP) { moby -= mob_speed; if (moby < 0) moby = 0; }
		else if (wp == VK_DOWN) { moby += mob_speed; if (moby + 16 > bgh) moby = bgh - 16; }
		else return DefWindowProc(hWnd, msg, wp, lp);

		i_img += 1; if (i_img > 3)i_img = 1;

		BitBlt(hdc_mem0, 0, 0, bgw, bgh, hdc_mem_bg, 0, 0, SRCCOPY);
		TransparentBlt(hdc_mem0, mobx, moby, 16, 16, hdc_mem_mob, i_img * 16 , 0, 16, 16, RGB(255, 255, 255));
		InvalidateRect(hWnd,NULL,FALSE);

		break;
	}
	case WM_DESTROY: {
		pPrInst->inf.hWnd_work = NULL;
		break;
	}
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
};

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
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); break;

		case IDC_TASK_ITEM_RADIO1:
		case IDC_TASK_ITEM_RADIO2:
		case IDC_TASK_ITEM_RADIO3:
		case IDC_TASK_ITEM_RADIO4:
		case IDC_TASK_ITEM_RADIO5:
		case IDC_TASK_ITEM_RADIO6:
			inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			if ((inf.panel_func_id == IDC_TASK_FUNC_RADIO1) && (inf.panel_type_id == IDC_TASK_ITEM_RADIO1)) {
				if (inf.hWnd_work == NULL) {
					InitWorkWnd(pPrInst->inf.hInstance, PrWndProc, TEXT("child"));
					pPrInst->inf.hWnd_work = CreateWindow(TEXT("child"),TEXT("Yard Simulator"),
						WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,PR_WND_X,PR_WND_Y,PR_WND_W,	PR_WND_H,
						pPrInst->inf.hWnd_parent,0,	pPrInst->inf.hInstance,	NULL);
					ShowWindow(pPrInst->inf.hWnd_work, SW_SHOW);
					UpdateWindow(pPrInst->inf.hWnd_work);
				}
				else {
					ShowWindow(inf.hWnd_work, SW_RESTORE);
					UpdateWindow(inf.hWnd_work);
				}
			}
			break;
		case IDSET: {
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
		case IDRESET: {
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
	}
	return 0;
};

void CPublicRelation::set_panel_tip_txt()
{
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"Type for Func1 \n\r 1:Open Work Wnd 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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
		wstr = L"Type for Func2 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"Type for Func4 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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
		wstr = L"Type for Func6 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
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