#pragma once
#include "ThreadObj.h"
#include "Actor.h"
#include <windowsx.h> //�R�����R���g���[���p
#include <commctrl.h> //�R�����R���g���[���p

#define PR_MAIN_WND_NAME		TEXT("pr_window")
#define ID_PR_WND				100
#define IDC_CHK_INFDISP			5000
#define PR_WND_W				1200
#define PR_WND_H				840
#define PR_WND_X				400
#define PR_WND_Y				0

#define NUM_OF_MOB				32
#define MOB_DISP_ALPHA			144 //���ߓx�@0-255�@0�F����


typedef struct _stPR_DISP
{
	int bgw, bgh;					//�w�i��ʂ̕��A����
	HBITMAP hBmp_bg;				//�w�i��ʃr�b�g�}�b�v�n���h��
	HBITMAP hBmp_mob[NUM_OF_MOB];	//���[�r���O�I�u�W�F�N�g�r�b�g�}�b�v�n���h��
	HDC hdc_mem0;					//������ʃ������f�o�C�X�R���e�L�X�g
	HDC hdc_mem_bg;					//�w�i��ʃ������f�o�C�X�R���e�L�X�g
	HDC hdc_mem_mob;				//�I�u�W�F�N�g��ʃ������f�o�C�X�R���e�L�X�g
	HDC hdc_mem_inf;				//������ʃ������f�o�C�X�R���e�L�X�g
	HFONT hfont_inftext;			//�e�L�X�g�p�t�H���g
	BLENDFUNCTION bf;				//�����ߐݒ�\����

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

	void set_panel_tip_txt();//�^�u�p�l����Static�e�L�X�g��ݒ�
private:
	BOOL InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);
	static LRESULT CALLBACK PrWndProc(HWND, UINT, WPARAM, LPARAM);
	void del_objects();
	void set_image(BOOL req_inf);
};

