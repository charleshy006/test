#ifdef _USE_BEHAVIAC_AI
#include "BehaviacAI.h"
#include "Being.h"

BEGIN_PROPERTIES_DESCRIPTION(CBehaviacAIGOBase)
	REGISTER_PROPERTY(m_property);
	REGISTER_METHOD(testMethod).PARAM_DISPLAY_INFO(L"test");
END_PROPERTIES_DESCRIPTION()

CBehaviacAIGOBase::CBehaviacAIGOBase()
{

}

CBehaviacAIGOBase::~CBehaviacAIGOBase()
{

}

bool CBehaviacAIGOBase::doActions(float dt)
{
	return true;
}

BEGIN_PROPERTIES_DESCRIPTION(CBehavicAIWorldState)

END_PROPERTIES_DESCRIPTION()

CBehavicAIWorldState::CBehavicAIWorldState()
{

}

CBehavicAIWorldState::~CBehavicAIWorldState()
{
	
}

CBehaviacAIMgr::~CBehaviacAIMgr()
{
}
	
CBehaviacAIMgr::CBehaviacAIMgr()
{
	m_pWs = NULL;
}

bool CBehaviacAIMgr::init()
{
	behaviac::Start();
	behaviac::Agent::Register<CBehavicAIWorldState>();
	behaviac::Agent::RegisterName<CBehavicAIWorldState>();
	behaviac::Agent::CreateInstance<CBehavicAIWorldState>();
	behaviac::World::GetInstance(0)->SetIdFlag(kIdMask_Wolrd);
	behaviac::Agent::Register<CBehaviacAIGOBase>();
	behaviac::Workspace::ExportMetas("./behaviac/ships.xml");
	Athena::logger->trace("初始化behaviacAI成功");
	return true;	
}

void CBehaviacAIMgr::timer(UINT32 cur)
{
	if (!m_pWs){
		return ;
	}
	//behaviac::Workspace::SetDeltaFrameTime(dt);
	m_pWs->btexec();
}

#endif
