#include "stdafx.h"
#include "mob.h"

CMob::CMob(){
	command = MOB_STAT_IDLE;
	status = MOB_STAT_IDLE;
}
CMob::~CMob(){}


/* SILO ***************************************************/
int CSilo::put_load(int pos, STLOAD load) {
	column[pos].weight += load.weight;
	if(load.weight >10) column[pos].material = load.material;
	return 0;
}

STLOAD CSilo::pop_load(int pos, STLOAD load) {
	column[pos].weight -= load.weight;
	if (column[pos].weight < 0) column[pos].weight = 0;
	return load;
}

int CSilo::clear_load() {
	for (int i = 0; i < SILO_COLUMN_NUM;i++) column[i].weight = 0;
	return 0;
}

/* BC  ***************************************************/

#define BC_MAX_W_1m 6000 //6ton
CBC::CBC() {
	for (int i = 0; i < BC_LINK_MAX; i++) {
		bclink[i] = this;
		this->head_unit.pos = 1;
		put_test_load = 0;
	}
};
CBC::~CBC() {};
int CBC::put_load_i(int i_pos, STLOAD load) {//PUT�́A�󂯈ʒu�̃C���f�b�N�X������
	LONG beltmm;
	
	if (BCtype & BC_CRUSH) {//�X�N���[���@�N���b�V���t�͋����I�ɃX�N���[���փv�b�g
		pscreen->buffer[SCREEN_BUF_BC].weight += (load.weight*SCREEN_RETIO / 100);
		pscreen->buffer[SCREEN_BUF_CRUSH].weight += (load.weight*(100 - SCREEN_RETIO) / 100);
		pscreen->buffer[SCREEN_BUF_BC].material = load.material;
		pscreen->buffer[SCREEN_BUF_CRUSH].material = load.material;
		return 0;
	}

	if (i_pos < BC_RCV_MAX) {
		beltmm = headpos_mm + pos_rcv[i_pos] * 1000; if (beltmm > l)beltmm -= l;
	}
	else {//i_pos���z�񐔂��傫���Ƃ��́A�w�b�h�ʒu�����m���Ɖ��߂���
		beltmm = headpos_mm + i_pos * 1000; if (beltmm > l)beltmm -= l;
	}

	int i_pos_belt = beltmm >> 10;

	belt[i_pos_belt].weight += load.weight;
	if (load.weight > 10) {
		belt[i_pos_belt].material = load.material;
		if (i_pos_belt > 1) {
			belt[i_pos_belt-1].material = belt[i_pos_belt + 1].material = load.material;
		}
	}

	//if (belt[i_pos_belt].weight > BC_MAX_W_1m) belt[i_pos_belt].weight= BC_MAX_W_1m;
	if (belt[i_pos_belt].weight > Kg100perM) {
		if (i_pos_belt > 1) {
			belt[i_pos_belt -1].weight += (belt[i_pos_belt].weight - Kg100perM)/2;
			belt[i_pos_belt + 1].weight += (belt[i_pos_belt].weight - Kg100perM) / 2;
			belt[i_pos_belt - 1].material = belt[i_pos_belt + 1].material = load.material;
		}
		else {
			belt[i_pos_belt + 1].weight += (belt[i_pos_belt].weight - Kg100perM) ;
			belt[i_pos_belt + 1].material = load.material;
		}
		belt[i_pos_belt].weight = Kg100perM;
	}


	return 0;
}
STLOAD CBC::pop_load(int pos, STLOAD load) {//POP�́A�o���ʒu��m�ʒu
	STLOAD load_ret;

	LONG beltmm = headpos_mm + pos * 1000; if (beltmm > l)beltmm -= l;
	int i_pos_belt = beltmm >> 10;

	if (belt[i_pos_belt].weight > load.weight) {
		belt[i_pos_belt].weight -= load.weight;
		load_ret.weight = load.weight;
	}
	else {
		load_ret.weight = belt[i_pos_belt].weight;
		belt[i_pos_belt].weight = 0;
	}

	load_ret.material = belt[i_pos_belt].material;
	return load_ret;
}
STLOAD CBC::put_load(int pos, STLOAD load) {//POP�́A�o���ʒu��m�ʒu
	STLOAD load_ret;

	LONG beltmm = headpos_mm + pos * 1000; if (beltmm > l)beltmm -= l;
	int i_pos_belt = beltmm >> 10;

	if (load.weight > Kg100perM)load_ret.weight = Kg100perM;
	else load_ret.weight = load.weight;
	load_ret.material = load.material;

	if (BCtype & BC_CRUSH) {
		pscreen->buffer[SCREEN_BUF_BC].weight += (load_ret.weight*SCREEN_RETIO/100);
		pscreen->buffer[SCREEN_BUF_CRUSH].weight += (load_ret.weight*(100-SCREEN_RETIO) / 100);
		pscreen->buffer[SCREEN_BUF_BC].material = load_ret.material;
	}
	else {
		belt[i_pos_belt].weight += load_ret.weight;
		belt[i_pos_belt].material = load_ret.material;
	}
	return load_ret;
}

