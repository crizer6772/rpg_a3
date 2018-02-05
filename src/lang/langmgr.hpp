#pragma once
#include "../hlpfunc.hpp"
#include "../incl_full.hpp"

class LanguageMgr
{
public:
	struct LangInfo
	{
		char* cName;
		ALLEGRO_CONFIG* cfg;
	};
	ALLEGRO_CONFIG* langlist;
	LangInfo* cLang;
	LangInfo* langInfo;
	size_t numLanguages;
	size_t LInfCapacity;
	//void SortLInf(int a, int b);
	bool ResizeLInfArray(size_t s);
	LangInfo* FindLangInfo(const char* name);
	void deinit();
//public:
	LanguageMgr();
	~LanguageMgr();

	bool LoadLanguageList(const char* filename);
	bool LanguageExists(const char* name);
	size_t GetNumLanguages();
	char* GetLanguageName(uint32_t id);
	char* GetLanguageFullName(uint32_t id);
	char* GetLanguageFullName(const char* name);

	bool SelectLanguage(uint32_t id);
	bool SelectLanguage(const char* name);

	char* GetString(const char* id);
};
