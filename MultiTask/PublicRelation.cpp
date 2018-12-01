#include "stdafx.h"
#include "PublicRelation.h"
#include "Smem.h"


CPublicRelation::CPublicRelation(){}

CPublicRelation::~CPublicRelation(){}

CPublicRelation* CPublicRelation:: pPrInst;

LPSTMobs CPublicRelation::pstMobs;
HANDLE CPublicRelation::hmue_mobs;
HWND CPublicRelation::hwndtip;
CMob* CPublicRelation::pmob_dlg;

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


	InitTipWnd(pPrInst->inf.hInstance, TipWndProc, TEXT("pr_tip"));
	hwndtip = CreateWindow(TEXT("pr_tip"),
		NULL,
		WS_CHILD,
		0,
		0,
		100,
		10,
		pPrInst->inf.hWnd_work,
		0,
		pPrInst->inf.hInstance,
		NULL);


	ShowWindow(hwndtip, SW_SHOW);
	UpdateWindow(hwndtip);


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
BOOL CPublicRelation::InitTipWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpszClassName) {

	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = TipWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hbrBackground = CreateSolidBrush(RGB(255,255,220));
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

#define ID_PR_TIMER 1

LRESULT CPublicRelation::PrWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	BITMAP bmp_info;
	static int mobw, mobh, mobx, moby, i_img;

	static HWND hinfchk;
	int nFramX, nFramY, nCaption, scrw, scrh, winw, winh, winx, winy, mob_speed = 4;
	HDC hdc;
	static POINTS mpts;
	TCHAR szBuf[128];
	CMob* pmob;
	HMENU hsubmenu, hpopmenu;
	HWND tmpwnd;

	switch (msg) {

	case WM_COMMAND: {
		
		switch (LOWORD(wp)) {
		case IDC_CHK_INFDISP:
			if (BST_CHECKED == SendMessage(hinfchk, BM_GETCHECK, 0, 0)) b_infchecked = TRUE;
			else b_infchecked = FALSE;
			SetFocus(hWnd);//親ウィンドウにフォーカスを持ってこないとキーイベントが入らない
			break;
		case IDM_PR_OPEN_DLG: {
			if (pmob_dlg != nullptr) {
				if ((pmob_dlg->type[0] == L'B') && (pmob_dlg->type[1] == L'C')) {
					tmpwnd = CreateDialog(pPrInst->inf.hInstance, L"IDD_BC_HANDLE", hWnd, (DLGPROC)PR_BCPANEL_PROC);
				}
				else if ((pmob_dlg->type[0] == L'T') && (pmob_dlg->type[1] == L'R')) {
					tmpwnd = CreateDialog(pPrInst->inf.hInstance, L"IDD_TRP_HANDLE", hWnd, (DLGPROC)PR_TRPPANEL_PROC);
				}
				else if ((pmob_dlg->type[0] == L'S') && (pmob_dlg->type[1] == L'C')) {
					tmpwnd = CreateDialog(pPrInst->inf.hInstance, L"IDD_TRP_HANDLE", hWnd, (DLGPROC)PR_TRPPANEL_PROC);
				}
				else if ((pmob_dlg->type[0] == L'H') && (pmob_dlg->type[1] == L'A')) {//払い出し機もTripper用のDialog利用
					tmpwnd = CreateDialog(pPrInst->inf.hInstance, L"IDD_TRP_HANDLE", hWnd, (DLGPROC)PR_TRPPANEL_PROC);
				}
				else if ((pmob_dlg->type[0] == L'S') && (pmob_dlg->type[1] == L'I')) {
					tmpwnd = CreateDialog(pPrInst->inf.hInstance, L"IDD_SILO_HANDLE", hWnd, (DLGPROC)PR_SILOPANEL_PROC);
				}
				else;
				wsprintf(szBuf, pmob_dlg->name);
				SetWindowText(tmpwnd, szBuf);
			}
		}break;
		case IDM_PR_ACT_DEACT: {
			if (pmob_dlg != nullptr) {
				if ((pmob_dlg->type[0] == L'C') && (pmob_dlg->type[1] == L'U')) {
					if(pmob_dlg->status == MOB_STAT_IDLE)pmob_dlg->status = MOB_STAT_ACT0;
					else pmob_dlg->status = MOB_STAT_IDLE;
				}
				else if ((pmob_dlg->type[0] == L'T') && (pmob_dlg->type[1] == L'R')) {
					CTripper * ptrp = (CTripper *)pmob_dlg;
					if (ptrp->get_command() & COM_TRP_DISCHARGE) {
						ptrp->reset_command(COM_TRP_DISCHARGE);
					}
					else {
						ptrp->set_command(COM_TRP_DISCHARGE);
					}
				}
				else if ((pmob_dlg->type[0] == L'S') && (pmob_dlg->type[1] == L'C')) {
					CScraper * ptrp = (CScraper *)pmob_dlg;
					if (ptrp->get_command() & COM_SCRP_DISCHARGE) {
						ptrp->reset_command(COM_TRP_DISCHARGE);
					}
					else {
						ptrp->set_command(COM_SCRP_DISCHARGE);
					}
				}
				else if ((pmob_dlg->type[0] == L'H') && (pmob_dlg->type[1] == L'A')) {
					CHarai * pharai = (CHarai *)pmob_dlg;
					if (pharai->get_command() & COM_HARAI_DISCHARGE) {
						pharai->reset_command(COM_HARAI_DISCHARGE);
					}
					else {
						pharai->set_command(COM_HARAI_DISCHARGE);
					}
				}
				else {

				}
			}
		}break;
		case IDM_PR_DMP_CHNG: {
			if (pmob_dlg != nullptr) {
				if ((pmob_dlg->type[0] == L'B') && (pmob_dlg->type[1] == L'C')) {
					CBC* pbc = (CBC *)pmob_dlg;
					int idev;
					if (pbc->BCtype & BC_TSHOOT4) idev = 4;
					else if (pbc->BCtype & BC_TSHOOT) idev = 3;
					else if (pbc->BCtype & BC_2WAY) idev = 3;
					else  idev = 2;

					pbc->head_unit.pos = (pbc->head_unit.pos + 1) % idev;
					if (pbc->head_unit.pos == BC_LINK_REVERSE) pbc->b_rverse = TRUE;
					else  pbc->b_rverse = FALSE;
				}
				else if ((pmob_dlg->type[0] == L'S') && (pmob_dlg->type[1] == L'C')) {
					CScraper* pscrp = (CScraper*)pmob_dlg;
					if (pscrp->SCRPtype == SCRP_8) {
						if (pscrp->command < COM_SCRP_ACT1) pscrp->command = COM_SCRP_ACT1;
						else if (pscrp->command > COM_SCRP_ACT2) pscrp->command = COM_SCRP_ACT1;
						else pscrp->command *=2;
					}
					else if (pscrp->SCRPtype == SCRP_9_2) {
						if (pscrp->command < COM_SCRP_ACT1) pscrp->command = COM_SCRP_ACT1;
						else if (pscrp->command > COM_SCRP_ACT7) pscrp->command = COM_SCRP_ACT1;
						else pscrp->command *= 2;
					}
					else if (pscrp->SCRPtype == SCRP_23) {
						if (pscrp->command < COM_SCRP_ACT1) pscrp->command = COM_SCRP_ACT1;
						else if (pscrp->command > COM_SCRP_ACT11) pscrp->command = COM_SCRP_ACT1;
						else pscrp->command *= 2;
					}
					else;
				}
				else;
			}
		}break;
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
		
		SetTimer(hWnd, ID_PR_TIMER, 200, NULL);
		
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;

		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 20, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER: {
		wsprintf(szBuf, TEXT("hpos_i BC1A[0]:%04d [1]:%04d [2]:%04d [3]:%04d [4]:%04d [5]:%04d"), pstMobs->mobs.bc[0][0].belt[0].weight, pstMobs->mobs.bc[0][0].belt[1].weight, pstMobs->mobs.bc[0][0].belt[2].weight, pstMobs->mobs.bc[0][0].belt[3].weight, pstMobs->mobs.bc[0][0].belt[4].weight, pstMobs->mobs.bc[0][0].belt[5].weight);
		TextOut(pPrInst->stdisp.hdc_mem_inf, pPrInst->stdisp.bgw - 500, 20, szBuf, lstrlen(szBuf));//マウス位置情報書き込み
		update_disp();
		break;
	}
	case WM_MOUSEMOVE: {
		mpts.x = LOWORD(lp); mpts.y = HIWORD(lp);//マウス位置
		PatBlt(pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, WHITENESS);//情報画面一旦クリア
		wsprintf(szBuf, TEXT("mouse x:%04d  y:%04d"), mpts.x, mpts.y);
		TextOut(pPrInst->stdisp.hdc_mem_inf, pPrInst->stdisp.bgw - 200, 5, szBuf, lstrlen(szBuf));//マウス位置情報書き込み
		
		pmob = stdisp.mobmap[(mpts.x - DISP_OFFSET_X) >> 3][(mpts.y - DISP_OFFSET_Y) >> 3];
		SendMessage(hwndtip, MSGID_UPDATE_DISP_TIP, 0L, (LPARAM)pmob);
		MoveWindow(hwndtip, mpts.x + 20, mpts.y -20, TIP_WND_W, TIP_WND_H,TRUE);
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
	case WM_RBUTTONDOWN: {
		mpts.x = LOWORD(lp); mpts.y = HIWORD(lp);
		hpopmenu = LoadMenu((HINSTANCE)GetWindowLong(hWnd,GWLP_HINSTANCE),L"PR_POPUP");
		pmob_dlg = stdisp.mobmap[(mpts.x - DISP_OFFSET_X) >> 3][(mpts.y - DISP_OFFSET_Y) >> 3];
		hsubmenu = GetSubMenu(hpopmenu, 0);
		

		if (pmob_dlg != nullptr) {
			if ((pmob_dlg->type[0] == L'C') && (pmob_dlg->type[1] == L'U')) {
				EnableMenuItem(hsubmenu, IDM_PR_ACT_DEACT, MF_ENABLED);
			}
			else if ((pmob_dlg->type[0] == L'T') && (pmob_dlg->type[1] == L'R')) {
				EnableMenuItem(hsubmenu, IDM_PR_ACT_DEACT, MF_ENABLED);
			}
			else if ((pmob_dlg->type[0] == L'S') && (pmob_dlg->type[1] == L'C')) {
				EnableMenuItem(hsubmenu, IDM_PR_ACT_DEACT, MF_ENABLED);
			}
			else if ((pmob_dlg->type[0] == L'H') && (pmob_dlg->type[1] == L'A')) {
				EnableMenuItem(hsubmenu, IDM_PR_ACT_DEACT, MF_ENABLED);
			}
			else {
				EnableMenuItem(hsubmenu, IDM_PR_ACT_DEACT, MF_GRAYED);
			}

			if ((pmob_dlg->type[0] == L'B') && (pmob_dlg->type[1] == L'C')) {
				EnableMenuItem(hsubmenu, IDM_PR_DMP_CHNG, MF_ENABLED);
			}
			else if ((pmob_dlg->type[0] == L'S') && (pmob_dlg->type[1] == L'C')) {
				EnableMenuItem(hsubmenu, IDM_PR_ACT_DEACT, MF_ENABLED);
			}
			else {
				EnableMenuItem(hsubmenu, IDM_PR_DMP_CHNG, MF_GRAYED);
			}
		}
		else {
			EnableMenuItem(hsubmenu, IDM_PR_OPEN_DLG, MF_GRAYED);
			EnableMenuItem(hsubmenu, IDM_PR_ACT_DEACT, MF_GRAYED);
			EnableMenuItem(hsubmenu, IDM_PR_DMP_CHNG, MF_GRAYED);
		}

		POINT pt;
		ClientToScreen(hWnd, &pt);
		TrackPopupMenu(hsubmenu, TPM_LEFTALIGN, mpts.x + 150, mpts.y, 0, hWnd, NULL);
		DestroyMenu(hpopmenu);

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
		KillTimer(hWnd, ID_PR_TIMER);
		break;
	}
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
};


LRESULT CPublicRelation::TipWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	static TCHAR szBuf[128];
	HDC hdc_tip;

	switch (msg) {
	case WM_CREATE: {

		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;

		hdc_tip = BeginPaint(hWnd, &ps);
		SetBkMode(hdc_tip, TRANSPARENT);
		SetTextColor(hdc_tip, RGB(128, 128, 128));
		TextOut(hdc_tip, 0, 0, szBuf, lstrlen(szBuf));
		EndPaint(hWnd, &ps);
		break;
	}
	case MSGID_UPDATE_DISP_TIP: {
		CMob* pmob;
		pmob = (CMob *)lp;
		if (pmob != nullptr) {
			wsprintf(szBuf, pmob->name);
		}
		else {
			wsprintf(szBuf, TEXT("#####"));
		}
	}break;
	case WM_DESTROY:
		break;

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

void CPublicRelation::set_mobmap(int com) {

	CMob* pobj;
	if (com == COM_MOBMAP_ALL) {
		//PANEL
		for (int i = 0; i < NUM_OF_EROOM; i++) {
			pobj = pstMobs->pmobs[MOB_ID_EROOM][i];
			putobj2map(pobj);
		}

		//Equipment
		//BC
		for (int i = 0; i < BC_LINES; i++) {
			for (int j = 0; j < BC_LINE_NUM; j++) {
				pobj = pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j];
				putobj2map(pobj);
			}
		}
		//SILO
		for (int i = 0; i < SILO_LINES; i++) {
			for (int j = 0; j < SILO_LINE_NUM; j++) {
				pobj = pstMobs->pmobs[MOB_ID_SILO][i*SILO_LINE_NUM + j];
				putobj2map(pobj);
			}
		}
		//CUL
		for (int i = 0; i < NUM_OF_CUL; i++) {
			pobj = pstMobs->pmobs[MOB_ID_CUL][i];
			putobj2map(pobj);
		}
		//CRUSHER
		for (int i = 0; i < NUM_OF_CRUSH; i++) {
			pobj = pstMobs->pmobs[MOB_ID_CRUSH][i];
			putobj2map(pobj);
		}
		//HARAIDASHIKI
		for (int i = 0; i < NUM_OF_HARAI; i++) {
			pobj = pstMobs->pmobs[MOB_ID_HARAI][i];
			putobj2map(pobj);
		}
		//TRIPPER
		for (int i = 0; i < NUM_OF_TRIPPER; i++) {
			pobj = pstMobs->pmobs[MOB_ID_TRIPPER][i];
			putobj2map(pobj);
		}
		//SCRAPER
		for (int i = 0; i < NUM_OF_SCRAPER; i++) {
			pobj = pstMobs->pmobs[MOB_ID_SCRAPER][i];
			putobj2map(pobj);
		}
	}
	else if (com == COM_MOBMAP_5A) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_A*BC_LINE_NUM +BC_L5];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_TRIPPER][LINE_A];
		putobj2map(pobj);
	}
	else if (com == COM_MOBMAP_5B) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_B*BC_LINE_NUM + BC_L5];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_TRIPPER][LINE_B];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_5C) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_C*BC_LINE_NUM + BC_L5];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_TRIPPER][LINE_C];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_11A) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_A*BC_LINE_NUM + BC_L11];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_11A];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_11B) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_B*BC_LINE_NUM + BC_L11];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_11B];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_11C) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_C*BC_LINE_NUM + BC_L11];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_11C];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_12A) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_A*BC_LINE_NUM + BC_L12];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_12A];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_12B) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_B*BC_LINE_NUM + BC_L12];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_12B];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_12C) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_C*BC_LINE_NUM + BC_L12];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_12C];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_13A) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_A*BC_LINE_NUM + BC_L13];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_13A];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_13B) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_B*BC_LINE_NUM + BC_L13];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_13B];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_13C) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_C*BC_LINE_NUM + BC_L13];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_HARAI][HARAI_13C];
		putobj2map(pobj);

	}
	else if (com == COM_MOBMAP_8A) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_A*BC_LINE_NUM + BC_L8];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_SCRAPER][LINE_D_SCR];
		putobj2map(pobj);
	}
	else if (com == COM_MOBMAP_8B) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_B*BC_LINE_NUM + BC_L8];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_SCRAPER][LINE_E_SCR];
		putobj2map(pobj);
	}
	else if (com == COM_MOBMAP_9_2A) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_A*BC_LINE_NUM + BC_L9_2];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_SCRAPER][LINE_F_SCR];
		putobj2map(pobj);
	}
	else if (com == COM_MOBMAP_9_2B) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_B*BC_LINE_NUM + BC_L9_2];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_SCRAPER][LINE_G_SCR];
		putobj2map(pobj);
	}
	else if (com == COM_MOBMAP_23A) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_A*BC_LINE_NUM + BC_L23];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_SCRAPER][LINE_H_SCR];
		putobj2map(pobj);
	}
	else if (com == COM_MOBMAP_23B) {
		pobj = pstMobs->pmobs[MOB_ID_BC][LINE_B*BC_LINE_NUM + BC_L23];
		putobj2map(pobj);
		pobj = pstMobs->pmobs[MOB_ID_SCRAPER][LINE_I_SCR];
		putobj2map(pobj);
	}
	else;
}

