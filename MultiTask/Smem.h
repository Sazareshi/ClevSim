#pragma once

#include "mob.h"
#include "panel.h"
#define MUTEXID_MOBS					0


//共有メモリの構成定義
typedef struct smembpdy_tag {
	ULONG simtime_ms;//シミュレーション上での時間
	STMobs mobs;
	ST_PanelOutSmem panels;
}ST_SMEM_BODY, *P_ST_SMEM_BODY;


#define SMEM_UNIT_SIZE					1000000 //1Mbyte
typedef struct smemdef_tag {
	union {
		char unit[SMEM_UNIT_SIZE];
		ST_SMEM_BODY stSmem;
	};
public:
	~smemdef_tag();

}ST_SMEM_FORMAT, *P_ST_SMEM_FORMAT;

class CSmem
{
public:
	CSmem();
	~CSmem();
};

