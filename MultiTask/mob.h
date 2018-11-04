#pragma once
#include "EParts.h"

#define MOB_TYPE_NUM			32	//Mobのタイプ数
#define MOB_MAX_NUM				128 //Mobの数

#define MOB_ID_BC				0 //ベルトコンベヤID
#define MOB_ID_HARAI			1 //払い出し機ID
#define MOB_ID_CRUSH			2 //クラッシャーID
#define MOB_ID_CUL				3 //CUL ID
#define MOB_ID_TRIPPER			4 //積み付け機 ID
#define MOB_ID_EROOM			5//屋外設備制御室 ID
#define MOB_ID_SILO				6//SILO ID

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
	WORD vol;//	体積　1000cm3(l)
	WORD weight;//重量kg
} STLOAD, *LPSTLOAD;

#define BC_DUMPER	0x0001//切替ダンパ有
#define BC_TSHOOT	0x0002//テレスコシュート有
#define BC_2WAY		0x0004//2方向
#define BC_2MOTOR	0x0010//2モータ
#define BC_3MOTOR	0x0020//3モータ
#define BC_HVOLT	0x0040//高圧モータ
#define BC_TRP		0x0100//トリッパー付
#define BC_DROP1	0x0200//払出用1号
#define BC_DROP2	0x0400//払出用2号
#define BC_RCV_MAX	12//BCの荷受け場所最大数
#define BC_MAX_LEN	800//BCの最大長さ
#define BC_MOTOR_MAX	3//最大モータ数
#define BC_LINK_MAX		3//BC接続先最大数

#define BC_COAL_DISP_PIXW	5//BC　石炭表示ピクセル幅


#define SIRO_COLUMN_NUM	12//サイロ区分数

class CSilo : public CMob 
{
public:
	CSilo() {};
	~CSilo() {};
	DWORD SILOtype;
	DWORD l;//長さmm
	DWORD l_offset;//コンベヤライン上のオフセット位置
	DWORD w;//幅mm
	DWORD capa_all;//全容量　kg
	DWORD capa1;//1区画あたり定格

	STLOAD column[SIRO_COLUMN_NUM];//サイロ区分エリア
	int put_load(int pos, STLOAD load);
	int remove_load(int pos, STLOAD load);
	int clear_load();
	DWORD pix2kg;
	int pix_columw;

private:

private:
};


#define COM_TRP_IDLE	0
#define COM_TRP_DISCHARGE	1
#define COM_TRP_MOVE		2


class CTripper : public CMob
{
public:
	CTripper() {};
	~CTripper() {};

	int  spd;//現在速度　mm/s
	int discharge(DWORD com, LONG dt);
	int pos_drop[SIRO_COLUMN_NUM];
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s
	CSilo* silo[4];//紐付きサイロ

private:

};

#define BC_HEAD_0		0
#define BC_HEAD_DUMPER	1
#define BC_HEAD_SHOOT	2

class CBCHead
{
public:
	CBCHead() {};
	~CBCHead() {};

	int type;// 0:plane, 1:dumper, 2:shoot
	int pos;

	int	activate(int com) { pos = com; };
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

	DWORD dir;//画面での向き　HIWORD　縦横　0が横　LOWORD　0が正方向
	DWORD l;//長さmm
	DWORD w;//幅mm
	DWORD ability;//定格搬送能力　ton/h
	int  spd;//現在速度　mm/s
	int  base_spd;//定格速度　mm/s
	int  trq;//トルク
	STLOAD	belt[BC_MAX_LEN];//ベルトは1024mm単位で考える
	int belt_size;//belt配列のサイズ
	int	ihead;//ベルトヘッド位置インデックス
	int	ihead_last;//ベルトヘッド位置前回値
	DWORD	headpos_mm;
	DWORD	headpos_pix;
	DWORD	Kg100perM;//BC1m当たりの定格搬送重量
	CBCHead head_unit;
	CTripper* ptrp;
	
	CBC* bclink[BC_LINK_MAX];//排出先BCポインタ
	CSilo* silolink;//排出先サイロポインタ　トリッパ、スクレーパ付きBCのみ
	int bclink_i[BC_LINK_MAX];//排出先BCの接続Drop位置インデックス
	int put_load(int pos, STLOAD load);
	void conveyor(DWORD com, LONG dt);
	void bc_reset() { headpos_mm = 0; memset(&pos_rcv, 0, sizeof(STLOAD) * BC_MAX_LEN); return; };//ベルトヘッド位置を0リセットして全石炭をクリア
	CMotor motor[BC_MOTOR_MAX];
	DWORD pix2mm;
	
private:

};
class CHarai : public CMob
{
public:
	CHarai() {};
	~CHarai() {};
private:

};
class CCrush : public CMob
{
public:
	CCrush() {};
	~CCrush() {};
private:

};

#define NUM_DROP_POINT_CUL 2
#define COM_CUL_IDLE	0
#define COM_CUL_DISCHARGE	1


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
	
	STLOAD set_load(WORD material, WORD density, WORD vol, WORD weight);
	STLOAD load_base;

private:

};

class CEroom : public CMob
{
public:
	CEroom() {};
	~CEroom() {};
private:

};

#define BC_LINES	3//BCの系統数A,B,C+循環
#define BC_LINE_NUM	26//BCの各系統の数
#define NUM_OF_HARAI			9
#define NUM_OF_CRUSH			2
#define NUM_OF_CUL				2
#define NUM_OF_TRIPPER			3
#define NUM_OF_EROOM			3
#define SILO_LINES				7	// 1号3line　2号設備2line　BIO　2line
#define SILO_LINE_NUM			4	//各ラインのサイロ数　Max　1号　4　（BIOは8サイロで1つと換算）


//共有メモリ配置定義
typedef struct _stMobsBody {
	CBC			bc[BC_LINES][BC_LINE_NUM];
	CHarai		haraiki[NUM_OF_HARAI];
	CCrush		crusher[NUM_OF_CRUSH];
	CCUL		cul[NUM_OF_CUL];
	CTripper	tripper[NUM_OF_TRIPPER];
	CEroom		eroom[NUM_OF_EROOM];
	CSilo		silo[SILO_LINES][BC_LINE_NUM];
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
#define DUMP1 0
#define DUMP2 1
#define DUMP3 2

#define BC_L1 0
#define BC_L2 1
#define BC_L3 2
#define BC_L4_1 3
#define BC_L4_2 4
#define BC_L4_3 5
#define BC_L5  6
#define BC_L6 7
#define BC_L7_1 8
#define BC_L7_2 9
#define BC_L7_3 10
#define BC_L8 11
#define BC_L11 12
#define BC_L12 13
#define BC_L13 14
#define BC_L15 15
#define BC_L16 16
#define BC_L17 17
#define BC_L18_1 18
#define BC_L18_2 19
#define BC_L19 20
#define BC_L20 21
#define BC_L21 22
#define BC_L22 23
#define BC_L23 24

#define BC_L30 0
#define BC_L31 1
#define BC_L32 2
#define BC_L33 3
#define BC_L34 4
#define BC_L35A 8
#define BC_L35B 9
#define BC_L8D 12




