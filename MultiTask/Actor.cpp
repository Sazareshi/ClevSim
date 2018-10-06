#include "stdafx.h"
#include "Actor.h"
#include "Smem.h"


CActor::CActor(){
}

CActor::~CActor(){
	DeleteObject(pstMobs->mobs.haraiki[0].hBmp_mob);
	DeleteObject(pstMobs->mobs.crusher[0].hBmp_mob);
}

CActor * CActor::pActInst;
int CActor::BMP_WH[MOB_TYPE_NUM][2] = { { 32,32 },{ 16,16 },{ 32,32 } ,{ 48,40 } };

void CActor::init_task(void* pobj) {

	pActInst = (CActor *)pobj;//スタティック変数にインスタンスポインタ登録

	 ///# 共有メモリ関連セット
	pstMobs = (LPSTMobs)&(((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.mobs);
	hmue_mobs = inf.hSmem_mutex[MUTEXID_MOBS];
	
	//共有メモリに構造体セット
	pstMobs = new(pstMobs) STMobs;
//	pstMobs = new STMobs;
	
	for (int i = 0; i < NUM_OF_BC; i++) {
		pstMobs->pmobs[MOB_ID_BC][i] = &(pstMobs->mobs.bc[i]);
		pstMobs->mobs.bc[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.bc[i].type, MOB_TYPE_BC);
		}

	HBITMAP hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_HARAI0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_HARAI; i++) {
		pstMobs->pmobs[MOB_ID_HARAI][i] = &(pstMobs->mobs.haraiki[i]);
		pstMobs->mobs.haraiki[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.haraiki[i].type, MOB_TYPE_HARAI);
		pstMobs->mobs.haraiki[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_HARAI][i]->area.bmpw = BMP_WH[MOB_ID_HARAI][0];
		pstMobs->pmobs[MOB_ID_HARAI][i]->area.bmph = BMP_WH[MOB_ID_HARAI][1];

	}
	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_CRUSH0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		pstMobs->pmobs[MOB_ID_CRUSH][i] = &(pstMobs->mobs.crusher[i]);
		pstMobs->mobs.crusher[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.crusher[i].type, MOB_TYPE_CRUSH);
		pstMobs->mobs.crusher[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_CRUSH][i]->area.bmpw = BMP_WH[MOB_ID_CRUSH][0];
		pstMobs->pmobs[MOB_ID_CRUSH][i]->area.bmph = BMP_WH[MOB_ID_CRUSH][1];
	}
	
	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_CUL0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_CUL; i++) {
		pstMobs->pmobs[MOB_ID_CUL][i] = &(pstMobs->mobs.cul[i]);
		pstMobs->mobs.cul[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.cul[i].type, MOB_TYPE_CUL);
		pstMobs->mobs.cul[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_CUL][i]->area.bmpw = BMP_WH[MOB_ID_CUL][0];
		pstMobs->pmobs[MOB_ID_CUL][i]->area.bmph = BMP_WH[MOB_ID_CUL][1];

	}


	///# INIファイル読み込み
	//wchar_t tbuf[32];
	//	DWORD	str_num = GetPrivateProfileString(COMM_SECT_OF_INIFILE, MC_PROTOKEY_OF_TYPE, L"C", tbuf, sizeof(tbuf), PATH_OF_INIFILE);

	///# CSVファイル読み込み

}

void CActor::routine_work(void *param) {
	//COMMON
	ws << L"ACTOR work activated!" << *(inf.psys_counter); tweet2owner(ws.str()); ws.str(L""); ws.clear();
	
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		if (pstMobs->pmobs[MOB_ID_CRUSH][i]->area.x < 1000) pstMobs->pmobs[MOB_ID_CRUSH][i]->area.x += 4 * (i+1);
		else  pstMobs->pmobs[MOB_ID_CRUSH][i]->area.x = 32;

		pstMobs->pmobs[MOB_ID_CRUSH][i]->area.y =  50*(i+1);

	}

	for (int i = 0; i < NUM_OF_HARAI; i++) {
		pstMobs->pmobs[MOB_ID_HARAI][i]->area.x = 100 * (i + 1);
		if (pstMobs->pmobs[MOB_ID_HARAI][i]->area.y < 1000) pstMobs->pmobs[MOB_ID_HARAI][i]->area.y += 4 * (i + 1);
		else  pstMobs->pmobs[MOB_ID_HARAI][i]->area.y = 16;

	}

	for (int i = 0; i < NUM_OF_CUL; i++) {
		pstMobs->pmobs[MOB_ID_CUL][i]->area.x = 290;
		pstMobs->pmobs[MOB_ID_CUL][i]->area.y = 5 + 55*i;

	}
	return;
};
