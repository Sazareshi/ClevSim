#pragma once

#include "CSock.h"

#define UDP_DATA_MAX 4080
#define UDP_TRANSACTION_MAX 4
#define UDP_EVENT_TIMEOUT 5000 //�\�P�b�g��Ԋm�F����

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
	DWORD seqno;//�V�[�P���X�ԍ�
	DWORD msglen;//�d����
	char  data[UDP_DATA_MAX - MSG_HEADER_SIZE];
}UDPCMD, *LPUDPCMD;

typedef struct _stUDPResData
{
	char resid[4];
	DWORD seqno;//�V�[�P���X�ԍ�
	DWORD msglen;//�d����
	char  data[UDP_DATA_MAX- MSG_HEADER_SIZE];
}UDPRES, *LPUDPRES;


typedef struct _stUDPTransactionMng
{
	DWORD nCommandSet;//�Z�b�g����Ă���R�}���h��
	DWORD com_step[UDP_TRANSACTION_MAX];
	DWORD com_msg_len[UDP_TRANSACTION_MAX];
	DWORD res_msg_len[UDP_TRANSACTION_MAX];
	UDPCMD com_msg[UDP_TRANSACTION_MAX];//�R�}���h�d��
	UDPRES res_msg[UDP_TRANSACTION_MAX];//���X�|���X�d��
	DWORD msg_header_size;
	DWORD msg_seqno[UDP_TRANSACTION_MAX];


	HANDLE hsock_event[UDP_SOCK_USE_EVENTS];//�񓯊������p�C�x���g�n���h��
	int sock_index;//CSock��芄�蓖�Ă�ꂽ�\�P�b�g�̃C���f�b�N�X
	int sock_protocol;//���p����\�P�b�g�̃v���g�R���@TCP/UDP
	int sock_type;//���p����\�P�b�g��Type�@Server/Client
	PCSTR  sock_ipaddr;//�T�[�o�[�\�P�b�g��IP�A�h���X�ւ̃|�C���^
	USHORT sock_port;//�T�[�o�[�\�P�b�g�̃|�[�g
	WORD   sock_event_status;//�\�P�b�g�̏��
	std::string sock_ip_str;//�T�[�o�[�\�P�b�g��IP�A�h���X

	HWND hwnd;//�g�����U�N�V�����̊����ʒm��E�B���h�E
	BOOL thread_end;
	HWND hmsg_listbox;//���b�Z�[�W�\���p���X�g�{�b�N�X�n���h��
	WORD if_index;//�S�C���^�[�t�F�C�X�̒��̎��ʃC���f�b�N�X
}UDPMsgMng, LPUDPMsgMng;

class CUDPtransaction
{
public:
	CUDPtransaction();
	~CUDPtransaction();
public:
	UDPMsgMng udpifmng;//UDP�v���g�R�������Ǘ��\����
		int com_transaction(int nCommand);	//�g�����U�N�V�����v��
	int res_transaction(int nCommand);	//�g�����U�N�V��������
	int init();	//������
	int Is_tranzaction_ready();	//�R�}���h���M�۔���
	int set_com_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...);//�R�}���h���b�Z�[�W�Z�b�g
	int set_res_msg(int pos, int type, DWORD seqno, const char* pdata, int writelen, ...);//���X�|���X���b�Z�[�W�Z�b�g
};


