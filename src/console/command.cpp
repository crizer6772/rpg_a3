#include "command.hpp"

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
	while(1)
	{
		char* cvName = r;
		char* cvValue = (char*)al_get_config_value(cfg, NULL, cvName);
		cCon->CreateCVar(cvName, cvValue);
		printf("console <- (%s, value: %s)\n", cCon->FindCVar(cvName)->name, cCon->FindCVar(cvName)->data);
		r = (char*)al_get_next_config_entry(&it);
		if(!r)
			break;
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
	con->AddCommand("importcfg", (void*)GCCMD_importcfg, "importcfg [name] - imports a config file to console variables");
	con->AddCommand("listcvars", (void*)GCCMD_listcvars, "lists cvars in current console");
	con->AddCommand("listcommands", (void*)GCCMD_listcommands, "list commands and pointers to their functions in current console");
}
