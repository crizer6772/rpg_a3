#include "langmgr.hpp"

int qsLInfCompare(const void* a, const void* b)
{
	LanguageMgr::LangInfo* va = (LanguageMgr::LangInfo*)a;
	LanguageMgr::LangInfo* vb = (LanguageMgr::LangInfo*)b;
	return strcmp_c(vb->cName, va->cName);
}
bool LanguageMgr::ResizeLInfArray(size_t s)
{
	size_t dCap = pow2round(s)<<1;
	if(LInfCapacity == 0)
	{
		langInfo = (LangInfo*)calloc(dCap, sizeof(LangInfo));
		if(!langInfo)
			return false;
		LInfCapacity = dCap;
	}
	else if(dCap != LInfCapacity)
	{
		langInfo = (LangInfo*)realloc(langInfo, dCap*sizeof(LangInfo));
		if(!langInfo)
			return false;
		if(dCap > LInfCapacity)
			memset(&langInfo[LInfCapacity], 0, (dCap-LInfCapacity)*sizeof(LangInfo));
		LInfCapacity = dCap;
	}
	return true;
}
LanguageMgr::LangInfo* LanguageMgr::FindLangInfo(const char* name)
{
	if(!name || !*name)
	{
		return NULL;
	}

	int l = 0;
	int h = numLanguages-1;
	int i = 0;
	while(l<=h)
	{
		i = l + (h-l)/2;
		int c = strcmp_c(langInfo[i].cName, name);
		if(!c)
			return &langInfo[i];
		if(c<0)
			h = i-1;
		else
			l = i+1;
	}

	return NULL;
}
void LanguageMgr::deinit()
{
	for(size_t i=0; i<numLanguages; i++)
	{
		if(langInfo[i].cfg)
			al_destroy_config(langInfo[i].cfg);
	}
	if(langlist)
		al_destroy_config(langlist);
	if(langInfo)
		delete[] langInfo;
	langlist = NULL;
	langInfo = NULL;
}
LanguageMgr::LanguageMgr()
{
	langlist = NULL;
	cLang = NULL;
	langInfo = NULL;
	numLanguages = 0;
	LInfCapacity = 0;
}
LanguageMgr::~LanguageMgr()
{
	if(langInfo)
		free(langInfo);
}

bool LanguageMgr::LoadLanguageList(const char* filename)
{
	deinit();
	langlist = al_load_config_file(filename);
	if(!langlist)
	{
		return false;
	}

	ALLEGRO_CONFIG_ENTRY* it;
	char* r = (char*)al_get_first_config_entry(langlist, NULL, &it);
	for(numLanguages=0; r; numLanguages++)
	{
		char* lName = r;
		char* lFilename = (char*)al_get_config_value(langlist, NULL, lName);
		ALLEGRO_CONFIG* nl = al_load_config_file(lFilename);
		if(nl)
		{
			bool a = ResizeLInfArray(numLanguages+1);
			if(!a)
				return false;
			langInfo[numLanguages].cName = lName;
			langInfo[numLanguages].cfg = nl;
		}
		else
		{
			numLanguages--;
		}
		r = (char*)al_get_next_config_entry(&it);
	}
	qsort(langInfo, numLanguages, sizeof(LangInfo), qsLInfCompare);
	return true;
}
bool LanguageMgr::LanguageExists(const char* name)
{
	return FindLangInfo(name);
}
size_t LanguageMgr::GetNumLanguages()
{
	return numLanguages;
}
char* LanguageMgr::GetLanguageName(uint32_t id)
{
	if(id >= numLanguages)
	{
		return NULL;
	}
	return langInfo[id].cName;
}
char* LanguageMgr::GetLanguageFullName(uint32_t id)
{
	if(id >= numLanguages)
	{
		return NULL;
	}
	return langInfo[id].cName;
}
char* LanguageMgr::GetLanguageFullName(const char* name)
{
	return (char*)al_get_config_value(FindLangInfo(name)->cfg, NULL, "langname");
}

bool LanguageMgr::SelectLanguage(uint32_t id)
{
	if(id >= numLanguages)
	{
		return false;
	}
	cLang = &langInfo[id];
	return true;
}
bool LanguageMgr::SelectLanguage(const char* name)
{
	cLang = FindLangInfo(name);
	return cLang;
}

char* LanguageMgr::GetString(const char* id)
{
	if(!cLang)
	{
		return (char*)id;
	}
	char* str = (char*)al_get_config_value(cLang->cfg, NULL, id);
	if(!str)
	{
		return (char*)id;
	}
	else
	{
		return str;
	}
}
