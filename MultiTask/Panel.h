#pragma once


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

#define PANEL_MAX 128
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
