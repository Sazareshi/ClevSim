#include "stdafx.h"
#include "EParts.h"


//�@CEParts�N���X

int CEParts::set_condition(double Vin) {
	set_V_in(Vin);//�q�悩��̓��� 
	set_V_out(get_V_in());
	return 0;
}

//�@CMCCB�N���X

int CMCCB::set_condition(double Vin) {
	set_V_in(Vin);//�q�悩��̓��� 
	if (get_status() == ON) {
		set_V_out(get_V_in());
	}
	else {
		set_V_out(0.0);
	};
	set_contact();
	return 0;
}

//�@CMC�N���X

int CMC::set_condition(double Vin) {
	set_V_in(Vin);//�q�悩��̓��� 
	if (get_coil() == ON) {
		set_V_out(get_V_in());
	}
	else {
		set_V_out(0.0);
	};
	return 0;
}

//�@CTransformer�N���X

int CTransformer::set_condition(double Vin) {
	set_V_in(Vin);//�q�悩��̓��� 
	set_V_out(get_V_in()*retio);
	return 0;
}

//�@CConvertor�N���X

int CConverter::set_condition(double Vin) {

	return 0;
}

//�@CInvertor�N���X
int CCartInverter::set_condition(double Vin) {

	return 0;
}

