#ifndef _GEM_CFG_H_
#define _GEM_CFG_H_

struct GemGroupInfo {
	int id;
	char name[64];
};

struct GemEntityInfo {
	int id;
	int resultID;
	int MaterialsID[4];
	int Synthesisnum;
	int consumegold;
	int Successrate;
	int needitem;
	int neednum;
	int show;
};

extern bool LoadGemCfg(void);
extern void GetGemGroupInfo(int index, struct GemGroupInfo *slot);
extern void GetGemEntityInfo(int key, int index, struct GemEntityInfo *slot);
extern int GetGemIsOpen();
extern int GetGemTimes();
extern int GetGemMax();
extern UINT32 GemGroupDataSize();
extern UINT32 GemEntitySize(int key);
extern void TestGemCfg(void);

#endif /* _GEM_CFG_H_ */

