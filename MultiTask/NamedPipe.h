#pragma once

#define PIP_USE_EVENTS		1
#define CLIENT_PIPE			2
#define SERVER_PIPE			1
#define NON_PIPE			0
#define PIPE_MAX_CLIENT		3


#define PIP_EVENT_IN_CONNECTED	1
#define PIP_EVENT_OUT_CONNECTED	2
#define PIP_EVENT_IO_CONNECTED	3
#define PIP_EVENT_MSGSND		4
#define PIP_EVENT_MSGRCV		8
#define PIP_EVENT_DISCONNECTED	16
#define PIP_EVENT_TIMEOUT		32
#define PIP_EVENT_ERROR		 	0xffff

#define PIPE_NUM_OF_RBUF		1
#define PIPE_NUM_OF_SBUF		2
#define PIPE_SIZE_OF_RBUF		1024
#define PIPE_SIZE_OF_SBUF		1024

#define PIPE_EVENTARRAY_NUM		PIPE_MAX_CLIENT+2	//�e�N���C�A���g����̃C�x���g�{exit+testsend
#define PIPE_EVENT_EXIT			PIPE_MAX_CLIENT
#define PIPE_EVENT_TEST			PIPE_MAX_CLIENT+1
#define PIPE_EVENT_TIMEOUT		10000
#define PIPE_EVENT_NUM			PIPE_EVENTARRAY_NUM

typedef struct _stPRcvBufPack
{
	TCHAR rbuf[PIPE_NUM_OF_RBUF][PIPE_SIZE_OF_RBUF];//ring buffer
	unsigned datsize[PIPE_NUM_OF_RBUF];
	unsigned rptr;
	unsigned wptr;
}StPRCVBUFPACK, *LPStPRCVBUFPACK;

typedef struct _stPSndBufPack
{
	TCHAR sbuf[PIPE_NUM_OF_SBUF][PIPE_SIZE_OF_SBUF];//ring buffer
	unsigned datsize[PIPE_NUM_OF_SBUF];
	unsigned rptr;
	unsigned wptr;
}StPSNDBUFPACK, *LPStPSNDVBUFPACK;


typedef struct _stNpipMng
{
	HANDLE hevent_exit;			//�X���b�h�I���p�C�x���g�n���h��
	HANDLE hevent_disconnect;	//�p�C�v�ؒf�ʒm�p�C�x���g�n���h��
	HANDLE hevent_testsend;		//�e�X�g���b�Z�[�W���M�p�C�x���g�n���h��
	HANDLE h_rw_event;			//�f�[�^����M�ʒm�p�C�x���g�n���h��
	HANDLE hEventArray[PIPE_EVENTARRAY_NUM];		//�C�x���g�ʒm�҂��p�z��
	HWND hmsg_listbox;			//���b�Z�[�W�\���p���X�g�{�b�N�X�n���h��
	BOOL thread_end;
	DWORD event_type[PIPE_MAX_CLIENT];			//���[�X���b�h����̒ʒm�C�x���g�^�C�v
	DWORD pip_status[PIPE_MAX_CLIENT];			//�p�C�v�̏��
	int pipe_type;//PIPE��Type�@Server/Client
	std::wstring in_pipe_name;//��M�p�C�v�̖��O
	std::wstring out_pipe_name;//���M�p�C�v�̖��O
	StPSNDBUFPACK sbufpack[PIPE_MAX_CLIENT];
	StPRCVBUFPACK rbufpack[PIPE_MAX_CLIENT];
	HANDLE hIPipe[PIPE_MAX_CLIENT];
	HANDLE hOPipe[PIPE_MAX_CLIENT];
}NpipMng, *LPNpipMng;

typedef struct _PipData
{
	int index;
	HANDLE hPipeIn;
	HANDLE hPipeOut;
}PipData, *LPPipData;

class CNamedPipe
{
public:
	static NpipMng ifmng;//�p�C�v�����Ǘ��\����
	int PreparePipe(HWND hWnd, int type);

	static unsigned __stdcall NPipeThreadConnect(LPVOID pVoid);
	static unsigned __stdcall NPipeThreadReadWrite(LPVOID pVoid);
	static BOOL WaitEvent(HANDLE hEvent);//�ړI�̃C�x���g�҂����ɌĂяo���X���b�h�����exit�C�x���g���`�F�b�N����

public:
	CNamedPipe();
	~CNamedPipe();
	static void set_pip_status(int index,unsigned id);
};

