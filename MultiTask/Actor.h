#pragma once
#include "ThreadObj.h"
#include "mob.h"
#include "panel.h"
#include "MultiTaskMain.h"

#define COM_CUL_IDLE	0
#define COM_CUL_DISCHARGE	1

class CActor :public CThreadObj
{
public:
	CActor();
	~CActor();

	void init_task(void* pobj);
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
	int cal_cul(int index, STLOAD load, LONG dt, DWORD com);

};

