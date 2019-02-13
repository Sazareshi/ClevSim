#pragma once
#if !defined(_EPARTS_CLASS_)
#define _EPARTS_CLASS_

#define ON 1
#define OFF 0


//#####電気回路部品ベースクラス
class CEParts
{
private:
	int phase, status;
	double I_in, I_out, V_in, V_out, Hz_in, Hz_out;

public:
	CEParts(void) { set_phase(3); set_status(ON); set_I_in(0.0); set_I_out(0.0); set_V_in(0.0); set_V_out(0.0); set_Hz_in(0.0); set_Hz_out(0.0); };
	~CEParts(void) { ; };
	int set_phase(int phase_val) { phase = phase_val; return 0; };      int get_phase() { return phase; };
	int set_status(int status_val) { status = status_val; return 0; };  int get_status() { return status; };
	int	set_I_in(double Current) { I_in = Current; return 0; };           double get_I_in() { return I_in; };
	int set_I_out(double Current) { I_out = Current; return 0; };         double get_I_out() { return I_out; };
	int set_V_in(double Voltage) { V_in = Voltage; return 0; };           double get_V_in() { return V_in; };
	int set_V_out(double Voltage) { V_out = Voltage; return 0; };         double get_V_out() { return V_out; };
	int set_Hz_out(double frequency) { Hz_out = frequency; return 0; };   double get_Hz_out() { return Hz_out; };
	int set_Hz_in(double frequency) { Hz_in = frequency; return 0; };   double get_Hz_in() { return Hz_in; };

	virtual int set_condition(double Vin);
};

//#####MCCB
class CMCCB : public CEParts

{
private:
	int a_contact, b_contact; //a接点、b接点

public:
	CMCCB(void) { set_status(ON); a_contact = ON; b_contact = OFF; };
	~CMCCB(void) { ; };
	int set_contact() { if (get_status()) { a_contact = ON; b_contact = OFF; } else { a_contact = OFF; b_contact = ON; }; return 0; };
	int get_a_contact() { return a_contact; }; int get_b_contact() { return b_contact; };
	int set_condition(double Vin);
};

//#####コンタクタ
class CMC : public CEParts

{
private:
	int a_contact, b_contact; //a接点、b接点
	int coil;//コイル

public:
	CMC(void) { set_coil(OFF); };
	~CMC(void) { ; };
	int set_coil(int status_on_off) { coil = status_on_off; if (coil) { a_contact = ON; b_contact = OFF; } else { a_contact = OFF; b_contact = ON; }; return 0; };
	int get_coil() { return a_contact; }
	int get_a_contact() { return a_contact; }; int get_b_contact() { return b_contact; };
	int set_condition(double Vin);
};

//#####トランス
class CTransformer : public CEParts

{
private:
	double retio; //変圧比　出力/入力

public:
	CTransformer(void) { set_retio(1.0); };
	~CTransformer(void) { ; };
	int set_retio(double value) { retio = value; return 0; }; double get_retio() { return retio; };
	int set_condition(double Vin);
};

//#####モータ
class CMotor : public CEParts

{
private:

public:
	CMotor(void) {;};
	~CMotor(void) {};
	DWORD kw;//容量
	DWORD status;//
	CMC* mc_source;
	int rps;//回転速度Hz
	int spd;//mm/sec
	int set_condition(double Vin) { return 0; };
	int set_HzOut() { set_Hz_out(get_Hz_in()); return 1; }
};

//#####コンバータ
// コンバータCC_Linkデータ構造体
#define CC_LINK_B_NUM_CNV 2
#define CC_LINK_W_NUM_CNV 4
#define CONV_TIME_HOLD_NUM 3
#define Tf_CONV 5.0

typedef struct
{
	INT16 X_CNV2PLC[CC_LINK_B_NUM_CNV], Y_PLC2CNV[CC_LINK_B_NUM_CNV];
	//X 0:正転中　1:逆転中　7:一括故障　C：モニタ中　D：速度設定完了　F:命令コード実行完了　１A:アラームリセット状態フラグ	１B:リモート局レディ	
	//Y 0:正転指令　1:逆転指令　C：モニタ指令　D：速度設定指令　F:命令コード実行要求　１Aアラームリセット要求
	INT16 W_CNV2PLC[CC_LINK_W_NUM_CNV], W_PLC2CNV[CC_LINK_W_NUM_CNV];
	//INV2PLC 0:モニタ値１　 1:モニタ値２　3:読出しデータ　4-7:モニタコード３－６　8：アラームコード　9：最新アラーム時モータ速度　A:最新アラーム時出力電流　B:最新アラーム時出力電圧　C:最新アラーム時通電時間　
	//PLC2INV 0:モニタコード　1:速度指令　rpm　2:命令コード　3：書込みデータ　4：モニタコード　電圧　C:トルク指令値0.01％単位
}CONV_CC_LINK_Data, *LPCONV_CC_LINK_Data;


