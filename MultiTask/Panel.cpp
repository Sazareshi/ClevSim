#include "stdafx.h"
#include "Panel.h"


CPanel::CPanel(){}
CPanel::~CPanel(){}

//MC âÆäOêßå‰î’
MC_OCE_A1::MC_OCE_A1(void) {
	power_in[0].Volt = 6600; 
	power_in[0].Hz = CITY_POWER_Hz;

	cb1Apmp.set_status(ON); cb1A1.set_status(ON); cb1A2.set_status(ON);
	cb2A.set_status(ON); cb3A1.set_status(ON); cb3A2.set_status(ON);
	cb3A3.set_status(ON); cbCUL1.set_status(ON); cbaux.set_status(ON); cbLC.set_status(ON);
}
MC_OCE_A1::	~MC_OCE_A1(void) {}
void MC_OCE_A1::init_panel() { return; };
void MC_OCE_A1::update_status() { return; };

MC_OCE_B1::MC_OCE_B1(void) {
	power_in[0].Volt = 6600;
	power_in[0].Hz = CITY_POWER_Hz;
	cb1Bpmp.set_status(ON); cb2Bpmp.set_status(ON);  cb1B1.set_status(ON); 
	cb1B2.set_status(ON);  cbBA.set_status(ON);  cb3B1.set_status(ON); 
	cb3B2.set_status(ON);  cb3B3.set_status(ON);   cbaux.set_status(ON);  cbLC.set_status(ON);
	cbCUL2.set_status(ON);  cbCUL3.set_status(ON); 
}
MC_OCE_B1::	~MC_OCE_B1(void){}
void MC_OCE_B1::init_panel() { return; };
void MC_OCE_B1::update_status() { return; };