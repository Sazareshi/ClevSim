#include "stdafx.h"
#include "NamedPipe.h"

CNamedPipe::CNamedPipe(){
	for (int i = 0; i < PIPE_MAX_CLIENT;i++) {
		ifmng.event_type[i] = 0;
		ifmng.pip_status[i] = PIP_EVENT_DISCONNECTED;
		ifmng.rbufpack[i].rptr = ifmng.rbufpack[i].wptr = 0;
	}
}

CNamedPipe::~CNamedPipe(){}

NpipMng CNamedPipe::ifmng;

int CNamedPipe::PreparePipe(HWND hWnd, int type){

	ifmng.hevent_exit  = ifmng.hEventArray[PIPE_EVENT_EXIT] = CreateEvent(NULL, TRUE, FALSE, NULL);//セキュリティ属性,手動リセット,初期状態,名前
	if (ifmng.hevent_exit == NULL) { MessageBox(hWnd, L"イベント作成失敗", L"Error", MB_OK); return 1; }
	ifmng.hevent_disconnect = CreateEvent(NULL, TRUE, FALSE, NULL);//セキュリティ属性,手動リセット,初期状態,名前
	if (ifmng.hevent_disconnect == NULL) { MessageBox(hWnd, L"イベント作成失敗", L"Error", MB_OK); return 1; }
	for (int i = 0; i < PIPE_MAX_CLIENT; i++) {
		ifmng.hEventArray[i] = CreateEvent(NULL, FALSE, FALSE, NULL);//セキュリティ属性,自動リセット,初期状態,名前
		if (ifmng.hEventArray[i] == NULL) { MessageBox(hWnd, L"イベント作成失敗", L"Error", MB_OK); return 1; }
	}
	ifmng.hevent_testsend= ifmng.hEventArray[PIPE_EVENT_TEST]=CreateEvent(NULL, FALSE, FALSE, NULL);//セキュリティ属性,手動リセット,初期状態,名前
	if (ifmng.hevent_testsend == NULL) { MessageBox(hWnd, L"イベント作成失敗", L"Error", MB_OK); return 1; }

	ifmng.thread_end = FALSE;
	
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NPipeThreadConnect, NULL, 0, NULL);
	if (hThread != 0) CloseHandle(hThread);

	return 0;
}