#define Rated_DC_Voltage 400
#define CONV_73TIMER     5000

class CConverter : public CEParts

{
public:
	CONV_CC_LINK_Data cc_link_data; //cc link送信データ
	double monitor_Vin_control, Vin_detect_sync, Vin_sub_control;
	double V0[CONV_TIME_HOLD_NUM];//内部電圧
	int in_run, out_73A;
	unsigned long time_count_hold[CONV_TIME_HOLD_NUM];
	int ID, PLL_counter, Ready_counter;

public:
	int set_cc_out(LPCONV_CC_LINK_Data lpset_data) { cc_link_data = *lpset_data; return 0; };		CONV_CC_LINK_Data get_cc_out() { return cc_link_data; };
	int set_monitor_Vin_control(double Voltage) { monitor_Vin_control = Voltage; return 0; };	double get_monitor_Vin_control() { return monitor_Vin_control; };
	int set_Vin_detect_sync(double Voltage) { Vin_detect_sync = Voltage; return 0; };			double get_Vin_detect_sync() { return Vin_detect_sync; };
	int set_Vin_sub_control(double Voltage) { Vin_sub_control = Voltage; return 0; };			double get_Vin_sub_control_sync() { return Vin_sub_control; };
	int set_in_run(int On_Off) { in_run = On_Off; return 0; };							int get_in_run() { return in_run; };
	int set_out_73A(int On_Off) { out_73A = On_Off; return 0; };							int get_out_73A() { return out_73A; };
	int set_ID(int idata) { ID = idata; return 0; };		int get_ID() { return ID; };

	INT16 get_monitor1() { return cc_link_data.W_CNV2PLC[0]; }; INT16 get_ack_code() { return cc_link_data.W_CNV2PLC[2]; }; INT16 get_read_data() { return cc_link_data.W_CNV2PLC[3]; };
	void set_monitor1(INT16 i16data) { cc_link_data.W_CNV2PLC[0] = i16data; }; void set_ack_code(INT16 i16data) { cc_link_data.W_CNV2PLC[2] = i16data; }; void set_read_data(INT16 i16data) { cc_link_data.W_CNV2PLC[3] = i16data; };

	INT16 get_monitor_code() { return cc_link_data.W_PLC2CNV[0]; }; INT16 get_com_code() { return cc_link_data.W_PLC2CNV[2]; }; INT16 get_write_data() { return cc_link_data.W_PLC2CNV[3]; };
	void set_monitor_code(INT16 i16data) { cc_link_data.W_PLC2CNV[0] = i16data; }; void set_com_code(INT16 i16data) { cc_link_data.W_PLC2CNV[2] = i16data; }; void set_write_data(INT16 i16data) { cc_link_data.W_PLC2CNV[3] = i16data; };

	int get_drv() { return (cc_link_data.X_CNV2PLC[0] >> 0) & 0x1; } int get_reg() { return (cc_link_data.X_CNV2PLC[0] >> 1) & 0x1; } int get_running() { return (cc_link_data.X_CNV2PLC[0] >> 2) & 0x1; } int get_i_limit() { return (cc_link_data.X_CNV2PLC[0] >> 3) & 0x1; }
	int get_life_forcast() { return (cc_link_data.X_CNV2PLC[0] >> 4) & 0x1; } int get_Y5_ope_ready() { return (cc_link_data.X_CNV2PLC[0] >> 6) & 0x1; } int get_all_fault() { return (cc_link_data.X_CNV2PLC[0] >> 7) & 0x1; }

