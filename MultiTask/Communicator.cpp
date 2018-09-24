#include "stdafx.h"
#include "Communicator.h"
#include "Helper.h"


CMCtransaction	CCommunicator::mc_handler;
CUDPtransaction	CCommunicator::udp_handler;
CNamedPipe		CCommunicator::npip_handler;
StCOMLAMPPACK	CCommunicator::st_com_lamp;
HWND CCommunicator::hlistbox_work;

CSock sock_handler;
CCommunicator* CCommunicator:: pComInst = NULL;//スタティック関数操作用インスタンスポインタ

CCommunicator::CCommunicator(){
	mc_handler.mcifmng.thread_end = FALSE;
	npip_handler.ifmng.thread_end = FALSE;
	udp_handler.udpifmng.thread_end = FALSE;

	st_com_lamp.ifname[SOCK_T_S] = L"SOCK_T_S";
	st_com_lamp.ifname[SOCK_T_C] = L"SOCK_T_C";
	st_com_lamp.ifname[SOCK_U_S] = L"SOCK_U_S";
	st_com_lamp.ifname[SOCK_U_C] = L"SOCK_U_C";
	st_com_lamp.ifname[PIPE_S] = L"PIPE_S";
	st_com_lamp.ifname[PIPE_C] = L"PIPE_C";
}

CCommunicator::~CCommunicator(){
	mc_handler.mcifmng.thread_end = TRUE;
	npip_handler.ifmng.thread_end = TRUE;
	Sleep(100);
}

void CCommunicator::routine_work(void *param) {
	//MCプロトコル
	MCMsgMng* pMCMsgMng = (MCMsgMng*)(&(mc_handler.mcifmng));			//MCプロトコル処理管理用構造体へのポインタ
	int MC_transaction_status;
	if (pMCMsgMng->sock_type == CLIENT_SOCKET) {
		if (pMCMsgMng->sock_event_status & FD_WRITE) {
			MC_transaction_status = mc_handler.com_transaction(0);
			if (MC_transaction_status == TRANZACTION_READY) {
				ws << L"Transactioin Req is Sent"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
			}
		}
	}
	else {
		if (sock_handler.rcv_check(0) > 0) {
			ws << L"Command received"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
		}
	}
	
	//PIPE
	//PulseEvent(npip_handler.ifmng.hevent_exit);
	//npip_handler.ifmng.sbufpack[0].datsize[0] = 1 * sizeof(TCHAR);
	//npip_handler.ifmng.sbufpack[0].sbuf[0][0] = L'a';
	//PulseEvent(npip_handler.ifmng.hevent_testsend);

	//COMMON
	ws << L"Routine work activated!" << *(inf.psys_counter); tweet2owner(ws.str()); ws.str(L""); ws.clear();

};

