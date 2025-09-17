#include "ProFilePhotoCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"

bool CProFilePhotoCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/profilephoto.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[头像]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pConf = doc.FirstChildElement("conf");
	if (!pConf){
		Athena::logger->error("[头像]配置加载错误,找不到Conf节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pProfessionNode = pConf->FirstChildElement("profession");
	while(pProfessionNode){
		UINT32 roleID = 0;
		std::map<UINT16, stProFilePhotoInfo> cond;

		if (pProfessionNode->Attribute("id",&iVal)){
			roleID = iVal;
		}
		TiXmlElement * pEntryNode = pProfessionNode->FirstChildElement("entry");
		while(pEntryNode){
			stProFilePhotoInfo one;

			if (pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}

			if (pEntryNode->Attribute("UnlockCondition",&iVal)){
				one.m_unlockCondition = iVal;
			}

			if (pEntryNode->Attribute("ConditionNum",&iVal)){
				one.m_conditionNum = iVal;
			}
			cond[one.m_id] = one;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
		m_conf[roleID] = cond;
		pProfessionNode = pProfessionNode->NextSiblingElement("profession");
	}

#ifdef _HDZ_DEBUG
	 std::map<UINT32, std::map<UINT16, stProFilePhotoInfo> >::iterator it = m_conf.begin();
	 for(;it != m_conf.end(); ++it){
	 	Athena::logger->trace("[头像]配置表: id :%d", it->first);
		 std::map<UINT16, stProFilePhotoInfo>::iterator subIt = it->second.begin();
		 for(;subIt != it->second.end(); ++subIt){
			 stProFilePhotoInfo & one = subIt->second;
			 Athena::logger->trace("	[头像]:id:%d, UnlockCondition:%d, ConditionNum:%d", one.m_id, one.m_unlockCondition, one.m_conditionNum);
		 }
	 }
#endif
	 return true;
}

stProFilePhotoInfo * CProFilePhotoCfg::getConfigurationByID(UINT32 roleID, UINT16 confID)
{
	std::map<UINT32, std::map<UINT16, stProFilePhotoInfo> >::iterator it = m_conf.find(roleID);
	if(it == m_conf.end()){
		return NULL;
	}

	std::map<UINT16, stProFilePhotoInfo>::iterator subIt = it->second.find(confID);
	if(subIt == it->second.end()){
		return NULL;
	}

	return &subIt->second;
}
