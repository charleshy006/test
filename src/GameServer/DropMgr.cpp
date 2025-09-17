#include "DropMgr.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "GameUser.h"

bool CDropMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/drop.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[掉落配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[掉落配置]读取配置文件找不到节点<root>");
		return false;
	}

	TiXmlElement * pDropNode = pRoot->FirstChildElement("drop");
	if (!pDropNode){
		Athena::logger->error("[掉落配置]找不到节点<drop>");
		return false;
	}
	
	INT32 iVal = 0;

	TiXmlElement * pDropGroupNode = pDropNode->FirstChildElement("Dropgroup");
	while (pDropGroupNode){
		stDropGroup group;
		if (pDropGroupNode->Attribute("ID",&iVal)){
			group.m_id = iVal;	
		}

		TiXmlElement * pCertainNode = pDropGroupNode->FirstChildElement("certain");
		if (pCertainNode){
			if (pCertainNode->Attribute("droptype",&iVal)){
				group.m_certainType = iVal;	
			}

			TiXmlElement * pEntryNode = pCertainNode->FirstChildElement("entry");
			while (pEntryNode){
				stDropEntry entry;
				if (pEntryNode->Attribute("id",&iVal)){
					entry.m_entryID = iVal;
				}

				if (pEntryNode->Attribute("item",&iVal)){
					entry.m_itemID = iVal;		
				}

				if (pEntryNode->Attribute("Dropnum",&iVal)){
					entry.m_num = iVal;	
				}

				if (pEntryNode->Attribute("DropRP",&iVal)){
					entry.m_rate = iVal;
				}

				group.m_certainVec.push_back(entry);
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
		}

		TiXmlElement * pProbabilityNode = pDropGroupNode->FirstChildElement("probability");
		if (pProbabilityNode){
			TiXmlElement * pEntryNode = pProbabilityNode->FirstChildElement("entry");
			while (pEntryNode){
				stDropEntry entry;
			
				if (pEntryNode->Attribute("id",&iVal)){
					entry.m_entryID = iVal;
				}

				if (pEntryNode->Attribute("item",&iVal)){
					entry.m_itemID = iVal;		
				}

				if (pEntryNode->Attribute("Dropnum",&iVal)){
					entry.m_num = iVal;	
				}

				if (pEntryNode->Attribute("DropRP",&iVal)){
					entry.m_rate = iVal;
				}

				group.m_probabilityVec.push_back(entry);
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
		}
		m_groups.insert(std::make_pair(group.m_id,group));
		pDropGroupNode = pDropGroupNode->NextSiblingElement("Dropgroup");
	}
#ifdef _HDZ_DEBUG
	std::map<UINT32,stDropGroup>::iterator it =  m_groups.begin();
	for (;it != m_groups.end();++it){
		stDropGroup & group = it->second;
		Athena::logger->trace("掉落组id=%d的掉落信息有:",it->first);
		for (UINT16 i = 0;i < group.m_certainVec.size(); ++i){
			stDropEntry & entry = group.m_certainVec[i];
			Athena::logger->trace("必定掉落itemID=%d,num=%d,rate=%d",entry.m_itemID,entry.m_num,entry.m_rate);
		}

		for (UINT16 i = 0;i < group.m_probabilityVec.size();++i){
			stDropEntry & entry = group.m_probabilityVec[i];
			Athena::logger->trace("必定掉落itemID=%d,num=%d,rate=%d",entry.m_itemID,entry.m_num,entry.m_rate);
		}
	}
#endif
	return true;
}

INT32  CDropMgr::randomFrom(std::vector<stDropEntry> & inVec)
{
	UINT32 totalRate = 0;
	std::map<UINT32,UINT32> index2RateMap;
	for (UINT16 i = 0;i < inVec.size(); ++i){
		stDropEntry & entry = inVec[i];
		totalRate += entry.m_rate;
		index2RateMap[i] = entry.m_rate;
	}		

	return g_selectByMap(index2RateMap,totalRate);
}

void CDropMgr::processDropGroup(UINT32 dropID,std::vector<stDropEntry> &outVec,UINT32 drawNum)
{
	std::map<UINT32,stDropGroup>::iterator it =  m_groups.find(dropID);

	if (it != m_groups.end()){
		stDropGroup & group = it->second;
		if (group.m_certainType == 1){//随机
			if (group.m_certainVec.size() > 0){
				UINT16 index = g_randBetween(0,group.m_certainVec.size() - 1);
				outVec.push_back(group.m_certainVec[index]);
			}
		}
		else {
			for (UINT16 i = 0;i < group.m_certainVec.size();++i){
				outVec.push_back(group.m_certainVec[i]);
			}
		}
		for (UINT16 i = 0;i < drawNum; ++i) {
			INT32 index = randomFrom(group.m_probabilityVec);
			if ((index != -1) && (index < (INT32)group.m_probabilityVec.size())){
				outVec.push_back(group.m_probabilityVec[index]);
			}	
		}
	}
}
