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
	
	//共有メモリに構造体セット
	pstMobs = new(pstMobs) STMobs;
	
	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {
			pstMobs->pmobs[MOB_ID_BC][i*BC_LINE_NUM + j] = &(pstMobs->mobs.bc[i][j]);
			pstMobs->mobs.bc[i][j].status = MOB_STAT_IDLE;
			wsprintf(pstMobs->mobs.bc[i][j].type, MOB_TYPE_BC);
		}
	}

	//Mob
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

		//CUL払い出し能力
		((CCUL*)(pstMobs->pmobs[MOB_ID_CUL][i]))->ability = 250;//250kg/s = 900ton/h
	}

	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_TRIP0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_TRIPPER; i++) {
		pstMobs->pmobs[MOB_ID_TRIPPER][i] = &(pstMobs->mobs.tripper[i]);
		pstMobs->mobs.tripper[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.tripper[i].type, MOB_TYPE_TRIPPER);
		pstMobs->mobs.tripper[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_TRIPPER][i]->bmpw = BMP_WH[MOB_ID_TRIPPER][0];
		pstMobs->pmobs[MOB_ID_TRIPPER][i]->bmph = BMP_WH[MOB_ID_TRIPPER][1];

	}

	hbmp = (HBITMAP)LoadImage(pActInst->inf.hInstance, TEXT("IDB_ERM0"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	for (int i = 0; i < NUM_OF_EROOM; i++) {
		pstMobs->pmobs[MOB_ID_EROOM][i] = &(pstMobs->mobs.eroom[i]);
		pstMobs->mobs.eroom[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.eroom[i].type, MOB_TYPE_EROOM);
		pstMobs->mobs.eroom[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_EROOM][i]->bmpw = BMP_WH[MOB_ID_EROOM][0];
		pstMobs->pmobs[MOB_ID_EROOM][i]->bmph = BMP_WH[MOB_ID_EROOM][1];

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
				case 6: ((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->l = _wtol(wstrtmp.c_str()); break;
				case 7: ((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->w = _wtol(wstrtmp.c_str()); break;
				case 8: ((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[0] = _wtoi(wstrtmp.c_str()); break;
				case 9: ((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[1] = _wtoi(wstrtmp.c_str()); break;
				case 10: ((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[2] = _wtoi(wstrtmp.c_str()); break;
				case 11:((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[3] = _wtoi(wstrtmp.c_str()); break;
				case 12:((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[4] = _wtoi(wstrtmp.c_str()); break;
				case 13: ((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[5] = _wtoi(wstrtmp.c_str()); break;
				case 14:((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[6] = _wtoi(wstrtmp.c_str()); break;
				case 15:((CBC*)(pstMobs->pmobs[MOB_ID_BC][itype[MOB_ID_BC]]))->pos_rcv[7] = _wtoi(wstrtmp.c_str()); break;

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
		else if (wstrtmp == MOB_TYPE_TRIPPER) {
			if (itype[MOB_ID_TRIPPER] >= NUM_OF_TRIPPER) continue;
			int k = 0;
			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_TRIPPER][itype[MOB_ID_TRIPPER]]->name[j] = wstrtmp[j];
					break;
				default:break;
				}
				k++;
			}
			itype[MOB_ID_TRIPPER]++;
		}
		else if (wstrtmp == MOB_TYPE_EROOM) {
			if (itype[MOB_ID_EROOM] >= NUM_OF_EROOM) continue;
			int k = 0;
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
		else {
			continue;
		}
	}
	
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

	//Equipment
	//CUL
	for (int i = 0; i < NUM_OF_CUL; i++) cal_cul(i, load_cul, ms_dt,COM_CUL_IDLE);

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

	for (int i = 0; i < NUM_OF_TRIPPER; i++) {
		if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT0;
	}
/*
	for (int i = 0; i < NUM_OF_EROOM; i++) {
		if (pstMobs->pmobs[MOB_ID_EROOM][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_EROOM][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_EROOM][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_EROOM][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_EROOM][i]->status = MOB_STAT_ACT0;
	}
*/

	ms_lasttime = ((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms; //前回シミュレーション時間保持

	return;
};

int CActor::cal_cul(int i, STLOAD load, LONG dt, DWORD com) {
	if (pstMobs->pmobs[MOB_ID_CUL][i]->status == MOB_STAT_IDLE);
	else if (pstMobs->pmobs[MOB_ID_CUL][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_CUL][i]->status = MOB_STAT_ACT1;
	else if (pstMobs->pmobs[MOB_ID_CUL][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_CUL][i]->status = MOB_STAT_ACT2;
	else pstMobs->pmobs[MOB_ID_CUL][i]->status = MOB_STAT_ACT0;
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