	int get_on_monitor() { return (cc_link_data.X_CNV2PLC[0] >> 12) & 0x1; } int get_ope_code_comp() { return (cc_link_data.X_CNV2PLC[0] >> 15) & 0x1; }
	/*bit0-9未使用*/int get_alarm_stat() { return (cc_link_data.X_CNV2PLC[1] >> 10) & 0x1; } int get_remote_ready() { return (cc_link_data.X_CNV2PLC[1] >> 11) & 0x1; }
	/*bit12-15未使用*/
	void set_drv() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 0); } void set_reg() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 1); } void set_running() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 2); } void set_i_limit() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 3); }
	void set_life_forcast() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 4); }  void set_Y5_ope_ready() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 6); } void set_all_fault() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 7); }
	void set_on_monitor() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 12); } void set_ope_code_comp() { cc_link_data.X_CNV2PLC[0] |= (0x0001 << 15); }
	/*bit0-9未使用*/void set_alarm_stat() { cc_link_data.X_CNV2PLC[1] |= (0x0001 << 10); } void set_remote_ready() { cc_link_data.X_CNV2PLC[1] |= (0x0001 << 11); }
	/*bit12-15未使用*/
	void clr_drv() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 0); } void clr_reg() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 1); } void clr_running() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 2); } void clr_i_limit() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 3); }
	void clr_life_forcast() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 4); } void clr_Y5_ope_ready() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 6); } void clr_all_fault() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 7); }
	void clr_on_monitor() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 12); } void clr_ope_code_comp() { cc_link_data.X_CNV2PLC[0] &= 0xffff ^ (0x0001 << 15); }
	/*bit0-9未使用*/void clr_alarm_stat() { cc_link_data.X_CNV2PLC[1] &= 0xffff ^ (0x0001 << 10); } void clr_remote_ready() { cc_link_data.X_CNV2PLC[1] &= 0xffff ^ (0x0001 << 11); }
	/*bit12-15未使用*/

	int get_com_run() { return (cc_link_data.Y_PLC2CNV[0] >> 0) & 0x1; } int get_X1() { return (cc_link_data.Y_PLC2CNV[0] >> 2) & 0x1; }
	/*bit4-11未使用*/
	int get_com_monitor() { return (cc_link_data.Y_PLC2CNV[0] >> 12) & 0x1; }/*13-14bit未使用*/  int get_ope_code_req() { return (cc_link_data.Y_PLC2CNV[0] >> 15) & 0x1; }
	/*bit0-9未使用*/int get_alarm_reset() { return (cc_link_data.Y_PLC2CNV[1] >> 10) & 0x1; }/*bit11-15未使用*/

	void set_com_run() { cc_link_data.Y_PLC2CNV[0] |= (0x0001 << 0); } void set_X1() { cc_link_data.Y_PLC2CNV[0] |= (0x0001 << 2); }
	/*bit4-11未使用*/
	void set_com_monitor() { cc_link_data.Y_PLC2CNV[0] |= (0x0001 << 12); } /*13-14bit未使用*/  void set_ope_code_req() { cc_link_data.Y_PLC2CNV[0] |= (0x0001 << 15); }
	/*bit0-9未使用*/void set_alarm_reset() { cc_link_data.Y_PLC2CNV[1] |= (0x0001 << 10); }	/*bit11-15未使用*/

	void clr_com_run() { cc_link_data.Y_PLC2CNV[0] &= 0xffff ^ (0x0001 << 0); } void clr_X1() { cc_link_data.Y_PLC2CNV[0] &= 0xffff ^ (0x0001 << 2); }
	/*bit4-11未使用*/
	void clr_com_monitor() { cc_link_data.Y_PLC2CNV[0] &= 0xffff ^ (0x0001 << 12); } int clr_spd_com() { cc_link_data.Y_PLC2CNV[0] &= 0xffff ^ (0x0001 << 13); } /*14bit未使用*/  void clr_ope_code_req() { cc_link_data.Y_PLC2CNV[0] &= 0xffff ^ (0x0001 << 15); }
	/*bit0-9未使用*/void clr_alarm_reclr() { cc_link_data.Y_PLC2CNV[1] &= 0xffff ^ (0x0001 << 10); } 	/*bit11-15未使用*/

	CConverter(void) {
		INT16* pint16 = (INT16*) & (this->cc_link_data.X_CNV2PLC[0]);
		for (int i = 0; i<(CC_LINK_B_NUM_CNV * 2 + CC_LINK_W_NUM_CNV * 2); i++, pint16++)*pint16 = 0;
		for (int i = 0; i<CONV_TIME_HOLD_NUM; i++) { time_count_hold[i] = 0; V0[i] = 0.0; };
		ID = PLL_counter = Ready_counter = 0;
	};
	~CConverter(void) { ; };
	int set_condition(double Vin);
};


