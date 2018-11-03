#include "stdafx.h"
#include "Actor.h"
#include "Smem.h"


CActor::CActor(){
	sim_accel_rate = 1;
}

CActor::~CActor(){
	DeleteObject(pstMobs->mobs.haraiki[0].hBmp_mob);
	DeleteObject(pstMobs->mobs.crusher[0].hBmp_mob);
	DeleteObject(pstMobs->mobs.cul[0].hBmp_mob);
}

CActor * CActor::pActInst;
//表示用アイコンBitMapのサイズをここでまとめて設定
int CActor::BMP_WH[MOB_TYPE_NUM][2] = { { 32,32 },{ 16,16 },{ 32,32 } ,{ 48,40 },{16,24 },{ 24,24 } };

void CActor::init_task(void* pobj) {

	pActInst = (CActor *)pobj;//スタティック変数にインスタンスポインタ登録

	 ///# 共有メモリ関連セット
	pstMobs = (LPSTMobs)&(((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.mobs);
	hmue_mobs = inf.hSmem_mutex[MUTEXID_MOBS];

	ULONG* pSimtime_ms = &(((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms);
	
	//共有メモリに構造体セット
	pstMobs = new(pstMobs) STMobs;


	//Mob
	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {
			pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j] = &(pstMobs->mobs.bc[i][j]);
			pstMobs->mobs.bc[i][j].status = MOB_STAT_IDLE;
			wsprintf(pstMobs->mobs.bc[i][j].type, MOB_TYPE_BC);
			pstMobs->mobs.bc[i][j].ptime_now = pSimtime_ms;
		}
	}

	for (int i = 0; i < SILO_LINES; i++) {
		for (int j = 0; j < SILO_LINE_NUM; j++) {
			pstMobs->pmobs[MOB_ID_SILO][i*SILO_LINE_NUM + j] = &(pstMobs->mobs.silo[i][j]);
			pstMobs->mobs.silo[i][j].status = MOB_STAT_IDLE;
			wsprintf(pstMobs->mobs.silo[i][j].type, MOB_TYPE_SILO);
			pstMobs->mobs.silo[i][j].ptime_now = pSimtime_ms;
		}
	}

	HBITMAP hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_HARAI0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_HARAI; i++) {
		pstMobs->pmobs[MOB_ID_HARAI][i] = &(pstMobs->mobs.haraiki[i]);
		pstMobs->mobs.haraiki[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.haraiki[i].type, MOB_TYPE_HARAI);
		pstMobs->mobs.haraiki[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_HARAI][i]->bmpw = BMP_WH[MOB_ID_HARAI][0];
		pstMobs->pmobs[MOB_ID_HARAI][i]->bmph = BMP_WH[MOB_ID_HARAI][1];
		pstMobs->pmobs[MOB_ID_HARAI][i]->ptime_now = pSimtime_ms;
	}

	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_CRUSH0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		pstMobs->pmobs[MOB_ID_CRUSH][i] = &(pstMobs->mobs.crusher[i]);
		pstMobs->mobs.crusher[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.crusher[i].type, MOB_TYPE_CRUSH);
		pstMobs->mobs.crusher[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_CRUSH][i]->bmpw = BMP_WH[MOB_ID_CRUSH][0];
		pstMobs->pmobs[MOB_ID_CRUSH][i]->bmph = BMP_WH[MOB_ID_CRUSH][1];
		pstMobs->pmobs[MOB_ID_CRUSH][i]->ptime_now = pSimtime_ms;
	}
	
	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_CUL0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_CUL; i++) {
		pstMobs->pmobs[MOB_ID_CUL][i] = &(pstMobs->mobs.cul[i]);
		pstMobs->mobs.cul[i].status = MOB_STAT_ACT0;
		wsprintf(pstMobs->mobs.cul[i].type, MOB_TYPE_CUL);
		pstMobs->mobs.cul[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_CUL][i]->bmpw = BMP_WH[MOB_ID_CUL][0];
		pstMobs->pmobs[MOB_ID_CUL][i]->bmph = BMP_WH[MOB_ID_CUL][1];
		pstMobs->pmobs[MOB_ID_CUL][i]->ptime_now = pSimtime_ms;

		//CUL払い出し能力
		((CCUL*)(pstMobs->pmobs[MOB_ID_CUL][i]))->ability = 250;//250kg/s = 900ton/h

		//Line選択
		((CCUL*)(pstMobs->pmobs[MOB_ID_CUL][i]))->bc_selbc = i;
	}

	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_TRIP0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_TRIPPER; i++) {
		pstMobs->pmobs[MOB_ID_TRIPPER][i] = &(pstMobs->mobs.tripper[i]);
		pstMobs->mobs.tripper[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.tripper[i].type, MOB_TYPE_TRIPPER);
		pstMobs->mobs.tripper[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_TRIPPER][i]->bmpw = BMP_WH[MOB_ID_TRIPPER][0];
		pstMobs->pmobs[MOB_ID_TRIPPER][i]->bmph = BMP_WH[MOB_ID_TRIPPER][1];
		pstMobs->pmobs[MOB_ID_TRIPPER][i]->ptime_now = pSimtime_ms;

	}

	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_ERM0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_EROOM; i++) {
		pstMobs->pmobs[MOB_ID_EROOM][i] = &(pstMobs->mobs.eroom[i]);
		pstMobs->mobs.eroom[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.eroom[i].type, MOB_TYPE_EROOM);
		pstMobs->mobs.eroom[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_EROOM][i]->bmpw = BMP_WH[MOB_ID_EROOM][0];
		pstMobs->pmobs[MOB_ID_EROOM][i]->bmph = BMP_WH[MOB_ID_EROOM][1];
		pstMobs->pmobs[MOB_ID_EROOM][i]->ptime_now = pSimtime_ms;
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
			if (itype[MOB_ID_BC] >= BC_LINES * BC_LINE_NUM) continue;
			int k = 0;

			CBC* pobj = (CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]);

			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pobj->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pobj->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pobj->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pobj->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pobj->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pobj->name[j] = wstrtmp[j];
					break;
				case 6: pobj->l = _wtol(wstrtmp.c_str()); break;
				case 7: pobj->w = _wtol(wstrtmp.c_str()); break;
				case 8: pobj->base_spd = _wtoi(wstrtmp.c_str()); break;
				case 9: pobj->BCtype = stoi(wstrtmp.c_str(), nullptr, 16); break;
				case 10: pobj->dir = stoi(wstrtmp.c_str(), nullptr, 16); break;
				case 11: pobj->ability = _wtoi(wstrtmp.c_str()); break;
				case 12:pobj->pos_rcv[0] = _wtoi(wstrtmp.c_str()); break;
				case 13:pobj->pos_rcv[1] = _wtoi(wstrtmp.c_str()); break;
				case 14:pobj->pos_rcv[2] = _wtoi(wstrtmp.c_str()); break;
				case 15:pobj->pos_rcv[3] = _wtoi(wstrtmp.c_str()); break;
				case 16:pobj->pos_rcv[4] = _wtoi(wstrtmp.c_str()); break;
				case 17:pobj->pos_rcv[5] = _wtoi(wstrtmp.c_str()); break;
				case 18:pobj->pos_rcv[6] = _wtoi(wstrtmp.c_str()); break;
				case 19:pobj->pos_rcv[7] = _wtoi(wstrtmp.c_str()); break;
				case 20:pobj->pos_rcv[8] = _wtoi(wstrtmp.c_str()); break;
				case 21:pobj->pos_rcv[9] = _wtoi(wstrtmp.c_str()); break;
				case 22:pobj->pos_rcv[10] = _wtoi(wstrtmp.c_str()); break;
				case 23:pobj->pos_rcv[11] = _wtoi(wstrtmp.c_str()); break;
				default:break;
				}
				k++;
			}

			itype[MOB_ID_BC]++;
		}
		else if (wstrtmp == MOB_TYPE_HARAI) {
			if (itype[MOB_ID_HARAI] >= NUM_OF_HARAI) continue;
			int k = 0;

			pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->exist = ON;
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

			pstMobs->pmobs[MOB_ID_CRUSH][itype[MOB_ID_CRUSH]]->exist = ON;
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

			pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->exist = ON;
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
		else if (wstrtmp == MOB_TYPE_TRIPPER) {
			if (itype[MOB_ID_TRIPPER] >= NUM_OF_TRIPPER) continue;
			int k = 0;

			pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]->exist = ON;
			while (getline(wstream, wstrtmp, L',')) {
				CTripper* pobj = (CTripper *)(pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]);
				switch (k) {
				case 0:pobj->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pobj->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pobj->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pobj->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pobj->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pobj->name[j] = wstrtmp[j];
					break;
				case 6: break;
				case 7: break;
				case 8: pobj->spd = _wtoi(wstrtmp.c_str()); break;
				case 9: break;
				case 10: break;
				case 11: pobj->ability = _wtoi(wstrtmp.c_str()); break;
				case 12:pobj->pos_drop[0] = _wtoi(wstrtmp.c_str()); break;
				case 13:pobj->pos_drop[1] = _wtoi(wstrtmp.c_str()); break;
				case 14:pobj->pos_drop[2] = _wtoi(wstrtmp.c_str()); break;
				case 15:pobj->pos_drop[3] = _wtoi(wstrtmp.c_str()); break;
				case 16:pobj->pos_drop[4] = _wtoi(wstrtmp.c_str()); break;
				case 17:pobj->pos_drop[5] = _wtoi(wstrtmp.c_str()); break;
				case 18:pobj->pos_drop[6] = _wtoi(wstrtmp.c_str()); break;
				case 19:pobj->pos_drop[7] = _wtoi(wstrtmp.c_str()); break;
				case 20:pobj->pos_drop[8] = _wtoi(wstrtmp.c_str()); break;
				case 21:pobj->pos_drop[9] = _wtoi(wstrtmp.c_str()); break;
				case 22:pobj->pos_drop[10] = _wtoi(wstrtmp.c_str()); break;
				case 23:pobj->pos_drop[11] = _wtoi(wstrtmp.c_str()); break;
				default:break;
				}
				k++;
			}
			itype[MOB_ID_TRIPPER]++;
		}
		else if (wstrtmp == MOB_TYPE_EROOM) {
			if (itype[MOB_ID_EROOM] >= NUM_OF_EROOM) continue;
			int k = 0;

			pstMobs->pmobs[MOB_ID_EROOM][itype[MOB_ID_EROOM]]->exist = ON;
			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pstMobs->pmobs[MOB_ID_EROOM][itype[MOB_ID_EROOM]]->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pstMobs->pmobs[MOB_ID_EROOM][itype[MOB_ID_EROOM]]->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pstMobs->pmobs[MOB_ID_EROOM][itype[MOB_ID_EROOM]]->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pstMobs->pmobs[MOB_ID_EROOM][itype[MOB_ID_EROOM]]->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pstMobs->pmobs[MOB_ID_EROOM][itype[MOB_ID_EROOM]]->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_EROOM][itype[MOB_ID_EROOM]]->name[j] = wstrtmp[j];
					break;
				default:break;
				}
				k++;
			}
			itype[MOB_ID_EROOM]++;
		}
		else if (wstrtmp == MOB_TYPE_SILO) {
			if (itype[MOB_ID_SILO] >= BC_LINES * BC_LINE_NUM) continue;
			int k = 0;

			CSilo* pobj = (CSilo*)(pstMobs->pmobs[MOB_ID_SILO][itype[MOB_ID_SILO]]);

			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pobj->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pobj->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pobj->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pobj->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pobj->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pobj->name[j] = wstrtmp[j];
					break;
				case 6: pobj->l = _wtol(wstrtmp.c_str()); break;
				case 7: pobj->w = _wtol(wstrtmp.c_str()); break;
				case 8: break;
				case 9: pobj->SILOtype = stoi(wstrtmp.c_str(), nullptr, 16); break;
				case 10: ; break;
				case 11: pobj->capa_all = _wtoi(wstrtmp.c_str()); break;
				default:break;
				}
				k++;
			}

			itype[MOB_ID_SILO]++;
		}
		else {
			continue;
		}
	}

	init_bclink();//BCの接続設定
	init_bc();//BC関連初期化


	///CUL積荷初期値
	load_cul.density = 1000;//密度kg/m3  1000kg/m3
	load_cul.material = LD_COAL1; //素材成分　
	load_cul.weight = load_cul.density;//1m当たりの重量kg
	load_cul.vol = 1000;//	体積　1000cm3(l)

	return;
}

