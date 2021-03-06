#pragma once
#include "EParts.h"
#include "Panel.h"

#define MOB_TYPE_NUM			32	//Mobのタイプ数
#define MOB_MAX_NUM				128 //Mobの数

#define MOB_ID_BC				0 //ベルトコンベヤID
#define MOB_ID_HARAI			1 //払い出し機ID
#define MOB_ID_CRUSH			2 //クラッシャーID
#define MOB_ID_CUL				3 //CUL ID
#define MOB_ID_TRIPPER			4 //積み付け機 ID
#define MOB_ID_EROOM			5//屋外設備制御室 ID
#define MOB_ID_SILO				6//SILO ID
#define MOB_ID_SCRAPER			7//スクレーパー ID
#define MOB_ID_HARAI_BIO		8//バイオサイロ払い出し機 ID
#define MOB_ID_SCREEN			9//スクリーン ID
#define MOB_ID_MAGSEPA			10//マグセパ ID
#define MOB_ID_KEIRYOU			11//計量器 ID
#define MOB_ID_KINKEN			12//金属検知器 ID
#define MOB_ID_SAMPLER			13//サンプラ ID


#define MOB_STAT_IDLE			0
#define MOB_STAT_FAULT			0xFF
#define MOB_STAT_ACT0			1
#define MOB_STAT_ACT1			2
#define MOB_STAT_ACT2			3
#define MOB_STAT_ACT3			4

#define MOB_TYPE_BC			TEXT("BC0000")
#define MOB_TYPE_HARAI		TEXT("HARAI0")
#define MOB_TYPE_CRUSH		TEXT("CRUSH0")
#define MOB_TYPE_CUL		TEXT("CUL000")
#define MOB_TYPE_TRIPPER	TEXT("TRIP00")
#define MOB_TYPE_EROOM		TEXT("EROOM0")
#define MOB_TYPE_SILO		TEXT("SILO00")
#define MOB_TYPE_SCRAPER	TEXT("SCRP00")
#define MOB_TYPE_HARAI_BIO	TEXT("HARAIB")
#define MOB_TYPE_SCREEN		TEXT("SCREEN")
#define MOB_TYPE_MAGSEPA	TEXT("MAGSEP")
#define MOB_TYPE_KEIRYOU	TEXT("KEIRYO")
#define MOB_TYPE_KINKEN		TEXT("KINKEN")
#define MOB_TYPE_SAMPLER	TEXT("SAMPLA")


#define MASK_DIR_X			0x00ff
#define MASK_DIR_LEFT		0x00ff
#define MASK_DIR_UP			0x00ff
#define MASK_DIR_Y			0xff00

#define MOB_COM_UPDATE	0x0000
#define MOB_COM_RESET	0x0001

typedef struct _stCUBEM //mob座標構造体3D
{
	LONG x;	LONG y;	LONG z; LONG w;	LONG h; LONG d;
}CUBEM, *LPCUBEM;

class CShoot {
public: 
	CShoot() {};
	~CShoot() {};
};

class CMob
{
public:
	CMob();
	~CMob();

public:
	TCHAR type[8];//機器種類名
	DWORD ID;//機器ID HIWORD=type LOWORD=id
	TCHAR name[16];//機器名
	double radz;//角度radian z軸回り
	CUBEM area; //シミュレーション空間上の配置エリア
	CUBEM area_real;//実システム空間上の配置エリア
	HBITMAP hBmp_mob;
	LONG bmpw; 
	LONG bmph;
	DWORD status;//対象の状態
	DWORD command;//対象への指令
	ULONG* ptime_now;//シミュレーション現在時刻参照アドレス
	ULONG  time_last;//シミュレーション前回時刻保持値
	BOOL exist;//存在するかどうか、ONなら定期処理実施
	BOOL b_bmp_aline_bottom;

	void setpos(LONG x, LONG y, LONG z) { area.x = x; area.y = y; area.z = z; return; }
	void setpos(int x, int y, int z) { area.x = x; area.y = y; area.z = z; return; }
	void setsize(LONG w, LONG h, LONG d) { area.x = w; area.h = h; area.d = d; return; }
	void setsize(int w, int h, int d) { area.x = w; area.d = d; return; }
	void setstatus(int i) { status = i; return; }
};

#define LD_BIO		0x80//バイオマス
#define LD_COAL1	0x01//石炭タイプ1
#define LD_COAL2	0x02//石炭タイプ2
#define LD_METAL1	0x10//金属有り