unsigned __stdcall CCommunicator::MCprotoThread(void *pVoid)
{
	DWORD dwRet, event_index, isock;
	wostringstream woss;
	MCMsgMng* pMCMsgMng = (MCMsgMng*)(&(mc_handler.mcifmng));			//MCプロトコル処理管理用構造体へのポインタ
	CCommunicator* pcomm = (CCommunicator*)pVoid;	//呼び出し元インスタンスのポインタ メインウィンドウへのメッセージ表示他用
	CHelper helper;

	///# ソケットを用意して Cient:Connect要求まで   Server:Accept待ちまで
	pMCMsgMng->sock_index = pcomm->start_MCsock(pMCMsgMng->sock_ipaddr, pMCMsgMng->sock_port, pMCMsgMng->sock_protocol, pMCMsgMng->sock_type);
	pMCMsgMng->hsock_event[INDEX_SOCK_SYS_EVENTS] = sock_handler.hEvents[pMCMsgMng->sock_index];
	pMCMsgMng->hsock_event[INDEX_SOCK_SND_EVENT] = sock_handler.sndbufpack[pMCMsgMng->sock_index].hsock_snd_event;
	
	while (pMCMsgMng->thread_end == FALSE) {
		dwRet = WSAWaitForMultipleEvents(MC_SOCK_USE_EVENTS, &(pMCMsgMng ->hsock_event[0]), FALSE, MC_EVENT_TIMEOUT, FALSE);
		if (dwRet != WSA_WAIT_FAILED) {//Errorではない
			event_index = dwRet - WSA_WAIT_EVENT_0;		//発生したイベントのインデックス
			isock = pMCMsgMng->sock_index;	//発生したイベントのソケットのインデクス
			
			//どのイベントが発生したかを判別する
			WSANETWORKEVENTS events;
			DWORD errCode;

			if (dwRet == WAIT_TIMEOUT) {//タイムアウトイベント
				continue;
			}
			if (dwRet == INDEX_SOCK_SND_EVENT) {//メッセージ送信OKイベント
				pcomm->sndmsgout2wwnd(helper.carray2wstr16(sock_handler.sndbufpack[pMCMsgMng->sock_index].sbuf[0], sock_handler.sndbufpack[pMCMsgMng->sock_index].datsize[0]));
			}

			dwRet = WSAEnumNetworkEvents(sock_handler.sock_packs[isock].socket, sock_handler.hEvents[isock], &events);
			if (dwRet == SOCKET_ERROR) {
				;
			}
			if (events.lNetworkEvents & FD_CONNECT) {
				errCode = events.iErrorCode[FD_CONNECT_BIT];
				if(errCode == 0){
					sock_handler.sock_connected(isock);
					pMCMsgMng->sock_event_status |= FD_CONNECT;
					woss << L"index:" << isock << L"  Connection OK"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				}
				else {
					pMCMsgMng->sock_event_status &= ~FD_CONNECT;
					
					int r = sock_handler.make_connection(pMCMsgMng->sock_index);
					if (r == S_OK || r == WSAEWOULDBLOCK) {
						woss << L"   Retry and Waiting Connection again"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						sock_handler.sock_packs[pMCMsgMng->sock_index].current_step = WAIT_CONNECTION;
					}
					else {
						woss << L"index:" << isock << L"  Connection Aborted iErrorCode  " << errCode; pcomm->txout2msg_listbox(woss.str()); 
					}
				}
			}
			if (events.lNetworkEvents & FD_READ) {

				errCode = events.iErrorCode[FD_READ_BIT];
				pMCMsgMng->sock_event_status |= FD_READ;
				sock_handler.sock_recv(isock);
				woss << L"index:" << isock << L"  FD_READ Triggerred "; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				sock_handler.rcvbufpack;//デバッグ時モニタ用

				char* pmsg = NULL;
				int msglen;
				while (int n_rcv = sock_handler.msg_pickup(isock, &pmsg, &msglen)) {
				woss << L"index:" << isock << L"  A MESSEGE is discarded"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				};
				pcomm->rcvmsgout2wwnd(helper.carray2wstr16(pmsg, msglen));

				if (pMCMsgMng->sock_type == CLIENT_SOCKET) {//クライアント
					for (int ires = 0; ires < pMCMsgMng->nCommandSet; ires++) {
						if (pMCMsgMng->com_step[ires] == MC_STP_WAIT_RES) {
							memcpy(&(pMCMsgMng->res_msg[ires]), pmsg, msglen);
							pMCMsgMng->com_step[ires] = MC_STP_IDLE;
							woss << L"index:" << isock << L"  Res OK RCV Buf -> " << sock_handler.rcvbufpack[isock].wptr; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
							break;
						}
					}
				}
				else {
					int com_index = mc_handler.check_com(pmsg);//コマンド判定
					memcpy(&(pMCMsgMng->com_msg[com_index]), pmsg, msglen);
					pMCMsgMng->com_step[com_index] = MC_STP_WAIT_RES;
					int nRet = mc_handler.res_transaction(com_index);
					if (nRet == TRANZACTION_FIN) {
						woss << L"Response was sent"; pcomm->txout2msg_listbox(woss.str());
					}
				}
				pMCMsgMng->sock_event_status &= ~FD_READ;
			}
			if (events.lNetworkEvents & FD_WRITE) {
				errCode = events.iErrorCode[FD_WRITE_BIT];
				pMCMsgMng->sock_event_status |= FD_WRITE;
				int icom;
				for (icom = 0; icom < pMCMsgMng->nCommandSet; icom++) {
					if (pMCMsgMng->com_step[icom] == MC_STP_START) {
						sock_handler.sock_send(isock, (const char*)(&pMCMsgMng->com_msg[icom].cmd0401), pMCMsgMng->com_msg_len[icom]);
						pMCMsgMng->com_step[icom] = MC_STP_WAIT_RES;
						woss << L"index:" << isock << L"  Write OK"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						break;
					}
				}
			}
			if (events.lNetworkEvents & FD_CLOSE) {
				errCode = events.iErrorCode[FD_CLOSE_BIT];
				sock_handler.sock_close(isock);
				pMCMsgMng->sock_event_status = FD_CLOSE;


				woss << L"index:" << isock << L"  FD_CLOSE is triggered"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();

				if (pMCMsgMng->sock_type == CLIENT_SOCKET) {//クライアントでは再接続指向
					pMCMsgMng->sock_index = pcomm->start_MCsock(pMCMsgMng->sock_ipaddr, pMCMsgMng->sock_port, pMCMsgMng->sock_protocol, pMCMsgMng->sock_type);
					pMCMsgMng->hsock_event[INDEX_SOCK_SYS_EVENTS] = sock_handler.hEvents[pMCMsgMng->sock_index];
					isock = pMCMsgMng->sock_index;
					if (sock_handler.sock_packs[isock].current_step == WAIT_CONNECTION) {
						woss << L"   Retry and Waiting Connection again"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						sock_handler.sock_packs[pMCMsgMng->sock_index].current_step = WAIT_CONNECTION;
						pMCMsgMng->sock_event_status &= ~FD_CLOSE;
					}
					else {
						woss << L"index:" << isock << L"  Connection Aborted iErrorCode  " << sock_handler.sock_err[isock].wErrCode; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						sock_handler.sock_close(isock);
					}
				}
				else {
					pMCMsgMng->sock_index = pcomm->start_MCsock(pMCMsgMng->sock_ipaddr, pMCMsgMng->sock_port, pMCMsgMng->sock_protocol, pMCMsgMng->sock_type);
					pMCMsgMng->hsock_event[INDEX_SOCK_SYS_EVENTS] = sock_handler.hEvents[pMCMsgMng->sock_index];
					isock = pMCMsgMng->sock_index;
					pMCMsgMng->sock_event_status &= ~FD_CLOSE;
					woss << L"   Retry Listening again"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				}
			}
			if (events.lNetworkEvents & FD_ACCEPT) {
				errCode = events.iErrorCode[FD_ACCEPT_BIT];
				pMCMsgMng->sock_event_status |= FD_ACCEPT;
				woss << L"index:" << isock << L"  FD_ACCEPT is triggered"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				if (sock_handler.sock_accept(isock) == S_OK) {
					woss << L"index:" << isock << L"  Switch to new socket"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				}
			}
		}
		else {
			;
		}

	}

	sock_handler.exit();

	return 1;
};