void CActor::routine_work(void *param) {

	//COMMON
	((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms = *(inf.psys_counter) * sim_accel_rate * SYSTEM_TICK_ms;
	ms_dt = ((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms - ms_lasttime;
	ws << L"ACTOR work activated!" << *(inf.psys_counter); tweet2owner(ws.str()); ws.str(L""); ws.clear();

	//PANEL
	for (int i = 0; i < NUM_OF_EROOM; i++) {
		if (pstMobs->pmobs[MOB_ID_EROOM][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_EROOM][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_EROOM][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_EROOM][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_EROOM][i]->status = MOB_STAT_ACT0;
	}

	//Equipment
	//BC
	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {
			if ((pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j])->exist == ON){
				DWORD index = i; index = (index << 16) + j;
				cal_bc(index, ms_dt, MOB_COM_UPDATE);
			}
		}
	}
	//SILO
	for (int i = 0; i < SILO_LINES; i++) {
		for (int j = 0; j < SILO_LINE_NUM; j++) {
			if ((pstMobs->pmobs[MOB_ID_SILO][i*SILO_LINE_NUM + j])->exist == ON) {
				DWORD index = i; index = (index << 16) + j;
				cal_silo(index, ms_dt, MOB_COM_UPDATE);
			}
		}
	}
	//CUL
	for (int i = 0; i < NUM_OF_CUL; i++) cal_cul(i, load_cul, ms_dt,COM_CUL_IDLE);
	//CRUSHER
	for (int i = 0; i < NUM_OF_CRUSH; i++) {
		if(pstMobs->pmobs[MOB_ID_CRUSH][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_CRUSH][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT0;
	}
	//HARAIDASHIKI
	for (int i = 0; i < NUM_OF_HARAI; i++) {
		if (pstMobs->pmobs[MOB_ID_HARAI][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_HARAI][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT0;
	}
	//TRIPPER
	for (int i = 0; i < NUM_OF_TRIPPER; i++) {
		if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT0;
	}

	ms_lasttime = ((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms; //前回シミュレーション時間保持

	return;
};

int CActor::cal_cul(DWORD i, STLOAD load, LONG dt, DWORD com) {
	CCUL* pcul =&(pstMobs->mobs.cul[i]);
	if (pcul->status == MOB_STAT_IDLE);
	else if (pcul->status == MOB_STAT_ACT0) pcul->status = MOB_STAT_ACT1;
	else if (pcul->status == MOB_STAT_ACT1) pcul->status = MOB_STAT_ACT2;
	else pcul->status = MOB_STAT_ACT0;

	if (pcul->status == MOB_STAT_IDLE) pcul->discharge(COM_CUL_IDLE, dt);
	else  pcul->discharge(COM_CUL_DISCHARGE, dt);

	pstMobs->pmobs[MOB_ID_CUL][i]->time_last = *(pstMobs->pmobs[MOB_ID_CUL][i]->ptime_now);
	return 0;
};

int CActor::cal_bc(DWORD index, LONG dt, DWORD com) {
	int line = HIWORD(index);
	int npos = LOWORD(index);
	CBC* pobj = &(pstMobs->mobs.bc[line][npos]);

	pobj->conveyor(com, dt);

	return 0;
};

int CActor::cal_silo(DWORD index, LONG dt, DWORD com) {
	int line = HIWORD(index);
	int npos = LOWORD(index);
	CSilo* pobj = &(pstMobs->mobs.silo[line][npos]);

	return 0;
};

int CActor::cal_tripper(DWORD index, LONG dt, DWORD com) {
	CTripper* pobj = &(pstMobs->mobs.tripper[index]);

	return 0;
};



LRESULT CALLBACK  CActor::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	int n;

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); break;

		case IDC_TASK_ITEM_RADIO1:
		case IDC_TASK_ITEM_RADIO2:
		case IDC_TASK_ITEM_RADIO3:
		case IDC_TASK_ITEM_RADIO4:
		case IDC_TASK_ITEM_RADIO5:
		case IDC_TASK_ITEM_RADIO6:
			inf.panel_type_id = LOWORD(wp); set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			break;
		case IDSET: {
			wstring wstr, wstr_tmp;
			if ((inf.panel_func_id == IDC_TASK_FUNC_RADIO1) && (inf.panel_type_id == IDC_TASK_ITEM_RADIO1)) {
				GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
				n = stoi(wstr_tmp);
				if ((n == 1) ||(n == 2)) { 
					if((pstMobs->pmobs[MOB_ID_CUL][n-1])->status == MOB_STAT_IDLE) (pstMobs->pmobs[MOB_ID_CUL][n - 1])->status = MOB_STAT_ACT0;
					else (pstMobs->pmobs[MOB_ID_CUL][n - 1])->status = MOB_STAT_IDLE;
				}
				else;
			}

/*
			//サンプルとしていろいろな型で読み込んで表示している
			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stod(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 2(i):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT2, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stoi(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 3(f):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT3, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stof(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 4(l):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT4, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stol(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 5(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT5, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			wstr += L",   Param 6(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT6, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			txout2msg_listbox(wstr);
*/

		}break;
		case IDRESET: {
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		}break;

		case IDC_TASK_OPTION_CHECK1:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK1) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION1;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;

		case IDC_TASK_OPTION_CHECK2:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK2) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION2;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;
		}

	}
	return 0;
};

void CActor::set_panel_tip_txt()
{
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"Type for Func1 \n\r 1:Change CUL Status 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:CUL No. 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO2: {
		wstr = L"Type for Func2 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO3: {
		wstr = L"Type for Func3 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"Type for Func4 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO5: {
		wstr = L"Type for Func5 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO6: {
		wstr = L"Type for Func6 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	default: {
		wstr = L"Type for Func? \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		wstr_type += L"(Param of type?) \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
	}break;
	}

	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET3), wstr.c_str());
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET4), wstr_type.c_str());
}

