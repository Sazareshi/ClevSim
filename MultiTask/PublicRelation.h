#pragma once
#include "ThreadObj.h"
#include <windowsx.h> //�R�����R���g���[���p
#include <commctrl.h> //�R�����R���g���[���p

#define PR_MAIN_WND_NAME		TEXT("pr_window")
#define ID_PR_WND				100
#define PR_WND_W				1200
#define PR_WND_H				840
#define PR_WND_X				400
#define PR_WND_Y				0


class CPublicRelation :
	public CThreadObj
{
public:
	CPublicRelation();
	~CPublicRelation();

	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void init_task(void* pobj);
	static CPublicRelation* pPrInst;
	static HBITMAP hBmp_bg, hBmp_mob;
	static HDC hdc_mem0, hdc_mem_bg, hdc_mem_mob, hdc_mem_inf;
	static HIMAGELIST hImgList_harai;	//�����o���@�A�j���C���[�W���X�g

	void set_panel_tip_txt();//�^�u�p�l����Static�e�L�X�g��ݒ�
private:
	BOOL InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);
	static LRESULT CALLBACK PrWndProc(HWND, UINT, WPARAM, LPARAM);
};

