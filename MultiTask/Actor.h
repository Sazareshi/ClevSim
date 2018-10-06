#pragma once
#include "ThreadObj.h"
#include "mob.h"



class CActor :public CThreadObj
{
public:
	CActor();
	~CActor();

	void init_task(void* pobj);
	static CActor* pActInst;
	static int BMP_WH[MOB_TYPE_NUM][2];
	LPSTMobs pstMobs;
	HANDLE hmue_mobs;
	void routine_work(void *param);
};