typedef struct _stLoad {
	WORD material;//素材成分　
	WORD density;//	密度　kg/m3
	LONG weight;//重量kg
} STLOAD, *LPSTLOAD;

#define BC_DUMPER	0x0001//切替ダンパ有
#define BC_2WAY		0x0002//2方向
#define BC_TSHOOT	0x0004//テレスコシュート3port
#define BC_TSHOOT4	0x0008//テレスコシュート4port
#define BC_KINKEN	0x0010//金属検知器
#define BC_KEIRYO	0x0020//計量器
#define BC_SAMPLE	0x0040//サンプラ
#define BC_MSEPA	0x0080//マグセパ付
#define BC_TRP		0x0100//トリッパー付
#define BC_SQR		0x0200//スクレーパ付
#define BC_DROP1	0x0400//払出用1号
#define BC_DROP2	0x0800//払出用2号
#define BC_DROPB	0x1000//払出用バイオ
#define BC_CRUSH	0x2000//クラッシャー付
#define BC_RCV_MAX	12//BCの荷受け場所最大数
#define BC_MAX_LEN	800//BCの最大長さ
#define BC_MOTOR_MAX	3//最大モータ数
#define BC_LINK_MAX		4//BC接続先最大数

#define BC_COAL_DISP_PIXW	5//BC　石炭表示ピクセル幅

#define SILO_COLUMN_NUM			12//サイロ区分数
#define SILO_COLUMN_NUM_BIO		8//サイロ区分数
#define SILO_COLUMN_NUM_BANK	6//サイロ区分数
#define SILO_TYPE_1				0x0001//1号サイロ
#define SILO_TYPE_2				0x0002//2号サイロ
#define SILO_TYPE_BIO			0x0004//バイオサイロ
#define SILO_TYPE_BANK			0x0008//バンカー


class CSilo : public CMob
{
public:
	CSilo() {};
	~CSilo() {};
	LONG SILOtype;
	LONG l;//長さmm
	LONG w;//幅mm
	LONG pos_bc_origin_put;//積コンベヤライン上のオフセット位置m　テールから
	LONG pos_bc_origin_pop;//払コンベヤライン上のオフセット位置m　テールから
	LONG capa_all;//全容量　kg
	LONG capa1;//1区画あたり定格
	LONG thresh_level;//積山平準化の判定値（隣セルとの差）
	DWORD dir;//画面での向き　HIWORD　縦横　0が横　LOWORD　0が正方向

	STLOAD column[SILO_COLUMN_NUM];//サイロ区分エリア
	int put_load(int pos, STLOAD load);//pos サイロ上受け位置ｍ
	STLOAD pop_load(int pos, STLOAD load);
	int clear_load();
	LONG pix2kg;
	int pix_columw;
	int ini_stock_percent;//初期ストック量

private:

private:
};

#define NUM_DROP_POINT_CUL 2
#define COM_CUL_IDLE	0
#define COM_CUL_DISCHARGE	1

#define BC_HEAD_0		0x0000
#define BC_HEAD_DUMPER	0x0001
#define BC_HEAD_DUAL	0x0002
#define BC_HEAD_SHOOT3	0x0004
#define BC_HEAD_SHOOT4	0x0008
#define BC_SILO_LINK_MAX	4

#define BC_LINK_REVERSE	2 //可逆コンベヤ　逆方向時のリンク先は配列2に固定
#define BC_22HEAD_BANK	1 

class CBCHead
{
public:
	CBCHead() {};
	~CBCHead() {};

	int type;// 0:plane, 1:dumper,2:Dual,3:shoot3,4:shoot4
	int pos;

	int	activate(int com) { pos = com; };
};
#define SCREEN_BUF_BC		0
#define SCREEN_BUF_CRUSH	1
#define SCREEN_BUF_NUM		2
#define SCREEN_COM_IDLE		0
#define SCREEN_COM_WORK		1
#define SCREEN_RETIO		80//スクリーンの透過率
class CScreen : public CMob
{
public:
	CScreen() { trans_ratio = SCREEN_RETIO; pix_columw = 10; };
	~CScreen() {};
	STLOAD buffer[SCREEN_BUF_NUM];//クラッシャー用バッファ 0:通過用　1：未通過用
	CMob* pbc;
	int trans_ratio;//透過率
	int ability;//バッファ能力　kg　（搬送能力　900ton/h->250kg/s）
	int pix_columw;//石炭描画石炭幅
	int buf_capa[SCREEN_BUF_NUM];//石炭描画石炭幅

