#ifndef __APPCONFIGBASE_H__
#define __APPCONFIGBASE_H__
#include <string>
#include "tinyxml/tinyxml.h"
#include <unistd.h>
class AppConfig
{
protected:
	TiXmlDocument m_doc;						//xml文件解析器
	std::string m_filename;						//文件名字
	//解析所有server公用的东西
	bool getNodeVal(const TiXmlNode * node);
public:
	AppConfig(const char* filename = "../../config/appconfig.xml");
	virtual ~AppConfig();
	//加载配置
	bool loadCfg(const char* name);
	virtual bool parseCustom(const TiXmlNode * node) { return true ; }
	//检查文件是否存在
	bool isFileExist(const char * fileName);
	//保存appConfig
	bool saveAppConfig(const char * fileName);
};
#endif

