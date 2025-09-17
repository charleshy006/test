#include "Chat.h"
#include <stdarg.h>
#include "Log4cxxHelp.h"
#include "GameUser.h"
#include "AthenaNew.h"
#include "TblDataMgr.h"
#include "ItemInfo.h"
#include "GlobalPacketProcessor.h"
#include "MapMgr.h"
#include "GameUserManager.h"
#include "Map.h"
#include "GameServer.h"
#include "GlobalSvrClient.h"
#include "CopyMap.h"
#include <tinyxml/tinyxml.h>

extern GameServer * g_gameServer;

std::map<std::string,ChatMgr::stGMCmdInfo>  ChatMgr::m_gmCmdCtx; 


ChatMgr::ChatMgr()
{
	m_gmCmdCtx.insert(std::make_pair("addItem",stGMCmdInfo(GM_TYPE_NORMAL,"addItem",ChatMgr::addItem,"增加一个道具")));
	m_gmCmdCtx.insert(std::make_pair("addDiamond",stGMCmdInfo(GM_TYPE_NORMAL,"addDiamond",ChatMgr::addDiamond,"增加钻石")));
	m_gmCmdCtx.insert(std::make_pair("addGoldCoin",stGMCmdInfo(GM_TYPE_NORMAL,"addGoldCoin",ChatMgr::addGoldCoin,"增加金币")));
	m_gmCmdCtx.insert(std::make_pair("addExp",stGMCmdInfo(GM_TYPE_NORMAL,"addExp",ChatMgr::addExp,"增加经验")));
	m_gmCmdCtx.insert(std::make_pair("levelup",stGMCmdInfo(GM_TYPE_NORMAL,"levelup",ChatMgr::levelup,"提升等级")));
	m_gmCmdCtx.insert(std::make_pair("levelupHero",stGMCmdInfo(GM_TYPE_NORMAL,"levelupHero",ChatMgr::levelupHero,"提升武将等级")));
	m_gmCmdCtx.insert(std::make_pair("printAllUser",stGMCmdInfo(GM_TYPE_NORMAL,"printAllUser",ChatMgr::printAllUser,"打印所有玩家")));
	m_gmCmdCtx.insert(std::make_pair("addPet",stGMCmdInfo(GM_TYPE_NORMAL,"addPet",ChatMgr::addPet,"添加一个宠物")));
	m_gmCmdCtx.insert(std::make_pair("test",stGMCmdInfo(GM_TYPE_NORMAL,"test",ChatMgr::test,"测试指令")));
	m_gmCmdCtx.insert(std::make_pair("addRes",stGMCmdInfo(GM_TYPE_NORMAL,"addRes",ChatMgr::addRes,"加资源")));
	m_gmCmdCtx.insert(std::make_pair("addTitle",stGMCmdInfo(GM_TYPE_NORMAL,"addTitle",ChatMgr::addTitle,"增加一个称号")));
	m_gmCmdCtx.insert(std::make_pair("addMail",stGMCmdInfo(GM_TYPE_NORMAL,"addMail",ChatMgr::addMail,"增加一封邮件")));
	m_gmCmdCtx.insert(std::make_pair("recharge",stGMCmdInfo(GM_TYPE_NORMAL,"recharge",ChatMgr::recharge,"充值")));
	m_gmCmdCtx.insert(std::make_pair("changesvr",stGMCmdInfo(GM_TYPE_NORMAL,"changesvr",ChatMgr::changeGameSvr,"换服")));
	m_gmCmdCtx.insert(std::make_pair("testRedis",stGMCmdInfo(GM_TYPE_NORMAL,"testRedis",ChatMgr::testRedis,"测试redis")));
	m_gmCmdCtx.insert(std::make_pair("changeMap",stGMCmdInfo(GM_TYPE_NORMAL,"changeMap",ChatMgr::changeMap,"测试changeMap")));
	m_gmCmdCtx.insert(std::make_pair("addContribute",stGMCmdInfo(GM_TYPE_NORMAL,"addContribute",ChatMgr::addContribute,"测试changeMap")));
	m_gmCmdCtx.insert(std::make_pair("finishAllLv",stGMCmdInfo(GM_TYPE_NORMAL,"finishAllLv",ChatMgr::finishAllLv,"完成所有关卡")));
	m_gmCmdCtx.insert(std::make_pair("finishTask",stGMCmdInfo(GM_TYPE_NORMAL,"finishTask",ChatMgr::finishTask,"完成某个任务")));
	m_gmCmdCtx.insert(std::make_pair("addActDegree",stGMCmdInfo(GM_TYPE_NORMAL,"addActDegree",ChatMgr::addActDegree,"增加公会贡献度")));
}

