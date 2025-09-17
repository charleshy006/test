#include "BattlePropValue.h"
#include "UtilFunction.h"

stBattlePropValue &  stBattlePropValue::operator += (const stBattlePropValue & one)
{   
	m_val1 += one.m_val1;
	m_val2 += one.m_val2;
	m_val3 += one.m_val3;
	m_val4 += one.m_val4;
	return *this;
} 

void stBattlePropValue::init(std::string str)
{
	std::vector<std::string> strVec;
	g_stringtok(strVec,str,"-");
	if (strVec.size() >= 1){ 
		m_propID = (UINT8)atoi(strVec[0].c_str());
	}   

	if (strVec.size() >= 2){ 
		m_val1 = (UINT32)atoi(strVec[1].c_str());
	}   

	if (strVec.size() >= 3){ 
		m_val2 = (UINT32)atoi(strVec[2].c_str());
	}   

	if (strVec.size() >= 4){ 
		m_val3 = (UINT32)atoi(strVec[3].c_str());
	}   

	if (strVec.size() >= 5){ 
		m_val4 = (UINT32)atoi(strVec[4].c_str());
	}   
}

