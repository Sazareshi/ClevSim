#pragma once
#include "EParts.h"

typedef struct _stPowerSupply{
	double v;
	double hz;
} ST_PS, *LPST_PS;

#define PANEL_DOUT_MAX 16
#define PANEL_AOUT_MAX 16
#define PANEL_POUT_MAX 16
typedef struct _st_PanelOut{
	WORD dout[PANEL_DOUT_MAX];
	WORD aout[PANEL_AOUT_MAX];
	LPST_PS pout[PANEL_POUT_MAX];
} ST_PanelOut, *LPST_PanelOut;
typedef struct _st_PowerSource {
	int Hz;
	int Volt;
	int Ampare;
} ST_PSource, *LPST_PSource;

#define PANEL_MAX 128
#define CITY_POWER_Hz 60

typedef struct _st_PanelOutSmem {//共有メモリ上への配置イメージ
	ST_PanelOut panel_out[PANEL_MAX];
} ST_PanelOutSmem, *LPST_PanelOutSmem;

//####################################
//パネルベースクラス
//####################################
class CPanel
{
public:
	CPanel(void);
	~CPanel(void);

	DWORD ID;
	TCHAR name[16];
	LPST_PanelOut pout;
	ST_PSource power_in[3];//入力電源
public:
	virtual void init_panel() = 0;
	virtual void rcv_signal() = 0;
	virtual void set_signal() = 0;
	virtual void update_status() = 0;
	void Div_64To16(INT64 source64, INT16* target16)
	{
		*target16 = INT16(source64 & 0x000000000000ffff);
		target16++;
		*target16 = INT16((source64 & 0x00000000ffff0000) >> 16);
		target16++;
		*target16 = INT16((source64 & 0x0000ffff00000000) >> 32);
		target16++;
		*target16 = INT16((source64 & 0xffff000000000000) >> 48);
		target16++;
	}

};

class RA_CP1 :CPanel
{
public:
	RA_CP1(void) {};
	~RA_CP1(void) {};
	void init_panel() { return; };
	void rcv_signal() { return; };
	void set_signal() { return; };
	void update_status() { return; };

};

class MC_OCE_A1:CPanel //メタクラ1期A系
{
public:
	MC_OCE_A1(void) ;
	~MC_OCE_A1(void);
	void init_panel() ;
	void rcv_signal() { return; };
	void set_signal() { return; };
	void update_status();
	CMCCB cb52_1a1, cb52_1a2;
	CMC cb1Apmp, cb1A1, cb1A2, cb2A, cb3A1, cb3A2, cb3A3, cbCUL1, cbaux, cbLC;
};
class MC_OCE_B1:CPanel //メタクラ1期B系
{
public:
	MC_OCE_B1(void);
	~MC_OCE_B1(void);
	void init_panel();
	void rcv_signal() { return; };
	void set_signal() { return; };
	void update_status();
	CMCCB cb52_1b1, cb52_1b2;
	CMC cb1Bpmp, cb2Bpmp, cb1B1, cb1B2, cbBA, cb3B1, cb3B2, cb3B3, cbCUL2, cbCUL3, cbaux, cbLC;
};

class CP_OCE_A1 :CPanel //PLC盤1期A系
{
public:
	CP_OCE_A1(void) {};
	~CP_OCE_A1(void) {};
	void init_panel() { return; };
	void rcv_signal() { return; };
	void set_signal() { return; };
	void update_status() { return; };
	void PLC_scan_proc() { return; };
};

class CP_OCE_B1 :CPanel //PLC盤1期A系
{
public:
	CP_OCE_B1(void) {};
	~CP_OCE_B1(void) {};
	void init_panel() { return; };
	void rcv_signal() { return; };
	void set_signal() { return; };
	void update_status() { return; };
	void PLC_scan_proc() { return; };

};