//#####インバータ
// インバータCC_Linkデータ構造体
#define CC_LINK_B_NUM_INV 4
#define CC_LINK_W_NUM_INV 16
#define Rated_InvHz 60.0 //1800 rpm
#define Rated_CarInvA 42.0 //21Ax2
#define Const_CarInvA 4.85 //定速時
#define Acc_CarInvA 17.03 //加速時
#define Rated_TNInvA 7.16
#define Const_TNInvA 3.16
#define Acc_TNInvA 8.01
#define INV_MAX_SPEED 20000
#define INV_TIME_HOLD_NUM 3
#define Tf_INV 1.0
#define Ratio_VperHz_Car 0.1583		//57m/min/60sec/60Hz 
#define Ratio_RPSperHz_TN 0.000422	//1.52rpm/60sec/60Hz 
#define RatedHzAcc_Car 15.8	// 0.25m/s2 / (57m/min/60sec) * 60Hz 
#define RatedHzAcc_TN 29.59	//0.0785 rad/s2 / (pi()/180)*360°*(1.52rpm/60sec ) *60Hz
#define VperF	7.0	//420V/60Hz
#define MinCarSpdHz 0.75//45r/min/60

typedef struct
{
	INT16 X_INV2PLC[CC_LINK_B_NUM_INV], Y_PLC2INV[CC_LINK_B_NUM_INV];
	//X 0:正転中　1:逆転中　7:一括故障　C：モニタ中　D：速度設定完了　F:命令コード実行完了　１A:アラームリセット状態フラグ	１B:リモート局レディ	
	//Y 0:正転指令　1:逆転指令　C：モニタ指令　D：速度設定指令　F:命令コード実行要求　１Aアラームリセット要求
	INT16 W_INV2PLC[CC_LINK_W_NUM_INV], W_PLC2INV[CC_LINK_W_NUM_INV];
	//INV2PLC 0:モニタ値１　 1:モニタ値２　3:読出しデータ　4-7:モニタコード３－６　8：アラームコード　9：最新アラーム時モータ速度　A:最新アラーム時出力電流　B:最新アラーム時出力電圧　C:最新アラーム時通電時間　
	//PLC2INV 0:モニタコード　1:速度指令　rpm　2:命令コード　3：書込みデータ　4：モニタコード　電圧　C:トルク指令値0.01％単位
}INV_CC_LINK_Data, *LPINV_CC_LINK_Data;


class CCartInverter : public CEParts

{
public:
	INV_CC_LINK_Data cc_link_data; //cc linkデータ
	double sub_Vin_control;
	int en1, db;
	int ID;
	int Connected_Area;
	int Connected_Cart;
	int Type;//0:Cart 1:TN
	double speed_com, speed_ref[INV_TIME_HOLD_NUM], acc_dec_ref;
	unsigned long time_count_hold[INV_TIME_HOLD_NUM];
	INT16 speed_code;

public:
	int set_cc_out(LPINV_CC_LINK_Data lpset_data) { cc_link_data = *lpset_data; return 0; };	LPINV_CC_LINK_Data get_cc_out() { return &cc_link_data; };
	int set_en1_status(int idata) { en1 = idata; return 0; };		int get_en1_statust() { return en1; };
	int set_db_status(int idata) { db = idata; return 0; };		int get_db1_statust() { return db; };
	int set_ID(int idata) { ID = idata; return 0; };		int get_ID() { return ID; };
	int set_connected_area(int idata) { Connected_Area = idata; return 0; };		int get_connected_area() { return Connected_Area; };
	int set_connected_cart(int ldata) { Connected_Cart = ldata; return 0; };		int get_connected_cart() { return Connected_Cart; };
	int set_sub_Vin_control(double Voltage) { sub_Vin_control = Voltage; return 0; };	double get_sub_Vin_control() { return sub_Vin_control; };
	double get_speed_ref() { return speed_ref[0]; };

