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
#define MC_EVENT_TIMEOUT 5000 //�\�P�b�g��Ԋm�F����

//���ʃw�b�_
typedef struct _stMCCommonHeader
{
	BYTE subheader[2];			//�T�u�w�b�_
	BYTE accessRoute_nwno;		//�A�N�Z�X�o�H�@�l�b�g���[�NNo.
	BYTE accessRoute_pcno;		//�A�N�Z�X�o�H�@�l�b�g���[�NNo.
	BYTE accessRoute_unitio[3];	//�A�N�Z�X�o�H�@���j�b�gIO�ԍ��A�ǔ�
}MCComHeader, *LPMCComHeader;

//�f�o�C�X�ǂݏo���t�H�[�}�b�g
typedef struct _stMCFormatCmd0401
{
	MCComHeader common_header;	//���ʃw�b�_
	BYTE data_len[2];			//�v���f�[�^��
	BYTE wait_time[2];			//�Ď��^�C�}�[

	BYTE cmd[2];
	BYTE sub_cmd[2];
	BYTE device_top[3];
	BYTE device_code;
	BYTE device_num[2];
}MCCmd0401ReadData, *LPMCCmd0401ReadData;

typedef struct _stMCFormatRes0401
{
	MCComHeader common_header;	//���ʃw�b�_
	BYTE data_len[2];			//�v���f�[�^��
	BYTE end_code[2];			//�I���R�[�h
	
	BYTE data[MC_DATA_MAX];		//�ǂݏo���f�[�^ or �G���[���
}MCRes0401ReadData, *LPMCRes0401ReadData;

//�f�o�C�X�������݃t�H�[�}�b�g
typedef struct _stMCFormatCmd1401
{
	MCComHeader common_header;	//���ʃw�b�_
	BYTE data_len[2];			//�v���f�[�^��
	BYTE wait_time[2];			//�Ď��^�C�}�[

	BYTE cmd[2];
	BYTE sub_cmd[2];
	BYTE device_top[3];
	BYTE device_code;
	BYTE device_num[2];

	BYTE data[MC_DATA_MAX];//�������݃f�[�^

}MCCmd1401WriteData, *LPMCCmd1401WriteData;

typedef struct _stMCFormatRes1401
{
	MCComHeader common_header;	//���ʃw�b�_
	BYTE data_len[2];			//�v���f�[�^��
	BYTE end_code[2];			//�I���R�[�h

	//�G���[���
	BYTE err_accessRoute_nwno;		//�A�N�Z�X�o�H�@�l�b�g���[�NNo.
	BYTE err_accessRoute_pcno;		//�A�N�Z�X�o�H�@PCNo.
	BYTE err_accessRoute_unitio[3];	//�A�N�Z�X�o�H�@���j�b�gIO�ԍ��A�ǔ�
	BYTE err_cmd[2];				//�G���[�R�}���h�R�[�h
	BYTE err_sub_cmd[2];			//�G���[�R�}���h�T�u�R�[�h

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
	int nCommandSet;//�Z�b�g����Ă���R�}���h��
	int com_step[MC_TRANSACTION_MAX];
	int com_msg_len[MC_TRANSACTION_MAX];
	int res_msg_len[MC_TRANSACTION_MAX];
	MCCMD com_msg[MC_TRANSACTION_MAX];//�R�}���h�d��
 	MCRES res_msg[MC_TRANSACTION_MAX];//���X�|���X�d��

	HANDLE hsock_event[MC_SOCK_USE_EVENTS];//�񓯊������p�C�x���g�n���h��
	int sock_index;//CSock��芄�蓖�Ă�ꂽ�\�P�b�g�̃C���f�b�N�X
	int sock_protocol;//���p����\�P�b�g�̃v���g�R���@TCP/UDP
	int sock_type;//���p����\�P�b�g��Type�@Server/Client
	PCSTR  sock_ipaddr;//���p����\�P�b�g��IP�A�h���X�ւ̃|�C���^
	USHORT sock_port;//���p����\�P�b�g�̃|�[�g
	WORD   sock_event_status;//�\�P�b�g�̏��
	std::string sock_ip_str;//���p����\�P�b�g��IP�A�h���X
	int seq_no;//�g�����U�N�V�����̃V�[�P���X�ԍ�
	int active_com;//���s���̃R�}���h�d��
	HWND hwnd;//�g�����U�N�V�����̊����ʒm��E�B���h�E
	BOOL thread_end;
	HWND hmsg_listbox;			//���b�Z�[�W�\���p���X�g�{�b�N�X�n���h��
	WORD if_index;//�S�C���^�[�t�F�C�X�̒��̎��ʃC���f�b�N�X
}MCMsgMng, *LPMCMsgMng;

class CMCtransaction
{
public:
	MCMsgMng mcifmng;//MC�v���g�R�������Ǘ��\����
	int com_transaction(int nCommand);	//�g�����U�N�V�����v��
	int res_transaction(int nCommand);	//�g�����U�N�V��������
	int set_com_msg(int pos, int type, int writelen, ...);//�R�}���h���b�Z�[�W�Z�b�g
	int set_res_msg(int pos, int type, const char* pdata, int writelen, ...);//�R�}���h���b�Z�[�W�Z�b�g
	int init();	//������
	int Is_tranzaction_ready();	//�R�}���h���M�۔���
	int check_com(const char* pch);	//�R�}���h���e�m�F�@�߂�l�́A��M�R�}���h�z��̃C���f�b�N�X

public:
	CMCtransaction();
	~CMCtransaction();

};