void CBC::conveyor(DWORD com, ULONG dt) {
	spd = base_spd;
	if ((BCtype & BC_HEAD_DUAL)&&(b_rverse)) {
		if (com == MOB_COM_UPDATE) {
			headpos_mm -= (dt * spd) / 1000;  if (headpos_mm < 0) headpos_mm += l;
			ihead = headpos_mm >> 10;
			headpos_pix = (headpos_mm / pix2mm);
		}
		else if (com == MOB_COM_RESET) {
			headpos_mm = 0; ihead = 0;
			for (int i = 0; i < BC_MAX_LEN; i++) {
				belt->weight = 0;
			}
		}
		else;
	}
	else {
		if (com == MOB_COM_UPDATE) {
			headpos_mm += (dt * spd) / 1000;  if (headpos_mm >= l) headpos_mm = headpos_mm % l;
			ihead = headpos_mm >> 10;
			headpos_pix = (headpos_mm / pix2mm);
		}
		else if (com == MOB_COM_RESET) {
			headpos_mm = 0; ihead = 0;
			for (int i = 0; i < BC_MAX_LEN; i++) {
				belt->weight = 0;
			}
		}
		else;
	}

	//�X�N���[�p�[�A�N���b�V���tBC�̏���
	if (BCtype & BC_CRUSH) {
		int iscreen, icrusher, weight;
		if (pscreen->status != MOB_STAT_IDLE) {
			iscreen = ihead + pos_rcv[0];
			if (iscreen > belt_size) iscreen -= belt_size;
			weight = pscreen->ability * dt / 1000;
			if (weight < pscreen->buffer[SCREEN_BUF_BC].weight) {
				pscreen->buffer[SCREEN_BUF_BC].weight -= weight;
				belt[iscreen].weight += weight;
			}
			else {
				belt[iscreen].weight += pscreen->buffer[SCREEN_BUF_BC].weight;
				pscreen->buffer[SCREEN_BUF_BC].weight = 0;
			}
			belt[iscreen].material = pscreen->buffer[SCREEN_BUF_BC].material;

			weight *= pscreen->trans_ratio / 100;
			if (weight < pscreen->buffer[SCREEN_BUF_BC].weight) {
				pscreen->buffer[SCREEN_BUF_BC].weight -= weight;
				pscreen->buffer[SCREEN_BUF_CRUSH].weight += weight;
			}
			else {
				pscreen->buffer[SCREEN_BUF_BC].weight = 0;
				pscreen->buffer[SCREEN_BUF_CRUSH].weight += weight;
			}
			belt[iscreen].material = pscreen->buffer[SCREEN_BUF_BC].material;
			pscreen->buffer[SCREEN_BUF_CRUSH].material = pscreen->buffer[SCREEN_BUF_BC].material;
		}
		if (pcrush->status != MOB_STAT_IDLE) {
			icrusher = ihead + pos_rcv[1];
			if (icrusher > belt_size) icrusher -= belt_size;
			weight = pcrush->ability * dt / 1000;
			if (weight < pscreen->buffer[SCREEN_BUF_CRUSH].weight) {
				pscreen->buffer[SCREEN_BUF_CRUSH].weight -= weight;
				belt[icrusher].weight += weight;
			}
			else {
				belt[icrusher].weight += pscreen->buffer[SCREEN_BUF_CRUSH].weight;
				pscreen->buffer[SCREEN_BUF_CRUSH].weight = 0;
			}
			belt[icrusher].material = pscreen->buffer[SCREEN_BUF_CRUSH].material;
		}
	}

	if (put_test_load) {//put_test_load�́A�e�X�g���[�h��u��ihead����̈ʒu
		STLOAD load;
		load.material = CCUL::load_base.material;
		load.weight = ability * dt / 7200;//ability * 1000/3600/1000*dt*0.5(50%)
		put_load_i(put_test_load, load);
	}

	if (ihead != ihead_last) {

		if(BCtype & BC_TRP){
			silolink[0]->put_load(SILO_COLUMN_NUM-1, belt[ihead_last]);
		}
		else if (BCtype & BC_SQR) {
			if ((ID & 0x00ff) == ID_BC_L8) {
				silolink[0]->put_load(SILO_COLUMN_NUM - 1, belt[ihead_last]);
			}
			else if ((ID & 0x00ff) == ID_BC_L9_2) {
				silolink[0]->put_load(SILO_COLUMN_NUM_BIO - 1, belt[ihead_last]);
			}
			else if ((ID & 0x00ff) == ID_BC_L23) {
				silolink[0]->put_load(SILO_COLUMN_NUM_BANK - 1, belt[ihead_last]);
			}
			else;
		}
		else {
			if ((ID & 0x00ff) == ID_BC_L22) {
				if (head_unit.pos == BC_22HEAD_BANK) {
					silolink[0]->put_load(0, belt[ihead_last]);
				}
				else {
					bclink[head_unit.pos]->put_load_i(bclink_i[head_unit.pos], belt[ihead_last]);
				}

			}
			else{
				bclink[head_unit.pos]->put_load_i(bclink_i[head_unit.pos], belt[ihead_last]);
			}

	//		if ((ID & 0x0129) && (belt[ihead_last].weight>10)) {
	//			int gg = 0;
	//		}
		}
		belt[ihead_last].weight = 0;
//		if(ihead_last > 0)	belt[ihead_last-1].weight = 0;
	}

	ihead_last = ihead;
}

