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
//�\���p�A�C�R��BitMap�̃T�C�Y�������ł܂Ƃ߂Đݒ�
int CActor::BMP_WH[MOB_TYPE_NUM][2] = { { 32,32 },{ 16,16 },{ 32,32 } ,{ 48,40 },{16,24 },{ 24,24 } };

void CActor::init_task(void* pobj) {

	pActInst = (CActor *)pobj;//�X�^�e�B�b�N�ϐ��ɃC���X�^���X�|�C���^�o�^

	 ///# ���L�������֘A�Z�b�g
	pstMobs = (LPSTMobs)&(((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.mobs);
	hmue_mobs = inf.hSmem_mutex[MUTEXID_MOBS];

	ULONG* pSimtime_ms = &(((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms);
	
	//���L�������ɍ\���̃Z�b�g
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
		pstMobs->mobs.cul[i].status = MOB_STAT_IDLE;
		wsprintf(pstMobs->mobs.cul[i].type, MOB_TYPE_CUL);
		pstMobs->mobs.cul[i].hBmp_mob = hbmp;
		pstMobs->pmobs[MOB_ID_CUL][i]->bmpw = BMP_WH[MOB_ID_CUL][0];
		pstMobs->pmobs[MOB_ID_CUL][i]->bmph = BMP_WH[MOB_ID_CUL][1];
		pstMobs->pmobs[MOB_ID_CUL][i]->ptime_now = pSimtime_ms;

		//CUL�����o���\��
		//((CCUL*)(pstMobs->pmobs[MOB_ID_CUL][i]))->ability = 250;//250kg/s = 900ton/h

		//Line�I��
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

	///# INI�t�@�C���ǂݍ���
	wchar_t tbuf[32];
	DWORD	str_num = GetPrivateProfileString(PATH_SECT_OF_INIFILE, PATH_KEY_OF_MOBCSV, L"mob.csv", tbuf, sizeof(tbuf), PATH_OF_INIFILE);

	///# CSV�t�@�C���ǂݍ���
	wstring	wstrpath = PATH_OF_EXEFILE; wstrpath += tbuf;
	wifstream ifs(wstrpath);
	wstring wstr;
	int itype[MOB_TYPE_NUM]; for (int i = 0; i < MOB_TYPE_NUM; i++)itype[i] = 0;
	getline(ifs, wstr);//�w�b�_�s�Ǎ�
	while (getline(ifs, wstr)) {
		wstring wstrtmp;
		wistringstream wstream(wstr);
		getline(wstream, wstrtmp, L',');//Type�L�[�Ǎ�
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

			CHarai* pobj = (CHarai *)pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]];
			pobj->exist = ON;
			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pobj->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pobj->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pobj->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pobj->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pobj->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_HARAI][itype[MOB_ID_HARAI]]->name[j] = wstrtmp[j];
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
			CCUL* pobj = (CCUL *) pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]];
			pobj->exist = ON;
			while (getline(wstream, wstrtmp, L',')) {
				switch (k) {
				case 0:pobj->ID = _wtoi(wstrtmp.c_str()); break;
				case 1:pobj->area.x = _wtoi(wstrtmp.c_str()); break;
				case 2:pobj->area.y = _wtoi(wstrtmp.c_str()); break;
				case 3:pobj->area.w = _wtoi(wstrtmp.c_str()); break;
				case 4:pobj->area.h = _wtoi(wstrtmp.c_str()); break;
				case 5:
					for (int j = 0; j < wstrtmp.size(); j++) pstMobs->pmobs[MOB_ID_CUL][itype[MOB_ID_CUL]]->name[j] = wstrtmp[j];
					break;
				case 6: break;
				case 7: break;
				case 8: break;
				case 9: break;
				case 10: break;
				case 11: pobj->ability =( _wtoi(wstrtmp.c_str()))*1000/3600; break;//T/H->Kg/s
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
				case 12:pobj->pos_drop[0][0] = _wtoi(wstrtmp.c_str()); break;
				case 13:pobj->pos_drop[0][1] = _wtoi(wstrtmp.c_str()); break;
				case 14:pobj->pos_drop[0][2] = _wtoi(wstrtmp.c_str()); break;
				case 15:pobj->pos_drop[0][3] = _wtoi(wstrtmp.c_str()); break;
				case 16:pobj->pos_drop[0][4] = _wtoi(wstrtmp.c_str()); break;
				case 17:pobj->pos_drop[0][5] = _wtoi(wstrtmp.c_str()); break;
				case 18:pobj->pos_drop[1][0] = _wtoi(wstrtmp.c_str()); break;
				case 19:pobj->pos_drop[1][1] = _wtoi(wstrtmp.c_str()); break;
				case 20:pobj->pos_drop[1][2] = _wtoi(wstrtmp.c_str()); break;
				case 21:pobj->pos_drop[1][3] = _wtoi(wstrtmp.c_str()); break;
				case 22:pobj->pos_drop[1][4] = _wtoi(wstrtmp.c_str()); break;
				case 23:pobj->pos_drop[1][5] = _wtoi(wstrtmp.c_str()); break;
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
			if (itype[MOB_ID_SILO] >= SILO_LINES * SILO_LINE_NUM) continue;
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
				case 10: pobj->dir = stoi(wstrtmp.c_str(), nullptr, 16); break;
				case 11: pobj->capa_all = _wtoi(wstrtmp.c_str()); break;
				case 12: pobj->pos_bc_origin_put = _wtoi(wstrtmp.c_str()); break;
				case 13: pobj->pos_bc_origin_pop = _wtoi(wstrtmp.c_str()); break;
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

	init_bclink();//BC�̐ڑ��ݒ�
	init_bc();//BC�֘A������
	init_trp();//Tripper�֘A������
	init_silo();//BC�T�C���֘A������


	///CUL�ω׏����l
	load_cul.density = 1000;//���xkg/m3  1000kg/m3
	load_cul.material = LD_COAL1; //�f�ސ����@
	load_cul.weight = load_cul.density;//1m������̏d��kg

	// CMob �|�C���^�}�b�v�Z�b�g
	CPublicRelation pr;
	pr.set_mobmap(COM_MOBMAP_ALL);

	return;
}

void CActor::routine_work(void *param) {

	//COMMON
	((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms = *(inf.psys_counter) * sim_accel_rate * SYSTEM_TICK_ms;
	ms_dt = ((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms - ms_lasttime;
	ws << L"ACTOR work activated!" << *(inf.psys_counter); tweet2owner(ws.str()); ws.str(L""); ws.clear();

	//PANEL
	for (int i = 0; i < NUM_OF_EROOM; i++) {
		if (pstMobs->pmobs[MOB_ID_EROOM][i]->status == MOB_STAT_IDLE);
		else if (pstMobs->pmobs[MOB_ID_EROOM][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_EROOM][i]->status = MOB_STAT_ACT1;
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
		if (pstMobs->pmobs[MOB_ID_CRUSH][i]->status == MOB_STAT_IDLE);
		else if(pstMobs->pmobs[MOB_ID_CRUSH][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_CRUSH][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_CRUSH][i]->status = MOB_STAT_ACT0;
	}
	//HARAIDASHIKI
	for (int i = 0; i < NUM_OF_HARAI; i++) {
		if (pstMobs->pmobs[MOB_ID_HARAI][i]->status == MOB_STAT_IDLE);
		else if (pstMobs->pmobs[MOB_ID_HARAI][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_HARAI][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_HARAI][i]->status = MOB_STAT_ACT0;
	}
	//TRIPPER
	for (int i = 0; i < NUM_OF_TRIPPER; i++) {
		if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->command == COM_TRP_IDLE) pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_IDLE;
		else if (!(pstMobs->pmobs[MOB_ID_TRIPPER][i]->command & COM_TRP_DISCHARGE)) pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT0;
		else if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->status == MOB_STAT_IDLE) pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT0;
		else if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->status == MOB_STAT_ACT0)pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT1;
		else if (pstMobs->pmobs[MOB_ID_TRIPPER][i]->status == MOB_STAT_ACT1)pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT2;
		else pstMobs->pmobs[MOB_ID_TRIPPER][i]->status = MOB_STAT_ACT0;
		cal_tripper(i,ms_dt, pstMobs->pmobs[MOB_ID_TRIPPER][i]->command);
	}

	ms_lasttime = ((P_ST_SMEM_FORMAT)inf.pSmem)->stSmem.simtime_ms; //�O��V�~�����[�V�������ԕێ�

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
#if 1
	for (int i = 0; i < SILO_COLUMN_NUM; i++) {
		int def1, def2;
		if (i == SILO_COLUMN_NUM - 1) {
			def1 = pobj->column[i].weight - pobj->column[i - 1].weight;
			if (def1 > pobj->thresh_level) {
				pobj->column[i].weight -= pobj->thresh_level / 4;
				pobj->column[i-1].weight += pobj->thresh_level / 4;
			}
		}
		else if(i == 0) {
			def2 = pobj->column[i].weight - pobj->column[i + 1].weight;
			if (def2 > pobj->thresh_level) {
				pobj->column[i].weight -= pobj->thresh_level / 4;
				pobj->column[i+1].weight += pobj->thresh_level / 4;
			}
		}
		else {
			def1 = pobj->column[i].weight - pobj->column[i - 1].weight;
			def2 = pobj->column[i].weight - pobj->column[i + 1].weight;
	
			if ((def1> pobj->thresh_level/2) && (def2 > pobj->thresh_level/2)){
				pobj->column[i].weight -= pobj->thresh_level / 4;
				pobj->column[i + 1].weight += pobj->thresh_level / 8;
				pobj->column[i - 1].weight += pobj->thresh_level / 8;
			}
			else if (def2 > pobj->thresh_level) {
				pobj->column[i].weight -= pobj->thresh_level / 4;
				pobj->column[i + 1].weight += pobj->thresh_level / 4;
			}
			else if (def1 > pobj->thresh_level) {
				pobj->column[i].weight -= pobj->thresh_level / 4;
				pobj->column[i-1].weight += pobj->thresh_level / 4;
			}
			else;

		}
	}
#endif
	return 0;
};

int CActor::cal_tripper(DWORD index, LONG dt, DWORD com) {
	CTripper* pobj = &(pstMobs->mobs.tripper[index]);
	int iret = pobj->move(com, dt, pobj->get_target());

	//�g���b�p�ړ��Ń}�b�v�X�V
	if (iret) {//�ړ��L��
		CPublicRelation pr;
		if (pobj->ID == LINE_A)  pr.set_mobmap(COM_MOBMAP_5A);
		else if (pobj->ID == LINE_B) pr.set_mobmap(COM_MOBMAP_5B);
		else if (pobj->ID == LINE_C) pr.set_mobmap(COM_MOBMAP_5C); 
		else;
	}

	return 0;
};
int CActor::cal_harai(DWORD index, LONG dt, DWORD com) {
	CHarai* pobj = &(pstMobs->mobs.haraiki[index]);
	int iret = pobj->move(com, dt, pobj->get_target());

	//���o�@�ړ��Ń}�b�v�X�V
	if (iret) {//�ړ��L��
		CPublicRelation pr;
		if (pobj->ID == HARAI_11A)  pr.set_mobmap(COM_MOBMAP_11A);
		else if (pobj->ID == HARAI_11B) pr.set_mobmap(COM_MOBMAP_11B);
		else if (pobj->ID == HARAI_11C) pr.set_mobmap(COM_MOBMAP_11C);
		else if (pobj->ID == HARAI_12A)  pr.set_mobmap(COM_MOBMAP_12A);
		else if (pobj->ID == HARAI_12B) pr.set_mobmap(COM_MOBMAP_12B);
		else if (pobj->ID == HARAI_12C) pr.set_mobmap(COM_MOBMAP_12C);
		else if (pobj->ID == HARAI_13A)  pr.set_mobmap(COM_MOBMAP_13A);
		else if (pobj->ID == HARAI_13B) pr.set_mobmap(COM_MOBMAP_13B);
		else if (pobj->ID == HARAI_13C) pr.set_mobmap(COM_MOBMAP_13C);
		else;
	}

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
			//�T���v���Ƃ��Ă��낢��Ȍ^�œǂݍ���ŕ\�����Ă���
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

void CActor::init_bc() {//BC�֘A�����ݒ�
	//Tripper�Z�b�g
	(pstMobs->mobs.bc[LINE_A][BC_L5]).ptrp = (CMob*)(&(pstMobs->mobs.tripper[LINE_A]));
	(pstMobs->mobs.bc[LINE_B][BC_L5]).ptrp = (CMob*)(&(pstMobs->mobs.tripper[LINE_B]));
	(pstMobs->mobs.bc[LINE_C][BC_L5]).ptrp = (CMob*)(&(pstMobs->mobs.tripper[LINE_C]));
	(pstMobs->mobs.bc[LINE_A][BC_L8]).ptrp = (CMob*)(&(pstMobs->mobs.tripper[LINE_D]));
	(pstMobs->mobs.bc[LINE_B][BC_L8]).ptrp = (CMob*)(&(pstMobs->mobs.tripper[LINE_E]));


	CBC* pbc;
	for (int i = 0; i < BC_LINES; i++) {
		for (int j = 0; j < BC_LINE_NUM; j++) {

			pbc = &(pstMobs->mobs.bc[i][j]);
			pbc->belt_size = (pbc->l) >> 10;

			if (j == 19)
				int tmp = 1;
		//# 
			pbc->Kg100perM = pbc->ability * 2500 / 9 /pbc->base_spd;// pbc->ability * 1000 / 3600 / (pbc->base_spd / 1000)
			if (pbc->ID != 0) pbc->exist = ON;
			else  pbc->exist = OFF;

			if ((pbc->dir)&MASK_DIR_Y)pbc->pix2mm = pbc->l / pbc->area.h;
			else pbc->pix2mm = pbc->l / pbc->area.w;

		//# �摜��̐ڑ��|�C���g�ݒ�
			//�w�b�h�摜�ʒu
			if (pbc->dir & MASK_DIR_Y) {//�c�^
				if (pbc->dir & MASK_DIR_UP) {//���
					pbc->imgpt_tail.x = pbc->area.x + pbc->area.w / 2;
					pbc->imgpt_tail.y = pbc->area.y;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w/2 ;
						if (!(pbc->BCtype & (BC_DUMPER | BC_TSHOOT))) {//�_���p�A�V���[�g����
							pbc->imgpt_top[k].y = pbc->area.y - 3;
						}
						else {
							pbc->imgpt_top[k].y = pbc->area.y - 8;
						}
						pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w / 2;
					}
				}
				else {
					pbc->imgpt_tail.x = pbc->area.x + pbc->area.w / 2;
					pbc->imgpt_tail.y = pbc->area.y + pbc->area.h;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						if (!(pbc->BCtype & (BC_DUMPER | BC_TSHOOT))) {//�_���p�A�V���[�g����
							pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h + 3;
						}
						else {
							pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h + 8;
						}
						pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w / 2;
					}

				}
			}
			else {//���^
				if (pbc->dir & MASK_DIR_LEFT) {
					pbc->imgpt_tail.x = pbc->area.x + pbc->area.w;
					pbc->imgpt_tail.y = pbc->area.y + pbc->area.h / 2;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						if (!(pbc->BCtype & (BC_DUMPER | BC_TSHOOT))) {//�_���p�A�V���[�g����
							pbc->imgpt_top[k].x = pbc->area.x - 3;
						}
						else {
							pbc->imgpt_top[k].x = pbc->area.x - 8;
						}
						pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h /2;
					}

				}
				else {
					pbc->imgpt_tail.x = pbc->area.x;
					pbc->imgpt_tail.y = pbc->area.y + pbc->area.h / 2;
					for (int k = 0; k < BC_LINK_MAX; k++) {
						if (!(pbc->BCtype & (BC_DUMPER | BC_TSHOOT))) {//�_���p�A�V���[�g����
							pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w + 3;
						}
						else {
							pbc->imgpt_top[k].x = pbc->area.x + pbc->area.w + 8;
						}
						pbc->imgpt_top[k].y = pbc->area.y + pbc->area.h / 2;
					}
				}
			}
			//�׎󂯈ʒu
			for (int k = 0; k < BC_RCV_MAX; k++) {
				if (pbc->dir & MASK_DIR_Y) {//�c�^
					if (pbc->dir & MASK_DIR_UP) {//���
						pbc->imgpt_rcv[k].x = pbc->area.x + pbc->area.w / 2;
	//					pbc->imgpt_rcv[k].y = pbc->area.y + pbc->area.h - ((pbc->area.h * pbc->pos_rcv[k]*1000) / pbc->l);
						pbc->imgpt_rcv[k].y = pbc->area.y + ((pbc->area.h * pbc->pos_rcv[k] * 1000) / pbc->l);
					}
					else {
						pbc->imgpt_rcv[k].x = pbc->area.x + pbc->area.w / 2;
	//					pbc->imgpt_rcv[k].y = pbc->area.y + ((pbc->area.h * pbc->pos_rcv[k]*1000) / pbc->l);
						pbc->imgpt_rcv[k].y = pbc->area.y + pbc->area.h - ((pbc->area.h * pbc->pos_rcv[k] * 1000) / pbc->l);
					}
				}
				else {
					if (pbc->dir & MASK_DIR_LEFT) {
//						pbc->imgpt_rcv[k].x = pbc->area.x + pbc->area.w - ((pbc->area.w * pbc->pos_rcv[k]*1000) / pbc->l);
						pbc->imgpt_rcv[k].x = pbc->area.x + ((pbc->area.w * pbc->pos_rcv[k] * 1000) / pbc->l);
						pbc->imgpt_rcv[k].y = pbc->area.y + pbc->area.h / 2;
					}
					else {
//						pbc->imgpt_rcv[k].x = pbc->area.x + ((pbc->area.w * pbc->pos_rcv[k]*1000) / pbc->l);
						pbc->imgpt_rcv[k].x = pbc->area.x + pbc->area.w - ((pbc->area.w * pbc->pos_rcv[k] * 1000) / pbc->l);
						pbc->imgpt_rcv[k].y = pbc->area.y + pbc->area.h / 2;
					}
				}
			}
		}
	}
	return;
};
void CActor::init_trp() {//TRIPPER�֘A�����ݒ�
	//BC�Z�b�g
	pstMobs->mobs.tripper[LINE_A].pbc = &(pstMobs->mobs.bc[LINE_A][BC_L5]);
	pstMobs->mobs.tripper[LINE_B].pbc = &(pstMobs->mobs.bc[LINE_B][BC_L5]);
	pstMobs->mobs.tripper[LINE_C].pbc = &(pstMobs->mobs.bc[LINE_C][BC_L5]);
	pstMobs->mobs.tripper[LINE_D].pbc = &(pstMobs->mobs.bc[LINE_A][BC_L8]);
	pstMobs->mobs.tripper[LINE_E].pbc = &(pstMobs->mobs.bc[LINE_B][BC_L8]);


	//SILO�Z�b�g
	for (int i = 0; i < 4; i++) {
		pstMobs->mobs.tripper[LINE_A].psilo[i] = &(pstMobs->mobs.silo[LINE_A][i]);
		pstMobs->mobs.tripper[LINE_B].psilo[i] = &(pstMobs->mobs.silo[LINE_B][i]);
		pstMobs->mobs.tripper[LINE_C].psilo[i] = &(pstMobs->mobs.silo[LINE_C][i]);
		pstMobs->mobs.tripper[LINE_D].psilo[i] = &(pstMobs->mobs.silo[LINE_D][i]);
		pstMobs->mobs.tripper[LINE_E].psilo[i] = &(pstMobs->mobs.silo[LINE_E][i]);

	}

	for (int i = 0; i < NUM_OF_TRIPPER; i++) {
		pstMobs->mobs.tripper[i].set_param();
	}

	return;
};
void CActor::init_bclink() {//BC�̐ڑ��ݒ�

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

		(pstMobs->mobs.bc[LINE_A][BC_L4_3]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L6]); (pstMobs->mobs.bc[LINE_A][BC_L4_3]).bclink_i[DUMP1] = 0;//BC6A 1
		(pstMobs->mobs.bc[LINE_A][BC_L4_3]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L6]); (pstMobs->mobs.bc[LINE_A][BC_L4_3]).bclink_i[DUMP2] = 0;//BC6A 1

		(pstMobs->mobs.bc[LINE_A][BC_L6]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L7]); (pstMobs->mobs.bc[LINE_A][BC_L6]).bclink_i[DUMP1] = 0;//BC7A 1
		(pstMobs->mobs.bc[LINE_A][BC_L6]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L8]); (pstMobs->mobs.bc[LINE_A][BC_L6]).bclink_i[DUMP2] = 0;//BC8A 1

		(pstMobs->mobs.bc[LINE_A][BC_L7]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L9]); (pstMobs->mobs.bc[LINE_A][BC_L7]).bclink_i[DUMP1] = 0;//BC7A 1
		(pstMobs->mobs.bc[LINE_A][BC_L7]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L8]); (pstMobs->mobs.bc[LINE_A][BC_L7]).bclink_i[DUMP2] = 0;//BC8B 1

		(pstMobs->mobs.bc[LINE_A][BC_L8]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L8]); (pstMobs->mobs.bc[LINE_A][BC_L8]).bclink_i[DUMP1] = 0;//BC8A 1
		(pstMobs->mobs.bc[LINE_A][BC_L8]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L8]); (pstMobs->mobs.bc[LINE_A][BC_L8]).bclink_i[DUMP2] = 0;//BC8A 1
		(pstMobs->mobs.bc[LINE_A][BC_L8]).silolink = &(pstMobs->mobs.silo[LINE_D][2]);

		(pstMobs->mobs.bc[LINE_A][BC_L9]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L9_1]); (pstMobs->mobs.bc[LINE_A][BC_L9]).bclink_i[DUMP1] = 0;//BC9_1A 1
		(pstMobs->mobs.bc[LINE_A][BC_L9]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_L9]).bclink_i[DUMP2] = 0;//BC9_2A 1

		(pstMobs->mobs.bc[LINE_A][BC_L9_1]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_L9_1]).bclink_i[DUMP1] = 0;//BC9_2B 1
		(pstMobs->mobs.bc[LINE_A][BC_L9_1]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_L9_1]).bclink_i[DUMP2] = 0;//BC9_2B 1

		(pstMobs->mobs.bc[LINE_A][BC_L9_2]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_L9_2]).bclink_i[DUMP1] = 0;//BC9_2B 1
		(pstMobs->mobs.bc[LINE_A][BC_L9_2]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_L9_2]).bclink_i[DUMP2] = 0;//BC9_2B 1
		(pstMobs->mobs.bc[LINE_A][BC_L9_2]).silolink = &(pstMobs->mobs.silo[LINE_F][0]);

		(pstMobs->mobs.bc[LINE_A][BC_L11]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L11]).bclink_i[DUMP1] = 9;//BC19A 9
		(pstMobs->mobs.bc[LINE_A][BC_L11]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L11]).bclink_i[DUMP2] = 9;//BC19B 9

		(pstMobs->mobs.bc[LINE_A][BC_L12]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L12]).bclink_i[DUMP1] = 6;//BC19A 6
		(pstMobs->mobs.bc[LINE_A][BC_L12]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L12]).bclink_i[DUMP2] = 6;//BC19B 6

		(pstMobs->mobs.bc[LINE_A][BC_L13]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L13]).bclink_i[DUMP1] = 3;//BC19A 3
		(pstMobs->mobs.bc[LINE_A][BC_L13]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L13]).bclink_i[DUMP2] = 3;//BC19B 3

		(pstMobs->mobs.bc[LINE_A][BC_L15]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_A][BC_L15]).bclink_i[DUMP1] = 7;//BC18A 8
		(pstMobs->mobs.bc[LINE_A][BC_L15]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_A][BC_L15]).bclink_i[DUMP2] = 7;//BC18B 8

		(pstMobs->mobs.bc[LINE_A][BC_L16]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_A][BC_L16]).bclink_i[DUMP1] = 4;//BC18A 5
		(pstMobs->mobs.bc[LINE_A][BC_L16]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_A][BC_L16]).bclink_i[DUMP2] = 4;//BC18B 5

		(pstMobs->mobs.bc[LINE_A][BC_L17]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_A][BC_L17]).bclink_i[DUMP1] = 1;//BC18A 2
		(pstMobs->mobs.bc[LINE_A][BC_L17]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_A][BC_L17]).bclink_i[DUMP2] = 1;//BC18B 2

		(pstMobs->mobs.bc[LINE_A][BC_L18]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L19]).bclink_i[DUMP1] = 0;//BC19A 1
		(pstMobs->mobs.bc[LINE_A][BC_L18]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L19]).bclink_i[DUMP2] = 0;//BC19B 1
		(pstMobs->mobs.bc[LINE_A][BC_L18]).bclink[DUMP3] = &(pstMobs->mobs.bc[LINE_C][BC_L19]); (pstMobs->mobs.bc[LINE_A][BC_L19]).bclink_i[DUMP3] = 0;//BC19C 1
		(pstMobs->mobs.bc[LINE_A][BC_L18]).bclink[DUMP4] = &(pstMobs->mobs.bc[LINE_C][BC_L32]); (pstMobs->mobs.bc[LINE_A][BC_L19]).bclink_i[DUMP4] = 0;//BC32C 1

		(pstMobs->mobs.bc[LINE_A][BC_L19]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L20]); (pstMobs->mobs.bc[LINE_A][BC_L19]).bclink_i[DUMP1] = 0;//BC20A 1
		(pstMobs->mobs.bc[LINE_A][BC_L19]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L20]); (pstMobs->mobs.bc[LINE_A][BC_L19]).bclink_i[DUMP2] = 0;//BC20B 1
		(pstMobs->mobs.bc[LINE_A][BC_L19]).bclink[DUMP3] = &(pstMobs->mobs.bc[LINE_C][BC_L30]); (pstMobs->mobs.bc[LINE_A][BC_L19]).bclink_i[DUMP3] = 0;//BC30C 1

		(pstMobs->mobs.bc[LINE_A][BC_L20]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L21]); (pstMobs->mobs.bc[LINE_A][BC_L20]).bclink_i[DUMP1] = 0;//BC21A 1
		(pstMobs->mobs.bc[LINE_A][BC_L20]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L21]); (pstMobs->mobs.bc[LINE_A][BC_L20]).bclink_i[DUMP2] = 0;//BC21A 1

		(pstMobs->mobs.bc[LINE_A][BC_L21]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L22]); (pstMobs->mobs.bc[LINE_A][BC_L21]).bclink_i[DUMP1] = 0;//BC22A 1
		(pstMobs->mobs.bc[LINE_A][BC_L21]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L22]); (pstMobs->mobs.bc[LINE_A][BC_L21]).bclink_i[DUMP2] = 0;//BC22A 1

		(pstMobs->mobs.bc[LINE_A][BC_L22]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L23]); (pstMobs->mobs.bc[LINE_A][BC_L22]).bclink_i[DUMP1] = 0;//BC23A 1
		(pstMobs->mobs.bc[LINE_A][BC_L22]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L23]); (pstMobs->mobs.bc[LINE_A][BC_L22]).bclink_i[DUMP2] = 0;//BC23A 1


		(pstMobs->mobs.bc[LINE_A][BC_L23]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L23]); (pstMobs->mobs.bc[LINE_A][BC_L23]).bclink_i[DUMP1] = 0;//BC23A 1
		(pstMobs->mobs.bc[LINE_A][BC_L23]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L23]); (pstMobs->mobs.bc[LINE_A][BC_L23]).bclink_i[DUMP2] = 0;//BC23A 1
		(pstMobs->mobs.bc[LINE_A][BC_L23]).silolink = &(pstMobs->mobs.silo[LINE_I][0]);//BANKER 2���@

		(pstMobs->mobs.bc[LINE_A][BC_LRC]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_LRC]).bclink_i[DUMP1] = 0;//BC9_2A 1
		(pstMobs->mobs.bc[LINE_A][BC_LRC]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_LRC]).bclink_i[DUMP2] = 0;//BC9_2A 1

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

		(pstMobs->mobs.bc[LINE_B][BC_L4_3]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L6]); (pstMobs->mobs.bc[LINE_B][BC_L4_3]).bclink_i[DUMP1] = 0;//BC6B 1
		(pstMobs->mobs.bc[LINE_B][BC_L4_3]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L6]); (pstMobs->mobs.bc[LINE_B][BC_L4_3]).bclink_i[DUMP2] = 0;//BC6B 1

		(pstMobs->mobs.bc[LINE_B][BC_L6]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L7]); (pstMobs->mobs.bc[LINE_B][BC_L6]).bclink_i[DUMP1] = 0;//BC7B 1
		(pstMobs->mobs.bc[LINE_B][BC_L6]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L8]); (pstMobs->mobs.bc[LINE_B][BC_L6]).bclink_i[DUMP2] = 0;//BC8A 1

		(pstMobs->mobs.bc[LINE_B][BC_L7]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L9]); (pstMobs->mobs.bc[LINE_B][BC_L7]).bclink_i[DUMP1] = 0;//BC7B 1
		(pstMobs->mobs.bc[LINE_B][BC_L7]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L8]); (pstMobs->mobs.bc[LINE_B][BC_L7]).bclink_i[DUMP2] = 0;//BC8B 1

		(pstMobs->mobs.bc[LINE_B][BC_L8]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L8]); (pstMobs->mobs.bc[LINE_B][BC_L8]).bclink_i[DUMP1] = 0;//BC8B 1
		(pstMobs->mobs.bc[LINE_B][BC_L8]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L8]); (pstMobs->mobs.bc[LINE_B][BC_L8]).bclink_i[DUMP2] = 0;//BC8B 1
		(pstMobs->mobs.bc[LINE_B][BC_L8]).silolink = &(pstMobs->mobs.silo[LINE_E][2]);

		(pstMobs->mobs.bc[LINE_B][BC_L9]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L9_1]); (pstMobs->mobs.bc[LINE_B][BC_L9]).bclink_i[DUMP1] = 0;//BC9_1B 1
		(pstMobs->mobs.bc[LINE_B][BC_L9]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L9_2]); (pstMobs->mobs.bc[LINE_B][BC_L9]).bclink_i[DUMP2] = 0;//BC9_2A 1

		(pstMobs->mobs.bc[LINE_B][BC_L9_1]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_B][BC_L9_1]).bclink_i[DUMP1] = 0;//BC9_2B 1
		(pstMobs->mobs.bc[LINE_B][BC_L9_1]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_B][BC_L9_1]).bclink_i[DUMP2] = 0;//BC9_2B 1

		(pstMobs->mobs.bc[LINE_B][BC_L9_2]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_L9_2]).bclink_i[DUMP1] = 0;//BC9_2B 1
		(pstMobs->mobs.bc[LINE_B][BC_L9_2]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_A][BC_L9_2]).bclink_i[DUMP2] = 0;//BC9_2B 1
		(pstMobs->mobs.bc[LINE_B][BC_L9_2]).silolink = &(pstMobs->mobs.silo[LINE_G][0]);

		(pstMobs->mobs.bc[LINE_B][BC_L11]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L11]).bclink_i[DUMP1] = 8;//BC19A 8
		(pstMobs->mobs.bc[LINE_B][BC_L11]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L11]).bclink_i[DUMP2] = 8;//BC19B 8

		(pstMobs->mobs.bc[LINE_B][BC_L12]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L12]).bclink_i[DUMP1] = 5;//BC19A 5
		(pstMobs->mobs.bc[LINE_B][BC_L12]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L12]).bclink_i[DUMP2] = 5;//BC19B 5

		(pstMobs->mobs.bc[LINE_B][BC_L13]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L13]).bclink_i[DUMP1] = 2;//BC19A 2
		(pstMobs->mobs.bc[LINE_B][BC_L13]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L13]).bclink_i[DUMP2] = 2;//BC19B 2

		(pstMobs->mobs.bc[LINE_B][BC_L15]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_B][BC_L15]).bclink_i[DUMP1] = 6;//BC18A 7
		(pstMobs->mobs.bc[LINE_B][BC_L15]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_B][BC_L15]).bclink_i[DUMP2] = 6;//BC18B 7

		(pstMobs->mobs.bc[LINE_B][BC_L16]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_B][BC_L16]).bclink_i[DUMP1] = 3;//BC18A 4
		(pstMobs->mobs.bc[LINE_B][BC_L16]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_B][BC_L16]).bclink_i[DUMP2] = 3;//BC18B 4

		(pstMobs->mobs.bc[LINE_B][BC_L17]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_B][BC_L17]).bclink_i[DUMP1] = 0;//BC18A 1
		(pstMobs->mobs.bc[LINE_B][BC_L17]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_B][BC_L17]).bclink_i[DUMP2] = 0;//BC18B 1

		(pstMobs->mobs.bc[LINE_B][BC_L18]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L19]).bclink_i[DUMP1] = 0;//BC19A 1
		(pstMobs->mobs.bc[LINE_B][BC_L18]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L19]).bclink_i[DUMP2] = 0;//BC19B 1
		(pstMobs->mobs.bc[LINE_B][BC_L18]).bclink[DUMP3] = &(pstMobs->mobs.bc[LINE_C][BC_L19]); (pstMobs->mobs.bc[LINE_B][BC_L19]).bclink_i[DUMP3] = 0;//BC19C 1
		(pstMobs->mobs.bc[LINE_B][BC_L18]).bclink[DUMP4] = &(pstMobs->mobs.bc[LINE_C][BC_L32]); (pstMobs->mobs.bc[LINE_B][BC_L19]).bclink_i[DUMP4] = 0;//BC32C 1

		(pstMobs->mobs.bc[LINE_B][BC_L19]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L20]); (pstMobs->mobs.bc[LINE_B][BC_L19]).bclink_i[DUMP1] = 0;//BC20A 1
		(pstMobs->mobs.bc[LINE_B][BC_L19]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L20]); (pstMobs->mobs.bc[LINE_B][BC_L19]).bclink_i[DUMP2] = 0;//BC20B 1
		(pstMobs->mobs.bc[LINE_B][BC_L19]).bclink[DUMP3] = &(pstMobs->mobs.bc[LINE_C][BC_L30]); (pstMobs->mobs.bc[LINE_B][BC_L19]).bclink_i[DUMP3] = 0;//BC30C 1

		(pstMobs->mobs.bc[LINE_B][BC_L20]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L21]); (pstMobs->mobs.bc[LINE_B][BC_L20]).bclink_i[DUMP1] = 0;//BC21B 1
		(pstMobs->mobs.bc[LINE_B][BC_L20]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L21]); (pstMobs->mobs.bc[LINE_B][BC_L20]).bclink_i[DUMP2] = 0;//BC21B 1

		(pstMobs->mobs.bc[LINE_B][BC_L21]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L22]); (pstMobs->mobs.bc[LINE_B][BC_L21]).bclink_i[DUMP1] = 0;//BC22B 1
		(pstMobs->mobs.bc[LINE_B][BC_L21]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L22]); (pstMobs->mobs.bc[LINE_B][BC_L21]).bclink_i[DUMP2] = 0;//BC22B 1

		(pstMobs->mobs.bc[LINE_B][BC_L22]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L23]); (pstMobs->mobs.bc[LINE_B][BC_L22]).bclink_i[DUMP1] = 0;//BC23B 1
		(pstMobs->mobs.bc[LINE_B][BC_L22]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L23]); (pstMobs->mobs.bc[LINE_B][BC_L22]).bclink_i[DUMP2] = 0;//BC23B 1

		(pstMobs->mobs.bc[LINE_B][BC_L23]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L23]); (pstMobs->mobs.bc[LINE_B][BC_L23]).bclink_i[DUMP1] = 0;//BC23B 1
		(pstMobs->mobs.bc[LINE_B][BC_L23]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L23]); (pstMobs->mobs.bc[LINE_B][BC_L23]).bclink_i[DUMP2] = 0;//BC23B 1
		(pstMobs->mobs.bc[LINE_B][BC_L23]).silolink = &(pstMobs->mobs.silo[LINE_I][0]);//BANKER 2���@

		(pstMobs->mobs.bc[LINE_B][BC_LRC]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_B][BC_LRC]).bclink_i[DUMP1] = 0;//BC9_2B 1
		(pstMobs->mobs.bc[LINE_B][BC_LRC]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L9_2]); (pstMobs->mobs.bc[LINE_B][BC_LRC]).bclink_i[DUMP2] = 0;//BC9_2B 1
	}
