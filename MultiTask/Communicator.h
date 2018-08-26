#pragma once
#include "ThreadObj.h"
#include "CSock.h"
#include "MCtransaction.h"
#include "MultiTaskMain.h"

#define IPADDR_MCSERVER "192.168.200.10"
#define NPORT_MCSERVER 30000
#define MC_EVENT_TIMEOUT 5000 //ソケット状態確認周期

class CCommunicator : public CThreadObj
{
public:
	static CMCtransaction mc_handler;
public:
	CCommunicator();
	~CCommunicator();
	void routine_work(void *param);
	static unsigned __stdcall MCprotoThread(void *pVoid);
	static LRESULT COM_PROC(HWND,UINT,WPARAM,LPARAM);
	unsigned start_MCsock(PCSTR ipaddr, USHORT port, int protocol, int type);
	void init_task(void* pobj);
	BOOL thread_end;

	void set_panel_tip_txt();//タブパネルのStaticテキストを設定

	static CCommunicator* pComInst;
	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	HWND CreateWorkWindow(HWND h_parent_wnd);

private:
	static CSock sock_handler;
	int	index_MCsock;
	HWND hrcvmsg_work;
	HWND hsndmsg_work;
	void rcvmsgout2wwnd(const wstring wstr);
protected:


};