/* CUL ***************************************************/
CCUL::CCUL() {
	ability = 250; //�ז�\��kg/s
	for (int i = 0; i < NUM_DROP_POINT_CUL; i++) bclink[i] = nullptr;
};
CCUL::~CCUL() {};
STLOAD CCUL::load_base;
int CCUL::discharge(DWORD com, LONG dt) {

	STLOAD load;

	if (com == COM_CUL_IDLE) {
		status = MOB_STAT_IDLE;
	}
	else if (com == COM_CUL_DISCHARGE) {

		load.material = load_base.material;

		//LONG cal = (ability *dt*(0x4000+ rand()))/(1000*0x7fff);

		LONG cal = (ability *dt) /(1000);
		load.weight = (WORD)cal;

		CBC* pbc;
		if (bc_selbc == 0) pbc = bclink[0];
		else  pbc = bclink[1];

		if(pbc != nullptr) pbc->put_load_i(bclink_i[0],load);
	}
	else;

	return 0;
};

/* TRIPPER ***************************************************/
int CTripper::discharge(DWORD com, LONG dt) {
	int i_silo, i_column, bc_pos_m;

	if(!(com & COM_TRP_DISCHARGE)) 	return 0;

	for (int i = 0; i < SILO_LINE_NUM; i++) {
		i_silo = i;
		if (pos < psilo[i]->pos_bc_origin_put * 1000 + psilo[i]->l) break;
	}
	for (int i = 0; i < SILO_COLUMN_NUM; i++) {
		i_column = i;
		if (pos < (psilo[i_silo]->pos_bc_origin_put * 1000) + psilo[i_silo]->l / SILO_COLUMN_NUM * (i+1)) break;
	}
	bc_pos_m = (pbc->l - pos) / 1000;//BC�w�b�h����̈ʒu

	STLOAD load;
	load.weight = ability * dt / 1000;
	psilo[i_silo]->put_load(i_column, pbc->pop_load(bc_pos_m, load));

	return 1;
};
int CTripper::move(DWORD com, LONG dt, int target) {
	int iret;

	iret = 1;

	if (com == COM_TRP_IDLE) {
		status = MOB_STAT_IDLE;
	}
	else if (com & COM_TRP_DISCHARGE) {
		discharge(com, dt);
	}
	else;

	if (com & COM_TRP_MOVE) {
		if (abs(pos - target) < TRP_MOVE_COMPLETE_RANGE) {
			pos = pos;
			iret = 0;
		}
		else if ((pos - target) > 0) {
			pos = pos - (dt * spd) / 1000;
			iret = 1;
		}
		else {
			pos = pos + (dt * spd) / 1000;
			iret = 1;
		}
	}
	else {
		pos = pos;
		iret = 0;
	}
	if (pos < pos_min)pos = pos_min;
	if (pos > pos_max)pos = pos_max;
	set_area(pos);
	return iret;
};
void CTripper::set_param(){
	if (psilo[0] != nullptr) {
		pos_min = psilo[0]->pos_bc_origin_put * 1000;
	}
	else {
		pos_min = 0;
	}
	if (psilo[3] != nullptr) {
		pos_max = psilo[3]->pos_bc_origin_put* 1000 + psilo[3]->l;
	}
	else {
		pos_max = 224000;
	}
	pos = pos_min;
	return;
};

