#pragma once
#include "ThreadObj.h"
#include "Actor.h"
#include <windowsx.h> //コモンコントロール用
#include <commctrl.h> //コモンコントロール用

#define PR_MAIN_WND_NAME		TEXT("pr_window")
#define ID_PR_WND				100
#define IDC_CHK_INFDISP			5000
#define PR_WND_W				1200
#define PR_WND_H				840
#define PR_WND_X				400
#define PR_WND_Y				0

#define NUM_OF_MOB				32
#define MOB_DISP_ALPHA			144 //透過度　0-255　0：透明


typedef struct _stPR_DISP
{
	int bgw, bgh;					//背景画面の幅、高さ
	HBITMAP hBmp_bg;				//背景画面ビットマップハンドル
	HBITMAP hBmp_mob[NUM_OF_MOB];	//ムービングオブジェクトビットマップハンドル
	HDC hdc_mem0;					//合成画面メモリデバイスコンテキスト
	HDC hdc_mem_bg;					//背景画面メモリデバイスコンテキスト
	HDC hdc_mem_mob;				//オブジェクト画面メモリデバイスコンテキスト
	HDC hdc_mem_inf;				//文字画面メモリデバイスコンテキスト
	HFONT hfont_inftext;			//テキスト用フォント
	BLENDFUNCTION bf;				//半透過設定構造体

}PR_DISP, *LPPR_DISP;


class CPublicRelation :	public CThreadObj
{
public:
	CPublicRelation();
	~CPublicRelation();

	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void init_task(void* pobj);
	static PR_DISP stdisp;
	static CPublicRelation* pPrInst;

	void set_panel_tip_txt();//タブパネルのStaticテキストを設定
private:
	BOOL InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);
	static LRESULT CALLBACK PrWndProc(HWND, UINT, WPARAM, LPARAM);
	void del_objects();
	void set_image(BOOL req_inf);
};