	STLOAD pop_load(int index, STLOAD load);//クラッシャー用バッファからPop

	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド

private:

};

#define CRUSH_COM_IDLE		0
#define CRUSH_COM_WORK		1
class CCrush : public CMob
{
public:
	CCrush() {};
	~CCrush() {};
	CMob* pbc;
	CScreen* pscreen;
	STLOAD crush_load(STLOAD load);//クラッシャー用バッファからPopしてBCへPut
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s

	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
private:

};

#define MAG_BC2A	0
#define MAG_BC2B	1
#define MAG_BC19A	2
#define MAG_BC19B	3
#define MAG_BC19C	4
#define COM_MAG_IDLE	0
#define COM_MAG_DISCHARGE	1
#define COM_MAG_MOVE		2

class CMSeparator : public CMob
{
public:
	CMSeparator() {};
	~CMSeparator() {};
	CMob* pbc;//設置BC

	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
private:

};

#define MDETECT_BC19A	0
#define MDETECT_BC19B	1
#define MDETECT_BC19C	2
#define COM_KIN_IDLE	0
#define COM_KIN_ACT		1
class CMDetector : public CMob
{
public:
	CMDetector() {};
	~CMDetector() {};
	CMob* pbc;//設置BC

	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
private:

};

#define METER_BC2A	0
#define METER_BC2B	1
#define METER_BC19A	2
#define METER_BC19B	3
#define METER_BC19C	4
#define METER_BC30	3
#define METER_BC32	4
#define COM_METER_IDLE	0
#define COM_METER_ACT	1

class CLoadMeter : public CMob
{
public:
	CLoadMeter() {};
	~CLoadMeter() {};
	CMob* pbc;//設置BC

	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
private:

};

#define SAMPLER_BC2A	0
#define SAMPLER_BC2B	1
#define SAMPLER_BC21A	2
#define SAMPLER_BC21B	3
#define COM_SAMPLE_IDLE	0
#define COM_SAMPLE_ACT	1
class CSampler : public CMob
{
public:
	CSampler() {};
	~CSampler() {};
	CMob* pbc;//設置BC

	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
private:

};

class CBC : public CMob
{
public:
	CBC();
	~CBC();
	DWORD BCtype; 
	int nrcv;//石炭受け個所数
	int pos_rcv[BC_RCV_MAX];//石炭受け位置m
	POINT imgpt_rcv[BC_RCV_MAX];//石炭受け画面位置
	POINT imgpt_top[3],imgpt_tail;


	CMSeparator* pmag;
	CMDetector* pdetect;
	CLoadMeter* pmeter;
	CSampler* psample;


	DWORD dir;//画面での向き　HIWORD　縦横　0が横　LOWORD　0が正方向
	LONG l;//長さmm
	LONG w;//幅mm
	LONG ability;//定格搬送能力　ton/h
	int  spd;//現在速度　mm/s
	int  base_spd;//定格速度　mm/s
	int  trq;//トルク
	STLOAD	belt[BC_MAX_LEN];//ベルトは1024mm単位で考える
	int belt_size;//belt配列のサイズ
	int	ihead;//ベルトヘッド位置インデックス
	int	ihead_last;//ベルトヘッド位置前回値
	LONG	headpos_mm;
	LONG	headpos_pix;
	LONG	Kg100perM;//BC1m当たりの定格搬送重量
	CBCHead head_unit;
	CMob* ptrp;
	CScreen* pscreen;
	CCrush* pcrush;
	
	CBC* bclink[BC_LINK_MAX];//排出先BCポインタ
	CSilo* silolink[BC_SILO_LINK_MAX];//排出先サイロポインタ　トリッパ、スクレーパ付きBCのみ
	int bclink_i[BC_LINK_MAX];//排出先BCの接続Drop位置インデックス
	int put_load_i(int i_pos, STLOAD load);//i_pos BC上実位置配列インデックス
	STLOAD put_load(int pos, STLOAD load);//pos BC上実位置m
	STLOAD pop_load(int pos, STLOAD load);//pos BC上実位置m
	void conveyor(DWORD com, ULONG dt);
	void bc_reset() { headpos_mm = 0; memset(&pos_rcv, 0, sizeof(STLOAD) * BC_MAX_LEN); return; };//ベルトヘッド位置を0リセットして全石炭をクリア
	CMotor motor[BC_MOTOR_MAX];
	LONG pix2mm;
	LONG put_test_load;//0以外でテールポジションからのインデックス位置に重量投入