//BCC
	{
		//5C
		(pstMobs->mobs.bc[LINE_C][BC_L5]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_C][BC_L5]); (pstMobs->mobs.bc[LINE_C][BC_L5]).bclink_i[DUMP1] = 0;//BC5C 1
		(pstMobs->mobs.bc[LINE_C][BC_L5]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_C][BC_L5]); (pstMobs->mobs.bc[LINE_C][BC_L5]).bclink_i[DUMP2] = 0;//BC5C 1
		(pstMobs->mobs.bc[LINE_C][BC_L5]).silolink = &(pstMobs->mobs.silo[LINE_C][3]);

		(pstMobs->mobs.bc[LINE_C][BC_L11]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_L11]).bclink_i[DUMP1] = 7;//BC19A 7
		(pstMobs->mobs.bc[LINE_C][BC_L11]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_L11]).bclink_i[DUMP2] = 7;//BC19B 7

		(pstMobs->mobs.bc[LINE_C][BC_L12]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_L12]).bclink_i[DUMP1] = 4;//BC19A 4
		(pstMobs->mobs.bc[LINE_C][BC_L12]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_L12]).bclink_i[DUMP2] = 4;//BC19B 4

		(pstMobs->mobs.bc[LINE_C][BC_L13]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_L13]).bclink_i[DUMP1] = 1;//BC19A 1
		(pstMobs->mobs.bc[LINE_C][BC_L13]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_L13]).bclink_i[DUMP2] = 1;//BC19B 1

		(pstMobs->mobs.bc[LINE_C][BC_L15]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_C][BC_L15]).bclink_i[DUMP1] = 5;//BC18A 6
		(pstMobs->mobs.bc[LINE_C][BC_L15]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_C][BC_L15]).bclink_i[DUMP2] = 5;//BC18B 6

		(pstMobs->mobs.bc[LINE_C][BC_L16]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L18]); (pstMobs->mobs.bc[LINE_C][BC_L16]).bclink_i[DUMP1] = 2;//BC18A 3
		(pstMobs->mobs.bc[LINE_C][BC_L16]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L18]); (pstMobs->mobs.bc[LINE_C][BC_L16]).bclink_i[DUMP2] = 2;//BC18B 3

		(pstMobs->mobs.bc[LINE_C][BC_L19]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L20]); (pstMobs->mobs.bc[LINE_C][BC_L19]).bclink_i[DUMP1] = 0;//BC20A 1
		(pstMobs->mobs.bc[LINE_C][BC_L19]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L20]); (pstMobs->mobs.bc[LINE_C][BC_L19]).bclink_i[DUMP2] = 0;//BC20B 1
		(pstMobs->mobs.bc[LINE_C][BC_L19]).bclink[DUMP3] = &(pstMobs->mobs.bc[LINE_C][BC_L30]); (pstMobs->mobs.bc[LINE_C][BC_L19]).bclink_i[DUMP3] = 0;//BC30C 1

		(pstMobs->mobs.bc[LINE_C][BC_L30]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_C][BC_L31]); (pstMobs->mobs.bc[LINE_C][BC_L30]).bclink_i[DUMP1] = 0;//BC31C 1
		(pstMobs->mobs.bc[LINE_C][BC_L30]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_C][BC_L31]); (pstMobs->mobs.bc[LINE_C][BC_L30]).bclink_i[DUMP2] = 0;//BC31C 1

		(pstMobs->mobs.bc[LINE_C][BC_L31]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L4_1]); (pstMobs->mobs.bc[LINE_C][BC_L31]).bclink_i[DUMP1] = 0;//BC4_1A 1
		(pstMobs->mobs.bc[LINE_C][BC_L31]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L4_1]); (pstMobs->mobs.bc[LINE_C][BC_L31]).bclink_i[DUMP2] = 0;//BC4_1B 1

		(pstMobs->mobs.bc[LINE_C][BC_L32]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_C][BC_L33]); (pstMobs->mobs.bc[LINE_C][BC_L32]).bclink_i[DUMP1] = 0;//BC33C 1
		(pstMobs->mobs.bc[LINE_C][BC_L32]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_C][BC_L33]); (pstMobs->mobs.bc[LINE_C][BC_L32]).bclink_i[DUMP2] = 0;//BC33C 1

		(pstMobs->mobs.bc[LINE_C][BC_L33]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_C][BC_L34]); (pstMobs->mobs.bc[LINE_C][BC_L33]).bclink_i[DUMP1] = 0;//BC34C 1
		(pstMobs->mobs.bc[LINE_C][BC_L33]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_A][BC_L8]); (pstMobs->mobs.bc[LINE_C][BC_L33]).bclink_i[DUMP2] = 0;//BC8A 1

		(pstMobs->mobs.bc[LINE_C][BC_L34]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_B][BC_L8]); (pstMobs->mobs.bc[LINE_C][BC_L34]).bclink_i[DUMP1] = 0;//BC8B 1
		(pstMobs->mobs.bc[LINE_C][BC_L34]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L8]); (pstMobs->mobs.bc[LINE_C][BC_L34]).bclink_i[DUMP2] = 0;//BC8B 1

		(pstMobs->mobs.bc[LINE_C][BC_BIO]).bclink[DUMP1] = &(pstMobs->mobs.bc[LINE_A][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_BIO]).bclink_i[DUMP1] = 0;//BC19A 1
		(pstMobs->mobs.bc[LINE_C][BC_BIO]).bclink[DUMP2] = &(pstMobs->mobs.bc[LINE_B][BC_L19]); (pstMobs->mobs.bc[LINE_C][BC_BIO]).bclink_i[DUMP2] = 0;//BC19B 1
	}
 
}
void CActor::init_silo() {
	CSilo* psilo;
	for (int i = 0; i < SILO_LINES; i++) {
		for (int j = 0; j < SILO_LINE_NUM; j++) {
			psilo = &(pstMobs->mobs.silo[i][j]);

			if (psilo->ID != 0) {
				psilo->capa1 = psilo->capa_all / SILO_COLUMN_NUM;
				psilo->exist = ON;
				psilo->pix_columw = psilo->area.h / SILO_COLUMN_NUM;
				psilo->pix2kg = psilo->capa1 / psilo->area.w;
				psilo->thresh_level = psilo->capa1 / 10;
			}
			else  psilo->exist = OFF;

			for (int i = 0; i < SILO_COLUMN_NUM; i++) {
				psilo->column[i].weight = 0;
			}
		}
	}

	return;
};
void CActor::init_harai() {//HARAI�֘A�����ݒ�
						 //BC�Z�b�g
	pstMobs->mobs.haraiki[HARAI_11A].pbc = &(pstMobs->mobs.bc[LINE_A][BC_L11]);
	pstMobs->mobs.haraiki[HARAI_11B].pbc = &(pstMobs->mobs.bc[LINE_B][BC_L11]);
	pstMobs->mobs.haraiki[HARAI_11C].pbc = &(pstMobs->mobs.bc[LINE_C][BC_L11]);
	pstMobs->mobs.haraiki[HARAI_12A].pbc = &(pstMobs->mobs.bc[LINE_A][BC_L12]);
	pstMobs->mobs.haraiki[HARAI_12B].pbc = &(pstMobs->mobs.bc[LINE_B][BC_L12]);
	pstMobs->mobs.haraiki[HARAI_12C].pbc = &(pstMobs->mobs.bc[LINE_C][BC_L12]);
	pstMobs->mobs.haraiki[HARAI_13A].pbc = &(pstMobs->mobs.bc[LINE_A][BC_L13]);
	pstMobs->mobs.haraiki[HARAI_13B].pbc = &(pstMobs->mobs.bc[LINE_B][BC_L13]);
	pstMobs->mobs.haraiki[HARAI_13C].pbc = &(pstMobs->mobs.bc[LINE_C][BC_L13]);
	//SILO�Z�b�g
	for (int i = 0; i < 4; i++) {
		pstMobs->mobs.haraiki[HARAI_11A].psilo[i] = &(pstMobs->mobs.silo[LINE_A][i]);
		pstMobs->mobs.haraiki[HARAI_11B].psilo[i] = &(pstMobs->mobs.silo[LINE_B][i]);
		pstMobs->mobs.haraiki[HARAI_11C].psilo[i] = &(pstMobs->mobs.silo[LINE_C][i]);
		pstMobs->mobs.haraiki[HARAI_12A].psilo[i] = &(pstMobs->mobs.silo[LINE_A][i]);
		pstMobs->mobs.haraiki[HARAI_12B].psilo[i] = &(pstMobs->mobs.silo[LINE_B][i]);
		pstMobs->mobs.haraiki[HARAI_12C].psilo[i] = &(pstMobs->mobs.silo[LINE_C][i]);		
		pstMobs->mobs.haraiki[HARAI_13A].psilo[i] = &(pstMobs->mobs.silo[LINE_A][i]);
		pstMobs->mobs.haraiki[HARAI_13B].psilo[i] = &(pstMobs->mobs.silo[LINE_B][i]);
		pstMobs->mobs.haraiki[HARAI_13C].psilo[i] = &(pstMobs->mobs.silo[LINE_C][i]);
	}

	for (int i = 0; i < NUM_OF_HARAI; i++) {
		pstMobs->mobs.haraiki[i].set_param();
	}

	return;
};