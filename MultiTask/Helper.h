#pragma once

//  共有メモリステータス
#define	OK_SHMEM			0			// 共有メモリ 生成/破棄正常
#define	ERR_SHMEM_CREATE	-1			// 共有メモリ Create異常
#define	ERR_SHMEM_VIEW		-2			// 共有メモリ View異常

#define	ON				1		// ON
#define	OFF				0		// OFF

#define	SPLIT_TOKEN_SIZE	128	// 

using namespace std;

class CHelper
{
public:
	CHelper();
	~CHelper();
	void Str2Wstr(const std::string &src, std::wstring &dest);
	void WStr2Str(const std::wstring &src, std::string &dest);
	void put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt);
	void put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_persent);
	int cmnCreateShmem(LPCTSTR, DWORD, HANDLE*, LPVOID*, DWORD*);	//共有メモリ設定
	int	cmnDeleteShMem(HANDLE*, LPVOID*);							//共有メモリ解除

	wstring warray2wstr16(WORD* w, int len);
	wstring carray2wstr16(char* c, int len);

	void splitbydelimiter(TCHAR token[][SPLIT_TOKEN_SIZE], PTCHAR pwstr, int size, TCHAR delimiter);

};