void CActor::init_bc() {//BC関連初期設定
	//Tripperセット
	(pstMobs->mobs.bc[LINE_A][BC_L5]).ptrp = &(pstMobs->mobs.tripper[LINE_A]);
	(pstMobs->mobs.bc[LINE_B][BC_L5]).ptrp = &(pstMobs->mobs.tripper[LINE_B]);
	(pstMobs->mobs.bc[LINE_B][BC_L5]).ptrp = &(pstMobs->mobs.tripper[LINE_C]);


	CBC* pbc;
	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {
			pbc = &(pstMobs->mobs.bc[i][j]);
			pbc->belt_size = (pbc->l) >> 10;
		//# 
			pbc->Kg100perM = pbc->ability * 2500 / 9 /pbc->base_spd;// pbc->ability * 1000 / 3600 / (pbc->base_spd / 1000)
			if (pbc->ID != 0) pbc->exist = ON;
			else  pbc->exist = OFF;

			if ((pbc->dir)&MASK_DIR_Y)pbc->pix2mm = pbc->l / pbc->area.h;
			else pbc->pix2mm = pbc->l / pbc->area.w;

		//# 画像上の接続ポイント設定
			//ヘッド画像位置
			if (pbc->dir & MASK_DIR_Y) {//縦型
				if (pbc->dir & MASK_DIR_UP) {//下向
					pbc->imgpt_tail.x = pbc->area.x + pbc->area.w / 2;
					pbc->imgpt_tail.y = pbc->area.y;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w/2 ;
						if (!(pbc->BCtype & 0x0003)) {//ダンパ、シュート無し
							pbc->imgpt_top[k].y = pbc->area.y + 3;
						}
						else {
							pbc->imgpt_top[k].y = pbc->area.y - (pbc->area.w / 2 + pbc->area.w * k);
						}
					}
				}
				else {
					pbc->imgpt_tail.x = pbc->area.x + pbc->area.w / 2;
					pbc->imgpt_tail.y = pbc->area.y + pbc->area.h;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						if (!(pbc->BCtype & 0x0003)) {//ダンパ、シュート無し
							pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h - 3;
						}
						else {
							pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h + pbc->area.w / 2 + pbc->area.w * k;
						}
						pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w / 2;
					}

				}
			}
			else {//横型
				if (pbc->dir & MASK_DIR_LEFT) {
					pbc->imgpt_tail.x = pbc->area.x + pbc->area.w;
					pbc->imgpt_tail.y = pbc->area.y + pbc->area.h / 2;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						if (!(pbc->BCtype & 0x0003)) {//ダンパ、シュート無し
							pbc->imgpt_top[k].x = pbc->area.x + 3;
						}
						else {
							pbc->imgpt_top[k].x = pbc->area.x + pbc->area.h / 2 - pbc->area.h * k;
						}
						pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h /2;
					}

				}
				else {
					pbc->imgpt_tail.x = pbc->area.x;
					pbc->imgpt_tail.y = pbc->area.y + pbc->area.h / 2;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						if (!(pbc->BCtype & 0x0003)) {//ダンパ、シュート無し
							pbc->imgpt_top[0].x = pbc->area.x + pbc->area.w - 3;
						}
						else {
							pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w - pbc->area.h / 2 + pbc->area.h * k;
						}
						pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h / 2;
					}
				}
			}
			//荷受け位置
			for (int k = 0; k < BC_RCV_MAX; k++) {
				if (pbc->dir & MASK_DIR_Y) {//縦型
					if (pbc->dir & MASK_DIR_UP) {//上向
						pbc->imgpt_rcv[k].x = pbc->area.x + pbc->area.w / 2;
						pbc->imgpt_rcv[k].y = pbc->area.y + pbc->area.h - ((pbc->area.h * pbc->pos_rcv[k]) / pbc->l);
					}
					else {
						pbc->imgpt_rcv[k].x = pbc->area.x + pbc->area.w / 2;
						pbc->imgpt_rcv[k].y = pbc->area.y + ((pbc->area.h * pbc->pos_rcv[k]) / pbc->l);
					}
				}
				else {
					if (pbc->dir & MASK_DIR_LEFT) {
						pbc->imgpt_rcv[k].x = pbc->area.x + pbc->area.w - ((pbc->area.w * pbc->pos_rcv[k]) / pbc->l);
						pbc->imgpt_rcv[k].y = pbc->area.y + pbc->area.h / 2;
					}
					else {
						pbc->imgpt_rcv[k].x = pbc->area.x + ((pbc->area.w * pbc->pos_rcv[k]) / pbc->l);
						pbc->imgpt_rcv[k].y = pbc->area.y + pbc->area.h / 2;
					}
				}
			}
		}
	}

	return;
};

