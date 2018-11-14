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

#define BC_MAX_W_1m 60000 //60ton
CBC::CBC() {
	for (int i = 0; i < BC_LINK_MAX; i++) {
		bclink[i] = this;
		this->head_unit.pos = 1;
		put_test_load = 0;
	}
};
CBC::~CBC() {};
int CBC::put_load(int pos, STLOAD load) {
	LONG beltmm;
	if (pos < BC_RCV_MAX) {
		beltmm = headpos_mm + pos_rcv[pos] * 1000; if (beltmm > l)beltmm -= l;
	}
	else {
		beltmm = headpos_mm + pos * 1000; if (beltmm > l)beltmm -= l;
	}

	int i_pos_belt = beltmm >> 10;

	belt[i_pos_belt].weight += load.weight;
	if (belt[i_pos_belt].weight > BC_MAX_W_1m) belt[i_pos_belt].weight= BC_MAX_W_1m;
	belt[i_pos_belt].material = load.material;
	return 0;
}
STLOAD CBC::pop_load(int pos, STLOAD load) {
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


void CBC::conveyor(DWORD com, LONG dt) {
	spd = base_spd;

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

	if (put_test_load) {//put_test_loadは、テストロードを置くiheadからの位置
		CCUL cul;
		STLOAD load;
		load.material = cul.load_base.material;
		load.weight = ability * dt / 3600;
		put_load(put_test_load, load);
	}

	if (ihead != ihead_last) {
		if(BCtype & BC_TRP){
			silolink->put_load(SILO_COLUMN_NUM-1, belt[ihead_last]);
		}
		else {
			if (head_unit.pos) {
				bclink[1]->put_load(bclink_i[1], belt[ihead_last]);
			}
			else {
				bclink[0]->put_load(bclink_i[0], belt[ihead_last]);
			}
		}
		belt[ihead_last].weight = 0;
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

		if(pbc != nullptr) pbc->put_load(bclink_i[0],load);
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

	if (!(com & COM_TRP_DISCHARGE)) 	return 0;

	for (int i = 0; i < SILO_LINE_NUM; i++) {
		i_silo = i;
		if (pos < psilo[i]->pos_bc_origin_put * 1000 + psilo[i]->l) break;
	}
	for (int i = 0; i < SILO_COLUMN_NUM; i++) {
		i_column = i;
		if (pos < (psilo[i_silo]->pos_bc_origin_put * 1000) + psilo[i_silo]->l / SILO_COLUMN_NUM * (i + 1)) break;
	}
	bc_pos_m = (pbc->l - pos) / 1000;//BCヘッドからの位置

	STLOAD load;
	load.weight = ability * dt / 1000;
	psilo[i_silo]->put_load(i_column, pbc->pop_load(bc_pos_m, load));

	return 1;
};
int CHarai::move(DWORD com, LONG dt, int target) {
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