unsigned __stdcall CCommunicator::NamedPipeThread(void *pVoid)
{
	wostringstream woss;
	CCommunicator* pcomm = (CCommunicator*)pVoid;	//呼び出し元インスタンスのポインタ メインウィンドウへのメッセージ表示他用
	CHelper helper;
	DWORD dwResult;

	///# パイプの準備
	npip_handler.PreparePipe(pcomm->inf.hWnd_parent, npip_handler.ifmng.pipe_type);
	
	while (npip_handler.ifmng.thread_end  == FALSE) {

		DWORD dwEventNo = WaitForMultipleObjects(PIPE_EVENT_NUM, npip_handler.ifmng.hEventArray, FALSE, PIPE_EVENT_TIMEOUT) - WAIT_OBJECT_0;

		if (dwEventNo == WAIT_TIMEOUT) {
//			woss << L"No PIPE Event: TimeOut"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			
			if (npip_handler.ifmng.pipe_type == SERVER_PIPE) {
				for (int i = 0; i < PIPE_MAX_CLIENT; i++) {
					npip_handler.set_pip_status(i, PIP_EVENT_TIMEOUT);
					if (npip_handler.ifmng.pip_status[i] != 0) {
						if (npip_handler.ifmng.pip_status[i] &= PIP_EVENT_IO_CONNECTED) st_com_lamp.tx_color[PIPE_S][i] = st_com_lamp.rx_color[PIPE_S][i] = COLID_COM_YELLOW;
						else st_com_lamp.tx_color[PIPE_S][i] = st_com_lamp.rx_color[PIPE_S][i] = COLID_COM_GRAY;
					}
					else {
						st_com_lamp.tx_color[PIPE_S][i] = st_com_lamp.rx_color[PIPE_S][i] = COLID_COM_GRAY;
					}
				}
			}
			else if (npip_handler.ifmng.pipe_type == CLIENT_PIPE) {
				if (npip_handler.ifmng.pip_status[0] != 0) {
					if (npip_handler.ifmng.pip_status[0] &= PIP_EVENT_IO_CONNECTED) st_com_lamp.tx_color[PIPE_C][0] = st_com_lamp.rx_color[PIPE_C][0] = COLID_COM_YELLOW;
					else st_com_lamp.tx_color[PIPE_C][0] = st_com_lamp.rx_color[PIPE_C][0] = COLID_COM_GRAY;
				}
				else {
					st_com_lamp.tx_color[PIPE_C][0] = st_com_lamp.rx_color[PIPE_C][0] = COLID_COM_GRAY;
				}
			}
			else;
			SendMessage(pcomm->inf.hWnd_work, MSGID_SET_WORK_LAMP, 0, 0);
		}
		else if (dwEventNo >= PIPE_EVENTARRAY_NUM) {//exit event
			woss << L"PIPE_Event: Invalid responce from WaitForMultipleObjects"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			for (int i = 0; i < PIPE_MAX_CLIENT; i++) {
				npip_handler.set_pip_status(i, PIP_EVENT_ERROR);
				st_com_lamp.tx_color[PIPE_S][i] = st_com_lamp.rx_color[PIPE_S][i] = COLID_COM_RED;
				st_com_lamp.tx_color[PIPE_C][i] = st_com_lamp.rx_color[PIPE_C][i] = COLID_COM_RED;
			}
			SendMessage(pcomm->inf.hWnd_work, MSGID_SET_WORK_LAMP, 0, 0);
			break;
		}
		else if (dwEventNo == PIPE_EVENT_EXIT) {//exit event
			woss << L"PIPE_Event: Exit"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			break;
		}
		else if (dwEventNo == PIPE_EVENT_TEST) {//test event
			woss << L"PIPE_Event: Test Send"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();

			HANDLE hpip = npip_handler.ifmng.hOPipe[0];
			TCHAR * tbuf = npip_handler.ifmng.sbufpack[0].sbuf[npip_handler.ifmng.sbufpack[0].wptr];
			DWORD dwTransferred = npip_handler.ifmng.sbufpack[0].datsize[0];
			TCHAR szBuf[256];
			
			WriteFile(hpip, tbuf, dwTransferred, &dwResult, NULL);
			npip_handler.set_pip_status(0, PIP_EVENT_MSGSND);

			wsprintf(szBuf, TEXT("No%d: %dバイト送信しました。 \n%s"), 0, dwTransferred, tbuf);
			SendMessage(npip_handler.ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)szBuf);

			if (npip_handler.ifmng.pipe_type == SERVER_PIPE) {
				if (npip_handler.ifmng.pip_status[0] &= PIP_EVENT_IO_CONNECTED) st_com_lamp.tx_color[PIPE_S][0] = COLID_COM_GREEN;
			}
			else if (npip_handler.ifmng.pipe_type == CLIENT_PIPE) {
				if (npip_handler.ifmng.pip_status[0] &= PIP_EVENT_IO_CONNECTED) st_com_lamp.tx_color[PIPE_C][0] = COLID_COM_GREEN;
			}
			else;
			SendMessage(pcomm->inf.hWnd_work, MSGID_SET_WORK_LAMP, 0, 0);
		}
		else if ((dwEventNo >= 0) && (dwEventNo < PIPE_MAX_CLIENT)) {//read/write event
			if (npip_handler.ifmng.event_type[dwEventNo] & PIP_EVENT_IN_CONNECTED) {
				woss << L"PIPE_Event: IN_CONNECTED"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				st_com_lamp.rx_color[PIPE_C][dwEventNo] = COLID_COM_DGREEN;
				st_com_lamp.rx_color[PIPE_S][dwEventNo] = COLID_COM_DGREEN;
				npip_handler.ifmng.event_type[dwEventNo] &= ~PIP_EVENT_IN_CONNECTED;
			}
			if (npip_handler.ifmng.event_type[dwEventNo] & PIP_EVENT_OUT_CONNECTED) {
				woss << L"PIPE_Event: OUT_CONNECTED"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				st_com_lamp.tx_color[PIPE_C][dwEventNo] = COLID_COM_DGREEN;
				st_com_lamp.tx_color[PIPE_S][dwEventNo] = COLID_COM_DGREEN;
				npip_handler.ifmng.event_type[dwEventNo] &= ~PIP_EVENT_OUT_CONNECTED;
			}
			if (npip_handler.ifmng.event_type[dwEventNo] & PIP_EVENT_MSGSND) {
				woss << L"PIPE_Event: MSG_SENT"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				npip_handler.ifmng.pip_status[dwEventNo] &= ~PIP_EVENT_MSGSND;
				st_com_lamp.tx_color[PIPE_C][dwEventNo] = COLID_COM_GREEN;
				st_com_lamp.tx_color[PIPE_S][dwEventNo] = COLID_COM_GREEN;
				npip_handler.ifmng.event_type[dwEventNo] &= ~PIP_EVENT_MSGSND;
			}
			if (npip_handler.ifmng.event_type[dwEventNo] & PIP_EVENT_MSGRCV) {
				woss << L"PIPE_Event: MSG_RECEIVED"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				
				wstring wstr = npip_handler.ifmng.rbufpack[dwEventNo].rbuf[npip_handler.ifmng.rbufpack[dwEventNo].rptr];
				pcomm->rcvmsgout2wwnd(wstr); wstr.clear();
				npip_handler.ifmng.pip_status[dwEventNo] &= ~PIP_EVENT_MSGRCV;
				st_com_lamp.rx_color[PIPE_C][dwEventNo] = COLID_COM_GREEN;
				st_com_lamp.rx_color[PIPE_S][dwEventNo] = COLID_COM_GREEN;
				npip_handler.ifmng.event_type[dwEventNo] &= ~PIP_EVENT_MSGRCV;

			}
			if (npip_handler.ifmng.event_type[dwEventNo] & PIP_EVENT_DISCONNECTED) {
				woss << L"PIPE_Event: DISCONNECTED"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				st_com_lamp.tx_color[PIPE_C][dwEventNo] = COLID_COM_GRAY;
				st_com_lamp.tx_color[PIPE_S][dwEventNo] = COLID_COM_GRAY;
				st_com_lamp.rx_color[PIPE_C][dwEventNo] = COLID_COM_GRAY;
				st_com_lamp.rx_color[PIPE_S][dwEventNo] = COLID_COM_GRAY;
				npip_handler.ifmng.event_type[dwEventNo] &= ~PIP_EVENT_DISCONNECTED;
			}
		//	npip_handler.ifmng.event_type[dwEventNo] = 0;
			SendMessage(pcomm->inf.hWnd_work, MSGID_SET_WORK_LAMP, 0, 0);
		}
		else {
			woss << L"PIPE_Event: Unexpected event No."<< dwEventNo; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
		}

		if (npip_handler.ifmng.pipe_type == SERVER_PIPE) {//サーバー側
	
		}
		else {//クライアント側

		}
	
	}
	
	return 0;
};

