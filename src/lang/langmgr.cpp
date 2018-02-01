#include "langmgr.hpp"

void LanguageMgr::SortLInf(int32_t a, int32_t b)
{
	if(b<=a)
	{
		return;
	}
	if(b-a==1)
	{
		LangInfo aux = langInfo[0];
		langInfo[0] = langInfo[1];
		langInfo[1] = aux;
		return;
	}
	int32_t i = a-1;
	int32_t j = b+1;
	for(int32_t n=0;;n++)
	{
		while(strcmp_c(langInfo[(a+b)/2].cName, langInfo[++i].cName) < 0);
		while(strcmp_c(langInfo[(a+b)/2].cName, langInfo[--j].cName) > 0);
		if(i <= j)
		{
			LangInfo aux = langInfo[i];
			langInfo[i] = langInfo[j];
			langInfo[j] = aux;
		}
		else
		{
			break;
		}
	}
	if(j > a)
		SortLInf(a, j);
	if(i < b)
		SortLInf(i, b);
}
void LanguageMgr::ResizeLInfArray(size_t s)
{
	size_t dCap = pow2round(s)<<1;
	if(LInfCapacity == 0)
	{
		langInfo = new LangInfo[dCap];
		memset(langInfo, 0, dCap*sizeof(LangInfo));
		LInfCapacity = dCap;
	}
	else if(dCap != LInfCapacity)
	{
		LangInfo* nl = new LangInfo[dCap];
		memset(nl, 0, dCap*sizeof(LangInfo));
		memcpy(nl, langInfo, ui32min(dCap, LInfCapacity)*sizeof(LangInfo));
		delete[] langInfo;
		langInfo = nl;
		LInfCapacity = dCap;
	}
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
		delete[] langInfo;
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
			ResizeLInfArray(numLanguages+1);
			langInfo[numLanguages].cName = lName;
			langInfo[numLanguages].cfg = nl;
		}
		else
		{
			numLanguages--;
		}
		r = (char*)al_get_next_config_entry(&it);
	}
	SortLInf(0, numLanguages-1);
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