	BOOL b_rverse;
	int setVperHz() { VperHz = double(base_spd) / 60.0; return 1; }
	int calspd() { return spd = (int)(motor[0].get_Hz_out()*VperHz); }

private:
	double VperHz;


};


#define COM_TRP_IDLE	0
#define COM_TRP_DISCHARGE	1
#define COM_TRP_MOVE		2
#define TRP_SIRO_DRP_NUM	6
#define TRP_SIRO_DRP_LAYER	2
#define TRP_MOVE_COMPLETE_RANGE	500

class CTripper : public CMob
{
public:
	CTripper() {};
	~CTripper() {};

	int  spd;//現在速度　mm/s
	int  pos;//現在位置　mm/s　紐付きBC上テールからの位置
	int  pos_max, pos_min;//移動範囲最大値、最小値mm


	int pos_drop[TRP_SIRO_DRP_LAYER][TRP_SIRO_DRP_NUM];//SILO上目標位置ｍ
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s
	CSilo* psilo[4];//紐付きサイロ
	CBC* pbc;//紐付BC
	

	int discharge(DWORD com, LONG dt);//紐付きBCから紐付きサイロへ石炭を移載
	int move(DWORD com, LONG dt,int target);//移動/移載　戻り値1：移動有り
	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
	void set_target(int target) { if (target < pos_min)pos_target = pos_min;else if (target > pos_max)pos_target = pos_max; else pos_target = target; };//指示コマンド
	int get_target() { return pos_target; }
	void set_param();//パラメータを展開

	void set_area(int pos) { area.y = pbc->area.y + pos/ pbc->pix2mm; return; };

private:
	int pos_target;

};

#define COM_SCRP_IDLE	0
#define COM_SCRP_DISCHARGE	0x0001
#define COM_SCRP_ACT1		0x0002
#define COM_SCRP_ACT2		0x0004
#define COM_SCRP_ACT3		0x0008
#define COM_SCRP_ACT4		0x0010
#define COM_SCRP_ACT5		0x0020
#define COM_SCRP_ACT6		0x0040
#define COM_SCRP_ACT7		0x0080
#define COM_SCRP_ACT8		0x0100
#define COM_SCRP_ACT9		0x0200
#define COM_SCRP_ACT10		0x0400
#define COM_SCRP_ACT11		0x0800
#define COM_SCRP_ACT12		0x1000

#define SCRP_SIRO_DRP_NUM	6
#define SCRP_SIRO_DRP_LAYER	2
#define SCRP_MOVE_COMPLETE_RANGE	500
#define SCRP_8		0x0002
#define SCRP_9_2	0x0004
#define SCRP_23		0x0008

class CScraper: public CMob
{
public:
	CScraper() {};
	~CScraper() {};

	int  spd;//現在速度　mm/s
	int  pos;//現在位置　mm/s　紐付きBC上テールからの位置
	int  pos_max, pos_min;//移動範囲最大値、最小値mm


	int pos_drop[SCRP_SIRO_DRP_LAYER][SCRP_SIRO_DRP_NUM];//SILO上目標位置ｍ
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s
	CSilo* psilo[4];//紐付きサイロ
	CBC* pbc;//紐付BC
	DWORD SCRPtype;


	int discharge(DWORD com, LONG dt);//紐付きBCから紐付きサイロへ石炭を移載
	int move(DWORD com, LONG dt, int target);//移動/移載　戻り値1：移動有り
	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
	void set_target(int target) { if (target < pos_min)pos_target = pos_min; else if (target > pos_max)pos_target = pos_max; else pos_target = target; };//指示コマンド
	int get_target() { return pos_target; }
	void set_param();//パラメータを展開

	void set_area(int pos) { 
		if (SCRPtype == SCRP_23) area.x = pbc->area.x + pos / pbc->pix2mm;
		else area.y = pbc->area.y + pos / pbc->pix2mm; 
		return; 
	};
		
private:
	int pos_target;

};


#define COM_HARAI_IDLE	0
#define COM_HARAI_DISCHARGE	1

#define COM_HARAI_MOVE		2
#define HARAI_SIRO_DRP_NUM	12
#define HARAI_MOVE_COMPLETE_RANGE	500

class CHarai : public CMob
{
public:
	CHarai() {};
	~CHarai() {};

	int  spd;//現在速度　mm/s
	int  pos;//現在位置　mm/s　紐付きBC上テールからの位置
	int  pos_max, pos_min;//移動範囲最大値、最小値mm


