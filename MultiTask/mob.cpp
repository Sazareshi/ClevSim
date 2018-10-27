#include "stdafx.h"
#include "mob.h"

CMob::CMob(){
	command = MOB_STAT_IDLE;
	status = MOB_STAT_IDLE;
}
CMob::~CMob(){}

/* BC  ***************************************************/

#define BC_MAX_W_1m 60000 //60ton
CBC::CBC() {
	for (int i = 0; i < BC_LINK_MAX; i++) bclink[i] = this;
};
CBC::~CBC() {};
int CBC::put_load(int pos, STLOAD load) {
	belt[pos_rcv[pos]].weight += load.weight;
	if (belt[pos_rcv[pos]].weight > BC_MAX_W_1m) belt[pos_rcv[pos]].weight= BC_MAX_W_1m;
	belt[pos_rcv[pos]].material = load.material;
	return 0;
}
void CBC::conveyor(DWORD com, LONG dt) {
	spd = base_spd;

	if (com == MOB_COM_UPDATE) {
		headpos_mm += (dt * spd) / 1000;  if (headpos_mm >= l) headpos_mm = headpos_mm % l;
		ihead = headpos_mm >> 10;
		headpos_pix = (headpos_mm / mm2pix);
	}
	else if (com == MOB_COM_RESET) {
		headpos_mm = 0; ihead = 0;
		for (int i = 0; i < BC_MAX_LEN; i++) {
			belt->vol = 0; belt->weight = 0;
		}
	}
	else;

	if (ihead != ihead_last) {
		if (head_unit.pos) {
			bclink[0]->put_load(bclink_i[0], belt[ihead_last]);
		}
		else {
			bclink[1]->put_load(bclink_i[1], belt[ihead_last]);
		}
		belt[ihead_last].vol = belt[ihead_last].vol = 0;
	}

	ihead_last = ihead;
}

/* CUL ***************************************************/
CCUL::CCUL() {
	ability = 250; //‰×–ð”\—Íkg/s
	load_base.material = LD_COAL1;
	load_base.density = 1200;//kg/1m3
	load_base.vol = 1000;// litter
	load_base.weight = 1200;//kg
	for (int i = 0; i < NUM_DROP_POINT_CUL; i++) bclink[i] = nullptr;

};
CCUL::~CCUL() {};

int CCUL::discharge(DWORD com, LONG dt) {

	STLOAD load;

	if (com == COM_CUL_IDLE) {
		status = MOB_STAT_IDLE;
	}
	else if (com == COM_CUL_DISCHARGE) {

		if (status == MOB_STAT_ACT0)status = MOB_STAT_ACT1;
		else if (status == MOB_STAT_ACT1) status = MOB_STAT_ACT2;
		else status = MOB_STAT_ACT0;

		load.material = load_base.material;
		load.weight = (ability * (WORD)dt) / 1000;

		CBC* pobj;

		if (bc_selA) pobj = bclink[0];
		else  pobj = bclink[1];

		if(pobj != nullptr) pobj->put_load(bclink_i[0],load);
	}
	else;

	return 0;
};
STLOAD CCUL::set_load(WORD material, WORD density, WORD vol, WORD weight) {
	load_base.material = material;
	load_base.density = density;
	load_base.vol = vol;
	load_base.weight = weight;

	return load_base;
};