	INT16 get_monitor1() { return cc_link_data.W_INV2PLC[0]; }; INT16 get_monitor2() { return cc_link_data.W_INV2PLC[1]; }; INT16 get_ack_code() { return cc_link_data.W_INV2PLC[2]; }; INT16 get_read_data() { return cc_link_data.W_INV2PLC[3]; };
	void set_monitor1(INT16 i16data) { cc_link_data.W_INV2PLC[0] = i16data; }; void set_monitor2(INT16 i16data) { cc_link_data.W_INV2PLC[1] = i16data; }; void set_ack_code(INT16 i16data) { cc_link_data.W_INV2PLC[2] = i16data; }; void set_read_data(INT16 i16data) { cc_link_data.W_INV2PLC[3] = i16data; };
	INT16 get_monitor3() { return cc_link_data.W_INV2PLC[4]; }; INT16 get_monitor4() { return cc_link_data.W_INV2PLC[5]; }; INT16 get_monitor5() { return cc_link_data.W_INV2PLC[6]; }; INT16 get_monitor6() { return cc_link_data.W_INV2PLC[7]; };
	void set_monitor3(INT16 i16data) { cc_link_data.W_INV2PLC[4] = i16data; }; void set_monitor4(INT16 i16data) { cc_link_data.W_INV2PLC[5] = i16data; }; void set_monitor5(INT16 i16data) { cc_link_data.W_INV2PLC[6] = i16data; }; void set_monitor6(INT16 i16data) { cc_link_data.W_INV2PLC[7] = i16data; };
	INT16 get_alarm_code() { return cc_link_data.W_INV2PLC[8]; }; INT16 get_alarm_spd() { return cc_link_data.W_INV2PLC[9]; }; INT16 get_alarm_i() { return cc_link_data.W_INV2PLC[10]; }; INT16 get_alarm_v() { return cc_link_data.W_INV2PLC[11]; };
	void set_alarm_code(INT16 i16data) { cc_link_data.W_INV2PLC[8] = i16data; }; void set_alarm_spd(INT16 i16data) { cc_link_data.W_INV2PLC[9] = i16data; }; void set_alarm_i(INT16 i16data) { cc_link_data.W_INV2PLC[10] = i16data; }; void set_alarm_v(INT16 i16data) { cc_link_data.W_INV2PLC[11] = i16data; };
	INT16 get_alarm_t() { return cc_link_data.W_INV2PLC[12]; };
	void set_alarm_t(INT16 i16data) { cc_link_data.W_INV2PLC[12] = i16data; };

	INT16 get_monitor_code() { return cc_link_data.W_PLC2INV[0]; }; INT16 get_speed_ref_W() { return cc_link_data.W_PLC2INV[1]; }; INT16 get_com_code() { return cc_link_data.W_PLC2INV[2]; }; INT16 get_write_data() { return cc_link_data.W_PLC2INV[3]; };
	void set_monitor_code(INT16 i16data) { cc_link_data.W_PLC2INV[0] = i16data; }; void set_speed_ref_W(INT16 i16data) { cc_link_data.W_PLC2INV[1] = i16data; }; void set_com_code(INT16 i16data) { cc_link_data.W_PLC2INV[2] = i16data; }; void set_write_data(INT16 i16data) { cc_link_data.W_PLC2INV[3] = i16data; };
	INT16 get_monitor_code3() { return cc_link_data.W_PLC2INV[4]; }; INT16 get_monitor_code4() { return cc_link_data.W_PLC2INV[5]; }; INT16 monitor_code5() { return cc_link_data.W_PLC2INV[6]; }; INT16 get_monitor_code6() { return cc_link_data.W_PLC2INV[7]; };
	void set_monitor_code3(INT16 i16data) { cc_link_data.W_PLC2INV[4] = i16data; }; void set_monitor_code4(INT16 i16data) { cc_link_data.W_PLC2INV[5] = i16data; }; void monitor_code5(INT16 i16data) { cc_link_data.W_PLC2INV[6] = i16data; }; void set_monitor_code6(INT16 i16data) { cc_link_data.W_PLC2INV[7] = i16data; };
	INT16 get_alarm_log() { return cc_link_data.W_PLC2INV[8]; };
	void set_alarm_log(INT16 i16data) { cc_link_data.W_PLC2INV[8] = i16data; };
	INT16 get_torq_ref() { return cc_link_data.W_PLC2INV[12]; };
	void set_torq_ref(INT16 i16data) { cc_link_data.W_PLC2INV[12] = i16data; };

