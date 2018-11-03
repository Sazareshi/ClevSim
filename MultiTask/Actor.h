#pragma once
#include "ThreadObj.h"
#include "mob.h"
#include "panel.h"
#include "MultiTaskMain.h"


class CActor :public CThreadObj
{
public:
	CActor();
	~CActor();

	void init_task(void* pobj);
	void init_bclink();
	static CActor* pActInst;
	static int BMP_WH[MOB_TYPE_NUM][2];
	LPSTMobs pstMobs;
	HANDLE hmue_mobs;
	void routine_work(void *param);
	int sim_accel_rate;//�V�~�����[�V�����̉������i�V�~�����[�V�������Ԃ𑁂߂�)
	ULONG ms_lasttime;//�V�~�����[�V�������ԑO��l
	ULONG ms_dt;//�V�~�����[�V�������ԑO��l�Ƃ̎��ԕ΍�
	STLOAD load_cul;

	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void set_panel_tip_txt();//�^�u�p�l����Static�e�L�X�g��ݒ�

private:
	int cal_cul(DWORD index, STLOAD load, LONG dt, DWORD com);
	int cal_bc(DWORD index, LONG dt, DWORD com);
	int cal_silo(DWORD index, LONG dt, DWORD com);
	int cal_tripper(DWORD index, LONG dt, DWORD com);
	void init_bc();

};

//BC�̃R�[�h�@���/���� Word��pmobs�̔z��̈ʒu��\��
#define BC01A	0x0000
#define BC01B	0x0100
#define BC02A	0x0001
#define BC02B	0x0101
#define BC03A	0x0002
#define BC03B	0x0102
#define BC041A	0x0003
#define BC041B	0x0103
#define BC042A	0x0004
#define BC042B	0x0104
#define BC043A	0x0005
#define BC043B	0x0105
#define BC05A	0x0006
#define BC05B	0x0106
#define BC05A	0x0006
#define BC05C	0x0206

