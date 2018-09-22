#pragma once

#include "CSock.h"

#define UDP_DATA_MAX 4080
#define UDP_TRANSACTION_MAX 4
#define UDP_EVENT_TIMEOUT 5000 //ソケット状態確認周期

#define UDP_STP_IDLE		0
#define UDP_STP_START		1
#define UDP_STP_WAIT_RES	0xffff


#define UDP_SOCK_NOT_REGISTERED -1
#define UDP_SOCK_USE_EVENTS		2
#define INDEX_SOCK_SYS_EVENTS	0
#define INDEX_SOCK_SND_EVENT	 1

#define UDP_COM_REQ_DATA		'?'
#define UDP_COM_EXIT			'#'
#define UDP_RES_REQ_DATA		'!'
#define UDP_RES_EXIT			'-'
#define UDP_REQ_DATA			0
#define UDP_REQ_EXIT			1

#define UDP_TRANZACTION_READY	0
#define UDP_TRANZACTION_FIN		0
#define UDP_TRANZACTION_BUSY	-1
#define UDP_TRANZACTION_ERROR	-2


#define MSG_HEADER_SIZE			12
typedef struct _stUDPCmdData
{
	char comid[4];
	DWORD seqno;//シーケンス番号
	DWORD msglen;//電文長
	char  data[UDP_DATA_MAX - MSG_HEADER_SIZE];
}UDPCMD, *LPUDPCMD;

typedef struct _stUDPResData
{
	char resid[4];
	DWORD seqno;//シーケンス番号
	DWORD msglen;//電文長
	char  data[UDP_DATA_MAX- MSG_HEADER_SIZE];
}UDPRES, *LPUDPRES;


typedef struct _stUDPTransactionMng
{
	DWORD nCommandSet;//セットされているコマンド数
	DWORD com_step[UDP_TRANSACTION_MAX];
	DWORD com_msg_len[UDP_TRANSACTION_MAX];
	DWORD res_msg_len[UDP_TRANSACTION_MAX];
	UDPCMD com_msg[UDP_TRANSACTION_MAX];//コマンド電文
	UDPRES res_msg[UDP_TRANSACTION_MAX];//レスポンス電文
	DWORD msg_header_size;
	DWORD msg_seqno[UDP_TRANSACTION_MAX];


	HANDLE hsock_event[UDP_SOCK_USE_EVENTS];//非同期処理用イベントハンドル
	int sock_index;//CSockより割り当てられたソケットのインデックス
	int sock_protocol;//利用するソケットのプロトコル　TCP/UDP
	int sock_type;//利用するソケットのType　Server/Client
	PCSTR  sock_ipaddr;//サーバーソケットのIPアドレスへのポインタ
	USHORT sock_port;//サーバーソケットのポート
	WORD   sock_event_status;//ソケットの状態
	std::string sock_ip_str;//サーバーソケットのIPアドレス

	HWND hwnd;//トランザクションの完了通知先ウィンドウ
	BOOL thread_end;
	HWND hmsg_listbox;//メッセージ表示用リストボックスハンドル
	WORD if_index;//全インターフェイスの中の識別インデックス
}UDPMsgMng, LPUDPMsgMng;

class CUDPtransaction
{
public:
	CUDPtransaction();
	~CUDPtransaction();
public:
	UDPMsgMng udpifmng;//UDPプロトコル処理管理構造体
		int com_transaction(int nCommand);	//トランザクション要求
	int res_transaction(int nCommand);	//トランザクション応答
	int init();	//初期化
	int Is_tranzaction_ready();	//コマンド送信可否判定
	int set_com_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...);//コマンドメッセージセット
	int set_res_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...);//レスポンスメッセージセット
};


