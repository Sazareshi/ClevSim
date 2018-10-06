#include "stdafx.h"
#include "Actor.h"
#include "Smem.h"


CActor::CActor(){
}

CActor::~CActor(){
	DeleteObject(pstMobs->mobs.haraiki[0].hBmp_mob);
	DeleteObject(pstMobs->mobs.crusher[0].hBmp_mob);
	DeleteObject(pstMobs->mobs.cul[0].hBmp_mob);
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
		pstMobs->pmobs[MOB_ID_HARAI][i]->bmpw = BMP_WH[MOB_ID_HARAI][0];
		pstMobs->pmobs[MOB_ID_HARAI][i]->bmph = BMP_WH[MOB_ID_HARAI][1];

	}
	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_CRUSH0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		pstMobs->pmobs[MOB_ID_CRUSH][i] = &(pstMobs->mobs.crusher[i]);
		pstMobs->mobs.crusher[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.crusher[i].type, MOB_TYPE_CRUSH);
		pstMobs->mobs.crusher[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_CRUSH][i]->bmpw = BMP_WH[MOB_ID_CRUSH][0];
		pstMobs->pmobs[MOB_ID_CRUSH][i]->bmph = BMP_WH[MOB_ID_CRUSH][1];
	}
	
	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_CUL0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_CUL; i++) {
		pstMobs->pmobs[MOB_ID_CUL][i] = &(pstMobs->mobs.cul[i]);
		pstMobs->mobs.cul[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.cul[i].type, MOB_TYPE_CUL);
		pstMobs->mobs.cul[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_CUL][i]->bmpw = BMP_WH[MOB_ID_CUL][0];
		pstMobs->pmobs[MOB_ID_CUL][i]->bmph = BMP_WH[MOB_ID_CUL][1];

	}


	///# INIファイル読み込み
	wchar_t tbuf[32];
	DWORD	str_num = GetPrivateProfileString(PATH_SECT_OF_INIFILE, PATH_KEY_OF_MOBCSV, L"mob.csv", tbuf, sizeof(tbuf), PATH_OF_INIFILE);

	///# CSVファイル読み込み
	wstring	wstrpath = PATH_OF_EXEFILE; wstrpath += tbuf;
	wifstream ifs(wstrpath);
	wstring wstr;
	int itype[MOB_TYPE_NUM]; for (int i = 0; i < MOB_TYPE_NUM; i++)itype[i] = 0;

	getline(ifs, wstr);//ヘッダ行読込

	while (getline(ifs, wstr)) {
		wstring wstrtmp;
		wistringstream wstream(wstr);
		getline(wstream, wstrtmp, L',');//Typeキー読込
		if (wstrtmp == MOB_TYPE_BC) {
			if (itype[MOB_ID_BC] >= NUM_OF_BC) continue;
			int k = 0;
			while (getline(wstream, wstrtmp, L',')) {
				switch(k) {
				case 0:pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]->name[j] = wstrtmp[j];
					break;
				default:break;
				}
				k++;
			}
			itype[MOB_ID_BC]++;
		}
		else if (wstrtmp == MOB_TYPE_HARAI) {
			if (itype[MOB_ID_HARAI] >= NUM_OF_HARAI) continue;
			int k = 0;
			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->name[j] = wstrtmp[j];
						break;
				default:break;
				}
				k++;
			}
			itype[MOB_ID_HARAI]++;
		}
		else if (wstrtmp == MOB_TYPE_CRUSH) {
			if (itype[MOB_ID_CRUSH] >= NUM_OF_CRUSH) continue;
			int k = 0;
			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pstMobs->pmobs[MOB_ID_CRUSH][itype[MOB_ID_CRUSH]]->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pstMobs->pmobs[MOB_ID_CRUSH][itype[MOB_ID_CRUSH]]->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pstMobs->pmobs[MOB_ID_CRUSH][itype[MOB_ID_CRUSH]]->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pstMobs->pmobs[MOB_ID_CRUSH][itype[MOB_ID_CRUSH]]->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pstMobs->pmobs[MOB_ID_CRUSH][itype[MOB_ID_CRUSH]]->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j<wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_CRUSH][itype[MOB_ID_CRUSH]]->name[j] = wstrtmp[j];
						break;
				default:break;
				}
				k++;
			}
			itype[MOB_ID_CRUSH]++;
		}
		else if (wstrtmp == MOB_TYPE_CUL) {
			if (itype[MOB_ID_CUL] >= NUM_OF_CUL) continue;
			int k = 0;
			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->name[j] = wstrtmp[j];
					break;
				default:break;
				}
				k++;
			}
			itype[MOB_ID_CUL]++;
		}
		else {
			continue;
		}
	}
	return;
}

void CActor::routine_work(void *param) {
	//COMMON
	ws << L"ACTOR work activated!" << *(inf.psys_counter); tweet2owner(ws.str()); ws.str(L""); ws.clear();
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		if(pstMobs->pmobs[MOB_ID_CRUSH][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_CRUSH][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT0;
	}

	for (int i = 0; i < NUM_OF_HARAI; i++) {
		if (pstMobs->pmobs[MOB_ID_HARAI][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_HARAI][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT0;
	}

	for (int i = 0; i < NUM_OF_CUL; i++) {
		if (pstMobs->pmobs[MOB_ID_CUL][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_CUL][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_CUL][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_CUL][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_CUL][i]->status = MOB_STAT_ACT0;

	}
	return;
};
