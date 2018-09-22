#include "stdafx.h"
#include "UDPtransaction.h"


CUDPtransaction::CUDPtransaction()
{
}


CUDPtransaction::~CUDPtransaction()
{
}

int CUDPtransaction::init() {

	//�\�P�b�g�p�f�[�^�N���A
	udpifmng.sock_index = UDP_SOCK_NOT_REGISTERED;
	udpifmng.hsock_event[INDEX_SOCK_SYS_EVENTS] = WSA_INVALID_EVENT;
	udpifmng.hsock_event[INDEX_SOCK_SND_EVENT] = WSA_INVALID_EVENT;
	udpifmng.sock_event_status = 0;
	udpifmng.msg_header_size = sizeof(udpifmng.com_msg[0].comid) + sizeof(udpifmng.com_msg[0].msglen) + sizeof(udpifmng.com_msg[0].seqno);

	if (udpifmng.sock_type == CLIENT_SOCKET) {
		//�R�}���h���b�Z�[�W�쐬 set_com_msg(�R�}���h�z��No, �R�}���h�^�C�v, Seqno,pchar,datalen,...);
		set_com_msg(UDP_REQ_DATA, UDP_REQ_DATA,0,NULL,0);
		set_res_msg(UDP_REQ_DATA, UDP_REQ_DATA, 0, NULL, 0);
	}
	else {
		//���X�|���X���b�Z�[�W�쐬
		set_com_msg(UDP_REQ_DATA, UDP_REQ_DATA, 0, NULL, 0);
		set_res_msg(UDP_REQ_DATA, UDP_REQ_DATA, 0, NULL,0);
	}
	//�Z�b�g����Ă���R�}���h�̐�
	udpifmng.nCommandSet = 1;
	//�R�}���h�̎��s�X�e�b�v������
	for (unsigned i = 0; i < udpifmng.nCommandSet; i++) {
		udpifmng.com_step[i] = UDP_STP_IDLE;
		udpifmng.msg_seqno[i] = 0;
	}

	return 0;
};

int CUDPtransaction::set_com_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...)//�������ރ��W�X�^�̃o�C�g��
{
	va_list arguments;

	//�R�}���h
	if (type == UDP_REQ_DATA) {
		udpifmng.com_msg[pos].comid[0] = UDP_COM_REQ_DATA;
	}else if(type == UDP_REQ_EXIT){
		udpifmng.com_msg[pos].comid[0] = UDP_COM_EXIT;
	}

	//�R�}���h�I�v�V����	
	va_start(arguments, writelen);//�����̏����� writelen�����̑S���������X�g�Ɋi�[
	char* pc = va_arg(arguments, char*);//�I�v�V����������
	/*
	udpifmng.com_msg[pos].comid[1] = *pc;
	udpifmng.com_msg[pos].comid[2] = *(pc + 1);
	udpifmng.com_msg[pos].comid[3] = *(pc + 2);
	*/
	udpifmng.com_msg[pos].comid[1] = 'a';
	udpifmng.com_msg[pos].comid[2] = 'b';
	udpifmng.com_msg[pos].comid[3] = 'c';

	va_end(arguments);


	udpifmng.com_msg_len[pos] = udpifmng.com_msg[pos].msglen = udpifmng.msg_header_size + writelen;	//�S�d����
	udpifmng.com_msg[pos].seqno = seqno;	//�V�[�P���XNo
	memcpy(udpifmng.com_msg[pos].data, pdata, writelen);	//���b�Z�[�W�{��
		
	return 0;
}

int CUDPtransaction::set_res_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...)//�������ރ��W�X�^�̃o�C�g��
{
	va_list arguments;

	//�����R�[�h
	if (type == UDP_REQ_DATA) {
		udpifmng.res_msg[pos].resid[0] = UDP_RES_REQ_DATA;
	}
	else if (type == UDP_REQ_EXIT) {
		udpifmng.res_msg[pos].resid[0] = UDP_RES_EXIT;
	}

	//�����I�v�V����	
	va_start(arguments, writelen);//�����̏����� writelen�����̑S���������X�g�Ɋi�[
	char* pc = va_arg(arguments, char*);//�擪�f�o�C�X�ԍ�
	/*
	udpifmng.res_msg[pos].resid[1] = *pc;
	udpifmng.res_msg[pos].resid[2] = *(pc + 1);
	udpifmng.res_msg[pos].resid[3] = *(pc + 2);
	*/
	udpifmng.res_msg[pos].resid[1] = 'x';
	udpifmng.res_msg[pos].resid[2] = 'y';
	udpifmng.res_msg[pos].resid[3] = 'z';
	va_end(arguments);
	
	udpifmng.res_msg_len[pos] = udpifmng.res_msg[pos].msglen = udpifmng.msg_header_size + writelen;	//�S�d����
	udpifmng.res_msg[pos].seqno = seqno;	//�V�[�P���XNo
	memcpy(udpifmng.res_msg[pos].data, pdata, writelen);	//���b�Z�[�W�{��

	return 0;
}

int CUDPtransaction::Is_tranzaction_ready() {

	for (unsigned i = 0; i < udpifmng.nCommandSet; i++) {
		if (udpifmng.com_step[i] != UDP_STP_IDLE) return UDP_TRANZACTION_BUSY;
	}

	return UDP_TRANZACTION_READY;
};

//�g�����U�N�V�����v����t
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
	//udpifmng.msg_seqno[nCommand]++;  �����d���ł�Seq no�͕ύX���Ȃ�

	return UDP_TRANZACTION_FIN;
}