	int get_on_fwd() { return (cc_link_data.X_INV2PLC[0] >> 0) & 0x1; } int get_on_rwd() { return (cc_link_data.X_INV2PLC[0] >> 1) & 0x1; } int get_Y1_spd_Nzero() { return (cc_link_data.X_INV2PLC[0] >> 2) & 0x1; } int get_Y2_spd_match() { return (cc_link_data.X_INV2PLC[0] >> 3) & 0x1; }
	int get_Y3_spd_reach() { return (cc_link_data.X_INV2PLC[0] >> 4) & 0x1; } int get_Y4_spd_detect() { return (cc_link_data.X_INV2PLC[0] >> 5) & 0x1; } int get_Y5_ope_ready() { return (cc_link_data.X_INV2PLC[0] >> 6) & 0x1; } int get_all_fault() { return (cc_link_data.X_INV2PLC[0] >> 7) & 0x1; }
	int get_E20_brk_opn() { return (cc_link_data.X_INV2PLC[0] >> 8) & 0x1; } int get_operating() { return (cc_link_data.X_INV2PLC[0] >> 9) & 0x1; } int get_E22() { return (cc_link_data.X_INV2PLC[0] >> 10) & 0x1; } int get_E23() { return (cc_link_data.X_INV2PLC[0] >> 11) & 0x1; }
	int get_on_monitor() { return (cc_link_data.X_INV2PLC[0] >> 12) & 0x1; } int get_spd_set_comp() { return (cc_link_data.X_INV2PLC[0] >> 13) & 0x1; } /*14bit未使用*/  int get_ope_code_comp() { return (cc_link_data.X_INV2PLC[0] >> 15) & 0x1; }
	/*bit0-9未使用*/int get_alarm_stat() { return (cc_link_data.X_INV2PLC[1] >> 10) & 0x1; } int get_remote_ready() { return (cc_link_data.X_INV2PLC[1] >> 11) & 0x1; }
	/*bit12-15未使用*/

