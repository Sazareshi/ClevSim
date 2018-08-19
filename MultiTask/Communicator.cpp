#include "stdafx.h"
#include "Communicator.h"
#include "Helper.h"


CMCtransaction CCommunicator::mc_handler;
CSock CCommunicator::sock_handler;

CCommunicator::CCommunicator(){
	thread_end = FALSE;
}

CCommunicator::~CCommunicator(){
	thread_end = TRUE;
	Sleep(100);
}

void CCommunicator::routine_work(void *param) {
	MCMsgMng* pMCMsgMng = (MCMsgMng*)(&(mc_handler.mcifmng));			//MC�v���g�R�������Ǘ��p�\���̂ւ̃|�C���^
	int MC_transaction_status;

	MC_transaction_status = mc_handler.req_transaction(0);

	ws << L"Routine work activated!" << *(inf.psys_counter);tweet2owner(ws.str()); ws.str(L""); ws.clear();

};

unsigned __stdcall CCommunicator::MCprotoThread(void *pVoid)
{
	DWORD dwRet, event_index, isock;
	wostringstream woss;
	MCMsgMng* pMCMsgMng = (MCMsgMng*)(&(mc_handler.mcifmng));			//MC�v���g�R�������Ǘ��p�\���̂ւ̃|�C���^
	CCommunicator* pcomm = (CCommunicator*)pVoid;	//�Ăяo�����C���X�^���X�̃|�C���^ ���C���E�B���h�E�ւ̃��b�Z�[�W�\�����p

	///# Cient �\�P�b�g��p�ӂ���Connect�v���܂�   Server �\�P�b�g��p�ӂ���Accept�҂��܂�
	pMCMsgMng->sock_index = pcomm->start_MCsock(pMCMsgMng->sock_ipaddr, pMCMsgMng->sock_port, pMCMsgMng->sock_protocol, pMCMsgMng->sock_type);
	pMCMsgMng->hsock_event = sock_handler.hEvents[pMCMsgMng->sock_index];
	
	while (pcomm->thread_end == FALSE) {
		dwRet = WSAWaitForMultipleEvents(MC_SOCK_USE, &(pMCMsgMng ->hsock_event), FALSE, MC_EVENT_TIMEOUT, FALSE);
		if (dwRet != WSA_WAIT_FAILED) {
			event_index = dwRet - WSA_WAIT_EVENT_0;		//���������C�x���g�̃C���f�b�N�X
			isock = pMCMsgMng->sock_index;	//���������C�x���g�̃\�P�b�g�̃C���f�N�X
			
			//�ǂ̃C�x���g�������������𔻕ʂ���
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
						woss << L"index:" << isock << L"  Connection Aborted iErrorCode  " << errCode; pcomm->txout2msg_listbox(woss.str()); //woss.str(L""); woss.clear();
					}
				}

			}
			if (events.lNetworkEvents & FD_READ) {
				errCode = events.iErrorCode[FD_READ_BIT];
				pMCMsgMng->sock_event_status |= FD_READ;
				sock_handler.sock_recv(isock);
				woss << L"index:" << isock << L"  Read OK"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				pMCMsgMng->sock_event_status &= ~FD_READ;
				sock_handler.rcvbufpack;//�f�o�b�O�����j�^�p

				for (int ires = 0; ires < pMCMsgMng->nCommandSet; ires++) {
					if (pMCMsgMng->com_step[ires] == MC_STP_WAIT_RES) {
						pMCMsgMng->com_step[ires] = MC_STP_IDLE;
						woss << L"index:" << isock  << L"  Res OK RCV Buf -> " << sock_handler.rcvbufpack[isock].wptr ; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						break;
					}
				}
			}
			if (events.lNetworkEvents & FD_WRITE) {
				errCode = events.iErrorCode[FD_WRITE_BIT];
				pMCMsgMng->sock_event_status |= FD_WRITE;
				int icom;
				for (icom = 0; icom < pMCMsgMng->nCommandSet; icom++) {
					if (pMCMsgMng->com_step[icom] == MC_STP_START) {
						sock_handler.sock_send(isock, (const char*)(&pMCMsgMng->com_msg[icom].cmd0401), pMCMsgMng->com_msg_len[icom]);
						pMCMsgMng->com_step[icom] = MC_STP_IDLE;
						woss << L"index:" << isock << L"  Write OK"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
						break;
					}
				}
				;
			}
			if (events.lNetworkEvents & FD_CLOSE) {
				errCode = events.iErrorCode[FD_CLOSE_BIT];
				pMCMsgMng->sock_event_status |= FD_CLOSE;
				sock_handler.sock_close(isock);
				woss << L"index:" << isock << L"  FD_CLOSE is triggered"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			}
			if (events.lNetworkEvents & FD_ACCEPT) {
				errCode = events.iErrorCode[FD_ACCEPT_BIT];
				pMCMsgMng->sock_event_status |= FD_ACCEPT;
				woss << L"index:" << isock << L"  FD_ACCEPT is triggered"; pcomm->txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
				;
			}
		}
		else {
			;
		}

	}

	sock_handler.exit();

	return 1;
};

