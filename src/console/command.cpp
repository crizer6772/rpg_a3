#include "command.hpp"


int GCCMD_sum(void* arg, void* con)
{
	char* str = (char*)arg;
	size_t sl = strlen(str);
	int32_t a = 0;
	int32_t b = 0;
	int p[2] = {-1,-1};
	int pp=0;
	for(size_t i=0; i<sl; i++)
	{
		if((str[i]>='0'&&str[i]<='9')||str[i]=='-')
		{
			if(i==0)
			{
				p[pp++]=i;
			}
			else if(str[i-1]!='-'&&(str[i-1]<='0'||str[i-1]>='9'))
			{
				p[pp++]=i;
			}
		}
	}
	if(p[0] != -1)
		a = atoi(&str[p[0]]);
	if(p[1] != -1)
		b = atoi(&str[p[1]]);
	return a+b;
}
int GCCMD_echo(void* arg, void* con)
{
	printf("%s\n", (char*)arg);
	return 311;
}
int GCCMD_importcfg(void* arg, void* con)
{
	char* cArg = (char*)arg;
	GameConsole* cCon = (GameConsole*)con;

	ALLEGRO_CONFIG* cfg = al_load_config_file(cArg);
	if(!cfg)
	{
		printf("error: %s does not exist\n", (char*)arg);
		return 0;
	}
	ALLEGRO_CONFIG_ENTRY* it;
	char* r = (char*)al_get_first_config_entry(cfg, NULL, &it);
	while(r)
	{
		char* cvName = r;
		char* cvValue = (char*)al_get_config_value(cfg, NULL, cvName);
		cCon->CreateCVar(cvName, cvValue);
		printf("console <- (%s, value: %s)\n", cCon->FindCVar(cvName)->name, cCon->FindCVar(cvName)->data);
		r = (char*)al_get_next_config_entry(&it);
	}
	al_destroy_config(cfg);
	return 1;
}
int GCCMD_listcvars(void* arg, void* con)
{
	GameConsole* cCon = (GameConsole*)con;
	cCon->ListCVarsToFile(stdout);
	return 1;
}
int GCCMD_listcommands(void* arg, void* con)
{
	GameConsole* cCon = (GameConsole*)con;
	cCon->ListCommandsToFile(stdout);
	return 1;
}

bool RegisterStandardCommands(GameConsole* con)
{
	con->AddCommand("sum", (void*)GCCMD_sum, "returns the sum of 2 integers, intended for use with %R");
	con->AddCommand("echo", (void*)GCCMD_echo, "do i really have to fucking explain");
	con->AddCommand("importcfg", (void*)GCCMD_importcfg, "importcfg [name] - imports a config file to console variables");
	con->AddCommand("listcvars", (void*)GCCMD_listcvars, "lists cvars in current console");
	con->AddCommand("listcommands", (void*)GCCMD_listcommands, "list commands and pointers to their functions in current console");
}