	void set_on_fwd() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 0); } void set_on_rwd() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 1); } void set_Y1_spd_Nzero() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 2); } void set_Y2_spd_match() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 3); }
	void set_Y3_spd_reach() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 4); } void set_Y4_spd_detect() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 5); } void set_Y5_ope_ready() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 6); } void set_all_fault() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 7); }
	void set_E20_brk_opn() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 8); } void set_operating() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 9); } void set_E22() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 10); } void set_E23() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 11); }
	void set_on_monitor() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 12); } int set_spd_set_comp() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 13); } /*14bit未使用*/  void set_ope_code_comp() { cc_link_data.X_INV2PLC[0] |= (0x0001 << 15); }
	/*bit0-9未使用*/void set_alarm_stat() { cc_link_data.X_INV2PLC[1] |= (0x0001 << 10); } void set_remote_ready() { cc_link_data.X_INV2PLC[1] |= (0x0001 << 11); }
	/*bit12-15未使用*/

	void clr_on_fwd() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 0); } void clr_on_rwd() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 1); } void clr_Y1_spd_Nzero() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 2); } void clr_Y2_spd_match() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 3); }
	void clr_Y3_spd_reach() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 4); } void clr_Y4_spd_detect() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 5); } void clr_Y5_ope_ready() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 6); } void clr_all_fault() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 7); }
	void clr_E20_brk_opn() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 8); } void clr_operating() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 9); } void clr_E22() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 10); } void clr_E23() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 11); }
	void clr_on_monitor() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 12); } int clr_spd_clr_comp() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 13); } /*14bit未使用*/  void clr_ope_code_comp() { cc_link_data.X_INV2PLC[0] &= 0xffff ^ (0x0001 << 15); }
	/*bit0-9未使用*/void clr_alarm_stat() { cc_link_data.X_INV2PLC[1] &= 0xffff ^ (0x0001 << 10); } void clr_remote_ready() { cc_link_data.X_INV2PLC[1] &= 0xffff ^ (0x0001 << 11); }
	/*bit12-15未使用*/

	int get_com_fwd() { return (cc_link_data.Y_PLC2INV[0] >> 0) & 0x1; } int get_com_rwd() { return (cc_link_data.Y_PLC2INV[0] >> 1) & 0x1; } int get_X1() { return (cc_link_data.Y_PLC2INV[0] >> 2) & 0x1; } int get_X2() { return (cc_link_data.Y_PLC2INV[0] >> 3) & 0x1; }
	int get_X3() { return (cc_link_data.Y_PLC2INV[0] >> 4) & 0x1; } int get_X4() { return (cc_link_data.Y_PLC2INV[0] >> 5) & 0x1; } int get_X5() { return (cc_link_data.Y_PLC2INV[0] >> 6) & 0x1; } int get_X6() { return (cc_link_data.Y_PLC2INV[0] >> 7) & 0x1; }
	int get_X8_fault_reset() { return (cc_link_data.Y_PLC2INV[0] >> 8) & 0x1; } int get_X7_free_run() { return (cc_link_data.Y_PLC2INV[0] >> 9) & 0x1; } int get_X9_ex_alarm() { return (cc_link_data.Y_PLC2INV[0] >> 10) & 0x1; } int get_X11_func_code() { return (cc_link_data.Y_PLC2INV[0] >> 11) & 0x1; }
	int get_com_monitor() { return (cc_link_data.Y_PLC2INV[0] >> 12) & 0x1; } int get_spd_com() { return (cc_link_data.Y_PLC2INV[0] >> 13) & 0x1; } /*14bit未使用*/  int get_ope_code_req() { return (cc_link_data.Y_PLC2INV[0] >> 15) & 0x1; }
	/*bit0-3未使用*/
	int get_X12() { return (cc_link_data.Y_PLC2INV[1] >> 4) & 0x1; } int get_X13() { return (cc_link_data.Y_PLC2INV[1] >> 5) & 0x1; }  int get_X14() { return (cc_link_data.Y_PLC2INV[1] >> 6) & 0x1; }/*bit7未使用*/
	/*bit8-9未使用*/int get_alarm_reset() { return (cc_link_data.Y_PLC2INV[1] >> 10) & 0x1; }/*bit11-15未使用*/
	void set_com_fwd() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 0); } void set_com_rwd() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 1); } void set_X1() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 2); } void set_X2() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 3); }
	void set_X3() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 4); } void set_X4() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 5); } void set_X5() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 6); } void set_X6() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 7); }
	void set_X8_fault_reset() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 8); } void set_X7_free_run() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 9); } void set_X9_ex_alar() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 10); } void set_X11_func_code() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 11); }
	void set_com_monitor() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 12); } int set_spd_com() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 13); } /*14bit未使用*/  void set_ope_code_req() { cc_link_data.Y_PLC2INV[0] |= (0x0001 << 15); }
	/*bit0-3未使用*/
	void set_X12() { cc_link_data.Y_PLC2INV[1] |= (0x0001 << 4); } void set_X13() { cc_link_data.Y_PLC2INV[1] |= (0x0001 << 5); } void set_X14() { cc_link_data.Y_PLC2INV[1] |= (0x0001 << 6); }/*bit7未使用*/
	/*bit8-9未使用*/void set_alarm_reset() { cc_link_data.Y_PLC2INV[1] |= (0x0001 << 10); } 	/*bit11-15未使用*/

	void clr_com_fwd() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 0); } void clr_com_rwd() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 1); } void clr_X1() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 2); } void clr_X2() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 3); }
	void clr_X3() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 4); } void clr_X4() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 5); } void clr_X5() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 6); } void clr_X6() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 7); }
	void clr_X8_fault_reclr() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 8); } void clr_X7_free_run() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 9); } void clr_X9_ex_alar() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 10); } void clr_X11_func_code() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 11); }
	void clr_com_monitor() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 12); } int clr_spd_com() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 13); } /*14bit未使用*/  void clr_ope_code_req() { cc_link_data.Y_PLC2INV[0] &= 0xffff ^ (0x0001 << 15); }
	/*bit0-3未使用*/
	void clr_X12() { cc_link_data.Y_PLC2INV[1] &= 0xffff ^ (0x0001 << 4); } void clr_X13() { cc_link_data.Y_PLC2INV[1] &= 0xffff ^ (0x0001 << 5); } void clr_X14() { cc_link_data.Y_PLC2INV[1] &= 0xffff ^ (0x0001 << 6); }/*bit7未使用*/
	/*bit8-9未使用*/void clr_alarm_reclr() { cc_link_data.Y_PLC2INV[1] &= 0xffff ^ (0x0001 << 10); } 	/*bit11-15未使用*/



	CCartInverter(void) {
		INT16* pint16 = (INT16*) & (this->cc_link_data.X_INV2PLC[0]);
		for (int i = 0; i<(CC_LINK_B_NUM_INV * 2 + CC_LINK_W_NUM_INV * 2); i++, pint16++)*pint16 = 0;
		for (int i = 0; i<INV_TIME_HOLD_NUM; i++)time_count_hold[i] = 0;
		Type = Connected_Area = Connected_Cart = 0;
	};
	~CCartInverter(void) { ; };
	int set_condition(double Vin);
};

//##### Switch
class CSwitch : public CEParts

{
private:
	int a_contact, b_contact; //a接点、b接点	

public:
	CSwitch(void) { a_contact = OFF; b_contact = ON; };
	~CSwitch(void) { ; };
	int set_a_contact(int status_on_off) { a_contact = status_on_off; return 0; }; int get_a_contact() { return a_contact; };
	int set_b_contact(int status_on_off) { a_contact = status_on_off; return 0; }; int get_b_contact() { return b_contact; };
	int set_condition(int status);
};

