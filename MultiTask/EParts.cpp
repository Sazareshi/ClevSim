#include "stdafx.h"
#include "EParts.h"


//　CEPartsクラス

int CEParts::set_condition(double Vin) {
	set_V_in(Vin);//客先からの入力 
	set_V_out(get_V_in());
	return 0;
}

//　CMCCBクラス

int CMCCB::set_condition(double Vin) {
	set_V_in(Vin);//客先からの入力 
	if (get_status() == ON) {
		set_V_out(get_V_in());
	}
	else {
		set_V_out(0.0);
	};
	set_contact();
	return 0;
}

//　CMCクラス

int CMC::set_condition(double Vin) {
	set_V_in(Vin);//客先からの入力 
	if (get_coil() == ON) {
		set_V_out(get_V_in());
	}
	else {
		set_V_out(0.0);
	};
	return 0;
}

//　CTransformerクラス

int CTransformer::set_condition(double Vin) {
	set_V_in(Vin);//客先からの入力 
	set_V_out(get_V_in()*retio);
	return 0;
}

//　CConvertorクラス

int CConverter::set_condition(double Vin) {

	return 0;
}

//　CInvertorクラス
int CCartInverter::set_condition(double Vin) {

	return 0;
}

