#include "stdafx.h"
#include "Actor.h"


CActor::CActor(){}

CActor::~CActor(){}

CActor * CActor::pActInst;
LPSTSmemAct CActor::pActSmem;

void CActor::init_task(void* pobj) {

	pActInst = (CActor *)pobj;//�X�^�e�B�b�N�ϐ��ɃC���X�^���X�|�C���^�o�^
	
	//���L�������ɍ\���̃Z�b�g
	pActSmem = new(pActInst->inf.pSmem) STSmemAct;


	///# INI�t�@�C���ǂݍ���
	wchar_t tbuf[32];
	//	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);

}
