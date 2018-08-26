#include "stdafx.h"
#include "Communicator.h"
#include "Helper.h"


CMCtransaction CCommunicator::mc_handler;
CSock sock_handler;
CCommunicator* CCommunicator:: pComInst = NULL;//スタティック関数操作用インスタンスポインタ

CCommunicator::CCommunicator(){
	thread_end = FALSE;
}

CCommunicator::~CCommunicator(){
	thread_end = TRUE;
	Sleep(100);
}

void CCommunicator::routine_work(void *param) {
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
	ws << L"Routine work activated!" << *(inf.psys_counter); tweet2owner(ws.str()); ws.str(L""); ws.clear();
};

unsigned __stdcall CCommunicator::MCprotoThread(void *pVoid)
{
	DWORD dwRet, event_index, isock;
	wostringstream woss;
	MCMsgMng* pMCMsgMng = (MCMsgMng*)(&(mc_handler.mcifmng));			//MCプロトコル処理管理用構造体へのポインタ
	CCommunicator* pcomm = (CCommunicator*)pVoid;	//呼び出し元インスタンスのポインタ メインウィンドウへのメッセージ表示他用

	///# ソケットを用意して Cient:Connect要求まで   Server:Accept待ちまで
	pMCMsgMng->sock_index = pcomm->start_MCsock(pMCMsgMng->sock_ipaddr, pMCMsgMng->sock_port, pMCMsgMng->sock_protocol, pMCMsgMng->sock_type);
	pMCMsgMng->hsock_event = sock_handler.hEvents[pMCMsgMng->sock_index];
	
	while (pcomm->thread_end == FALSE) {
		dwRet = WSAWaitForMultipleEvents(MC_SOCK_USE, &(pMCMsgMng ->hsock_event), FALSE, MC_EVENT_TIMEOUT, FALSE);
		if (dwRet != WSA_WAIT_FAILED) {//TimeOutイベントではない
			event_index = dwRet - WSA_WAIT_EVENT_0;		//発生したイベントのインデックス
			isock = pMCMsgMng->sock_index;	//発生したイベントのソケットのインデクス
			
			//どのイベントが発生したかを判別する
			WSANETWORKEVENTS events;
			DWORD errCode;

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
				woss << pmsg; pcomm->rcvmsgout2wwnd(woss.str()); woss.str(L""); woss.clear();
				

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
					pMCMsgMng->hsock_event = sock_handler.hEvents[pMCMsgMng->sock_index];
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
					pMCMsgMng->hsock_event = sock_handler.hEvents[pMCMsgMng->sock_index];
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

LRESULT CCommunicator::COM_PROC(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)){
			case WM_INITDIALOG:
				break;
			case IDC_COMM_CLOSE:
				pComInst->inf.hWnd_work = NULL;
				DestroyWindow(hWnd);
				break;
			default:
				return FALSE;
		}break;
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
			hsndmsg_work = GetDlgItem(inf.hWnd_work, IDC_COMM_SENT_MSG);;
		}
	}
	return NULL;
};

void CCommunicator::init_task(void* pobj) {
	unsigned ThreadID;
	HANDLE hThread;
	pComInst = (CCommunicator *)pobj;//スタティック変数にインスタンスポインタ登録
	inf.hWnd_work = NULL;
	
	///# INIファイル読み込み
	wchar_t tbuf[32];
	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	if (tbuf[0] == L'C')mc_handler.mcifmng.sock_type = CLIENT_SOCKET;
	else mc_handler.mcifmng.sock_type = SERVER_SOCKET;

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

	///# その他　MCプロトコル処理オブジェクトの初期化
	mc_handler.init();

	//MCプロトコル処理実行
	hThread = (HANDLE) _beginthreadex(
		NULL,					//Security
		0,						//Stack size
		MCprotoThread,			//スレッド関数
		pobj,					//自タスクのオブジェクトへのポインタ
		0,						//初期フラグ
		&ThreadID				//スレッドIDを受け取るアドレス
	);

	if (hThread == 0) {
		ws << L"Failed MCprotoThread starting"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
	}
	else {
		ws << L"MCprotoThread started"; txout2msg_listbox(ws.str()); ws.str(L""); ws.clear();
		CloseHandle(hThread);
	}

}

unsigned CCommunicator::start_MCsock(PCSTR ipaddr, USHORT port,int protocol, int type) {

	CSock sock_handler;
	int ID;
	IN_ADDR sa;
	wostringstream woss;

	if (sock_handler.init_ok == NOT_INITIALIZED) {
		if (sock_handler.wsa_init() == S_OK) {
			woss << L"WSA Start OK"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.init_ok = WSA_INITIAL_OK;
		}
	}

	int r = inet_pton(AF_INET, ipaddr, &(sa.S_un.S_addr));

	if (sock_handler.create(&ID, sa, port, protocol, type) == S_OK) {
		woss << L"Creat Sock OK index = " << ID << L",  Proto:" << protocol << L",  Type:" << type << L",  IP:" << ipaddr << L",  PORT:" << port ; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
		sock_handler.sock_packs[ID].current_step = SOCK_PREPARED;
	}
	else {
		sock_handler.sock_packs[ID].current_step = SOCK_NOT_CREATED;
	}

	r = sock_handler.make_connection(ID);
	if ((protocol == SOCK_STREAM )&& (type == CLIENT_SOCKET)) {
		if (r == S_OK || r == WSAEWOULDBLOCK) {
			woss << L"index:" << ID << L"  Waiting Connection"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_packs[ID].current_step = WAIT_CONNECTION;
		}
		else {
			sock_handler.GetSockMsg(r, sock_handler.sock_err[ID].errmsg, SOCK_ERR_MSG_MAX);
			woss << L"Connection Preparation Failed:" << sock_handler.sock_err[ID].errmsg; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_close(ID);
			sock_handler.sock_err[ID].wErrCode = r;
		}
	}
	else if ((protocol == SOCK_STREAM) && (type == SERVER_SOCKET)) {
		if (r == S_OK) {
			woss << L"index:" << ID << L"  Listening Start"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_packs[ID].current_step = LISTENING;
		}
		else {
			sock_handler.GetSockMsg(r, sock_handler.sock_err[ID].errmsg, SOCK_ERR_MSG_MAX);
			woss << L"Connection Preparation Failed:" << sock_handler.sock_err[ID].errmsg; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_close(ID);
		}
	}
	else {
		;
	}

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
	if (inf.hWnd_work != NULL) SetWindowText(hrcvmsg_work, wstr.c_str()); return;
}; 


