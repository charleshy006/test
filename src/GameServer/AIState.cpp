#include "AIState.h"
#include "AINpc.h"

AIState::AIState()
{
}

AIState::~AIState()
{
}

void AIState::actionIdle(CAINpc* pAI, UINT64 uTime)
{
	pAI->actionIdle(uTime);	
}


void AIState::actionDead(CAINpc* pAI, UINT64 uTime)
{
	pAI->actionDead(uTime);	
}

void AIState::actionCombat(CAINpc* pAI, UINT64 uTime)
{
	pAI->actionCombat(uTime);	
}


void AIState::actionGohome(CAINpc* pAI, UINT64 uTime)
{

	pAI->actionGohome(uTime);	
}

///////////////////////////////////////////////////////////////////////////////
bool IdleState::doAction(CAINpc* pAI,UINT64 uTime)
{
	actionIdle(pAI,uTime);	
	return true;
}


bool CombatState::doAction(CAINpc* pAI,UINT64 uTime)
{
	actionCombat(pAI,uTime);	
	return true;
}

bool DeadState::doAction(CAINpc* pAI,UINT64 uTime)
{
	actionDead(pAI,uTime);	
	return true;
}

bool GoHomeState::doAction(CAINpc* pAI,UINT64 uTime)
{
	actionGohome(pAI,uTime);	
	return true;
}