void CActor::init_bclink() {//BCの接続設定

	//CUL
	//CUL01
	pstMobs->mobs.cul[0].bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][0]); pstMobs->mobs.cul[0].bclink_i[DUMP1] = 0;//BC1A 1
	pstMobs->mobs.cul[0].bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][0]); pstMobs->mobs.cul[0].bclink_i[DUMP2] = 0;//BC1B 1
																												  //CUL02
	pstMobs->mobs.cul[1].bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][0]); pstMobs->mobs.cul[1].bclink_i[DUMP1] = 1;//BC1A 2
	pstMobs->mobs.cul[1].bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][0]); pstMobs->mobs.cul[1].bclink_i[DUMP2] = 1;//BC1B 2

 //BCA
	{
		//1A
		(pstMobs->mobs.bc[LINE_A][BC_L1]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L2]); (pstMobs->mobs.bc[LINE_A][BC_L1]).bclink_i[DUMP1] = 0;//BC2A 1
		(pstMobs->mobs.bc[LINE_A][BC_L1]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L2]); (pstMobs->mobs.bc[LINE_A][BC_L1]).bclink_i[DUMP2] = 0;//BC2A 1
																																					//2A	
		(pstMobs->mobs.bc[LINE_A][BC_L2]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L3]); (pstMobs->mobs.bc[LINE_A][BC_L2]).bclink_i[DUMP1] = 0;//BC3A 1
		(pstMobs->mobs.bc[LINE_A][BC_L2]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L3]); (pstMobs->mobs.bc[LINE_A][BC_L2]).bclink_i[DUMP2] = 0;//BC3A 1
																																					//3A	
		(pstMobs->mobs.bc[LINE_A][BC_L3]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L4_1]); (pstMobs->mobs.bc[LINE_A][BC_L3]).bclink_i[DUMP1] = 0;//BC41A 1
		(pstMobs->mobs.bc[LINE_A][BC_L3]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L5]); (pstMobs->mobs.bc[LINE_A][BC_L3]).bclink_i[DUMP2] = 0;//BC5A 1
																																					//4-1A	
		(pstMobs->mobs.bc[LINE_A][BC_L4_1]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L4_2]); (pstMobs->mobs.bc[LINE_A][BC_L4_1]).bclink_i[DUMP1] = 0;//BC42A 1
		(pstMobs->mobs.bc[LINE_A][BC_L4_1]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L5]); (pstMobs->mobs.bc[LINE_A][BC_L4_1]).bclink_i[DUMP2] = 0;//BC5B 1
																																						//4-2A	
		(pstMobs->mobs.bc[LINE_A][BC_L4_2]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L4_3]); (pstMobs->mobs.bc[LINE_A][BC_L4_2]).bclink_i[DUMP1] = 0;//BC43A 1
		(pstMobs->mobs.bc[LINE_A][BC_L4_2]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_C][BC_L5]); (pstMobs->mobs.bc[LINE_A][BC_L4_2]).bclink_i[DUMP2] = 0;//BC5C 1

																																						//5A
		(pstMobs->mobs.bc[LINE_A][BC_L5]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L5]); (pstMobs->mobs.bc[LINE_A][BC_L5]).bclink_i[DUMP1] = 0;//BC5A 1
		(pstMobs->mobs.bc[LINE_A][BC_L5]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L5]); (pstMobs->mobs.bc[LINE_A][BC_L5]).bclink_i[DUMP2] = 0;//BC5A 1
		(pstMobs->mobs.bc[LINE_A][BC_L5]).silolink = &(pstMobs->mobs.silo[LINE_A][3]);
	}