	int pos_drop[HARAI_SIRO_DRP_NUM];//SILO上目標位置ｍ
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s
	CSilo* psilo[4];//紐付きサイロ
	CBC* pbc;//紐付BC


	int discharge(DWORD com, LONG dt);//紐付きBCへ紐付きサイロから石炭を移載
	int move(DWORD com, LONG dt, int target);//移動/移載　戻り値1：移動有り
	void set_command(DWORD com) { command |= com; };//指示コマンド
	void reset_command(DWORD com) { command &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
	void set_target(int target) { if (target < pos_min)pos_target = pos_min; else if (target > pos_max)pos_target = pos_max; else pos_target = target; };//指示コマンド
	int get_target() { return pos_target; }
	void set_param();//パラメータを展開

	void set_area(int pos) { 
		area.y = pbc->area.y + pos / pbc->pix2mm; 
		return; };

private:
	int pos_target;


private:

};

#define COM_HARAI_BIO_IDLE	0
#define COM_HARAI_BIO_DISCHARGE1	0x0001
#define COM_HARAI_BIO_DISCHARGE2	0x0002
#define COM_HARAI_BIO_DISCHARGE3	0x0004
#define COM_HARAI_BIO_DISCHARGE4	0x0008
#define COM_HARAI_BIO_DISCHARGE5	0x0010
#define COM_HARAI_BIO_DISCHARGE6	0x0020
#define COM_HARAI_BIO_DISCHARGE7	0x0040
#define COM_HARAI_BIO_DISCHARGE8	0x0080

#define STAT_HARAI_BIO_IDLE			0x0000
#define STAT_HARAI_BIO_DISCHARGE	0x0001


class CHaraiBio : public CMob
{
public:
	CHaraiBio() {};
	~CHaraiBio() {};

	int stat[SILO_COLUMN_NUM_BIO];//各払い出し装置の状態
	int com_column[SILO_COLUMN_NUM_BIO];//各払い出し装置へのコマンド状態
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s
	CSilo* psilo;//紐付きサイロ
	CBC* pbc;//紐付BC

	int discharge(DWORD com, LONG dt);//紐付きBCへ紐付きサイロから石炭を移載
	int move(DWORD com, LONG dt, int target);//移動/移載　戻り値1：移動有り
	void set_command(DWORD com, int i) { com_column[i] |= com; };//指示コマンド
	void reset_command(DWORD com, int i) { com_column[i] &= ~com; };//指示コマンド
	DWORD get_command() { return command; };//指示コマンド
	void set_param();//パラメータを展開

	void set_area(int pos) {
		area.y = pbc->area.y + pos / pbc->pix2mm;
		return;
	};

private:

private:

};

class CCUL : public CMob
{
public:
	CCUL();
	~CCUL();
	STLOAD dc_load;//排出積荷内容
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s

	CBC* bclink[NUM_DROP_POINT_CUL];//排出先BCポインタ
	int bclink_i[NUM_DROP_POINT_CUL];//排出先BCの接続Drop位置インデックス

	int bc_selbc;//払い出しline選択 0:A,1:B

	int discharge(DWORD com, LONG dt);
	
	static STLOAD load_base;

private:

};

#define ER_PANEL_MAX 32
class CEroom : public CMob
{
public:
	CEroom() {};
	~CEroom() {};

	CPanel* ppanel[ER_PANEL_MAX];
	virtual void init_room() {return;};

private:

};

class CEroom_OC : public CEroom
{
public:
	CEroom_OC() {};
	~CEroom_OC() {};

