#pragma once

#include "CSock.h"

#define MC_DATA_MAX 2048
#define MC_TRANSACTION_MAX 16

#define MC_STP_IDLE		0
#define MC_STP_START	1
#define MC_STP_WAIT_RES 2


#define MC_TYP_READ_D	1
#define MC_TYP_READ_R	2
#define MC_TYP_WRITE_D	4
#define MC_TYP_WRITE_R	8

#define MC_SOCK_NOT_REGISTERED -1
#define MC_SOCK_USE_EVENTS	 2
#define INDEX_SOCK_SYS_EVENTS	 0
#define INDEX_SOCK_SND_EVENT	 1

#define POS_READ_D100	0
#define POS_WRITE_D110	1
#define POS_READ_R0_R303	2
#define POS_READ_R304__	3

#define SIZE_OF_FAULT_TRIGGER	304
#define SIZE_OF_TRACE_RECORD	96

#define TRANZACTION_READY	0
#define TRANZACTION_FIN		0
#define TRANZACTION_BUSY	-1
#define TRANZACTION_ERROR	-2

#define IPADDR_MCSERVER "192.168.200.10"
#define NPORT_MCSERVER 30000
#define MC_EVENT_TIMEOUT 5000 //ソケット状態確認周期

//共通ヘッダ
typedef struct _stMCCommonHeader
{
	BYTE subheader[2];			//サブヘッダ
	BYTE accessRoute_nwno;		//アクセス経路　ネットワークNo.
	BYTE accessRoute_pcno;		//アクセス経路　ネットワークNo.
	BYTE accessRoute_unitio[3];	//アクセス経路　ユニットIO番号、局番
}MCComHeader, *LPMCComHeader;

//デバイス読み出しフォーマット
typedef struct _stMCFormatCmd0401
{
	MCComHeader common_header;	//共通ヘッダ
	BYTE data_len[2];			//要求データ長
	BYTE wait_time[2];			//監視タイマー

	BYTE cmd[2];
	BYTE sub_cmd[2];
	BYTE device_top[3];
	BYTE device_code;
	BYTE device_num[2];
}MCCmd0401ReadData, *LPMCCmd0401ReadData;

typedef struct _stMCFormatRes0401
{
	MCComHeader common_header;	//共通ヘッダ
	BYTE data_len[2];			//要求データ長
	BYTE end_code[2];			//終了コード
	
	BYTE data[MC_DATA_MAX];		//読み出しデータ or エラー情報
}MCRes0401ReadData, *LPMCRes0401ReadData;

//デバイス書き込みフォーマット
typedef struct _stMCFormatCmd1401
{
	MCComHeader common_header;	//共通ヘッダ
	BYTE data_len[2];			//要求データ長
	BYTE wait_time[2];			//監視タイマー

	BYTE cmd[2];
	BYTE sub_cmd[2];
	BYTE device_top[3];
	BYTE device_code;
	BYTE device_num[2];

	BYTE data[MC_DATA_MAX];//書き込みデータ

}MCCmd1401WriteData, *LPMCCmd1401WriteData;

typedef struct _stMCFormatRes1401
{
	MCComHeader common_header;	//共通ヘッダ
	BYTE data_len[2];			//要求データ長
	BYTE end_code[2];			//終了コード

	//エラー情報
	BYTE err_accessRoute_nwno;		//アクセス経路　ネットワークNo.
	BYTE err_accessRoute_pcno;		//アクセス経路　PCNo.
	BYTE err_accessRoute_unitio[3];	//アクセス経路　ユニットIO番号、局番
	BYTE err_cmd[2];				//エラーコマンドコード
	BYTE err_sub_cmd[2];			//エラーコマンドサブコード

}MCRes1401WriteData, *LPMCRes1401WriteData;

typedef struct _stMCCmdData
{
	union
	{
		MCCmd0401ReadData cmd0401;
		MCCmd1401WriteData cmd1401;
	};
}MCCMD, *LPMCCMD;

typedef struct _stMCResData
{
	union
	{
		MCRes0401ReadData res0401;
		MCRes1401WriteData res1401;
	};
}MCRES, *LPMCRES;


typedef struct _stMCTransactionMng
{
	int nCommandSet;//セットされているコマンド数
	int com_step[MC_TRANSACTION_MAX];
	int com_msg_len[MC_TRANSACTION_MAX];
	int res_msg_len[MC_TRANSACTION_MAX];
	MCCMD com_msg[MC_TRANSACTION_MAX];//コマンド電文
 	MCRES res_msg[MC_TRANSACTION_MAX];//レスポンス電文

	HANDLE hsock_event[MC_SOCK_USE_EVENTS];//非同期処理用イベントハンドル
	int sock_index;//CSockより割り当てられたソケットのインデックス
	int sock_protocol;//利用するソケットのプロトコル　TCP/UDP
	int sock_type;//利用するソケットのType　Server/Client
	PCSTR  sock_ipaddr;//利用するソケットのIPアドレスへのポインタ
	USHORT sock_port;//利用するソケットのポート
	WORD   sock_event_status;//ソケットの状態
	std::string sock_ip_str;//利用するソケットのIPアドレス
	int seq_no;//トランザクションのシーケンス番号
	int active_com;//実行中のコマンド電文
	HWND hwnd;//トランザクションの完了通知先ウィンドウ
	BOOL thread_end;
	HWND hmsg_listbox;			//メッセージ表示用リストボックスハンドル
	WORD if_index;//全インターフェイスの中の識別インデックス
}MCMsgMng, *LPMCMsgMng;

class CMCtransaction
{
public:
	MCMsgMng mcifmng;//MCプロトコル処理管理構造体
	int com_transaction(int nCommand);	//トランザクション要求
	int res_transaction(int nCommand);	//トランザクション応答
	int set_com_msg(int pos, int type, int writelen, ...);//コマンドメッセージセット
	int set_res_msg(int pos, int type, const char* pdata, int writelen, ...);//コマンドメッセージセット
	int init();	//初期化
	int Is_tranzaction_ready();	//コマンド送信可否判定
	int check_com(const char* pch);	//コマンド内容確認　戻り値は、受信コマンド配列のインデックス

public:
	CMCtransaction();
	~CMCtransaction();

};

