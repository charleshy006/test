#include <tinyxml/tinyxml.h>
#include <string>
#include <vector>
#include "TypesDef.h"
#include "Log4cxxHelp.h"
#include "GemCfg.h"

#undef CONFIG_ERROR_RETURN
#define CONFIG_ERROR_RETURN(format, ...)				\
	do {												\
		Athena::logger->error(format, ##__VA_ARGS__);	\
		return false;									\
	} while(0)

static std::vector<struct GemGroupInfo> group_data;
static __gnu_cxx::hash_map<int, std::vector<struct GemEntityInfo> > entity_data;
static int isopen;
static int times;
static int max;

static bool PushGroupData(TiXmlElement *xmlnode) {
	struct GemGroupInfo temp;
	const char *s;

	bzero(&temp, sizeof(GemGroupInfo));
	if (!(xmlnode->Attribute("id", &temp.id)))
		return false;
	if (!(s = xmlnode->Attribute("name")))
		return false;
	strcpy(temp.name, s);
	group_data.push_back(temp);

	return true;
}

static bool PushEntityData(int key, TiXmlElement *xmlnode) {
	struct GemEntityInfo temp;
	char buffer[64] = {0};
	const char *s = NULL;
	const char *delimit = ";";
	char *p = buffer;

	bzero(&temp, sizeof(GemEntityInfo));
	if (!(xmlnode->Attribute("id", &temp.id)))
		return false;
	if (!(xmlnode->Attribute("resultID", &temp.resultID)))
		return false;
	if (!(s = xmlnode->Attribute("MaterialsID")))
		return false;
	strcpy(buffer, s);
	for (int i(0); i < 4; ++i) {
		temp.MaterialsID[i] = atoi(strsep(&p, delimit));
	}
	if (!(xmlnode->Attribute("Synthesisnum", &temp.Synthesisnum)))
		return false;
	if (!(xmlnode->Attribute("consumegold", &temp.consumegold)))
		return false;
	if (!(xmlnode->Attribute("Successrate", &temp.Successrate)))
		return false;
	if (!(xmlnode->Attribute("needitem", &temp.needitem)))
		return false;
	if (!(xmlnode->Attribute("neednum", &temp.neednum)))
		return false;
	if (!(xmlnode->Attribute("show", &temp.show)))
		return false;
	entity_data[key].push_back(temp);
	
	return true;
}

bool LoadGemCfg(void) {
	group_data.reserve(4);

	TiXmlDocument doc;
	const char *fp = "./conf/Gemsynthesis.xml";
	if (!doc.LoadFile(fp))
		CONFIG_ERROR_RETURN("打开配置文件%s错误!", fp);
	TiXmlElement *root;
	if (!(root = doc.FirstChildElement("root")))
		CONFIG_ERROR_RETURN("%s配置文件找不到root节点!", fp);
	TiXmlElement *Synthesis_node;
	if (!(Synthesis_node = root->FirstChildElement("Synthesis")))
		CONFIG_ERROR_RETURN("%s配置文件找不到Synthesis节点!", fp);
	TiXmlElement *group_node;
	if (!(group_node = Synthesis_node->FirstChildElement("group")))
		CONFIG_ERROR_RETURN("%s配置文件找不到group节点!", fp);
	int index(0);
	while (group_node) {
		if (!PushGroupData(group_node))
			CONFIG_ERROR_RETURN("%s配置文件group节点找不到某个属性", fp);
		TiXmlElement *entry_node;
		if (!(entry_node = group_node->FirstChildElement("entry")))
			CONFIG_ERROR_RETURN("%s配置文件找不到entry节点!", fp);
		while (entry_node) {
			if (!PushEntityData(group_data[index].id, entry_node))
				CONFIG_ERROR_RETURN("%s配置文件entry节点找不到某个属性", fp);
			entry_node = entry_node->NextSiblingElement("entry");
		}
		++index;
		group_node = group_node->NextSiblingElement("group");
	}
	TiXmlElement *Beecham_node;
	if (!(Beecham_node = root->FirstChildElement("Beecham")))
		CONFIG_ERROR_RETURN("%s配置文件找不到Beecham节点!", fp);
	if (!(Beecham_node->Attribute("open", &isopen)))
		CONFIG_ERROR_RETURN("%s配置文件找不到Beecham节点的open属性!", fp);
	TiXmlElement *Beecham_entrynode;
	if (!(Beecham_entrynode = Beecham_node->FirstChildElement("entry")))
		CONFIG_ERROR_RETURN("%s配置文件找不到Beecham节点的子节点entry!", fp);
	if (!(Beecham_entrynode->Attribute("num", &times)))
		CONFIG_ERROR_RETURN("%s配置文件找不到Beecham节点的子节点entry的num属性!", fp);
	if (!(Beecham_entrynode->Attribute("max", &max)))
		CONFIG_ERROR_RETURN("%s配置文件找不到Beecham节点的子节点entry的max属性!", fp);

	return true;
}

void GetGemGroupInfo(int index, struct GemGroupInfo *slot)
{
	slot->id = group_data[index].id;
	strcpy(slot->name, group_data[index].name);
}

void GetGemEntityInfo(int key, int index, struct GemEntityInfo *slot)
{
	slot->id = entity_data[key][index].id;
	slot->resultID = entity_data[key][index].resultID;
	for (int i(0); i < 4; ++i)
		slot->MaterialsID[i] = entity_data[key][index].MaterialsID[i];
	slot->Synthesisnum = entity_data[key][index].Synthesisnum;
	slot->consumegold = entity_data[key][index].consumegold;
	slot->Successrate = entity_data[key][index].Successrate;
	slot->needitem = entity_data[key][index].needitem;
	slot->neednum = entity_data[key][index].neednum;
	slot->show = entity_data[key][index].show;
}

int GetGemIsOpen() {
	return isopen;
}

int GetGemTimes() {
	return times;
}

int GetGemMax() {
	return max;
}

UINT32 GemGroupDataSize() {
	return group_data.size();
}

UINT32 GemEntitySize(int key) {
	return entity_data[key].size();
}

void TestGemCfg(void) {
	struct GemGroupInfo groupdata;
	for (unsigned int i(0); i < group_data.size(); ++i) {
		GetGemGroupInfo(i, &groupdata);
		Athena::logger->trace("id:%d, name:%s", groupdata.id, groupdata.name);
	}
	struct GemEntityInfo entitydata;
	for (int i(1); i <= 4; ++i) {
		for (unsigned int j(0); j < entity_data[i].size(); ++j) {
			GetGemEntityInfo(i, j, &entitydata);
			Athena::logger->trace("id:%d, resultID:%d, MaterialsID[0]:%d, MaterialsID[1]:%d, MaterialsID[2]:%d, MaterialsID[3]:%d, Synthesisnum:%d, consumegold:%d, Successrate:%d, needitem:%d, neednum:%d, show:%d", entitydata.id, entitydata.resultID, entitydata.MaterialsID[0], entitydata.MaterialsID[1], entitydata.MaterialsID[2], entitydata.MaterialsID[3], entitydata.Synthesisnum, entitydata.consumegold, entitydata.Successrate, entitydata.needitem, entitydata.neednum, entitydata.show);
		}
	}
	Athena::logger->trace("isopen:%d, times:%d, max:%d", GetGemIsOpen(), GetGemTimes(), GetGemMax());
}

