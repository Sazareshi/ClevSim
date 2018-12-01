#pragma once
#include "ThreadObj.h"
#include "mob.h"
#include <windowsx.h> //�R�����R���g���[���p
#include <commctrl.h> //�R�����R���g���[���p

#define PR_MAIN_WND_NAME		TEXT("pr_window")
#define ID_PR_WND				100
#define IDC_CHK_INFDISP			5000
#define PR_WND_W				1200
#define PR_WND_H				840
#define PR_WND_X				400
#define PR_WND_Y				0
#define TIP_WND_W				100
#define TIP_WND_H				15

#define DISP_OFFSET_X			25
#define DISP_OFFSET_Y			12

#define NUM_OF_MOB				32
#define MOB_DISP_ALPHA			160 //���ߓx�@0-255�@0�F����

#define MSGID_UPDATE_DISP_PR	5001
#define MSGID_UPDATE_DISP_TIP	5002


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
	CMob * mobmap[PR_WND_W / 8 + 1][PR_WND_H / 8 + 1];//UI���擾�pMap

}PR_DISP, *LPPR_DISP;

#define COM_MOBMAP_ALL 0
#define COM_MOBMAP_5A 0x0001
#define COM_MOBMAP_5B 0x0002
#define COM_MOBMAP_5C 0x0003
#define COM_MOBMAP_8A 0x0004
#define COM_MOBMAP_8B 0x0005
#define COM_MOBMAP_9_2A 0x0006
#define COM_MOBMAP_9_2B 0x0007
#define COM_MOBMAP_23A 0x0008
#define COM_MOBMAP_23B 0x0009
#define COM_MOBMAP_11A 0x0010
#define COM_MOBMAP_11B 0x0020
#define COM_MOBMAP_11C 0x0030
#define COM_MOBMAP_12A 0x0040
#define COM_MOBMAP_12B 0x0050
#define COM_MOBMAP_12C 0x0060
#define COM_MOBMAP_13A 0x0070
#define COM_MOBMAP_13B 0x0080
#define COM_MOBMAP_13C 0x0090





class CPublicRelation :	public CThreadObj
{
public:
	CPublicRelation();
	~CPublicRelation();

	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void init_task(void* pobj);
	static PR_DISP stdisp;
	static CPublicRelation* pPrInst;


	void routine_work(void *param);

	void set_panel_tip_txt();//�^�u�p�l����Static�e�L�X�g��ݒ�

	static LPSTMobs pstMobs;
	static HANDLE hmue_mobs;

	static void set_mobmap(int com);
	static LRESULT PR_BCPANEL_PROC(HWND, UINT, WPARAM, LPARAM);
	static LRESULT PR_TRPPANEL_PROC(HWND, UINT, WPARAM, LPARAM);
	static LRESULT PR_SILOPANEL_PROC(HWND, UINT, WPARAM, LPARAM);

private:
	BOOL InitWorkWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);
	BOOL InitTipWnd(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR lpzClassName);
	static LRESULT CALLBACK PrWndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK TipWndProc(HWND, UINT, WPARAM, LPARAM);
	void del_objects();
	void set_image(BOOL req_inf);
	static void update_disp();
	static BOOL b_infchecked;
	static void putobj2map(CMob* pobj);
	static HWND hwndtip;
	static CMob* pmob_dlg;

};

#define BC_HEAD_SIZE	8
