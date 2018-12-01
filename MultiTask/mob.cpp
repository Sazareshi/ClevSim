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
int CBC::put_load_i(int i_pos, STLOAD load) {//PUTは、受け位置のインデックスが引数
	LONG beltmm;
	if (i_pos < BC_RCV_MAX) {
		beltmm = headpos_mm + pos_rcv[i_pos] * 1000; if (beltmm > l)beltmm -= l;
	}
	else {//i_posが配列数より大きいときは、ヘッド位置からのm数と解釈する
		beltmm = headpos_mm + i_pos * 1000; if (beltmm > l)beltmm -= l;
	}

	int i_pos_belt = beltmm >> 10;

	belt[i_pos_belt].weight += load.weight;
	if (belt[i_pos_belt].weight > BC_MAX_W_1m) belt[i_pos_belt].weight= BC_MAX_W_1m;
	belt[i_pos_belt].material = load.material;
	return 0;
}
STLOAD CBC::pop_load(int pos, STLOAD load) {//POPは、出し位置のm位置
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

	belt[i_pos_belt].material = load_ret.material = load.material;
	return load_ret;
}
STLOAD CBC::put_load(int pos, STLOAD load) {//POPは、出し位置のm位置
	STLOAD load_ret;

	LONG beltmm = headpos_mm + pos * 1000; if (beltmm > l)beltmm -= l;
	int i_pos_belt = beltmm >> 10;

	if (load.weight > Kg100perM)load.weight = Kg100perM;
	belt[i_pos_belt].weight += load.weight;
	load_ret.weight = load.weight;
	belt[i_pos_belt].material = load_ret.material = load.material;

	return load_ret;
}

void CBC::conveyor(DWORD com, LONG dt) {
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


	if (put_test_load) {//put_test_loadは、テストロードを置くiheadからの位置
		CCUL cul;
		STLOAD load;
		load.material = cul.load_base.material;
		load.weight = ability * dt / 7200;//ability * 1000/3600/1000*dt*0.5(50%)
		put_load_i(put_test_load, load);
	}

	if (ihead != ihead_last) {

		if(BCtype & BC_TRP){
			silolink[0]->put_load(SILO_COLUMN_NUM-1, belt[ihead_last]);
		}
		else {
			bclink[head_unit.pos]->put_load_i(bclink_i[head_unit.pos], belt[ihead_last]);
		}
		belt[ihead_last].weight = 0;
//		if(ihead_last > 0)	belt[ihead_last-1].weight = 0;
	}

	ihead_last = ihead;
}

/* CUL ***************************************************/
CCUL::CCUL() {
	ability = 250; //荷役能力kg/s
	load_base.material = LD_COAL1;
	load_base.density = 1200;//kg/1m3
	load_base.weight = 1200;//kg
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
STLOAD CCUL::set_load(WORD material, WORD density, WORD vol, WORD weight) {
	load_base.material = material;
	load_base.density = density;
	load_base.weight = weight;

	return load_base;
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
	bc_pos_m = (pbc->l - pos) / 1000;//BCヘッドからの位置

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
		if (pos < psilo[i]->pos_bc_origin_pop * 1000 + psilo[i]->l) break;//対象サイロインデックス取得でブレーク
	}
	for (int i = 0; i < SILO_COLUMN_NUM; i++) {
		i_column = i;
		if (pos < (psilo[i_silo]->pos_bc_origin_pop * 1000) + psilo[i_silo]->l / SILO_COLUMN_NUM * (i + 1)) break;//対象インデックス取得でブレーク
	}

	bc_pos_m = (pbc->l - pos) / 1000;//BCヘッドからの位置（石炭投下ポイント）

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

	bc_pos_m = (pbc->l - pos) / 1000;//BCヘッドからの位置

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