void CPublicRelation::putobj2map(CMob* pobj) {

	if (pobj->exist == ON) {
		int map_x = pobj->area.x >> 3;
		int map_y;
		if (pobj->b_bmp_aline_bottom) {
			map_y = (pobj->area.y - pobj->area.h) >> 3;
		}
		else {
			map_y = pobj->area.y >> 3;
		}
		for (int ii = 0; ii < (pobj->area.w >> 3); ii++) {
			for (int jj = 0; jj < (pobj->area.h >> 3); jj++) {
				stdisp.mobmap[map_x + ii][map_y + jj] = pobj;
			}
		}
	}
};

void CPublicRelation::update_disp() {
	static int i_img2;
	int mobw, mobh, mobx, moby;

	//i_img2 += 1; if (i_img2 > 3)i_img2 = 1;

	BitBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_bg, 0, 0, SRCCOPY);

	//#BC
	CBC* pbc;

	//輪郭描画
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(NULL_BRUSH));
	for (int i = 0; i < BC_LINES; i++) {

		SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(DC_PEN));
		if(i==0)SetDCPenColor(pPrInst->stdisp.hdc_mem0, RGB(255, 0, 255));
		else if(i == 1)SetDCPenColor(pPrInst->stdisp.hdc_mem0, RGB(0, 255, 0));
		else SetDCPenColor(pPrInst->stdisp.hdc_mem0, RGB(0, 0, 255));


		for (int j = 0; j < BC_LINE_NUM; j++) {
			if ((pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j])->exist == ON) {
				pbc = (CBC*)(pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j]);
				Rectangle(pPrInst->stdisp.hdc_mem0, pbc->area.x, pbc->area.y, pbc->area.x + pbc->area.w, pbc->area.y + pbc->area.h);
			}
		}
	}
	//ヘッド位置描画
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(DC_BRUSH));
	SetDCBrushColor(pPrInst->stdisp.hdc_mem0, RGB(192, 0, 192));
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(NULL_PEN));
	//SetDCPenColor(pPrInst->stdisp.hdc_mem0, BLACK_PEN);
	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {
			if ((pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j])->exist == ON) {
				pbc =(CBC*)( pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j]);
				mobx = pbc->area.x + pbc->headpos_pix;
				moby = pbc->area.y;
				if ((pbc->dir) & MASK_DIR_Y) {//縦配置
					if ((pbc->dir) & MASK_DIR_UP) {//逆向き
						mobx = pbc->area.x + pbc->area.w ;
						moby = pbc->area.y + pbc->area.h - pbc->headpos_pix;
						Ellipse(pPrInst->stdisp.hdc_mem0, mobx, moby, mobx + BC_HEAD_SIZE, moby + BC_HEAD_SIZE);
					}
					else {
						mobx = pbc->area.x + pbc->area.w;
						moby = pbc->area.y + pbc->headpos_pix;
						Ellipse(pPrInst->stdisp.hdc_mem0, mobx, moby, mobx + BC_HEAD_SIZE, moby + BC_HEAD_SIZE);
					}
				}
				else {//横配置
					if ((pbc->dir) & MASK_DIR_LEFT) {//逆向き
						mobx = pbc->area.x + pbc->area.w - pbc->headpos_pix;
						moby = pbc->area.y + pbc->area.h;
						Ellipse(pPrInst->stdisp.hdc_mem0, mobx, moby, mobx + BC_HEAD_SIZE, moby + BC_HEAD_SIZE);
					}
					else {
						mobx = pbc->area.x + pbc->headpos_pix;
						moby = pbc->area.y;
						Ellipse(pPrInst->stdisp.hdc_mem0, mobx, moby, mobx - BC_HEAD_SIZE, moby - BC_HEAD_SIZE);
					}
				}
			}
		}
	}
	//BCリンク位置描画
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(DC_PEN));
	SetDCPenColor(pPrInst->stdisp.hdc_mem0, RGB(255, 150, 0));
	POINT linkpt[2];
	CBC* pbc2;
	CCUL* pcul;
	linkpt[0].x = 0; linkpt[0].y = 0; linkpt[1].x = 300; linkpt[1].y = 300;
	for (int i = 0; i < NUM_OF_CUL; i++) {
		pcul = &(pstMobs->mobs.cul[i]);
		if (pcul->exist == ON) {
			linkpt[0].x = pcul->area.x; linkpt[0].y = pcul->area.y + (pcul->area.h)/2;
			pbc2 = pcul->bclink[pcul->bc_selbc];
			linkpt[1] = pbc2->imgpt_rcv[pcul->bclink_i[pcul->bc_selbc]];
			MoveToEx(pPrInst->stdisp.hdc_mem0, linkpt[0].x, linkpt[0].y, NULL);
			LineTo(pPrInst->stdisp.hdc_mem0, linkpt[1].x, linkpt[1].y);
		}
	}
	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {
			pbc = &(pstMobs->mobs.bc[i][j]);
			if(pbc->exist == ON){
				linkpt[0] = pbc->imgpt_top[pbc->head_unit.pos];
				pbc2 = pbc->bclink[pbc->head_unit.pos];
				if ((pbc->BCtype & BC_TRP)|| (pbc->BCtype & BC_SQR)) {
					if (pbc->silolink[0]->dir & MASK_DIR_Y) {
						linkpt[1].x = pbc->silolink[0]->area.x; linkpt[1].y = pbc->silolink[0]->area.y + pbc->silolink[0]->area.h;
					}
					else {
						linkpt[1].x = pbc->silolink[0]->area.x + pbc->silolink[0]->area.w; linkpt[1].y = pbc->silolink[0]->area.y + pbc->silolink[0]->area.h / 2;
					}

				}
				else {
					linkpt[1] = pbc2->imgpt_rcv[pbc->bclink_i[pbc->head_unit.pos]];
				}
				MoveToEx(pPrInst->stdisp.hdc_mem0, linkpt[0].x, linkpt[0].y, NULL);
				LineTo(pPrInst->stdisp.hdc_mem0, linkpt[1].x, linkpt[1].y);
			}
		}
	}


	//搬送石炭描画
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(GRAY_BRUSH));
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(NULL_PEN));
	int ptl, ptt, ptr, ptb;

	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {
			pbc = &(pstMobs->mobs.bc[i][j]);
			if (pbc->exist == ON) {
				int num_draw,num_accumlate,i_accum;
				int level,level100;
				if ((pbc->dir) & MASK_DIR_Y) num_draw = pbc->area.h / BC_COAL_DISP_PIXW;//縦配置 RECT描画個数
				else num_draw = pbc->area.w / BC_COAL_DISP_PIXW;//横配置 RECT描画個数

	//			num_accumlate = (pbc->pix2mm*BC_COAL_DISP_PIXW)>>10;//1要素描画の為の積算数　ベルト長さ1024mm単位
				num_accumlate = pbc->belt_size/num_draw;//1要素描画の為の積算数　ベルト長さ1024mm単位

				i_accum = pbc->ihead;//計算対象belt配列のインデックス 以降ヘッド位置から描画となる
				level100 = pbc->Kg100perM * num_accumlate;//ベルト搬送能力による1m辺りの100％荷重量　x　描画で纏めるm数

				for (int k = 0; k < num_draw; k++) {
					//描画範囲の石炭積算重量
					level = 0;
					for (int i_sum = 0; i_sum < num_accumlate; i_sum++,i_accum++) {
					//	int i_cal = i_accum + pbc->ihead; if (i_cal > pbc->belt_size) i_cal -= pbc->belt_size;
						if (!(i_accum < pbc->belt_size))i_accum = 0;
						level += pbc->belt[i_accum].weight;
					}

					if ((pbc->dir) & MASK_DIR_Y) {//縦配置
						ptr = pbc->area.x + pbc->area.w;
						ptl = ptr - (pbc->area.w * level) / level100;
						if ((pbc->dir) & MASK_DIR_X) {//上向
							ptt = pbc->area.y + BC_COAL_DISP_PIXW * k;
							ptb = ptt + BC_COAL_DISP_PIXW;
						}
						else {
							ptb = pbc->area.y + pbc->area.h - BC_COAL_DISP_PIXW * k;
							ptt = ptb - BC_COAL_DISP_PIXW;
						}

					}
					else {
						ptb = pbc->area.y + pbc->area.h;
						ptt = ptb - (pbc->area.h * level) / level100;
						if ((pbc->dir) & MASK_DIR_X) {//左行
							ptl = pbc->area.x + BC_COAL_DISP_PIXW * k;
							ptr = ptl + BC_COAL_DISP_PIXW;
						}
						else {
							ptr = pbc->area.x + pbc->area.w - BC_COAL_DISP_PIXW * k;
							ptl = ptr - BC_COAL_DISP_PIXW;
						}
					}
					Rectangle(pPrInst->stdisp.hdc_mem0, ptl, ptt, ptr, ptb);
				}

			}
		}
	}

	//#クラッシャー
	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_CRUSH][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_CRUSH][i]->status;
		mobx = pstMobs->pmobs[MOB_ID_CRUSH][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_CRUSH][i]->area.y;
		mobw = pstMobs->pmobs[MOB_ID_CRUSH][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_CRUSH][i]->bmph;
		//AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}

	//#払い出し機
	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_HARAI][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_HARAI; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_HARAI][i]->status;
		mobx = pstMobs->pmobs[MOB_ID_HARAI][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_HARAI][i]->area.y;
		mobw = pstMobs->pmobs[MOB_ID_HARAI][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_HARAI][i]->bmph;
		//	AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}

	//#CUL
	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_CUL][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_CUL; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_CUL][i]->status;
		mobx = pstMobs->pmobs[MOB_ID_CUL][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_CUL][i]->area.y;
		mobw = pstMobs->pmobs[MOB_ID_CUL][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_CUL][i]->bmph;
		//AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}

	//#トリッパー
	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_TRIPPER][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_TRIPPER; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_TRIPPER][i]->status;
		mobw = pstMobs->pmobs[MOB_ID_TRIPPER][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_TRIPPER][i]->bmph;
		mobx = pstMobs->pmobs[MOB_ID_TRIPPER][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_TRIPPER][i]->area.y - mobh;
		//AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}

	//#スクレーパー

	for (int i = 0; i < NUM_OF_SCRAPER; i++) {
		SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_SCRAPER][i]->hBmp_mob);
		CMob* pobj = pstMobs->pmobs[MOB_ID_SCRAPER][i];
		i_img2 = pobj->status;
		mobw = pobj->bmpw;	mobh = pobj->bmph;
		mobx = pobj->area.x;	moby = pobj->area.y - mobh;
		//AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}

	//#電気室
	SelectObject(pPrInst->stdisp.hdc_mem_mob, pstMobs->pmobs[MOB_ID_EROOM][0]->hBmp_mob);
	for (int i = 0; i < NUM_OF_EROOM; i++) {
		i_img2 = pstMobs->pmobs[MOB_ID_EROOM][i]->status;
		mobx = pstMobs->pmobs[MOB_ID_EROOM][i]->area.x;	moby = pstMobs->pmobs[MOB_ID_EROOM][i]->area.y;
		mobw = pstMobs->pmobs[MOB_ID_EROOM][i]->bmpw;	mobh = pstMobs->pmobs[MOB_ID_EROOM][i]->bmph;
		//AlphaBlend(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, pPrInst->stdisp.bf);
		TransparentBlt(pPrInst->stdisp.hdc_mem0, mobx, moby, mobw, mobh, pPrInst->stdisp.hdc_mem_mob, i_img2 * mobw, 0, mobw, mobh, RGB(255, 255, 255));
	}



	//#サイロ
	CSilo* psilo;

	//輪郭描画
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(NULL_BRUSH));
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(DC_PEN));
	SetDCPenColor(pPrInst->stdisp.hdc_mem0, RGB(100, 100, 100));

	for (int i = 0; i < SILO_LINES; i++) {
		for (int j = 0; j < SILO_LINE_NUM; j++) {
			if ((pstMobs->pmobs[MOB_ID_SILO][i*SILO_LINE_NUM + j])->exist == ON) {
				psilo = (CSilo*)(pstMobs->pmobs[MOB_ID_SILO][i*SILO_LINE_NUM + j]);
				if (psilo->SILOtype == SILO_TYPE_BIO) {
					for(int i_column = 0; i_column < SILO_COLUMN_NUM_BIO ; i_column++)
						Rectangle(pPrInst->stdisp.hdc_mem0, psilo->area.x, psilo->area.y + psilo->pix_columw * i_column, psilo->area.x + psilo->area.w, psilo->area.y + psilo->pix_columw * (i_column +1));
				}
				else if (psilo->SILOtype == SILO_TYPE_BANK) {
					for (int i_column = 0; i_column < SILO_COLUMN_NUM_BANK; i_column++)
						Rectangle(pPrInst->stdisp.hdc_mem0, psilo->area.x + psilo->pix_columw * i_column, psilo->area.y, psilo->area.x + psilo->pix_columw * (i_column + 1), psilo->area.y + psilo->area.h );
				}
				else {
					Rectangle(pPrInst->stdisp.hdc_mem0, psilo->area.x, psilo->area.y, psilo->area.x + psilo->area.w, psilo->area.y + psilo->area.h);
				}
			}
		}
	}

	//搬送石炭描画
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(GRAY_BRUSH));
	SelectObject(pPrInst->stdisp.hdc_mem0, GetStockObject(NULL_PEN));
	//貯炭描画
	for (int i = 0; i < SILO_LINES; i++) {
		for (int j = 0; j < SILO_LINE_NUM; j++) {
			psilo = &(pstMobs->mobs.silo[i][j]);
			if (psilo->exist == ON) {
				if (psilo->SILOtype == SILO_TYPE_BIO) {
					for (int k = 0; k < SILO_COLUMN_NUM_BIO; k++) {
						ptr = psilo->area.x + psilo->area.w;
						ptl = ptr - (psilo->area.w * psilo->column[k].weight) / psilo->capa1;
						ptt = psilo->area.y + psilo->pix_columw * k;
						ptb = ptt + psilo->pix_columw;
						Rectangle(pPrInst->stdisp.hdc_mem0, ptl, ptt, ptr, ptb);
					}
				}
				else if (psilo->SILOtype == SILO_TYPE_BANK) {
					for (int k = 0; k < SILO_COLUMN_NUM_BANK; k++) {
						ptr = psilo->area.x + psilo->pix_columw * (k +1);
						ptl = ptr - (psilo->pix_columw * psilo->column[k].weight) / psilo->capa1;
						ptt = psilo->area.y;
						ptb = ptt + psilo->area.h;
						Rectangle(pPrInst->stdisp.hdc_mem0, ptl, ptt, ptr, ptb);
					}
				}
				else {
					for (int k = 0; k < SILO_COLUMN_NUM; k++) {
						ptr = psilo->area.x + psilo->area.w;
						ptl = ptr - (psilo->area.w * psilo->column[k].weight) / psilo->capa1;
						ptt = psilo->area.y + psilo->pix_columw * k;
						ptb = ptt + psilo->pix_columw;
						Rectangle(pPrInst->stdisp.hdc_mem0, ptl, ptt, ptr, ptb);
					}
				}
			}
		}
	}


	if (b_infchecked)TransparentBlt(pPrInst->stdisp.hdc_mem0, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, pPrInst->stdisp.hdc_mem_inf, 0, 0, pPrInst->stdisp.bgw, pPrInst->stdisp.bgh, RGB(255, 255, 255));
	InvalidateRect(pPrInst->inf.hWnd_work, NULL, FALSE);
	return;
};
LRESULT CPublicRelation::PR_BCPANEL_PROC(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	PAINTSTRUCT ps;
	
	static CBC* pbc;

	switch (msg) {
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case IDC_DMPER_RADIO1: {
			pbc->head_unit.pos = 0;
			pbc->b_rverse = FALSE;
			SendMessage(GetDlgItem(hWnd, IDC_DMPER_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
		}break;
		case IDC_DMPER_RADIO2: {
			pbc->head_unit.pos = 1;
			pbc->b_rverse = FALSE;
			SendMessage(GetDlgItem(hWnd, IDC_DMPER_RADIO2), BM_SETCHECK, BST_CHECKED, 0L);
		}break;
		case IDC_DMPER_RADIO3: {
			pbc->head_unit.pos = 2;
			pbc->b_rverse = TRUE;
			SendMessage(GetDlgItem(hWnd, IDC_DMPER_RADIO3), BM_SETCHECK, BST_CHECKED, 0L);
		}break;
		case IDC_PR_CHECK_BC_PUTLOAD: {
			if (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_PR_CHECK_BC_PUTLOAD)) {
				pbc->put_test_load = (pbc->l >> 10) - 10;//テールから10m位置
				//			pbc->put_test_load = 20;
			}
			else {
				pbc->put_test_load = 0;
			}

		}break;


		case IDCANCEL: {
			EndDialog(hWnd, LOWORD(wp));
		}break;
		default: return FALSE;
		}
	}break;
	case WM_INITDIALOG: {
		pbc = (CBC*)pmob_dlg;//MOUSE MOVEでオブジェクトのポインタはセットされている
		if(pbc->head_unit.pos == 0) SendMessage(GetDlgItem(hWnd, IDC_DMPER_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
		else if(pbc->head_unit.pos == 1) SendMessage(GetDlgItem(hWnd, IDC_DMPER_RADIO2), BM_SETCHECK, BST_CHECKED, 0L);
		else if (pbc->head_unit.pos == 2) SendMessage(GetDlgItem(hWnd, IDC_DMPER_RADIO3), BM_SETCHECK, BST_CHECKED, 0L);

		if(pbc->put_test_load)SendMessage(GetDlgItem(hWnd, IDC_PR_CHECK_BC_PUTLOAD), BM_SETCHECK, BST_CHECKED, 0L);
		else SendMessage(GetDlgItem(hWnd, IDC_PR_CHECK_BC_PUTLOAD), BM_SETCHECK, BST_UNCHECKED, 0L);
	}break;
	case WM_PAINT: {
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
};
LRESULT CPublicRelation::PR_TRPPANEL_PROC(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	PAINTSTRUCT ps;
	wstring wstr;
	static CTripper* ptrp;
	static CHarai* pharai;
	static CScraper* pscrp;


	switch (msg) {
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case IDC_TRPCHECK_COM_MOVE: {

			if (pmob_dlg->type[0] == L'T') {
				if (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_TRPCHECK_COM_MOVE)) {
					ptrp->set_command(COM_TRP_MOVE);
					int n = GetDlgItemText(hWnd, IDC_TRPEDIT_COM_TARGET, (LPTSTR)wstr.c_str(), 128);
					if (n) ptrp->set_target(stoi(wstr));
				}
				else {
					ptrp->reset_command(COM_TRP_MOVE);
				}
			}
			else if (pmob_dlg->type[0] == L'H') {
				if (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_TRPCHECK_COM_MOVE)) {
					pharai->set_command(COM_HARAI_MOVE);
					int n = GetDlgItemText(hWnd, IDC_TRPEDIT_COM_TARGET, (LPTSTR)wstr.c_str(), 128);
					if (n) pharai->set_target(stoi(wstr));
				}
				else {
					pharai->reset_command(COM_HARAI_MOVE);
				}
			}
			else if (pmob_dlg->type[0] == L'S') {
				if (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_TRPCHECK_COM_MOVE)) {
					ptrp->set_command(COM_SCRP_ACT1);
					int n = GetDlgItemText(hWnd, IDC_TRPEDIT_COM_TARGET, (LPTSTR)wstr.c_str(), 128);
					if (n) ptrp->set_target(stoi(wstr));
				}
				else {
					ptrp->reset_command(COM_SCRP_ACT1);
				}
			}
			else;
		}break;
		case IDC_TRPCHECK_COM_DISCHARGE: {
			if (pmob_dlg->type[0] == L'T') {
				if (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_TRPCHECK_COM_DISCHARGE)) {
					ptrp->set_command(COM_TRP_DISCHARGE);
				}
				else {
					ptrp->reset_command(COM_TRP_DISCHARGE);
				}
			}
			else if (pmob_dlg->type[0] == L'H') {
				if (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_TRPCHECK_COM_DISCHARGE)) {
					pharai->set_command(COM_HARAI_DISCHARGE);
				}
				else {
					pharai->reset_command(COM_HARAI_DISCHARGE);
				}
			}
			if (pmob_dlg->type[0] == L'S') {
				if (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_TRPCHECK_COM_DISCHARGE)) {
					pscrp->set_command(COM_SCRP_DISCHARGE);
				}
				else {
					pscrp->reset_command(COM_SCRP_DISCHARGE);
				}
			}
			else;
		}break;

		case IDCANCEL: {
			EndDialog(hWnd, LOWORD(wp));
		}break;
		case IDOK: {
			int n = GetDlgItemText(hWnd, IDC_TRPEDIT_COM_TARGET, (LPTSTR)wstr.c_str(), 128);
			if (pmob_dlg->type[0] == L'T') {
				if (n) ptrp->set_target(stoi(wstr));
			}
			else if (pmob_dlg->type[0] == L'H') {
				if (n) pharai->set_target(stoi(wstr));
			}
			else;
		}break;
		default: return FALSE;
		}
	}break;
	case WM_INITDIALOG: {
		ptrp = (CTripper*)pmob_dlg;//MOUSE MOVEでオブジェクトのポインタはセットされている
		pharai = (CHarai*)pmob_dlg;//トリッパーと払い出し機共用
		pscrp = (CScraper*)pmob_dlg;//トリッパーと払い出し機共用
		if (pmob_dlg->type[0] == L'T'){
			if (ptrp->command & COM_TRP_MOVE) SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_MOVE), BM_SETCHECK, BST_CHECKED, 0L);
			else  SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_MOVE), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (ptrp->command & COM_TRP_DISCHARGE) SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_DISCHARGE), BM_SETCHECK, BST_CHECKED, 0L);
			else  SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_DISCHARGE), BM_SETCHECK, BST_UNCHECKED, 0L);
		}
		else if (pmob_dlg->type[0] == L'H') {
			if (pharai->command & COM_HARAI_MOVE) SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_MOVE), BM_SETCHECK, BST_CHECKED, 0L);
			else  SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_MOVE), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (pharai->command & COM_TRP_DISCHARGE) SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_DISCHARGE), BM_SETCHECK, BST_CHECKED, 0L);
			else  SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_DISCHARGE), BM_SETCHECK, BST_UNCHECKED, 0L);
		}
		else if (pmob_dlg->type[0] == L'S') {
			if (pscrp->command > COM_SCRP_DISCHARGE) SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_MOVE), BM_SETCHECK, BST_CHECKED, 0L);
			else  SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_MOVE), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (pscrp->command & COM_SCRP_DISCHARGE) SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_DISCHARGE), BM_SETCHECK, BST_CHECKED, 0L);
			else  SendMessage(GetDlgItem(hWnd, IDC_TRPCHECK_COM_DISCHARGE), BM_SETCHECK, BST_UNCHECKED, 0L);
		}
		else;


	}break;
	case WM_PAINT: {
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
};
LRESULT CPublicRelation::PR_SILOPANEL_PROC(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	PAINTSTRUCT ps;
	wstring wstr;
	static CSilo* psilo;

	switch (msg) {
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case IDC_PR_BUTTON_SILO_100: {
			for (int i = 0; i < SILO_COLUMN_NUM; i++) {
				psilo->column[i].weight = psilo->capa1;
			}
		}break;
		case IDC_PR_BUTTON_SILO_50: {
			for (int i = 0; i < SILO_COLUMN_NUM; i++) {
				psilo->column[i].weight = psilo->capa1/2;
			}
		}break;
		case IDC_PR_BUTTON_SILO_0: {
			for (int i = 0; i < SILO_COLUMN_NUM; i++) {
				psilo->column[i].weight = 0;
			}
		}break;

		case IDCANCEL: {
			EndDialog(hWnd, LOWORD(wp));
		}break;
		case IDOK: {

		}break;
		default: return FALSE;
		}
	}break;
	case WM_INITDIALOG: {
		psilo = (CSilo*)pmob_dlg;//MOUSE MOVEでオブジェクトのポインタはセットされている
	}break;
	case WM_PAINT: {
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
};

