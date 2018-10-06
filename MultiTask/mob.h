#pragma once

#define MOB_TYPE_NUM			32	//Mobのタイプ数
#define MOB_MAX_NUM				32  //Mobのタイプ数

#define MOB_ID_BC				0 //ベルトコンベヤID
#define MOB_ID_HARAI			1 //払い出し機ID
#define MOB_ID_CRUSH			2 //クラッシャーID
#define MOB_ID_CUL				3 //CUL ID


#define NUM_OF_BC				30
#define NUM_OF_HARAI			9
#define NUM_OF_CRUSH			2
#define NUM_OF_CUL				2

#define MOB_STAT_IDLE			0
#define MOB_STAT_FAULT			0xFF
#define MOB_STAT_ACT1			1
#define MOB_STAT_ACT2			2
#define MOB_STAT_ACT3			3

#define MOB_TYPE_BC			TEXT("BC0000")
#define MOB_TYPE_HARAI		TEXT("HARAI0")
#define MOB_TYPE_CRUSH		TEXT("CRUSH0")
#define MOB_TYPE_CUL		TEXT("CUL000")


typedef struct _stRECTM
{
	LONG x;	LONG y;	LONG w;	LONG h;
	LONG bmpw; LONG bmph;
}RECTM, *LPLECTM;

class CMob
{
public:
	CMob();
	~CMob();

public:
	DWORD ID;
	TCHAR type[8];
	TCHAR name[8];
	RECTM area;
	HBITMAP hBmp_mob;
	DWORD status;
	void setpos(LONG x, LONG y) { area.x = x; area.y = y; return; }
	void setpos(int x, int y) { area.x = x; area.y = y; return; }
	void setsize(LONG w, LONG h) { area.x = w; area.y = h; return; }
	void setsize(int w, int h) { area.x = w; area.y = h; return; }
	void setstatus(int i) { status = i; return; }
};

class CBC : public CMob
{
public:
	CBC() {};
	~CBC() {};
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

class CCUL : public CMob
{
public:
	CCUL() {};
	~CCUL() {};
private:

};





typedef struct _stMobsBody {
	CBC		bc[NUM_OF_BC];
	CHarai	haraiki[NUM_OF_HARAI];
	CCrush	crusher[NUM_OF_CRUSH];
	CCUL	cul[NUM_OF_CUL];
}STMobsBody, *LPSTMobsBody;


typedef struct _stMobs {
	BOOL	bUpdate;
	CMob* 	pmobs[MOB_TYPE_NUM][MOB_MAX_NUM];
	STMobsBody	mobs;
}STMobs, *LPSTMobs;



