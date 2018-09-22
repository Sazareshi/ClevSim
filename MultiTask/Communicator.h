#pragma once
#include "ThreadObj.h"
#include "CSock.h"
#include "MCtransaction.h"
#include "UDPtransaction.h"
#include "MultiTaskMain.h"
#include "NamedPipe.h"

#define MSGID_SET_WORK_LAMP	WM_USER +1//WORK WINDOWのランプ描画　wp：ランプNo.　lp：色ID
#define COLID_COM_NULL		0
#define COLID_COM_YELLOW	1
#define COLID_COM_RED		2
#define COLID_COM_GREEN		3
#define COLID_COM_GRAY		4
#define COLID_COM_DGREEN	5
#define COLID_COM_WHITE		6

#define NUM_OF_IF			8
#define NUM_OF_CLIENT		3
#define NUM_OF_IF_ACT		2

#define SOCK_T_S			0
#define SOCK_T_C			1
#define SOCK_U_S			2
#define SOCK_U_C			3
#define PIPE_S				4
#define PIPE_C				5

#define COM_LMPD		    10
#define COM_TxRxW		    23
#define COM_LMPY			40
#define COM_LMPTX0          170
#define COM_LMPTX1          235
#define COM_LMPTX2          298
#define COM_LMPRX0          COM_LMPTX0+COM_TxRxW
#define COM_LMPRX1          COM_LMPTX1+COM_TxRxW
#define COM_LMPRX2          COM_LMPTX2+COM_TxRxW


typedef struct _stComLampPack
{
	LPCWSTR ifname[NUM_OF_IF];//IFタイプ表示名
	int tx_ellipse_pos[NUM_OF_CLIENT][4] = {{ COM_LMPTX0, COM_LMPY, COM_LMPTX0 + COM_LMPD, COM_LMPY + COM_LMPD },
											{ COM_LMPTX1, COM_LMPY, COM_LMPTX1 + COM_LMPD, COM_LMPY + COM_LMPD },
											{ COM_LMPTX2, COM_LMPY, COM_LMPTX2 + COM_LMPD, COM_LMPY + COM_LMPD }};
	int tx_color[NUM_OF_IF][NUM_OF_CLIENT] = { { COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE } };

	int rx_ellipse_pos[NUM_OF_CLIENT][4] = {{ COM_LMPRX0, COM_LMPY, COM_LMPRX0 + COM_LMPD, COM_LMPY + COM_LMPD },
											{ COM_LMPRX1, COM_LMPY, COM_LMPRX1 + COM_LMPD, COM_LMPY + COM_LMPD },
											{ COM_LMPRX2, COM_LMPY, COM_LMPRX2 + COM_LMPD, COM_LMPY + COM_LMPD } };
	int rx_color[NUM_OF_IF][NUM_OF_CLIENT] = { { COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE },
												{ COLID_COM_WHITE , COLID_COM_WHITE ,COLID_COM_WHITE } };
	
	int i_disp_if= 0;
	int num_of_act;
	HBRUSH hBrushRed, hBrushGreen, hBrushGray, hBrushYellow, hBrushNull, hBrushDGreen , hBrushWhite;
	HDC hdc;
	HWND hifpb_work;
	int dispif[NUM_OF_IF] = { 0,0,0,0,0,0,0,0 };
}StCOMLAMPPACK, *LPCOMLAMPPACK;


class CCommunicator : public CThreadObj
{
public:
	static CMCtransaction mc_handler;
	static CUDPtransaction udp_handler;
	static CNamedPipe npip_handler;
	static StCOMLAMPPACK st_com_lamp;
	static CSock sock_handler;
public:
	CCommunicator();
	~CCommunicator();
	void routine_work(void *param);
	static unsigned __stdcall MCprotoThread(void *pVoid);
	static unsigned __stdcall NamedPipeThread(void *pVoid);
	static unsigned __stdcall UDPThread(void *pVoid);
	static LRESULT COM_PROC(HWND,UINT,WPARAM,LPARAM);
	unsigned start_MCsock(PCSTR ipaddr, USHORT port, int protocol, int type);
	unsigned start_UDPsock(PCSTR ipaddr, USHORT port, int type);
	void init_task(void* pobj);

	void set_panel_tip_txt();//タブパネルのStaticテキストを設定

	static CCommunicator* pComInst;
	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	HWND CreateWorkWindow(HWND h_parent_wnd);
	static HWND hlistbox_work;
private:
	int	index_MCsock;
	HWND hrcvmsg_work;
	HWND hsndmsg_work;
	void rcvmsgout2wwnd(const wstring wstr);
	void sndmsgout2wwnd(const wstring wstr);
	HANDLE hThreadMC;
	HANDLE hThreadPIP;
	HANDLE hThreadUDP;

	static void update_lamp();
	static HBRUSH sel_lmp_brush(int);
protected:


};