void CCommunicator::init_task(void* pobj) {
	unsigned ThreadID;
	HANDLE hThread;
	
	///# MC�v���g�R�������I�u�W�F�N�g�̏�����
	mc_handler.init();

	///# INI�t�@�C���ǂݍ���
	wchar_t tbuf[32];
	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	if (tbuf[0] == L'C')mc_handler.mcifmng.sock_type = CLIENT_SOCKET;
	else mc_handler.mcifmng.sock_type = SERVER_SOCKET;

	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_PROTO, L"TCP", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	if (wcscmp(tbuf, L"TCP") == 0)mc_handler.mcifmng.sock_protocol = SOCK_STREAM;
	else mc_handler.mcifmng.sock_protocol = SOCK_DGRAM;

	///# MC�v���g�R���ŗ��p����\�P�b�g��IP�ƃ|�[�g�Z�b�g
	wstring wstr;
	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_IP, L"0.0.0.0", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	wstr += tbuf;
	CHelper helper; helper.WStr2Str(wstr, mc_handler.mcifmng.sock_ip_str);
	mc_handler.mcifmng.sock_ipaddr = mc_handler.mcifmng.sock_ip_str.c_str();

	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_PORT, L"30000", tbuf, sizeof(tbuf), PATH_OF_INIFILE);
	mc_handler.mcifmng.sock_port = _wtoi(tbuf);

	//mc_handler.mcifmng.sock_ipaddr = "192.168.3.2";//IPADDR_MCSERVER;
	//mc_handler.mcifmng.sock_port = NPORT_MCSERVER;
			
	//MC�v���g�R���������s
	hThread = (HANDLE) _beginthreadex(
		NULL,					//Security
		0,						//Stack size
		MCprotoThread,			//�X���b�h�֐�
		pobj,					//���^�X�N�̃I�u�W�F�N�g�ւ̃|�C���^
		0,						//�����t���O
		&ThreadID				//�X���b�hID���󂯎��A�h���X
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

	r = sock_handler.make_connection(ID);
	if ((protocol == SOCK_STREAM )&& (type == CLIENT_SOCKET)) {
		if (r == S_OK || r == WSAEWOULDBLOCK) {
			woss << L"index:" << ID << L"  Waiting Connection"; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
			sock_handler.sock_packs[ID].current_step = WAIT_CONNECTION;
		}
		else {
			sock_handler.GetSockMsg(r, sock_handler.sock_err[ID].errmsg, SOCK_ERR_MSG_MAX);
			woss << L"Connection Preparation Failed:" << sock_handler.sock_err[ID].errmsg; txout2msg_listbox(woss.str()); woss.str(L""); woss.clear();
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
		}
	}
	else {
		;
	}

	return ID;
}
