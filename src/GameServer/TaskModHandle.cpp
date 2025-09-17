#include "TaskModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "GuildConfig.h"
#include "GlobalSvrClient.h"

extern GameServer * g_gameServer;

TaskModHandle::TaskModHandle()
{
	registerHandle(&TaskModHandle::reqAcceptTask);
	registerHandle(&TaskModHandle::reqCommitTask);
	registerHandle(&TaskModHandle::chgTaskSubTs);
	registerHandle(&TaskModHandle::saveCurrentTask);
	registerHandle(&TaskModHandle::batchAcceptTask);
	registerHandle(&TaskModHandle::reqScoreReward);
	registerHandle(&TaskModHandle::openTaskWin);
}

TaskModHandle::~TaskModHandle()
{

}

bool TaskModHandle::handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<TaskModHandle,GameUser>::handle(pUser,pData,cmdLen,funID); 
}

void TaskModHandle::reqAcceptTask(GameUser * pUser,TaskProto::stReqAcceptTask * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	TaskProto::stAcceptTaskError retCmd;
	stTaskDBInfo * pTask = pUser->m_taskMgr.acceptTask(recvCmd->taskid());
	if (!pTask){
		retCmd.set_reason(TaskProto::stAcceptTaskError::eCon_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}
}

void TaskModHandle::reqCommitTask(GameUser * pUser,TaskProto::stReqCommitTask * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	TaskProto::stCommitTaskResult retCmd;
	retCmd.set_taskid(recvCmd->taskid());

	if (pUser->m_taskMgr.commitTask(recvCmd->taskid())){
		retCmd.set_result(TaskProto::stCommitTaskResult::eSUCCESS);	
		pUser->sendProto2Me(retCmd);
		pUser->m_taskMgr.checkDailyTask();
		return ;
	}
	else {
		retCmd.set_result(TaskProto::stCommitTaskResult::eCON_NOT_ENOUGH);	
		pUser->sendProto2Me(retCmd);
		return ;
	}
}

void TaskModHandle::chgTaskSubTs(GameUser * pUser,TaskProto::stChgTaskSubStatus * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	stTaskDBInfo * pTask = pUser->m_taskMgr.getTaskInfo(recvCmd->taskid());
	if (pTask){
		pTask->m_subTs = recvCmd->subts();
	}
}

void TaskModHandle::saveCurrentTask(GameUser * pUser,TaskProto::stSaveCurrentTask * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_currentTaskID = recvCmd->curtaskid();
	pUser->m_currentTaskStage = recvCmd->curstage();

	if (0 == pUser->m_currentTaskID){
		pUser->m_currentTaskStage = 0;
	}
}

void TaskModHandle::batchAcceptTask(GameUser * pUser,TaskProto::stReqBatchAcceptTask * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	TaskProto::stRetBatchAccpetTask retCmd;
	for (UINT16 i = 0;i < recvCmd->taskids_size();++i){
		TaskProto::stSynTaskInfo * pSyn = retCmd.add_taskinfos();
		stTaskDBInfo * pTask = pUser->m_taskMgr.acceptTask(recvCmd->taskids(i),false,false);
		if (pTask && pSyn){
			pTask->fillSynInfo(*pSyn);
		}
	}
	pUser->sendProto2Me(retCmd);
}

void TaskModHandle::reqScoreReward(GameUser * pUser,TaskProto::stReqDailyTaskScoreReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	pUser->m_taskMgr.gotScoreReward(recvCmd->id());
}

void TaskModHandle::openTaskWin(GameUser * pUser,TaskProto::stReqOpenTaskWindow * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	if (pUser->m_taskMgr.isNeedRefreshDailyTask()){
		pUser->m_taskMgr.sendAllDailyTask();
		pUser->m_taskMgr.setNeedRefreshDailyTask(false);
	}
}