unsigned __stdcall CNamedPipe::NPipeThreadConnect(LPVOID pVoid) {
	
	OVERLAPPED ov;
	TCHAR      szData[PIPE_SIZE_OF_RBUF], szBuf[PIPE_SIZE_OF_RBUF];
	DWORD      dwClientProcessId;
	int        i = 0;

	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (ifmng.pipe_type == SERVER_PIPE) {
		SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)TEXT("Server PIPE started!！"));
		for (;;) {
			HANDLE hPipeIn = CreateNamedPipe(ifmng.in_pipe_name.c_str(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE |	PIPE_WAIT, PIPE_MAX_CLIENT, sizeof(szData), sizeof(szData), 5000, NULL);
			HANDLE hPipeOut = CreateNamedPipe(ifmng.out_pipe_name.c_str(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, PIPE_MAX_CLIENT, sizeof(szData), sizeof(szData), 5000, NULL);
			if (hPipeIn == INVALID_HANDLE_VALUE || hPipeOut == INVALID_HANDLE_VALUE) {
				if (GetLastError() == ERROR_PIPE_BUSY) {
					SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)TEXT("最大同時接続 発生"));
					if (!WaitEvent(ifmng.hevent_disconnect))
					break;
					for (i = 0; i < PIPE_MAX_CLIENT; i++) {
						if (ifmng.pip_status[i] & PIP_EVENT_DISCONNECTED) {
							ifmng.pip_status[i] &= ~PIP_EVENT_DISCONNECTED;
							break;
						}
					}
					ResetEvent(ifmng.hevent_disconnect);

					continue;
				}
				else {
					SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)TEXT("名前付きパイプの作成に失敗"));
					ifmng.event_type[i] = 0;
					break;
				}
			}

			ZeroMemory(&ov, sizeof(OVERLAPPED));
			ov.hEvent = hEvent;
			ConnectNamedPipe(hPipeIn, &ov);
			wsprintf(szBuf, TEXT("No%d 接続待ち"), i); SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)szBuf);
			if (!WaitEvent(hEvent))
				break;
			ConnectNamedPipe(hPipeOut, NULL);

			GetNamedPipeClientProcessId(hPipeIn, &dwClientProcessId);
			wsprintf(szBuf, TEXT("No%d(PID %x) 接続"), i, dwClientProcessId);SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)szBuf);
			set_pip_status(i, PIP_EVENT_IO_CONNECTED);

			LPPipData lpPipData = (LPPipData)HeapAlloc(GetProcessHeap(), 0, sizeof(PipData));
			lpPipData->hPipeIn = ifmng.hIPipe[i] =hPipeIn;
			lpPipData->hPipeOut = ifmng.hOPipe[i] = hPipeOut;
			lpPipData->index = i++;
			if (i >= PIPE_MAX_CLIENT) i = 0;

			HANDLE  hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NPipeThreadReadWrite, lpPipData, 0, NULL);
		}
	}
	else if (ifmng.pipe_type == CLIENT_PIPE) {
		LPPipData lpPipData = (LPPipData)HeapAlloc(GetProcessHeap(), 0, sizeof(PipData));
		lpPipData->hPipeOut = ifmng.hOPipe[0] = CreateFile(ifmng.in_pipe_name.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		lpPipData->hPipeIn = ifmng.hIPipe[0] = CreateFile(ifmng.out_pipe_name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		lpPipData->index = 0;
		SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)TEXT("Client PIPE started!！"));
		for (;;) {
			if (lpPipData->hPipeOut == INVALID_HANDLE_VALUE || lpPipData->hPipeIn == INVALID_HANDLE_VALUE) {
				if (GetLastError() == ERROR_PIPE_BUSY) {
					WaitNamedPipe(ifmng.in_pipe_name.c_str(), NMPWAIT_WAIT_FOREVER);
					lpPipData->hPipeOut = ifmng.hOPipe[0] = CreateFile(ifmng.in_pipe_name.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
					lpPipData->hPipeIn = ifmng.hIPipe[0] = CreateFile(ifmng.out_pipe_name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
					lpPipData->index = 0;
				}
				else {
					SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)TEXT("パイプへの接続に失敗しました。"));
					PulseEvent(ifmng.hevent_exit);
					ifmng.event_type[0] = 0;
					break;
				}
			}
			else { 
				SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)TEXT("Connct successful"));
				set_pip_status(lpPipData->index, PIP_EVENT_IO_CONNECTED);
				break;
			}
		}
		HANDLE  hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NPipeThreadReadWrite, lpPipData, 0, NULL);
	}
	else {
		PulseEvent(ifmng.hevent_exit);
	};
	CloseHandle(hEvent);
	return 0;
};
unsigned __stdcall CNamedPipe::NPipeThreadReadWrite(LPVOID pVoid) {
	
	LPPipData  lpData = (LPPipData)pVoid;
	HANDLE     hEvent;
	OVERLAPPED ov;
	TCHAR      szData[256], szBuf[512];
	DWORD      dwResult, dwTransferred;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	for (;;) {
		ZeroMemory(&ov, sizeof(OVERLAPPED));
		ZeroMemory(szData, sizeof(szData));
		ov.hEvent = hEvent;
		ifmng.pip_status[lpData->index] &= ~PIP_EVENT_MSGRCV;
		ReadFile(lpData->hPipeIn, szData, sizeof(szData), &dwResult, &ov);
		if (!WaitEvent(hEvent))
			break;
		GetOverlappedResult(lpData->hPipeIn, &ov, &dwTransferred, TRUE);
		if (dwTransferred == 0) {
			if (ifmng.pipe_type == SERVER_PIPE) {
				wsprintf(szBuf, TEXT("No%d 切断"), lpData->index);
				SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)szBuf);
				SetEvent(ifmng.hevent_disconnect);
			}
			else {
				MessageBox(NULL, TEXT("サーバーとの接続が切断されました。"), TEXT("クライアント"), MB_OK);
			}
			set_pip_status(lpData->index, PIP_EVENT_DISCONNECTED);

			break;
		}
		ReadFile(lpData->hPipeIn, szData, sizeof(szData), &dwResult, &ov);

		set_pip_status(lpData->index, PIP_EVENT_MSGRCV);

		wsprintf(szBuf, TEXT("No%d: %dバイト受信しました。"), lpData->index, dwTransferred);
		SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)szBuf);
		ZeroMemory(ifmng.rbufpack[lpData->index].rbuf[ifmng.rbufpack[lpData->index].wptr], PIPE_SIZE_OF_RBUF);
		ifmng.rbufpack[lpData->index].datsize[ifmng.rbufpack[lpData->index].wptr] = dwTransferred;
		memcpy(ifmng.rbufpack[lpData->index].rbuf[ifmng.rbufpack[lpData->index].wptr], szData, dwTransferred);