/* HARAIKI ***************************************************/
int CHarai::discharge(DWORD com, LONG dt) {
	int i_silo, i_column, bc_pos_m;

	if (!(com & COM_HARAI_DISCHARGE)) 	return 0;

	for (int i = 0; i < SILO_LINE_NUM; i++) {
		i_silo = i;
		if (pos < psilo[i]->pos_bc_origin_pop * 1000 + psilo[i]->l) break;//�ΏۃT�C���C���f�b�N�X�擾�Ńu���[�N
	}
	for (int i = 0; i < SILO_COLUMN_NUM; i++) {
		i_column = i;
		if (pos < (psilo[i_silo]->pos_bc_origin_pop * 1000) + psilo[i_silo]->l / SILO_COLUMN_NUM * (i + 1)) break;//�ΏۃC���f�b�N�X�擾�Ńu���[�N
	}

	bc_pos_m = (pbc->l - pos) / 1000;//BC�w�b�h����̈ʒu�i�ΒY�����|�C���g�j

	STLOAD load;
	
	if (psilo[i_silo]->column[i_column].weight < ability * dt / 1000) {
		load.weight = psilo[i_silo]->column[i_column].weight;
	}
	else {
		load.weight = ability * dt / 1000;
	}
	psilo[i_silo]->pop_load(i_column, pbc->put_load(bc_pos_m, load));

	return 1;
};
int CHarai::move(DWORD com, LONG dt, int target) {
	int iret;

	iret = 1;

	if (com == COM_HARAI_IDLE) {
		status = MOB_STAT_IDLE;
	}
	else if (com & COM_HARAI_DISCHARGE) {
		discharge(com, dt);
	}
	else;

	if (com & COM_HARAI_MOVE) {
		if (abs(pos - target) < HARAI_MOVE_COMPLETE_RANGE) {
			pos = pos;
			iret = 0;
		}
		else if ((pos - target) > 0) {
			pos = pos - (dt * spd) / 1000;
			iret = 1;
		}
		else {
			pos = pos + (dt * spd) / 1000;
			iret = 1;
		}
	}
	else {
		pos = pos;
		iret = 0;
	}
	if (pos < pos_min)pos = pos_min;
	if (pos > pos_max)pos = pos_max;
	set_area(pos);
	return iret;
};
void CHarai::set_param() {
	if (psilo[0] != nullptr) {
		pos_min = psilo[0]->pos_bc_origin_pop * 1000;
	}
	else {
		pos_min = 0;
	}
	if (psilo[3] != nullptr) {
		pos_max = psilo[3]->pos_bc_origin_pop * 1000 + psilo[3]->l;
	}
	else {
		pos_max = 224000;
	}
	pos = pos_min;
	return;
};

/* HARAIKI BIO***************************************************/
int CHaraiBio::discharge(DWORD com, LONG dt) {
	int i_column;

	if (!(com & 0x00ff)) 	return 0;

	if (com & COM_HARAI_BIO_DISCHARGE1) com_column[0] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[0] = STAT_HARAI_BIO_IDLE;
	if (com & COM_HARAI_BIO_DISCHARGE2) com_column[1] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[1] = STAT_HARAI_BIO_IDLE;
	if (com & COM_HARAI_BIO_DISCHARGE3) com_column[2] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[2] = STAT_HARAI_BIO_IDLE;
	if (com & COM_HARAI_BIO_DISCHARGE4) com_column[3] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[3] = STAT_HARAI_BIO_IDLE;
	if (com & COM_HARAI_BIO_DISCHARGE5) com_column[4] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[4] = STAT_HARAI_BIO_IDLE;
	if (com & COM_HARAI_BIO_DISCHARGE6) com_column[5] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[5] = STAT_HARAI_BIO_IDLE;
	if (com & COM_HARAI_BIO_DISCHARGE7) com_column[6] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[6] = STAT_HARAI_BIO_IDLE;
	if (com & COM_HARAI_BIO_DISCHARGE8) com_column[7] = STAT_HARAI_BIO_DISCHARGE;
	else com_column[7] = STAT_HARAI_BIO_IDLE;

	STLOAD load;
	for (i_column = 0; i_column < SILO_COLUMN_NUM_BIO; i_column++) {
		if (com_column[i_column] == STAT_HARAI_BIO_IDLE) continue;
	
		if (psilo->column[i_column].weight < ability * dt / 1000) {
			load.weight = psilo->column[i_column].weight;
		}
		else {
			load.weight = ability * dt / 1000;
		}
		psilo->pop_load(i_column, pbc->put_load(pbc->pos_rcv[i_column], load));
	}
	return 1;
};
int CHaraiBio::move(DWORD com, LONG dt, int target) {
	int iret;

	iret = 1;

	if (com == COM_HARAI_IDLE) {
		status = MOB_STAT_IDLE;
	}
	else if (com & 0x00ff) {
		discharge(com, dt);
	}
	else;

	return iret;
};
void CHaraiBio::set_param() {
	return;
};

