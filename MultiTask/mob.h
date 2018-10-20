#pragma once

#define MOB_TYPE_NUM			32	//Mobのタイプ数
#define MOB_MAX_NUM				128 //Mobの数

#define MOB_ID_BC				0 //ベルトコンベヤID
#define MOB_ID_HARAI			1 //払い出し機ID
#define MOB_ID_CRUSH			2 //クラッシャーID
#define MOB_ID_CUL				3 //CUL ID
#define MOB_ID_TRIPPER			4 //積み付け機 ID
#define MOB_ID_EROOM			5//屋外設備制御室 ID




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
	TCHAR type[8];
	int type_code;
	DWORD ID;
	TCHAR name[16];
	double radz;//角度radian z軸回り
	CUBEM area; //シミュレーション空間上の配置エリア
	CUBEM area_real;//実システム空間上の配置エリア
	HBITMAP hBmp_mob;
	LONG bmpw; 
	LONG bmph;
	DWORD status;//対象の状態
	DWORD command;//対象への指令
	ULONG* ptime_now;
	ULONG* ptime_last;

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
	WORD vol;//	BC1mの体積　1000cm3(l)
	WORD weight;//BC1m当たりの重量kg
} STLOAD, *LPSTLOAD;

#define BC_DUMPER	0x0001//切替ダンパ有
#define BC_TSHOOT	0x0002//テレスコシュート有
#define BC_2WAY		0x0004//2方向
#define BC_2MOTOR	0x0010//2モータ
#define BC_HVOLT	0x0020//高圧モータ
#define BC_TRP		0x0100//トリッパー付
#define BC_DROP1	0x0200//払出用1号
#define BC_DROP2	0x0400//払出用2号
#define BC_RCV_MAP	8//BCの荷受け場所最大数
#define BC_MAX_LEN	800//BCの最大長さ



class CBC : public CMob
{
public:
	CBC() {};
	~CBC() {};
	DWORD BCtype; 
	char cline;//A,B,C
	int nrcv;//石炭受け個所数he
	int pos_rcv[BC_RCV_MAP];//石炭受け位置m
	DWORD l;//長さmm
	DWORD w;//幅mm
	int  spd;//現在速度　mm/s
	int  trq;//トルク
	STLOAD	belt[BC_MAX_LEN];//ベルトは1024mm単位で考える
	int	ihead;//ベルトヘッド位置インデックス
	DWORD	headpos_mm;
	int put_load(int pos, STLOAD load);
	STLOAD pop_load(int pos);
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

class CCUL : public CMob
{
public:
	CCUL() {};
	~CCUL() {};
	STLOAD dc_load;//排出積荷内容
	CBC*	pBCA, pBCB;//排出先コンベヤ
	int		iputA, iputB;//排出先コンベヤの位置インデックス
	int ability;//払い出し能力　kg/s　900ton/h->250kg/s
	LPSTLOAD pt_drop[NUM_DROP_POINT_CUL];//コンベヤへのドロップポイント

private:
	int discharge();
	STLOAD set_load(WORD material,WORD density,	WORD vol,WORD weight);
};

class CTripper : public CMob
{
public:
	CTripper() {};
	~CTripper() {};
private:

};

class CEroom : public CMob
{
public:
	CEroom() {};
	~CEroom() {};
private:

};

#define BC_LINES	3//BCの系統数A,B,C
#define BC_LINE_NUM	30//BCの各系統の数
#define NUM_OF_HARAI			9
#define NUM_OF_CRUSH			2
#define NUM_OF_CUL				2
#define NUM_OF_TRIPPER			3
#define NUM_OF_EROOM			3

//共有メモリ配置定義
typedef struct _stMobsBody {
	CBC			bc[BC_LINES][BC_LINE_NUM];
	CHarai		haraiki[NUM_OF_HARAI];
	CCrush		crusher[NUM_OF_CRUSH];
	CCUL		cul[NUM_OF_CUL];
	CTripper	tripper[NUM_OF_TRIPPER];
	CEroom		eroom[NUM_OF_EROOM];
}STMobsBody, *LPSTMobsBody;


typedef struct _stMobs {
	BOOL	bUpdate;
	CMob* 	pmobs[MOB_TYPE_NUM][MOB_MAX_NUM];
	STMobsBody	mobs;
}STMobs, *LPSTMobs;