//BCB
	{
		//1B
		(pstMobs->mobs.bc[LINE_B][BC_L1]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L2]); (pstMobs->mobs.bc[LINE_B][BC_L1]).bclink_i[DUMP1] = 0;//BC2A 1
		(pstMobs->mobs.bc[LINE_B][BC_L1]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L2]); (pstMobs->mobs.bc[LINE_B][BC_L1]).bclink_i[DUMP2] = 0;//BC2A 1
																																					//2B	
		(pstMobs->mobs.bc[LINE_B][BC_L2]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L3]); (pstMobs->mobs.bc[LINE_B][BC_L2]).bclink_i[DUMP1] = 0;//BC3A 1
		(pstMobs->mobs.bc[LINE_B][BC_L2]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L3]); (pstMobs->mobs.bc[LINE_B][BC_L2]).bclink_i[DUMP2] = 0;//BC3A 1
																																					//3B	
		(pstMobs->mobs.bc[LINE_B][BC_L3]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L4_1]); (pstMobs->mobs.bc[LINE_B][BC_L3]).bclink_i[DUMP1] = 0;//BC41A 1
		(pstMobs->mobs.bc[LINE_B][BC_L3]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L5]); (pstMobs->mobs.bc[LINE_B][BC_L3]).bclink_i[DUMP2] = 0;//BC5A 1
																																					//4-1B	
		(pstMobs->mobs.bc[LINE_B][BC_L4_1]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L4_2]); (pstMobs->mobs.bc[LINE_B][BC_L4_1]).bclink_i[DUMP1] = 0;//BC42A 1
		(pstMobs->mobs.bc[LINE_B][BC_L4_1]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L5]); (pstMobs->mobs.bc[LINE_B][BC_L4_1]).bclink_i[DUMP2] = 0;//BC5B 1
																																						//4-2B	
		(pstMobs->mobs.bc[LINE_B][BC_L4_2]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L4_3]); (pstMobs->mobs.bc[LINE_B][BC_L4_2]).bclink_i[DUMP1] = 0;//BC43A 1
		(pstMobs->mobs.bc[LINE_B][BC_L4_2]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_C][BC_L5]); (pstMobs->mobs.bc[LINE_B][BC_L4_2]).bclink_i[DUMP2] = 0;//BC5C 1

																																						//5B
		(pstMobs->mobs.bc[LINE_B][BC_L5]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L5]); (pstMobs->mobs.bc[LINE_B][BC_L5]).bclink_i[DUMP1] = 0;//BC5B 1
		(pstMobs->mobs.bc[LINE_B][BC_L5]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L5]); (pstMobs->mobs.bc[LINE_B][BC_L5]).bclink_i[DUMP2] = 0;//BC5B 1
		(pstMobs->mobs.bc[LINE_B][BC_L5]).silolink = &(pstMobs->mobs.silo[LINE_B][3]);
	}
//BCC
	{
		//5C
		(pstMobs->mobs.bc[LINE_C][BC_L5]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_C][BC_L5]); (pstMobs->mobs.bc[LINE_C][BC_L5]).bclink_i[DUMP1] = 0;//BC5C 1
		(pstMobs->mobs.bc[LINE_C][BC_L5]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_C][BC_L5]); (pstMobs->mobs.bc[LINE_C][BC_L5]).bclink_i[DUMP2] = 0;//BC5C 1
		(pstMobs->mobs.bc[LINE_C][BC_L5]).silolink = &(pstMobs->mobs.silo[LINE_C][3]);
	}
 
}