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
	int sim_accel_rate;//シミュレーションの加速率（シミュレーション時間を早める)
	ULONG ms_lasttime;//シミュレーション時間前回値
	ULONG ms_dt;//シミュレーション時間前回値との時間偏差
	STLOAD load_cul;

	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void set_panel_tip_txt();//タブパネルのStaticテキストを設定

private:
	int cal_cul(int index, STLOAD load, LONG dt, DWORD com);

};

