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

#define PIPE_EVENTARRAY_NUM		PIPE_MAX_CLIENT+2	//各クライアントからのイベント＋exit+testsend
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
	HANDLE hevent_exit;			//スレッド終了用イベントハンドル
	HANDLE hevent_disconnect;	//パイプ切断通知用イベントハンドル
	HANDLE hevent_testsend;		//テストメッセージ送信用イベントハンドル
	HANDLE h_rw_event;			//データ送受信通知用イベントハンドル
	HANDLE hEventArray[PIPE_EVENTARRAY_NUM];		//イベント通知待ち用配列
	HWND hmsg_listbox;			//メッセージ表示用リストボックスハンドル
	BOOL thread_end;
	DWORD event_type[PIPE_MAX_CLIENT];			//末端スレッドからの通知イベントタイプ
	DWORD pip_status[PIPE_MAX_CLIENT];			//パイプの状態
	int pipe_type;//PIPEのType　Server/Client
	std::wstring in_pipe_name;//受信パイプの名前
	std::wstring out_pipe_name;//送信パイプの名前
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
	static NpipMng ifmng;//パイプ処理管理構造体
	int PreparePipe(HWND hWnd, int type);

	static unsigned __stdcall NPipeThreadConnect(LPVOID pVoid);
	static unsigned __stdcall NPipeThreadReadWrite(LPVOID pVoid);
	static BOOL WaitEvent(HANDLE hEvent);//目的のイベント待ち中に呼び出しスレッドからのexitイベントもチェックする

public:
	CNamedPipe();
	~CNamedPipe();
	static void set_pip_status(int index,unsigned id);
};

