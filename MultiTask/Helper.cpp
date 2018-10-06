#include "stdafx.h"
#include "Helper.h"

CHelper::CHelper(){}

CHelper::~CHelper(){}

/*****************************************************************************
string型をwstring型に変換します
- 引数　const std::string &src：変換前入力　, std::wstring &dest：変換後出力　
- 戻り値　NA
*****************************************************************************/
void CHelper::Str2Wstr(const std::string &src, std::wstring &dest) {
	
	setlocale(LC_ALL, "");
	
	wchar_t *wcs = new wchar_t[src.length() + 1];
	
	size_t ret_val;
	mbstowcs_s(&ret_val, wcs, src.length() + 1, src.c_str(), _TRUNCATE);
	
	dest = wcs;delete[] wcs;
	return;
}

/*****************************************************************************
wstring型をstring型に変換します
- 引数　const std::wstring &dest：変換前入力　, std::string &src：変換後出力　
- 戻り値　NA
*****************************************************************************/
void CHelper::WStr2Str(const std::wstring &src, std::string &dest){

	setlocale(LC_ALL, "");

	char *str = new char[src.length() + 1];

	size_t ret_val;
	wcstombs_s(&ret_val, str, src.length() + 1, src.c_str(), _TRUNCATE);

	dest = str; delete[] str;
	return;
}

/*****************************************************************************************
指定されたDC、位置に指定されたビットマップを書き込む（等倍）
- 引数	HDC hdc：書込先DC, HBITMAP hbmp：素材ビットマップ,
		POINT dst_pt：書込先の位置, POINT src_pt：書込元の位置
- 戻り値　NA
******************************************************************************************/
void CHelper::put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt) {

	if ((hdc == NULL) | (hbmp == NULL)) return;

	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	int BMP_W = (int)bmp.bmWidth;
	int BMP_H = (int)bmp.bmHeight;

	HDC hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);

	BitBlt(hdc, dst_pt.x, dst_pt.y, BMP_W, BMP_H, hmdc, src_pt.x, src_pt.y, SRCCOPY);
	DeleteDC(hmdc);

	return;
};

/******************************************************************************************************
指定されたDC、位置に指定されたビットマップを書き込む（変倍）
- 引数	HDC hdc：書込先DC, HBITMAP hbmp：素材ビットマップ,
		POINT dst_pt：書込先の位置, POINT src_pt：書込元の位置,int retio_percent:書込先倍率
- 戻り値	NA
******************************************************************************************************/
void CHelper::put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_percent) {

	if ((hdc == NULL) | (hbmp == NULL)) return;

	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	int BMP_W = (int)bmp.bmWidth;
	int BMP_H = (int)bmp.bmHeight;

	HDC hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);

	StretchBlt(hdc, dst_pt.x, dst_pt.y, BMP_W*retio_percent / 100, BMP_H * retio_percent / 100, hmdc, src_pt.x, src_pt.y, BMP_W, BMP_H, SRCCOPY);
	DeleteDC(hmdc);

	return;
};

/****************************************************************************************************************************************************
共有メモリを確保する
- 引数
	-入力　LPCTSTR szName：共有メモリ名, DWORD dwSize：確保するサイズ,
	-出力　HANDLE* hMapFile：ファイルマップオブジェクトハンドル, LPVOID* pMapTop:共有メモリ先頭アドレス, DWORD* dwExist:　GetLastError()エラー有り

- 戻り値　0:正常完了　-1,-2:異常完了
****************************************************************************************************************************************************/
int CHelper::cmnCreateShmem(LPCTSTR szName, DWORD dwSize, HANDLE* hMapFile, LPVOID* pMapTop, DWORD* dwExist) {
	DWORD	highByte = 0;			// 共有メモリは32bitサイズ以内を想定
	DWORD	lowByte = dwSize;		// 32bitサイズ以上は定義不可

									// 初期化
	*hMapFile = NULL;
	*pMapTop = NULL;
	*dwExist = OFF;

	// ファイル・マッピング・オブジェクトを作成(ページファイルを使用 ,セキュリティ属性なし ,読み込み/書き込みアクセス ,共有メモリのサイズ ,共有メモリ名)
	*hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, highByte, lowByte, szName);
	if (*hMapFile == NULL) return(ERR_SHMEM_CREATE);
	else {
		if (GetLastError() == ERROR_ALREADY_EXISTS)	*dwExist = ON;
		// ファイル・ビューを作成(共有メモリのハンドル, アクセスの種類, ファイル オフセット(上位32ビット), ファイル オフセット(下位32ビット), マップするバイト数→0はファイルﾙマッピングオブジェクト全体)
		*pMapTop = MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, dwSize);
		if (*pMapTop == NULL) {
			CloseHandle(*hMapFile);	// 共有メモリを閉じる
			*hMapFile = NULL; *pMapTop = NULL;
			return(ERR_SHMEM_VIEW);
		}
	}
	return(OK_SHMEM);
}

/****************************************************************************************************************************************************
共有メモリを開放する
- 引数　HANDLE* hMapFile,：解放先ファイルマップオブジェクト LPVOID* pMapTop：解放先メモリ先頭アドレス
- 戻り値　0:正常完了,-2:異常完了
****************************************************************************************************************************************************/
int	CHelper::cmnDeleteShMem(HANDLE* hMapFile, LPVOID* pMapTop)
{
	int		ret = OK_SHMEM;		// 関数ステータス
								
	if (*pMapTop != NULL) {
		if (!UnmapViewOfFile(*pMapTop)) {	// ファイル・ビューの削除
			ret = ERR_SHMEM_VIEW;
		}
	}

	// ファイル・マッピング・オブジェクトを閉じる
	if (*hMapFile != NULL) {
		CloseHandle(*hMapFile);
	}

	// ポインタ初期化
	*hMapFile = NULL;
	*pMapTop = NULL;

	return(ret);
}

/****************************************************************************************************************************************************
WORDの配列をwstring文字列にする
****************************************************************************************************************************************************/
wstring CHelper::warray2wstr16(WORD* w, int len) {
	wostringstream ws;
	ws << std::hex;
	for (int i = 0; i < len; i++) {
		ws << setw(4) <<setfill(L'0')<< *(w + i);
	}
	return ws.str();
};
/****************************************************************************************************************************************************
CHARの配列をwstring文字列にする
****************************************************************************************************************************************************/
wstring CHelper::carray2wstr16(char* c, int len) {
	wostringstream ws;
	ws << std::hex;
	for (int i = 0; i < len; i++) {
		ws << setw(2) << setfill(L'0') << (0x00ff &(WORD)(*(c + i)));
	}
	return ws.str();
};


/****************************************************************************************************************************************************
文字列を区切り文字でtokenに分割する

****************************************************************************************************************************************************/
void CHelper::splitbydelimiter(TCHAR token[][SPLIT_TOKEN_SIZE], PTCHAR wstr, int size, TCHAR delimiter) {
	int ptr = 0;
	int idx = 0;

	for (unsigned int i = 0; i < lstrlen(wstr); i++) {
		if ((*(wstr + i) != delimiter) && (*(wstr + i) != TEXT('\r')) && (*(wstr + i) != TEXT('\n')))	token[idx][ptr++] = *(wstr + i);
		else {
			token[idx++][ptr] = TEXT('\0');
			ptr = 0;
			if (idx >= size)break;
		}
	}

}