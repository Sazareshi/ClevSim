#include "stdafx.h"
#include "PublicRelation.h"


CPublicRelation::CPublicRelation()
{
}


CPublicRelation::~CPublicRelation()
{
}

CPublicRelation* CPublicRelation:: pPrInst;

void CPublicRelation::init_task(void* pobj) {

	HINSTANCE hInst;
	hInst = (HINSTANCE)GetWindowLong(inf.hWnd_parent, GWLP_HINSTANCE);
	HWND hWnd;

	pPrInst = (CPublicRelation *)pobj;//スタティック変数にインスタンスポインタ登録
	
	///# 作業ウィンドウ生成
	InitWorkWnd(hInst, PrWndProc, L"test");
	hWnd = CreateWindow(PR_MAIN_WND_NAME, NULL, WS_CHILD,
		0, 0, PR_WND_W, PR_WND_H, inf.hWnd_parent, (HMENU)3000, hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	//inf.hWnd_work = CreateDialog(inf.hInstance, L"IDD_COMM_DLG", inf.hWnd_parent, (DLGPROC)PrWndProc);

	///# INIファイル読み込み
	wchar_t tbuf[32];
//	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	   	 
}

BOOL CPublicRelation::InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName) {
	
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCWSTR)lpzClassName;

	return RegisterClass(&wc);
}

LRESULT CPublicRelation::PrWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	PAINTSTRUCT ps;
	TCHAR edit_wstr[1024];

	switch (msg) {
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case IDC_BUTTON_COM_MSGSEND: {

		}break;
		case IDC_BUTTON_COM_IFSEL: {

		}break;
		case IDC_COMM_CLOSE: {

		}break;
		default: return FALSE;
		}
	}break;
	case WM_INITDIALOG: {
		;
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