bool ChatMgr::initGMAccountInfo()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/GMAccountSet.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->trace("[GM账号配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("GMAccounts");
	if (!pRoot){
		return false;
	}
	
	TiXmlElement * pGMNode = pRoot->FirstChildElement("Set");
	while (pGMNode){
		const char * szVal = NULL;
		INT32 iVal = 0;
		pGMNode->Attribute("authority",&iVal);
		szVal = pGMNode->Attribute("account");
		if (szVal){
			m_gmAccountCfg[szVal] = iVal;	
		}
		pGMNode = pGMNode->NextSiblingElement("Set");
	}

	for (GMAccountCfg_IT it = m_gmAccountCfg.begin();it != m_gmAccountCfg.end();++it){
		Athena::logger->trace("初始化GM帐号配置,%s,%d",it->first.c_str(),it->second);
	}

	fileName = Athena::global["confDir"] + "/AuthorizeSet.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->trace("[GM指令权限配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	pRoot = doc.FirstChildElement("Authorize");
	if (!pRoot){
		return false;
	}
	
	TiXmlElement * pSetNode = pRoot->FirstChildElement("Set");
	while (pSetNode){
		const char * szVal = NULL;
		INT32 iVal = 0;
		pSetNode->Attribute("authority",&iVal);
		szVal = pSetNode->Attribute("command");
		if (szVal){
			std::map<std::string,stGMCmdInfo>::iterator it = m_gmCmdCtx.find(szVal);
			if (it != m_gmCmdCtx.end()){
				it->second.m_gmType = iVal;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("设置了gm指令%s的权限为%d",szVal,iVal);
#endif
			}
		}
		pSetNode  = pSetNode->NextSiblingElement("Set");
	}
	return true;
}

UINT8 ChatMgr::getGMAccountType(const char * pAccount)
{
	GMAccountCfg_IT it = m_gmAccountCfg.find(pAccount);
	if (it != m_gmAccountCfg.end()){
		return it->second;
	}
	return GM_TYPE_NONE;
}

void ChatMgr::parseGMCmd(KVPair & prop,const char* cmdLine, const char* flag)
{
	std::vector<std::string> sv;
	g_stringtok(sv,cmdLine, flag);
	for (std::vector<std::string>::const_iterator it = sv.begin(); it != sv.end(); ++it) {
		std::vector<std::string> ssv;
		g_stringtok(ssv, *it, "=");
		if (2 == ssv.size()){
			prop.setProperty(ssv[0], ssv[1]);
		}
	}
}

bool ChatMgr::exec(GameUser * pUser,std::string & cmd)
{
	cmd += " ";
	if (cmd[0] != '/' || cmd[1] != '/'){
		return false;
	}
	
	std::string cmdName ;
	std::string::size_type pos = cmd.find(" ");
	if ((pos != std::string::npos) && pos >= 2){
		cmdName = cmd.substr(2,pos - 2);
	}
	
	std::map<std::string,stGMCmdInfo>::iterator subIt = m_gmCmdCtx.find(cmdName);

	if (subIt == m_gmCmdCtx.end()){
		return true;
	}

#ifndef _ALL_DEBUG
	/*if (pUser->m_gmType < subIt->second.m_gmType)*/
	if (pUser->m_roleInfo.m_gm != eGMType_GM){
		Athena::logger->error("[执行GM指令]试图执行没有权限的指令");
		return true;
	}
#endif

	KVPair prop;
	parseGMCmd(prop,cmd.c_str()," \t\n");
#ifdef _HDZ_DEBUG
	Athena::logger->trace("执行的GM指令是%s",cmdName.c_str());
#endif
	return subIt->second.m_fun(pUser,prop);	
}

bool ChatMgr::addItem(GameUser *pUser,KVPair & prop)
{
	UINT32 itemID = (UINT32)atoi(prop["itemID"].c_str());
	UINT32 num  = (UINT32)atoi(prop["num"].c_str());

	if (0 == num){
		num = 1;
	}
	
	if (pUser->m_bagMgr.addItem2Bag(itemID,num,0,"GM指令")){
		Athena::logger->trace("添加道具到包裹成功!");
	}
	else {
		Athena::logger->error("添加道具到包裹失败!");
	}

	/*CItem * pItem = CBagMgr::createItem(itemID,num,0);
	if (pItem){
		UINT16 index = pUser->m_bagMgr.getBagFreeIndex(pItem->m_itemInfo.m_pos.m_bagType);
		pItem->m_itemInfo.m_pos.m_pos = index;
		if (pUser->m_bagMgr.addItem2Bag(pItem)){
			Athena::logger->trace("添加道具到包裹成功!");
		}
		else {
			SAFE_DELETE(pItem);
			Athena::logger->error("添加道具到包裹失败!");
		}
	}*/
	return true;
}

bool ChatMgr::addDiamond(GameUser *pUser,KVPair & prop)
{
	UINT32 num = (UINT32)atoi(prop["num"].c_str());
	//pUser->addDiamond(num,"gm指令");
	pUser->addResource(eResource_Diamond,num,"gm指令");
	return true;
}

bool ChatMgr::addGoldCoin(GameUser * pUser,KVPair & prop)
{
	UINT32 num = (UINT32)atoi(prop["num"].c_str());
	//pUser->addGoldCoin(num,"GM指令");
	pUser->addResource(eResource_GoldCoin,num,"GM指令");
	return true;
}

bool ChatMgr::addExp(GameUser * pUser,KVPair & prop)
{
	if (!pUser){
		return true;
	}
	UINT32 num = (UINT32)atoi(prop["num"].c_str());
	pUser->addExp(num);
	return true;
}

bool ChatMgr::levelup(GameUser * pUser,KVPair & prop)
{
	if (!pUser){
		return true;
	}
	UINT16 level = (UINT16)atoi(prop["num"].c_str());
	if (!g_playGrownBaseMgr.get(stPlayGrownBase::getFindID(pUser->getRoleID(),level))){
		Athena::logger->error("要提升到的等级不存在!");
		return true;	
	}

	if (pUser->m_roleInfo.m_level >= level){
		return true;
	}
	UINT16 loop = level - pUser->m_roleInfo.m_level;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("使用gm指令升级level=%d,loop=%d",level,loop);
#endif
	for (UINT16 i = 0; i < loop ; ++i ){
		pUser->levelup();	
	}

	Role::stSynRoleLevel  synCmd;
	synCmd.set_curexp(pUser->m_roleInfo.m_exp);
	synCmd.set_curlevel(pUser->m_roleInfo.m_level);
	pUser->sendProto2Me(synCmd);
	return true;
}

bool ChatMgr::levelupHero(GameUser * pUser,KVPair & prop)
{
	if (!pUser){
		return true;
	}
	return true;	
}

bool ChatMgr::printAllUser(GameUser * pUser,KVPair & prop)
{
	GameUserManager::getSingleton().printAllUser();
	return true;
}


bool ChatMgr::test(GameUser * pUser,KVPair & prop)
{
	if (!pUser){
		return true;
	}

	if ("addHero" == prop["act"]) {
		UINT32 heroID = (UINT32)atoi(prop["heroID"].c_str());
		UINT16 heroLv = (UINT16)atoi(prop["lv"].c_str());
		HeroInfo *pHeroInfo =  pUser->m_heroMgr.createHero(heroID,heroLv);
		if (pHeroInfo) {
			if (!pUser->m_heroMgr.addHeroInfo(pHeroInfo)) {
				SAFE_DELETE(pHeroInfo);
				Athena::logger->error("添加武将失败!");
			}
			else {
				Athena::logger->trace("添加武将成功!");
				pUser->setupRoleState();
			}
		}
	}
	else if ("rand" == prop["act"]) {
		UINT32 hitNum = 0;
		UINT32 loopNum = (UINT32)atoi(prop["loopNum"].c_str());
		UINT32 rate = (UINT32)atoi(prop["rate"].c_str());
		for (UINT32 i = 0;i < loopNum; ++i) {
			if (g_selectByPercent(rate)){
				hitNum += 1;
			}	
		}
		Athena::logger->trace("百分之%d的概率出现了%d次命中",rate,hitNum);
	}
	else if ("newguide" == prop["act"]){
		UINT16 step = (UINT16)atoi(prop["step"].c_str());
		pUser->m_roleInfo.m_newBieGuideID = step;
		Athena::logger->trace("玩家的新手引导阶段修改到了:%d",pUser->m_roleInfo.m_newBieGuideID);
	}
	else if ("notify" == prop["act"]){
		/*Role::stChatCmd retCmd;
		retCmd.set_type(Role::CHAT_TYPE_NOTICE);
		retCmd.set_content(prop["content"]);
		retCmd.set_srccharid(pUser->getID());
		retCmd.set_srccharname(pUser->getName());
		GameUserManager::getSingleton().sendProto2World(retCmd);*/
		Global::stProcessGM cmd;
		cmd.set_charid(pUser->getID());
		UINT32 delay = (UINT32)atoi(prop["delay"].c_str());
		UINT32 num = (UINT32)atoi(prop["num"].c_str());
		UINT32 interval = (UINT32)atoi(prop["interval"].c_str());
		cmd.add_intparam(delay);
		cmd.add_intparam(num);
		cmd.add_intparam(interval);
		cmd.add_strparam(prop["content"]);
		cmd.set_type(Global::GMType_SendNotice);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}
	else if ("batchSendMail" == prop["act"]){
		Global::stProcessGM cmd;
		cmd.set_charid(pUser->getID());
		cmd.set_type(Global::GMType_BatchSendMail);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	}
	return true;
}

bool ChatMgr::addPet(GameUser * pUser,KVPair & prop)
{
	UINT32 heroID = (UINT32)atoi(prop["heroID"].c_str());
	UINT16 heroLv = (UINT16)atoi(prop["lv"].c_str());
	HeroInfo *pHeroInfo =  pUser->m_heroMgr.createHero(heroID,heroLv);
	if (pHeroInfo) {
		if (!pUser->m_heroMgr.addHeroInfo(pHeroInfo)) {
			SAFE_DELETE(pHeroInfo);
			Athena::logger->error("添加武将失败!");
		}
		else {
			Athena::logger->trace("添加武将成功!");
			pUser->setupRoleState();
		}
	}
	return true;
}

bool ChatMgr::addRes(GameUser * pUser,KVPair & prop)
{
	UINT8 resType = (UINT8)atoi(prop["resType"].c_str());
	UINT32 num = (UINT32)atoi(prop["num"].c_str());
	pUser->addResource((eResource)resType,num,"GM指令");
	return true;
}

bool ChatMgr::addTitle(GameUser * pUser,KVPair & prop)
{
	UINT32 titleID = (UINT32)atoi(prop["id"].c_str());	
	pUser->m_titleMgr.addTitle(titleID);
	return true;
}

bool ChatMgr::addMail(GameUser * pUser,KVPair & prop)
{
	Global::stProcessGM retCmd;
	retCmd.set_charid(pUser->getID());
	
	std::string title = prop["title"];
	std::string content = prop["content"];

	retCmd.add_strparam(title);
	retCmd.add_strparam(content);

	std::string reward = prop["reward"];
	std::vector<std::string> strVec;
	g_stringtok(strVec,reward.c_str(),";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string> subStrVec;
		g_stringtok(subStrVec,strVec[i].c_str(),"-");
		if (subStrVec.size() >= 2){
			retCmd.add_intparam((UINT32)atoi(subStrVec[0].c_str()));
			retCmd.add_intparam((UINT32)atoi(subStrVec[1].c_str()));
		}
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
	return true;
}

bool ChatMgr::recharge(GameUser *pUser,KVPair & prop)
{
	UINT32 money = (UINT32)atoi(prop["num"].c_str());
	UINT16 id = (UINT16)atoi(prop["id"].c_str());
	std::ostringstream os ;
	os << "testid" << g_gameServer->getSec();
	std::string tid = os.str();
	pUser->m_rmbShop.buy(money,id,tid);
	return true;
}

bool ChatMgr::changeGameSvr(GameUser * pUser,KVPair & prop)
{
	UINT16 svrID = (UINT16)atoi(prop["svrID"].c_str());
	
	Global::stReqChangeGameSvr cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_gamesvrid(svrID);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	return true;
}

bool ChatMgr::testRedis(GameUser * pUser,KVPair & prop)
{
	Global::stProcessGM retCmd;
	retCmd.set_type(Global::GMType_OpRedis);
	retCmd.set_charid(pUser->getID());
	std::string key = prop["key"];
	std::string op  = prop["op"];
	retCmd.add_strparam(key);
	retCmd.add_strparam(op);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
	return true;

}

bool ChatMgr::changeMap(GameUser * pUser,KVPair & prop)
{
	if (!pUser){
		return true;
	}	
	
	UINT32 levelID = (UINT32)atoi(prop["levelID"].c_str());

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser->getName(),levelID);
	if (!pCopyMap){
		return true;
	}
	CMapMgr::getSingleton().changeMap(pUser,pCopyMap);

	UINT32 otherID = (UINT32)atoi(prop["charID"].c_str());
	GameUser * pOtherUser = GameUserManager::getSingleton().getUser(otherID);
	if (pOtherUser){
		CMapMgr::getSingleton().changeMap(pOtherUser,pCopyMap);	
	}
	return true;
}

bool ChatMgr::addContribute(GameUser * pUser,KVPair & prop)
{
	if (!pUser){
		return true;
	}	

	UINT32 addNum = (UINT32)atoi(prop["num"].c_str());
	
	Global::stAddGuildContribute cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_contribute(addNum);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	//pUser->addContribute(addNum,"GM指令");
	pUser->addResource(eResource_Guild_Contri,addNum,"GM指令");
	return true;
}

bool ChatMgr::sendNotify(const char * szContent)
{
	if (!szContent){
		return false;
	}
	Role::stChatCmd retCmd;
	retCmd.set_type(Role::CHAT_TYPE_NOTICE);
	retCmd.set_content(szContent);
	retCmd.set_srccharid(0);
	retCmd.set_srccharname("系统");
	GameUserManager::getSingleton().sendProto2World(retCmd);
	return true;	
}

bool ChatMgr::sendObtainItemNotify(GameUser * pUser,const char * szKey,UINT32 itemID,UINT32 num,UINT32 boxID)
{
	if (!pUser || !szKey){
		return true;
	}
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);
	if (!pItemBase || !pItemBase->m_bNeedNotify){
		return true;
	}
	
	/*if (CItem::isWuHunShiByType(pItemBase->m_type) && (num < 20)){
		return true;
	}*/
	
	itemID = CItem::realItemIDByJobAndSex(itemID,pUser->getJob(),pUser->getSex());
#ifdef _THAILAND_VER
	char szNotify[300] = { 0 };
#else
	char szNotify[100] = { 0 };
#endif

	bzero(szNotify,sizeof(szNotify));
	if (boxID){
		snprintf(szNotify,sizeof(szNotify),"[key=%s][player=%s][box=%d][item=%d][count=%d]",szKey,pUser->getName(),boxID,itemID,num);
	}
	else {
		snprintf(szNotify,sizeof(szNotify),"[key=%s][player=%s][item=%d][count=%d]",szKey,pUser->getName(),itemID,num);
	}
	ChatMgr::sendNotify(szNotify);     
	return true;
}

bool ChatMgr::finishAllLv(GameUser * pUser,KVPair & prop)
{
	pUser->m_levelMgr.finishAllPVELevel();
	return true;
}

bool ChatMgr::finishTask(GameUser * pUser,KVPair & prop)
{
	UINT32 taskID = (UINT32)atoi(prop["taskID"].c_str());
	pUser->m_taskMgr.forceFinish(taskID,true);
	return true;	
}

bool ChatMgr::addActDegree(GameUser * pUser,KVPair & prop)
{
	if (!pUser){
		return true;
	}
	
	UINT32 addNum = (UINT32)atoi(prop["num"].c_str());
	Global::stProcessGM cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_type(Global::GMType_ADD_GUILD_ACTIVITY_DEGREE);
	cmd.add_intparam(addNum);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	return true;
}
