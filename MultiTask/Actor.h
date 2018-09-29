#pragma once
#include "ThreadObj.h"
#include "mob.h"

#define MOB_ID_HARAI			0 //�����o���@ID
#define MOB_ID_CRUSH			1 //�N���b�V���[ID

#define NUM_OF_HARAI			9
#define NUM_OF_CRUSH			2


typedef struct _stSmemAct{
	CHarai	haraiki[NUM_OF_HARAI];
	CCrush	crusher[NUM_OF_CRUSH];
}STSmemAct, *LPSTSmemAct;

class CActor :
	public CThreadObj
{
public:
	CActor();
	~CActor();

	void init_task(void* pobj);
	static CActor* pActInst;
	static LPSTSmemAct pActSmem;
};

