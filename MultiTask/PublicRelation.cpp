#include "stdafx.h"
#include "PublicRelation.h"
#include "Smem.h"


CPublicRelation::CPublicRelation(){}

CPublicRelation::~CPublicRelation(){}

CPublicRelation* CPublicRelation:: pPrInst;

LPSTMobs CPublicRelation::pstMobs;
HANDLE CPublicRelation::hmue_mobs;

PR_DISP CPublicRelation::stdisp;

BOOL CPublicRelation::b_infchecked;

void CPublicRelation::init_task(void* pobj) {

	pPrInst = (CPublicRelation *)pobj;//スタティック変数にインスタンスポインタ登録
	
	///# 作業ウィンドウ生成
		//MOBの透過率設定用構造体設定
	pPrInst->stdisp.bf.SourceConstantAlpha = MOB_DISP_ALPHA;  pPrInst->stdisp.bf.BlendOp = AC_SRC_OVER; pPrInst->stdisp.bf.BlendFlags = 0; pPrInst->stdisp.bf.AlphaFormat = 0;
		//MOBの画像配列セット
	for (int i = 0; i < NUM_OF_MOB; i++) stdisp.hBmp_mob[i] = NULL;//ムービングオブジェクトのハンドル初期化
	stdisp.hBmp_mob[MOB_ID_HARAI] = (HBITMAP)LoadImage(pPrInst->inf.hInstance, TEXT("IDB_HARAI0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	stdisp.hBmp_mob[MOB_ID_CRUSH] = (HBITMAP)LoadImage(pPrInst->inf.hInstance, TEXT("IDB_CRUSH0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	
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

	///# 共有メモリ関連セット
	pstMobs = (LPSTMobs)&(((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.mobs);
	hmue_mobs = inf.hSmem_mutex[MUTEXID_MOBS];
	
	///# INIファイル読み込み
//	wchar_t tbuf[32];
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

void CPublicRelation::routine_work(void *param) {

	//COMMON
	ws << L"PR work activated!" << *(inf.psys_counter); tweet2owner(ws.str()); ws.str(L""); ws.clear();

	SendMessage(pPrInst->inf.hWnd_work, MSGID_UPDATE_DISP_PR, 0, 0);

	return;
};

LRESULT CPublicRelation::PrWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	BITMAP bmp_info;
	static int mobw, mobh, mobx, moby, i_img;

	static HWND hinfchk;
	int nFramX, nFramY, nCaption, scrw, scrh, winw, winh, winx, winy, mob_speed = 4;
	HDC hdc;
	static POINTS mpts;
	TCHAR szBuf[64];

	switch (msg) {

	case WM_COMMAND: {
		
		switch (LOWORD(wp)) {
		case IDC_CHK_INFDISP:
			if (BST_CHECKED == SendMessage(hinfchk, BM_GETCHECK, 0, 0)) b_infchecked = TRUE;
			else b_infchecked = FALSE;
			SetFocus(hWnd);//親ウィンドウにフォーカスを持ってこないとキーイベントが入らない
			break;
		default:
			break;
		}
		break;
	}

	case MSGID_UPDATE_DISP_PR: {
		update_disp();
	}break;

	case WM_CREATE: {
		nFramX = GetSystemMetrics(SM_CXSIZEFRAME);//ウィンドウ周囲の幅
		nFramY = GetSystemMetrics(SM_CYSIZEFRAME);//ウィンドウ周囲の高さ
		nCaption = GetSystemMetrics(SM_CYCAPTION);//タイトルバーの高さ
		scrw = GetSystemMetrics(SM_CXSCREEN);//プライマモニタの幅
		scrh = GetSystemMetrics(SM_CYSCREEN);//プライマモニタの高さ
		pPrInst->stdisp.hfont_inftext = CreateFont(12, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));


		pPrInst->stdisp.hBmp_bg = (HBITMAP)LoadImage(pPrInst->inf.hInstance, TEXT("IDB_BACKGROUND"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		GetObject(pPrInst->stdisp.hBmp_bg, (int)sizeof(bmp_info), &bmp_info);
		pPrInst->stdisp.bgw = bmp_info.bmWidth; pPrInst->stdisp.bgh = bmp_info.bmHeight;
		pPrInst->stdisp.hdc_mem_bg = CreateCompatibleDC(NULL);
		SelectObject(pPrInst->stdisp.hdc_mem_bg, pPrInst->stdisp.hBmp_bg);
			

		GetObject(pPrInst->stdisp.hBmp_mob[MOB_ID_CRUSH], (int)sizeof(bmp_info), &bmp_info);
		mobw = bmp_info.bmWidth; mobh = bmp_info.bmHeight;
		pPrInst->stdisp.hdc_mem_mob = CreateCompatibleDC(NULL);
		SelectObject(pPrInst->stdisp.hdc_mem_mob, pPrInst->stdisp.hBmp_mob[MOB_ID_CRUSH]);

		HBITMAP htmp = CreateCompatibleBitmap(pPrInst->stdisp.hdc_mem_bg, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh);
		pPrInst->stdisp.hdc_mem_inf = CreateCompatibleDC(NULL);
		SelectObject(pPrInst->stdisp.hdc_mem_inf, htmp);
		PatBlt(pPrInst->stdisp.hdc_mem_inf,0,0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh,WHITENESS);
		wsprintf(szBuf, TEXT("mouse x:%04d  y:%04d"), 0, 0);
		SetTextColor(pPrInst->stdisp.hdc_mem_inf,RGB(128,128,128));
		SelectObject(pPrInst->stdisp.hdc_mem_inf, pPrInst->stdisp.hfont_inftext);
		TextOut(pPrInst->stdisp.hdc_mem_inf, pPrInst->stdisp.bgw - 200, 5, szBuf, lstrlen(szBuf));
		DeleteObject(htmp);
		
		pPrInst->stdisp.hdc_mem0 = CreateCompatibleDC(NULL);
		hdc = GetDC(hWnd);
		HBITMAP hDummy = CreateCompatibleBitmap(hdc, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh);
		SelectObject(pPrInst->stdisp.hdc_mem0, hDummy);
		ReleaseDC(hWnd, hdc);
		DeleteObject(hDummy);
		
		BitBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_bg, 0, 0, SRCCOPY);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobh, mobh, pPrInst->stdisp.hdc_mem_mob, 0, 0, mobh, mobh, RGB(255, 255, 255));
		if(b_infchecked)TransparentBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, RGB(255, 255, 255));

		winw = pPrInst->stdisp.bgw + nFramX * 2; winh = pPrInst->stdisp.bgh + nFramY * 2 + nCaption; winx = (scrw - winw) / 2; winy = (scrh - winh) / 2;
		MoveWindow(hWnd, winx - 20, winy - 10, winw + 40, winh + 20, TRUE);
		
		hinfchk=CreateWindow(L"button", TEXT(":inf"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,winw-40,5,45,15, hWnd, (HMENU)IDC_CHK_INFDISP, pPrInst->inf.hInstance, NULL);
		
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;

		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 20, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_MOUSEMOVE: {
		mpts.x = LOWORD(lp); mpts.y = HIWORD(lp);//マウス位置
		PatBlt(pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, WHITENESS);//情報画面一旦クリア
		wsprintf(szBuf, TEXT("mouse x:%04d  y:%04d"), mpts.x, mpts.y);
		TextOut(pPrInst->stdisp.hdc_mem_inf, pPrInst->stdisp.bgw - 200, 5, szBuf, lstrlen(szBuf));//マウス位置情報書き込み

	/*
		BitBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_bg, 0, 0, SRCCOPY);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobh, mobh, pPrInst->stdisp.hdc_mem_mob, i_img * mobh, 0, mobh, mobh, RGB(255, 255, 255));
		if (b_infchecked)TransparentBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, RGB(255, 255, 255));

		InvalidateRect(pPrInst->inf.hWnd_work, NULL, FALSE);
	*/
		update_disp();
		break;
	}
	case WM_LBUTTONUP: {
		mpts.x = LOWORD(lp); mpts.y = HIWORD(lp);
		break;
	}
	case WM_LBUTTONDOWN: {
		mpts.x = LOWORD(lp); mpts.y = HIWORD(lp);
		break;
	}
	case WM_KEYDOWN: {
		if (wp == VK_LEFT) { mobx -= mob_speed; if (mobx < 0)mobx = 0; }
		else if(wp == VK_RIGHT) { mobx += mob_speed; if (mobx + mobh > pPrInst->stdisp.bgw ) mobx = pPrInst->stdisp.bgw- mobh; }
		else if (wp == VK_UP) { moby -= mob_speed; if (moby < 0) moby = 0; }
		else if (wp == VK_DOWN) { moby += mob_speed; if (moby + mobh > pPrInst->stdisp.bgh) moby = pPrInst->stdisp.bgh - mobh; }
		else return DefWindowProc(hWnd, msg, wp, lp);

		i_img += 1; if (i_img > 3)i_img = 1;

		BitBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_bg, 0, 0, SRCCOPY);
		AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobh, mobh, pPrInst->stdisp.hdc_mem_mob, i_img * mobh, 0, mobh, mobh, pPrInst->stdisp.bf);
		//		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, 16, 16, pPrInst->stdisp.hdc_mem_mob, i_img * 16, 0, 16, 16, RGB(255, 255, 255));
		if (b_infchecked)TransparentBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, RGB(255, 255, 255));
		InvalidateRect(pPrInst->inf.hWnd_work, NULL, FALSE);

		break;
	}
	case WM_DESTROY: {
		pPrInst->inf.hWnd_work = NULL;
		pPrInst->del_objects();
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

void CPublicRelation::del_objects() {
	DeleteObject(stdisp.hBmp_bg);
	for (int i = 0; i < NUM_OF_MOB; i++) {
		if (stdisp.hBmp_mob[i] != NULL) DeleteObject(stdisp.hBmp_mob[i]);
	}
	DeleteObject(stdisp.hdc_mem_bg);
	DeleteObject(stdisp.hdc_mem_mob);
	DeleteObject(stdisp.hdc_mem_inf);
	DeleteObject(stdisp.hfont_inftext);
};

void CPublicRelation::set_image(BOOL req_inf) {
	BitBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_bg, 0, 0, SRCCOPY);
//	TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, 16, 16, pPrInst->stdisp.hdc_mem_mob, i_img * 16, 0, 16, 16, RGB(255, 255, 255));
	TransparentBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, RGB(255, 255, 255));
	InvalidateRect(pPrInst->inf.hWnd_work, NULL, FALSE);
	return;
};

void CPublicRelation::update_disp() {
	static int i_img2;
	int mobw, mobh, mobx, moby;

	//i_img2 += 1; if (i_img2 > 3)i_img2 = 1;

	BitBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_bg, 0, 0, SRCCOPY);

	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_CRUSH][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_CRUSH][i]->status;
		mobx = pstMobs->pmobs[MOB_ID_CRUSH][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_CRUSH][i]->area.y;
		mobw = pstMobs->pmobs[MOB_ID_CRUSH][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_CRUSH][i]->bmph;
		//AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}
	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_HARAI][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_HARAI; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_HARAI][i]->status;
		mobx = pstMobs->pmobs[MOB_ID_HARAI][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_HARAI][i]->area.y;
		mobw = pstMobs->pmobs[MOB_ID_HARAI][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_HARAI][i]->bmph;
		//	AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}
	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_CUL][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_CUL; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_CUL][i]->status;
		mobx = pstMobs->pmobs[MOB_ID_CUL][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_CUL][i]->area.y;
		mobw = pstMobs->pmobs[MOB_ID_CUL][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_CUL][i]->bmph;
		//AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}
	if (b_infchecked)TransparentBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, RGB(255, 255, 255));
	InvalidateRect(pPrInst->inf.hWnd_work, NULL, FALSE);
	return;
};
;