//##### PBL
class CPBL : public CEParts

{
private:
	int pb_status, lamp_status; //a接点、b接点

public:
	CPBL(void) { pb_status = OFF; lamp_status = OFF; };
	~CPBL(void) { ; };
	int set_pb_status(int status_on_off) { pb_status = status_on_off; return 0; }; int get_pb_status() { return pb_status; };
	int set_lamp_status(int status_on_off) { lamp_status = status_on_off; return 0; }; int get_lamp_status() { return lamp_status; };
	int set_condition(int PB_status, int Lmp_status) { pb_status = PB_status; lamp_status = Lmp_status; };
};

//##### DC電源

#define DC24V 24.0
#define DC110V 110.0

class CDCPower : public CEParts

{
private:
	double DC_voltage;
public:
	CDCPower(void) { ; };
	~CDCPower(void) { ; };
	int set_DC_voltage(double DCV) { DC_voltage = DCV; return 0; }; double get_DC_voltage() { return DC_voltage; };
	int set_condition(double Vin) { if (Vin > 10.0) set_V_out(DC_voltage); else set_V_out(0.0); return 0; };
};


//##### DIO Card

#define DI_Unit 1
#define DO_Unit 2

class CDIO64 : public CEParts

{
private:
	long long	signal;
	int type;
public:
	CDIO64(void) { signal = 0; type = 0xffff; };
	~CDIO64(void) { ; }
	int set_bit(int p) { long long mask = 0x00000001; signal = (signal | (mask << p)); return 0; };
	int clear_bit(int p) {
		long long mask = 0x00000001;
		mask = 0xffffffffffffffff ^ (mask << p);
		signal &= mask;
		return 0;
	};
	int get_bit(int p) { long long mask = 0x00000001; long long temp = (signal&(mask << p)); if (temp)return 1; else return 0; };
	long long get_signal() { return signal; }; int set_signal(long long status) { signal = status; return 0; };
	int all_bit_reset() { signal = 0; return 0; };	int all_bit_set() { signal = 0xffffffff; return 0; };
	int set_type(int data) { type = data; }; int get_type() { return type; };
};

class CDIO32 : public CEParts

{
private:
	long long	signal;
	int type;
public:
	CDIO32(void) { signal = 0; type = 0xffff; };
	~CDIO32(void) { ; }
	int set_bit(int p) { long long mask = 0x00000001; signal = (signal | (mask << p)); return 0; };
	int clear_bit(int p) {
		long long mask = 0x00000001;
		mask = 0xffffffffffffffff ^ (mask << p);
		signal &= mask;
		return 0;
	};
	int get_bit(int p) { long long mask = 0x00000001; long long temp = (signal&(mask << p)); if (temp)return 1; else return 0; };
	long long get_signal() { return signal; }; int set_signal(long long status) { signal = status; return 0; };
	int all_bit_reset() { signal = 0; return 0; };	int all_bit_set() { signal = 0xffffffff; return 0; };
	int set_type(int data) { type = data; }; int get_type() { return type; };
};

//##### AO Card

class CAO_2CH_UNIT : public CEParts

{
private:
	double	ch01_output, ch02_output;

public:
	CAO_2CH_UNIT(void) { ch01_output = 0.0; ch02_output = 0.0; };
	~CAO_2CH_UNIT(void) { ; }
	int set_output(int ch1_ref, int ch2_ref) { ch01_output = (double)(ch1_ref * 10 / 4096); ch02_output = (double)(ch2_ref * 10 / 4096); return 0; };
	double get_output_ch1() { return ch01_output; }; double get_output_ch2() { return ch02_output; };
};

//##### AI Card

class CAI_2CH_UNIT : public CEParts

{
private:
	UINT16	ch01_input, ch02_input;

public:
	CAI_2CH_UNIT(void) { ch01_input = 0; ch02_input = 0; };
	~CAI_2CH_UNIT(void) { ; }
	int set_input(double ch1_in, double ch2_in) {
		//CH01 i=4mA-12mA ->count= 0- 16000 count=(i-4)x2000  
		ch01_input = (INT16)((ch1_in - 4.0) * 2000);
		//CH02
		ch02_input = (INT16)(ch2_in*4096.0 / 20.0);
		return 0;
	};
	INT16 get_input_ch1() { return ch01_input; }; INT16 get_input_ch2() { return ch02_input; };
};



#endif