	void init_room() { return; }
	MC_OCE_A1 mca1;
	MC_OCE_B1 mcb1;

private:

};


#define BC_LINES	3//BCの系統数A,B,C+循環
#define BC_LINE_NUM	26//BCの各系統の数
#define NUM_OF_HARAI			15
#define NUM_OF_CRUSH			2
#define NUM_OF_CUL				2
#define NUM_OF_TRIPPER			3
#define NUM_OF_SCRAPER			6
#define NUM_OF_EROOM			3
#define SILO_LINES				9	// 1号3line　2号設備2line　BIO　2line BANKER 2line
#define SILO_LINE_NUM			4	//1期工事各ラインのサイロ数
#define SILO_LINE_NUM2			3	//2期工事各ラインのサイロ数
#define SILO_LINE_NUM_BIO		1	//バイオ各ラインのサイロ数
#define SILO_LINE_NUM_BANK		2	//バンカーラインのサイロ数
#define NUM_OF_HARAI_BIO		2   //バイオサイロのサイロ数
#define NUM_OF_SCREEN			2
#define NUM_OF_MAGSEPA			5
#define NUM_OF_KINKEN			3
#define NUM_OF_KEIRYOUKI		7
#define NUM_OF_SAMPLER			4



//共有メモリ配置定義
typedef struct _stMobsBody {
	CBC			bc[BC_LINES][BC_LINE_NUM];
	CHarai		haraiki[NUM_OF_HARAI];
	CCrush		crusher[NUM_OF_CRUSH];
	CCUL		cul[NUM_OF_CUL];
	CTripper	tripper[NUM_OF_TRIPPER];
	CEroom		eroom[NUM_OF_EROOM];
	CSilo		silo[SILO_LINES][SILO_LINE_NUM];
	CScraper	scraper[NUM_OF_SCRAPER];
	CHaraiBio	haraikiBio[NUM_OF_HARAI_BIO];
	CScreen		screen[NUM_OF_SCREEN];
	CMSeparator magsepa[NUM_OF_MAGSEPA];
	CMDetector 	kinken[NUM_OF_KINKEN];
	CLoadMeter 	keiryoki[NUM_OF_KEIRYOUKI];
	CSampler	sampler[NUM_OF_SAMPLER];
	CEroom_OC	erm_outcommon;//
}STMobsBody, *LPSTMobsBody;

typedef struct _stMobs {
	BOOL	bUpdate;
	CMob* 	pmobs[MOB_TYPE_NUM][MOB_MAX_NUM];
	STMobsBody	mobs;
}STMobs, *LPSTMobs;


#define LINE_A 0
#define LINE_B 1
#define LINE_C 2
#define LINE_D 3
#define LINE_E 4
#define LINE_F 5
#define LINE_G 6
#define LINE_H 7
#define LINE_I 8
#define LINE_D_SCR 0
#define LINE_E_SCR 1
#define LINE_F_SCR 2
#define LINE_G_SCR 3
#define LINE_H_SCR 4
#define LINE_I_SCR 5
#define DUMP1 0
#define DUMP2 1
#define DUMP3 2
#define DUMP4 3

#define BC_L1	0
#define BC_L2	1
#define BC_L3	2
#define BC_L4_1 3
#define BC_L4_2 4
#define BC_L4_3 5
#define BC_L5	6
#define BC_L6	7
#define BC_L7	8
#define BC_L8	9
#define BC_L9	10
#define BC_L9_1 11
#define BC_L9_2 12
#define BC_L11	13
#define BC_L12	14
#define BC_L13	15
#define BC_L15	16
#define BC_L16	17
#define BC_L17	18
#define BC_L18	19
#define BC_L19	20
#define BC_L20	21
#define BC_L21	22
#define BC_L22	23
#define BC_L23	24
#define BC_LRC	25

#define BC_L30 0
#define BC_L31 1
#define BC_L32 2
#define BC_L33 3
#define BC_L34 4
#define BC_BIO 5

#define ID_BC_L1	10
#define ID_BC_L2	20
#define ID_BC_L3	30
#define ID_BC_L4_1 41
#define ID_BC_L4_2 42
#define ID_BC_L4_3 43
#define ID_BC_L5	50
#define ID_BC_L6	60
#define ID_BC_L7	70
#define ID_BC_L8	80
#define ID_BC_L9	90
#define ID_BC_L9_1 91
#define ID_BC_L9_2 92
#define ID_BC_L11	110
#define ID_BC_L12	120
#define ID_BC_L13	130
#define ID_BC_L15	150
#define ID_BC_L16	160
#define ID_BC_L17	170
#define ID_BC_L18	180
#define ID_BC_L19	190
#define ID_BC_L20	200
#define ID_BC_L21	210
#define ID_BC_L22	220
#define ID_BC_L23	230
#define ID_BC_LRC	240

#define ID_BC_L30 300
#define ID_BC_L31 310
#define ID_BC_L32 320
#define ID_BC_L33 330
#define ID_BC_L34 340
#define ID_BC_BIO 350

#define HARAI_11A	0
#define HARAI_11B	1
#define HARAI_11C	2
#define HARAI_12A	3
#define HARAI_12B	4
#define HARAI_12C	5
#define HARAI_13A	6
#define HARAI_13B	7
#define HARAI_13C	8
#define HARAI_15A	9
#define HARAI_15B	10
#define HARAI_15C	11
#define HARAI_16A	12
#define HARAI_16B	13
#define HARAI_16C	14
#define HARAI_17A	0
#define HARAI_17B	1