//		if (ifmng.pipe_type == SERVER_PIPE) {
		if ((ifmng.pipe_type == SERVER_PIPE)|| (ifmng.pipe_type == CLIENT_PIPE)) {
			ZeroMemory(ifmng.sbufpack[lpData->index].sbuf[ifmng.sbufpack[lpData->index].wptr], PIPE_SIZE_OF_SBUF);
			ifmng.sbufpack[lpData->index].datsize[ifmng.sbufpack[lpData->index].wptr] = dwTransferred;
			memcpy(ifmng.sbufpack[lpData->index].sbuf[ifmng.sbufpack[lpData->index].wptr], szData, dwTransferred);
			
			WriteFile(lpData->hPipeOut, ifmng.sbufpack[lpData->index].sbuf[ifmng.sbufpack[lpData->index].wptr], dwTransferred, &dwResult, NULL);
			set_pip_status(lpData->index, PIP_EVENT_MSGSND);
			szData[dwTransferred] = L'\0';
			wsprintf(szBuf, TEXT("No%d: %dバイト送信しました。"), lpData->index, dwTransferred);
			SendMessage(ifmng.hmsg_listbox, LB_INSERTSTRING, 0, (LPARAM)szBuf);

			//WriteFile(lpData->hPipeOut, szData, (lstrlen(szData) + 1) * sizeof(TCHAR), &dwResult, NULL);
		}
	}

	DisconnectNamedPipe(lpData->hPipeIn);
	DisconnectNamedPipe(lpData->hPipeOut);
	CloseHandle(lpData->hPipeIn);
	CloseHandle(lpData->hPipeOut);
	CloseHandle(hEvent);
	HeapFree(GetProcessHeap(), 0, lpData);

	return 0;

};

BOOL CNamedPipe::WaitEvent(HANDLE hEvent) {
	HANDLE hEventArray[2];

	hEventArray[0] = ifmng.hevent_exit;
	hEventArray[1] = hEvent;

	DWORD dwEventNo = WaitForMultipleObjects(2, hEventArray, FALSE, INFINITE) - WAIT_OBJECT_0;

	if (dwEventNo == 0)	return FALSE;
	else if (dwEventNo == 1) return TRUE;
	else return FALSE;
};

void CNamedPipe::set_pip_status(int index,unsigned id) {
	ifmng.event_type[index] |= id;
	ifmng.pip_status[index] |= id;
	SetEvent(ifmng.hEventArray[index]);
	if(id == PIP_EVENT_DISCONNECTED)ifmng.pip_status[index] &= ~PIP_EVENT_IO_CONNECTED;
	if (id & PIP_EVENT_IO_CONNECTED)ifmng.pip_status[index] &= ~PIP_EVENT_DISCONNECTED;
	if (id & PIP_EVENT_TIMEOUT) {
		ifmng.pip_status[index] &= ~PIP_EVENT_TIMEOUT;
		ifmng.event_type[index] &= ~PIP_EVENT_TIMEOUT;
	}
};