unsigned __stdcall CCommunicator::UDPThread(void *pVoid)
{
	DWORD dwRet, event_index, isock;
	wostringstream woss;
	UDPMsgMng* pUDPMsgMng = (UDPMsgMng*)(&(udp_handler.udpifmng));//UDPプロトコル処理管理用構造体へのポインタ
	CCommunicator* pcomm = (CCommunicator*)pVoid;	//呼び出し元インスタンスのポインタ メインウィンドウへのメッセージ表示他用
	CHelper helper;

	///# ソケットを用意して Cient:送信先アドレスセットまで   Server:bindまで
	pUDPMsgMng->sock_index = pcomm->start_UDPsock(pUDPMsgMng->sock_ipaddr, pUDPMsgMng->sock_port, pUDPMsgMng->sock_type);
	pUDPMsgMng->hsock_event[INDEX_SOCK_SYS_EVENTS] = sock_handler.hEvents[pUDPMsgMng->sock_index];
	pUDPMsgMng->hsock_event[INDEX_SOCK_SND_EVENT] = sock_handler.sndbufpack[pUDPMsgMng->sock_index].hsock_snd_event;

	while (pUDPMsgMng->thread_end == FALSE) {
		dwRet = WSAWaitForMultipleEvents(UDP_SOCK_USE_EVENTS, &(pUDPMsgMng->hsock_event[0]), FALSE, UDP_EVENT_TIMEOUT, FALSE);
		if (dwRet != WSA_WAIT_FAILED) {//Errorではない
			event_index = dwRet - WSA_WAIT_EVENT_0;		//発生したイベントのインデックス
			isock = pUDPMsgMng->sock_index;	//発生したイベントのソケットのインデクス

			//どのイベントが発生したかを判別する
			WSANETWORKEVENTS events;
			DWORD errCode;
			if (dwRet == WAIT_TIMEOUT) {//タイムアウトイベント
				if (st_com_lamp.tx_color[SOCK_U_C][0] != COLID_COM_WHITE) {
					st_com_lamp.tx_color[SOCK_U_C][0] = COLID_COM_YELLOW;
					st_com_lamp.tx_color[SOCK_U_S][0] = COLID_COM_YELLOW;
					st_com_lamp.rx_color[SOCK_U_C][0] = COLID_COM_YELLOW;
					st_com_lamp.rx_color[SOCK_U_S][0] = COLID_COM_YELLOW;
					SendMessage(pcomm->inf.hWnd_work, MSGID_SET_WORK_LAMP, 0, 0);
				}
				continue;
			}

			if (dwRet == INDEX_SOCK_SND_EVENT) {//メッセージ送信要求イベント
				if (udp_handler.com_transaction(UDP_REQ_DATA) == UDP_TRANZACTION_READY) {
					woss << L"UDP_Event: MSG SENT"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
					pcomm->sndmsgout2wwnd(helper.carray2wstr16(sock_handler.sndbufpack[isock].sbuf[0], sock_handler.sndbufpack[isock].datsize[0]));
					st_com_lamp.tx_color[SOCK_U_C][0] = COLID_COM_GREEN;
					st_com_lamp.tx_color[SOCK_U_S][0] = COLID_COM_GREEN;
					if (st_com_lamp.rx_color[SOCK_U_C][0] == COLID_COM_YELLOW) {
						st_com_lamp.rx_color[SOCK_U_C][0] = COLID_COM_GRAY;
						st_com_lamp.rx_color[SOCK_U_S][0] = COLID_COM_GRAY;
					}
				}
				else {
					woss << L"UDP_ERROR: MSG SENT"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				}
			}

			dwRet = WSAEnumNetworkEvents(sock_handler.sock_packs[isock].socket, sock_handler.hEvents[isock], &events);
	
			if (dwRet == SOCKET_ERROR) {
				;
			}

			if (events.lNetworkEvents & FD_READ) {

				st_com_lamp.rx_color[SOCK_U_C][0] = COLID_COM_GREEN;
				st_com_lamp.rx_color[SOCK_U_S][0] = COLID_COM_GREEN;
				
				errCode = events.iErrorCode[FD_READ_BIT];
				pUDPMsgMng->sock_event_status |= FD_READ;
				sock_handler.sock_recv(isock);
				woss << L"index:" << isock << L"  FD_READ Triggerred "; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				sock_handler.rcvbufpack;//デバッグ時モニタ用

				char* pmsg = NULL;
				int msglen;
				while (int n_rcv = sock_handler.msg_pickup(isock, &pmsg, &msglen)) {//リングバッファに溜まっているメッセージは読み飛ばす
					woss << L"index:" << isock << L"  A MESSEGE is discarded"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				};
				pcomm->rcvmsgout2wwnd(helper.carray2wstr16(pmsg, msglen));

				Sleep(100);
				
				if (*pmsg == UDP_COM_REQ_DATA) {//受信メッセージ　データ要求
					udp_handler.udpifmng.com_step[UDP_REQ_DATA] = UDP_STP_WAIT_RES;
					udp_handler.udpifmng.msg_seqno[UDP_REQ_DATA] = ((LPUDPCMD)pmsg)->seqno;

					if (udp_handler.res_transaction(UDP_REQ_DATA) == UDP_TRANZACTION_READY) {
						woss << L"UDP_Event: MSG SENT"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						pcomm->sndmsgout2wwnd(helper.carray2wstr16(sock_handler.sndbufpack[isock].sbuf[0], sock_handler.sndbufpack[isock].datsize[0]));
						st_com_lamp.tx_color[SOCK_U_C][0] = COLID_COM_GREEN;
						st_com_lamp.tx_color[SOCK_U_S][0] = COLID_COM_GREEN;
						if (st_com_lamp.rx_color[SOCK_U_C][0] == COLID_COM_YELLOW) {
							st_com_lamp.rx_color[SOCK_U_C][0] = COLID_COM_GRAY;
							st_com_lamp.rx_color[SOCK_U_S][0] = COLID_COM_GRAY;
						}
					}
					else {
						woss << L"UDP_ERROR: MSG SENT"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
					}

				}
				else if(*pmsg == UDP_RES_REQ_DATA) {
					if (udp_handler.com_transaction(UDP_REQ_DATA) == UDP_TRANZACTION_READY) {
						woss << L"UDP_Event: MSG SENT"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						pcomm->sndmsgout2wwnd(helper.carray2wstr16(sock_handler.sndbufpack[isock].sbuf[0], sock_handler.sndbufpack[isock].datsize[0]));
						st_com_lamp.tx_color[SOCK_U_C][0] = COLID_COM_GREEN;
						st_com_lamp.tx_color[SOCK_U_S][0] = COLID_COM_GREEN;
						if (st_com_lamp.rx_color[SOCK_U_C][0] == COLID_COM_YELLOW) {
							st_com_lamp.rx_color[SOCK_U_C][0] = COLID_COM_GRAY;
							st_com_lamp.rx_color[SOCK_U_S][0] = COLID_COM_GRAY;
						}
					}
					else {
						woss << L"UDP_ERROR: MSG SENT"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
					}
				}
				else;

				pUDPMsgMng->sock_event_status &= ~FD_READ;
			}

			if (events.lNetworkEvents & FD_WRITE) {
				errCode = events.iErrorCode[FD_WRITE_BIT];
				pUDPMsgMng->sock_event_status |= FD_WRITE;
			}

			SendMessage(pcomm->inf.hWnd_work, MSGID_SET_WORK_LAMP, 0, 0);
		}
		else {
			;
		}
	}

	sock_handler.exit();

	return 1;
};

