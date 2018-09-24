#pragma once
#include "ThreadObj.h"

#define PR_MAIN_WND_NAME		TEXT("pr_window")
#define ID_PR_WND				100
#define PR_WND_W				600
#define PR_WND_H				600


class CPublicRelation :
	public CThreadObj
{
public:
	CPublicRelation();
	~CPublicRelation();
	void init_task(void* pobj);
	static CPublicRelation* pPrInst;

private:
	BOOL InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);
	static LRESULT CALLBACK PrWndProc(HWND, UINT, WPARAM, LPARAM);
};

