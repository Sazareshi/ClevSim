#include "stdafx.h"
#include "UDPtransaction.h"


CUDPtransaction::CUDPtransaction()
{
}


CUDPtransaction::~CUDPtransaction()
{
}

int CUDPtransaction::init() {

	//ソケット用データクリア
	udpifmng.sock_index = UDP_SOCK_NOT_REGISTERED;
	udpifmng.hsock_event[INDEX_SOCK_SYS_EVENTS] = WSA_INVALID_EVENT;
	udpifmng.hsock_event[INDEX_SOCK_SND_EVENT] = WSA_INVALID_EVENT;
	udpifmng.sock_event_status = 0;
	udpifmng.msg_header_size = sizeof(udpifmng.com_msg[0].comid) + sizeof(udpifmng.com_msg[0].msglen) + sizeof(udpifmng.com_msg[0].seqno);

	if (udpifmng.sock_type == CLIENT_SOCKET) {
		//コマンドメッセージ作成 set_com_msg(コマンド配列No, コマンドタイプ, Seqno,pchar,datalen,...);
		set_com_msg(UDP_REQ_DATA, UDP_REQ_DATA,0,NULL,0);
		set_res_msg(UDP_REQ_DATA, UDP_REQ_DATA, 0, NULL, 0);
	}
	else {
		//レスポンスメッセージ作成
		set_com_msg(UDP_REQ_DATA, UDP_REQ_DATA, 0, NULL, 0);
		set_res_msg(UDP_REQ_DATA, UDP_REQ_DATA, 0, NULL,0);
	}
	//セットされているコマンドの数
	udpifmng.nCommandSet = 1;
	//コマンドの実行ステップ初期化
	for (unsigned i = 0; i < udpifmng.nCommandSet; i++) {
		udpifmng.com_step[i] = UDP_STP_IDLE;
		udpifmng.msg_seqno[i] = 0;
	}

	return 0;
};

int CUDPtransaction::set_com_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...)//書き込むレジスタのバイト数
{
	va_list arguments;

	//コマンド
	if (type == UDP_REQ_DATA) {
		udpifmng.com_msg[pos].comid[0] = UDP_COM_REQ_DATA;
	}else if(type == UDP_REQ_EXIT){
		udpifmng.com_msg[pos].comid[0] = UDP_COM_EXIT;
	}

	//コマンドオプション	
	va_start(arguments, writelen);//引数の初期化 writelenより後ろの全引数をリストに格納
	char* pc = va_arg(arguments, char*);//オプション文字列
	/*
	udpifmng.com_msg[pos].comid[1] = *pc;
	udpifmng.com_msg[pos].comid[2] = *(pc + 1);
	udpifmng.com_msg[pos].comid[3] = *(pc + 2);
	*/
	udpifmng.com_msg[pos].comid[1] = 'a';
	udpifmng.com_msg[pos].comid[2] = 'b';
	udpifmng.com_msg[pos].comid[3] = 'c';

	va_end(arguments);


	udpifmng.com_msg_len[pos] = udpifmng.com_msg[pos].msglen = udpifmng.msg_header_size + writelen;	//全電文長
	udpifmng.com_msg[pos].seqno = seqno;	//シーケンスNo
	memcpy(udpifmng.com_msg[pos].data, pdata, writelen);	//メッセージ本文
		
	return 0;
}

int CUDPtransaction::set_res_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...)//書き込むレジスタのバイト数
{
	va_list arguments;

	//応答コード
	if (type == UDP_REQ_DATA) {
		udpifmng.res_msg[pos].resid[0] = UDP_RES_REQ_DATA;
	}
	else if (type == UDP_REQ_EXIT) {
		udpifmng.res_msg[pos].resid[0] = UDP_RES_EXIT;
	}

	//応答オプション	
	va_start(arguments, writelen);//引数の初期化 writelenより後ろの全引数をリストに格納
	char* pc = va_arg(arguments, char*);//先頭デバイス番号
	/*
	udpifmng.res_msg[pos].resid[1] = *pc;
	udpifmng.res_msg[pos].resid[2] = *(pc + 1);
	udpifmng.res_msg[pos].resid[3] = *(pc + 2);
	*/
	udpifmng.res_msg[pos].resid[1] = 'x';
	udpifmng.res_msg[pos].resid[2] = 'y';
	udpifmng.res_msg[pos].resid[3] = 'z';
	va_end(arguments);
	
	udpifmng.res_msg_len[pos] = udpifmng.res_msg[pos].msglen = udpifmng.msg_header_size + writelen;	//全電文長
	udpifmng.res_msg[pos].seqno = seqno;	//シーケンスNo
	memcpy(udpifmng.res_msg[pos].data, pdata, writelen);	//メッセージ本文

	return 0;
}

int CUDPtransaction::Is_tranzaction_ready() {

	for (unsigned i = 0; i < udpifmng.nCommandSet; i++) {
		if (udpifmng.com_step[i] != UDP_STP_IDLE) return UDP_TRANZACTION_BUSY;
	}

	return UDP_TRANZACTION_READY;
};

//トランザクション要求受付
int CUDPtransaction::com_transaction(int nCommand) {
	CSock sock_handler;

	udpifmng.com_step[nCommand] = UDP_STP_START;

	set_com_msg(nCommand, nCommand, udpifmng.msg_seqno[nCommand], NULL, 0);

	if (udpifmng.com_msg_len[nCommand] != sock_handler.sock_send(udpifmng.sock_index, (const char*)(&(udpifmng.com_msg[nCommand])), udpifmng.com_msg_len[nCommand])) {
		udpifmng.com_step[nCommand] = UDP_STP_IDLE;
		return UDP_TRANZACTION_ERROR;
	}
	udpifmng.com_step[nCommand] = UDP_STP_WAIT_RES;



	udpifmng.msg_seqno[nCommand]++;

	return UDP_TRANZACTION_READY;
}

int CUDPtransaction::res_transaction(int nCommand) {
	CSock sock_handler;
	set_res_msg(nCommand, nCommand, udpifmng.msg_seqno[nCommand], "aa", 2);
	if (udpifmng.com_step[nCommand] == UDP_STP_WAIT_RES) {
		if (udpifmng.res_msg_len[nCommand] != sock_handler.sock_send(udpifmng.sock_index, (const char*)(&(udpifmng.res_msg[nCommand])), udpifmng.res_msg_len[nCommand])) {
			udpifmng.com_step[nCommand] = UDP_STP_IDLE;
			return UDP_TRANZACTION_ERROR;
		}
	}
	else {
		return UDP_TRANZACTION_ERROR;
	}
	udpifmng.com_step[nCommand] = UDP_STP_IDLE;
	//udpifmng.msg_seqno[nCommand]++;  応答電文ではSeq noは変更しない

	return UDP_TRANZACTION_FIN;
}