LRESULT CCommunicator::COM_PROC(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	PAINTSTRUCT ps;
	TCHAR edit_wstr[1024];

	switch (msg) {
	case WM_COMMAND:{
		switch (LOWORD(wp)) {
		case IDC_BUTTON_COM_MSGSEND: {
			DWORD nwRet = GetWindowText(GetDlgItem(hWnd, IDC_COMM_SENT_MSG), npip_handler.ifmng.sbufpack[0].sbuf[0], sizeof(edit_wstr));
			npip_handler.ifmng.sbufpack[0].datsize[0] = nwRet * sizeof(TCHAR);
			npip_handler.ifmng.sbufpack[0].sbuf[0][nwRet] = L'\0';

			PulseEvent(npip_handler.ifmng.hevent_testsend);
			PulseEvent(sock_handler.sndbufpack[udp_handler.udpifmng.sock_index].hsock_snd_event);


		}break;
		case IDC_BUTTON_COM_IFSEL: {
			st_com_lamp.i_disp_if++;
			if (st_com_lamp.i_disp_if >= st_com_lamp.num_of_act)st_com_lamp.i_disp_if = 0;
			update_lamp();
		}break;
		case IDC_COMM_CLOSE: {
			pComInst->inf.hWnd_work = NULL;
			DestroyWindow(hWnd);
		}break;
		default: return FALSE;
		}
	}break;
	case WM_INITDIALOG: {
		st_com_lamp.hdc = GetDC(hWnd);
		st_com_lamp.hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		st_com_lamp.hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
		st_com_lamp.hBrushGray = (HBRUSH)GetStockObject(GRAY_BRUSH);
		st_com_lamp.hBrushYellow = CreateSolidBrush(RGB(255, 255, 0));
		st_com_lamp.hBrushNull = (HBRUSH)GetStockObject(NULL_BRUSH);
		st_com_lamp.hBrushDGreen = CreateSolidBrush(RGB(0, 150, 0));
		st_com_lamp.hBrushWhite = (HBRUSH)GetStockObject(WHITE_BRUSH);

		hlistbox_work = GetDlgItem(hWnd, IDC_LIST_COM1);
		st_com_lamp.hifpb_work = GetDlgItem(hWnd, IDC_BUTTON_COM_IFSEL);
		//SendMessage(hlistbox_work, LB_INSERTSTRING, (WPARAM)0, (LPARAM)L"Message From Threads");
		npip_handler.ifmng.hmsg_listbox = mc_handler.mcifmng.hmsg_listbox = udp_handler.udpifmng.hmsg_listbox = hlistbox_work;
	}break;
	case MSGID_SET_WORK_LAMP: {
		update_lamp();
	}break;
	case WM_PAINT:{
		BeginPaint(hWnd, &ps);
		update_lamp();
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:{
		DeleteObject(st_com_lamp.hBrushRed);
		DeleteObject(st_com_lamp.hBrushGreen);
		DeleteObject(st_com_lamp.hBrushYellow);
		DeleteObject(st_com_lamp.hBrushDGreen);
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
};

HWND  CCommunicator::CreateWorkWindow(HWND h_parent_wnd) {

	if (pComInst->inf.hWnd_work == NULL) {
		inf.hWnd_work = CreateDialog(inf.hInstance, L"IDD_COMM_DLG", inf.hWnd_parent, (DLGPROC)COM_PROC);
		if (inf.hWnd_work != NULL) {
			hrcvmsg_work = GetDlgItem(inf.hWnd_work, IDC_COMM_RCVMSG);
			hsndmsg_work = GetDlgItem(inf.hWnd_work, IDC_COMM_SENT_MSG);
			hlistbox_work = GetDlgItem(inf.hWnd_work, IDC_LIST_COM1);
		}
	}
	return NULL;
};

void CCommunicator::init_task(void* pobj) {
	unsigned ThreadID;
	unsigned IF_index=0;
	pComInst = (CCommunicator *)pobj;//スタティック変数にインスタンスポインタ登録
	inf.hWnd_work = NULL;

	
	///# INIファイル読み込み
	wchar_t tbuf[32];

	//MC Protocol
	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	if (tbuf[0] == L'C') {
		mc_handler.mcifmng.sock_type = CLIENT_SOCKET;
		st_com_lamp.dispif[IF_index]=SOCK_T_C;
		mc_handler.mcifmng.if_index = IF_index;
		IF_index++;
	}
	else if (tbuf[0] == L'S') {
		mc_handler.mcifmng.sock_type = SERVER_SOCKET;
		st_com_lamp.dispif[IF_index]=SOCK_T_S;
		mc_handler.mcifmng.if_index = IF_index;
		IF_index++;
	}
	else {
		mc_handler.mcifmng.sock_type = NON_SOCKET;
	}

	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_PROTO, L"TCP", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	if (wcscmp(tbuf, L"TCP") == 0)mc_handler.mcifmng.sock_protocol = SOCK_STREAM;
	else mc_handler.mcifmng.sock_protocol = SOCK_DGRAM;

	///# MCプロトコルで利用するソケットのIPとポートセット
	wstring wstr;
	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_IP, L"192.168.100.1", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	wstr += tbuf;
	CHelper helper; helper.WStr2Str(wstr, mc_handler.mcifmng.sock_ip_str);
	mc_handler.mcifmng.sock_ipaddr = mc_handler.mcifmng.sock_ip_str.c_str();

	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_PORT, L"30000", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	mc_handler.mcifmng.sock_port = _wtoi(tbuf);

	//mc_handler.mcifmng.sock_ipaddr = "192.168.3.2";//IPADDR_MCSERVER;
	//mc_handler.mcifmng.sock_port = NPORT_MCSERVER;
	
	//PIPE
	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, PIP_PROTOKEY_OF_TYPE, L"N", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	if (tbuf[0] == L'C') { 
		npip_handler.ifmng.pipe_type = CLIENT_PIPE; 
		st_com_lamp.dispif[IF_index] = PIPE_C;
		npip_handler.ifmng.if_index = IF_index;
		IF_index++;
	}
	else if (tbuf[0] == L'S') { 
		npip_handler.ifmng.pipe_type = SERVER_PIPE;
		st_com_lamp.dispif[IF_index] = PIPE_S;
		npip_handler.ifmng.if_index = IF_index;
		IF_index++;
	}
	else npip_handler.ifmng.pipe_type = NON_PIPE;

	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, PIP_PROTOKEY_OF_INNAME, L"//./pipe/PipeIn", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	npip_handler.ifmng.in_pipe_name = tbuf;

	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, PIP_PROTOKEY_OF_OUTNAME, L"//./pipe/PipeOut", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	npip_handler.ifmng.out_pipe_name = tbuf;

	//UDP
	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, UDP_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	if (tbuf[0] == L'C') {
		udp_handler.udpifmng.sock_type = CLIENT_SOCKET;
		st_com_lamp.dispif[IF_index] = SOCK_U_C;
		udp_handler.udpifmng.if_index = IF_index;
		IF_index++;
	}
	else if (tbuf[0] == L'S') {
		udp_handler.udpifmng.sock_type = SERVER_SOCKET;
		st_com_lamp.dispif[IF_index] = SOCK_U_S;
		udp_handler.udpifmng.if_index = IF_index;
		IF_index++;
	}
	else {
		udp_handler.udpifmng.sock_type = NON_SOCKET;
	}

	udp_handler.udpifmng.sock_protocol = SOCK_DGRAM;

	///# UDPプロトコルで利用するソケットのIPとポートセット
	wstr.clear();
	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, UDP_PROTOKEY_OF_IP, L"192.168.100.1", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	wstr += tbuf;
	helper.WStr2Str(wstr, udp_handler.udpifmng.sock_ip_str);
	udp_handler.udpifmng.sock_ipaddr = udp_handler.udpifmng.sock_ip_str.c_str();

	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, UDP_PROTOKEY_OF_PORT, L"30000", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	udp_handler.udpifmng.sock_port = _wtoi(tbuf);

	st_com_lamp.num_of_act = IF_index;
	
	///# 作業ウィンドウクリエイト
	CreateWorkWindow(inf.hWnd_parent);

	///# その他　MCプロトコル処理オブジェクトの初期化
	mc_handler.init();
	udp_handler.init();

	//MCプロトコル処理実行スレッド
	if(mc_handler.mcifmng.sock_type != NON_SOCKET)
		hThreadMC = (HANDLE) _beginthreadex(
			NULL,					//Security
			0,						//Stack size
			MCprotoThread,			//スレッド関数
			pobj,					//自タスクのオブジェクトへのポインタ
			0,						//初期フラグ
			&ThreadID				//スレッドIDを受け取るアドレス
		);
	

	if (hThreadMC == 0) {
		ws << L"MCprotoThread not working"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
	}
	else {
		ws << L"MCprotoThread started"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
		CloseHandle(hThreadMC);
	}

	//パイプ処理実行スレッド
	if(npip_handler.ifmng.pipe_type != NON_PIPE)
		hThreadPIP = (HANDLE)_beginthreadex(
			NULL,					//Security
			0,						//Stack size
			NamedPipeThread,
			pobj,
			0,
			&ThreadID
		);

	if (hThreadPIP == 0) {
		ws << L"PipeThread not working"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
	}
	else {
		ws << L"PipeThread started"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
		CloseHandle(hThreadPIP);
	}

	//UDP処理実行スレッド
	if (udp_handler.udpifmng.sock_type != NON_SOCKET)
		hThreadUDP = (HANDLE)_beginthreadex(
			NULL,					//Security
			0,						//Stack size
			UDPThread,				//スレッド関数
			pobj,					//自タスクのオブジェクトへのポインタ
			0,						//初期フラグ
			&ThreadID				//スレッドIDを受け取るアドレス
		);


	if (hThreadUDP == 0) {
		ws << L"UDPThread not working"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
	}
	else {
		ws << L"UDPThread started"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
		CloseHandle(hThreadUDP);
	}
}

unsigned CCommunicator::start_MCsock(PCSTR ipaddr, USHORT port,int protocol, int type) {

	CSock sock_handler;
	int ID;
	IN_ADDR sa;
	wostringstream woss;

	if (sock_handler.init_ok == NOT_INITIALIZED) {
		if (sock_handler.wsa_init() == S_OK) {
			woss << L"MC WSA Start OK"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.init_ok = WSA_INITIAL_OK;
		}
		else {
			return SOCKET_ERROR;
		}
	}

	int r = inet_pton(AF_INET, ipaddr, &(sa.S_un.S_addr));

	if (sock_handler.create(&ID, sa, port, protocol, type) == S_OK) {
		woss << L"MC Creat Sock OK index = " << ID << L",  Proto:" << protocol << L",  Type:" << type << L",  IP:" << ipaddr << L",  PORT:" << port ; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
		sock_handler.sock_packs[ID].current_step = SOCK_PREPARED;
	}
	else {
		return SOCKET_ERROR;//sock_handler.sock_packs[ID].current_step = SOCK_NOT_CREATED;
	}

	r = sock_handler.make_connection(ID);
	if ((protocol == SOCK_STREAM )&& (type == CLIENT_SOCKET)) {
		if (r == S_OK || r == WSAEWOULDBLOCK) {
			woss << L"index:" << ID << L"  MC Waiting Connection"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_packs[ID].current_step = WAIT_CONNECTION;
		}
		else {
			sock_handler.GetSockMsg(r, sock_handler.sock_err[ID].errmsg, SOCK_ERR_MSG_MAX);
			woss << L"MC Connection Preparation Failed:" << sock_handler.sock_err[ID].errmsg; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_close(ID);
			sock_handler.sock_err[ID].wErrCode = r;
		}
	}
	else if ((protocol == SOCK_STREAM) && (type == SERVER_SOCKET)) {
		if (r == S_OK) {
			woss << L"index:" << ID << L"  MC Listening Start"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_packs[ID].current_step = LISTENING;
		}
		else {
			sock_handler.GetSockMsg(r, sock_handler.sock_err[ID].errmsg, SOCK_ERR_MSG_MAX);
			woss << L"MC Connection Preparation Failed:" << sock_handler.sock_err[ID].errmsg; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_close(ID);
		}
	}
	else {
		return SOCKET_ERROR;
	}

	return ID;
}

unsigned CCommunicator::start_UDPsock(PCSTR ipaddr, USHORT port,  int type) {

	CSock sock_handler;
	int ID;
	IN_ADDR sa;
	wostringstream woss;

	if (sock_handler.init_ok == NOT_INITIALIZED) {
		if (sock_handler.wsa_init() == S_OK) {
			woss << L"UDP WSA Start OK"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.init_ok = WSA_INITIAL_OK;
		}
		else {
			return SOCKET_ERROR;
		}
	}

	int r = inet_pton(AF_INET, ipaddr, &(sa.S_un.S_addr));

	if (sock_handler.create(&ID, sa, port, SOCK_DGRAM, type) == S_OK) {
		woss << L"UDP Creat Sock OK index = " << ID << L",  Proto:" << SOCK_DGRAM << L",  Type:" << type << L",  IP:" << ipaddr << L",  PORT:" << port; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
		sock_handler.sock_packs[ID].current_step = SOCK_PREPARED;
		st_com_lamp.tx_color[SOCK_U_C][0] = st_com_lamp.rx_color[SOCK_U_C][0] = COLID_COM_GRAY;
		st_com_lamp.tx_color[SOCK_U_S][0] = st_com_lamp.rx_color[SOCK_U_S][0] = COLID_COM_GRAY;
	}
	else {
		st_com_lamp.tx_color[SOCK_U_C][0] = st_com_lamp.rx_color[SOCK_U_C][0] = COLID_COM_RED;
		st_com_lamp.tx_color[SOCK_U_S][0] = st_com_lamp.rx_color[SOCK_U_S][0] = COLID_COM_RED;
		return SOCK_ERROR;
	}

	r = sock_handler.make_connection(ID);

	return ID;
}

LRESULT CALLBACK CCommunicator::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); break;

		case IDC_TASK_ITEM_RADIO1:
		case IDC_TASK_ITEM_RADIO2:
		case IDC_TASK_ITEM_RADIO3:
		case IDC_TASK_ITEM_RADIO4:
		case IDC_TASK_ITEM_RADIO5:
		case IDC_TASK_ITEM_RADIO6:
			inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			if ((inf.panel_func_id == IDC_TASK_FUNC_RADIO1) && (inf.panel_type_id == IDC_TASK_ITEM_RADIO1)) {
				if (inf.hWnd_work == NULL) CreateWorkWindow(inf.hWnd_parent);
			}
			break;
		case IDSET: {
			wstring wstr, wstr_tmp;

			//サンプルとしていろいろな型で読み込んで表示している
			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stod(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 2(i):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT2, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stoi(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 3(f):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT3, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stof(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 4(l):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT4, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stol(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 5(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT5, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			wstr += L",   Param 6(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT6, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			txout2msg_listbox(wstr);


		}break;
		case IDRESET: {
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		}break;

		case IDC_TASK_OPTION_CHECK1:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK1) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION1;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;

		case IDC_TASK_OPTION_CHECK2:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK2) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION2;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;
		}
	}
	return 0;
};

void CCommunicator::set_panel_tip_txt()
{
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"Type for Func1 \n\r 1:Open Inf Wnd 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO2: {
		wstr = L"Type for Func2 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO3: {
		wstr = L"Type for Func3 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"Type for Func4 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO5: {
		wstr = L"Type for Func5 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO6: {
		wstr = L"Type for Func6 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	default: {
		wstr = L"Type for Func? \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		wstr_type += L"(Param of type?) \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
	}break;
	}

	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET3), wstr.c_str());
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET4), wstr_type.c_str());
}

