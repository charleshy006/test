#ifndef __H_TASK_MOD_HANDLE_H__
#define __H_TASK_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "GameUser.h"
#include "GameProtoMsgHandleBase.h"
#include "Task.pb.h"

using namespace net;
class TaskModHandle : public ProtoMsgHandle<TaskModHandle,GameUser>,public GameProtoMsgHandleBase
{
public :
	//构造
	TaskModHandle();
	//析构
	~TaskModHandle();
	//处理网络协议
	bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求接受任务
	void reqAcceptTask(GameUser * pUser,TaskProto::stReqAcceptTask * recvCmd);
	//请求完成任务
	void reqCommitTask(GameUser * pUser,TaskProto::stReqCommitTask * recvCmd);
	//改变任务子状态
	void chgTaskSubTs(GameUser * pUser,TaskProto::stChgTaskSubStatus * recvCmd);
	//正在进行的任务
	void saveCurrentTask(GameUser * pUser,TaskProto::stSaveCurrentTask * recvCmd);
	//请求批量接受任务
	void batchAcceptTask(GameUser * pUser,TaskProto::stReqBatchAcceptTask * recvCmd);
	//请求积分奖励
	void reqScoreReward(GameUser * pUser,TaskProto::stReqDailyTaskScoreReward * recvCmd);
	//打开任务界面
	void openTaskWin(GameUser * pUser,TaskProto::stReqOpenTaskWindow * recvCmd);
};
#endif
