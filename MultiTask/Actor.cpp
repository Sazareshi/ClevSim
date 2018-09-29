#include "stdafx.h"
#include "Actor.h"


CActor::CActor(){}

CActor::~CActor(){}

CActor * CActor::pActInst;
LPSTSmemAct CActor::pActSmem;

void CActor::init_task(void* pobj) {

	pActInst = (CActor *)pobj;//スタティック変数にインスタンスポインタ登録
	
	//共有メモリに構造体セット
	pActSmem = new(pActInst->inf.pSmem) STSmemAct;


	///# INIファイル読み込み
	wchar_t tbuf[32];
	//	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);

}