/* SCRAPER ***************************************************/
int CScraper::discharge(DWORD com, LONG dt) {
	int i_silo, i_column, bc_pos_m;

	if (!(com & COM_SCRP_DISCHARGE)) 	return 0;

	if (SCRPtype == SCRP_8) {
		for (int i = 0; i < SILO_LINE_NUM2; i++) {
			i_silo = i;
			if (pos < psilo[i]->pos_bc_origin_put * 1000 + psilo[i]->l) break;
		}
		for (int i = 0; i < SILO_COLUMN_NUM; i++) {
			i_column = i;
			if (pos < (psilo[i_silo]->pos_bc_origin_put * 1000) + psilo[i_silo]->l / SILO_COLUMN_NUM * (i + 1)) break;
		}
	}
	else if (SCRPtype == SCRP_9_2) {
		i_silo = 0;
		for (int i = 0; i < SILO_COLUMN_NUM; i++) {
			i_column = i;
			if (pos < (psilo[i_silo]->pos_bc_origin_put * 1000) + psilo[i_silo]->l / SILO_COLUMN_NUM_BIO * (i + 1)) break;
		}
	}
	else if (SCRPtype == SCRP_23) {
		for (int i = 0; i < SILO_LINE_NUM_BANK; i++) {
			i_silo = i;
			if (pos < psilo[i]->pos_bc_origin_put * 1000 + psilo[i]->l) break;
		}
		for (int i = 0; i < SILO_COLUMN_NUM; i++) {
			i_column = i;
			if (pos < (psilo[i_silo]->pos_bc_origin_put * 1000) + psilo[i_silo]->l / SILO_COLUMN_NUM_BANK * (i + 1)) break;
		}
	}
	else;

	bc_pos_m = (pbc->l - pos) / 1000;//BC�w�b�h����̈ʒu

	STLOAD load;
	load.weight = ability * dt / 1000;
	psilo[i_silo]->put_load(i_column, pbc->pop_load(bc_pos_m, load));

	return 1;
};
int CScraper::move(DWORD com, LONG dt, int target) {
	int iret;

	iret = 1;

	if (com == COM_SCRP_IDLE) {
		status = MOB_STAT_IDLE;
	}
	else if (com & COM_SCRP_DISCHARGE) {
		discharge(com, dt);
	}
	else;

	if (com & COM_SCRP_ACT1) pos = pos_drop[0][0];
	else if (com & COM_SCRP_ACT2) pos = pos_drop[0][1];
	else if (com & COM_SCRP_ACT3) pos = pos_drop[0][2];
	else if (com & COM_SCRP_ACT4) pos = pos_drop[0][3];
	else if (com & COM_SCRP_ACT5) pos = pos_drop[0][4];
	else if (com & COM_SCRP_ACT6) pos = pos_drop[0][5];
	else if (com & COM_SCRP_ACT7) pos = pos_drop[1][0];
	else if (com & COM_SCRP_ACT8) pos = pos_drop[1][1];
	else if (com & COM_SCRP_ACT9) pos = pos_drop[1][2];
	else if (com & COM_SCRP_ACT10) pos = pos_drop[1][3];
	else if (com & COM_SCRP_ACT11) pos = pos_drop[1][4];
	else if (com & COM_SCRP_ACT12) pos = pos_drop[1][5];
	else {
		pos = pos;
		iret = 0;
	}
	if (pos < pos_min)pos = pos_min;
	if (pos > pos_max)pos = pos_max;
	set_area(pos);
	return iret;
};
void CScraper::set_param() {
	if (psilo[0] != nullptr) {
		pos_min = psilo[0]->pos_bc_origin_put * 1000;
		if (SCRPtype == SCRP_8) {
			pos_max = pbc->l;
		}
		else if (SCRPtype == SCRP_9_2) {
			pos_max = pbc->l;
		}
		else if (SCRPtype == SCRP_23) {
			pos_max = pbc->l;
		}
		else;
	}
	else {
		pos_min = 0;
		pos_max = 224000;
	}
	pos = pos_min;
	return;
};