void CCommunicator::rcvmsgout2wwnd(const wstring wstr) {
	if (inf.hWnd_work != NULL) 
		SetWindowText(hrcvmsg_work, wstr.c_str()); 
	return;
}; 
void CCommunicator::sndmsgout2wwnd(const wstring wstr) {
	if (inf.hWnd_work != NULL) 
		SetWindowText(hsndmsg_work, wstr.c_str()); 
	return;
};

void CCommunicator::update_lamp() {
	int code = st_com_lamp.dispif[st_com_lamp.i_disp_if];//dispif[]に各IFの種別コード（表示セット色配列へのindex）が入っている
	SetWindowText(st_com_lamp.hifpb_work, st_com_lamp.ifname[code]);
	for (int i = 0; i < NUM_OF_CLIENT; i++) {
		SelectObject(st_com_lamp.hdc, sel_lmp_brush(st_com_lamp.tx_color[code][i]));//表示色は各IFが自割り当て配列の部分に状況に応じてセットしておく
		Ellipse(st_com_lamp.hdc, st_com_lamp.tx_ellipse_pos[i][0], st_com_lamp.tx_ellipse_pos[i][1], st_com_lamp.tx_ellipse_pos[i][2], st_com_lamp.tx_ellipse_pos[i][3]);
		SelectObject(st_com_lamp.hdc, sel_lmp_brush(st_com_lamp.rx_color[code][i]));//表示色は各IFが自割り当て配列の部分に状況に応じてセットしておく
		Ellipse(st_com_lamp.hdc, st_com_lamp.rx_ellipse_pos[i][0], st_com_lamp.rx_ellipse_pos[i][1], st_com_lamp.rx_ellipse_pos[i][2], st_com_lamp.rx_ellipse_pos[i][3]);
	}
	return;
}
HBRUSH CCommunicator::sel_lmp_brush(int code) {
	switch (code) {
	case COLID_COM_NULL: return st_com_lamp.hBrushNull;
	case COLID_COM_YELLOW: return st_com_lamp.hBrushYellow;
	case COLID_COM_RED: return st_com_lamp.hBrushRed;
	case COLID_COM_GREEN: return st_com_lamp.hBrushGreen;
	case COLID_COM_GRAY: return st_com_lamp.hBrushGray;
	case COLID_COM_DGREEN: return st_com_lamp.hBrushDGreen;
	case COLID_COM_WHITE: return st_com_lamp.hBrushWhite;
	default: return st_com_lamp.hBrushNull;
	}
	
};

