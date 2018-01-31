#include "console.hpp"
#include "../hlpfunc.hpp"

ConsoleVariable::ConsoleVariable()
{
	memset(name, 0, GC_CVAR_MAX_NAME_SIZE);
	memset(data, 0, GC_CVAR_MAX_DATA_SIZE);
}
ConsoleVariable::ConsoleVariable(const char* name, const char* value)
{
	ConsoleVariable();
	strncpy(this->name, name, GC_CVAR_MAX_NAME_SIZE);
	strncpy(this->data, value, GC_CVAR_MAX_DATA_SIZE);
}
ConsoleCommand::ConsoleCommand()
{
	func = NULL;
	memset(name, 0, GC_CMD_MAX_NAME_SIZE);
	memset(help, 0, GC_CMD_MAX_HELP_SIZE);
}
ConsoleCommand::ConsoleCommand(const char* name, void* ptr)
{
	func = ptr;
	strncpy(this->name, name, GC_CMD_MAX_NAME_SIZE);
	memset(help, 0, GC_CMD_MAX_HELP_SIZE);
}
ConsoleCommand::ConsoleCommand(const char* name, void* ptr, const char* help)
{
	func = ptr;
	strncpy(this->name, name, GC_CMD_MAX_NAME_SIZE);
	strncpy(this->help, help, GC_CMD_MAX_HELP_SIZE);
}

GameConsole::GameConsole()
{
	memset(cvars, 0, GC_MAX_CVARS*sizeof(ConsoleVariable));
	memset(cmd, 0, GC_MAX_CMD*sizeof(ConsoleCommand));
	numCVars = 0;
	numCommands = 0;
	cLogBufW = 0;
	cLogBufH = 0;
	ConsoleLogBuf = NULL;
}
/**
CVAR RELATED FUNCTIONS
**/
ConsoleVariable* GameConsole::FindCVar(const char* name)
{
	if(!name || !*name)
	{
		return NULL;
	}

	int l = 0;
	int h = numCVars-1;
	int i = 0;
	while(l<=h)
	{
		i = l + (h-l)/2;
		int c = strcmp_c(cvars[i].name, name);
		if(!c)
			return &cvars[i];
		if(c<0)
			h = i-1;
		else
			l = i+1;
	}

	return NULL;
}
bool GameConsole::CreateCVar(const char* name, const char* value)
{
	if(numCVars >= GC_MAX_CVARS-1)
	{
		return false;
	}
	if(FindCVar(name))
	{
		SetCVar(name, value);
		return true;
	}
	cvars[numCVars++] = ConsoleVariable(name, value);
	SortCVars();
	return true;
}
bool GameConsole::RemoveCVar(const char* name)
{
	ConsoleVariable* v = FindCVar(name);
	if(!v)
	{
		return false;
	}

    memmove(v, v+1, ((numCVars--)-(1+v-cvars)) * sizeof(ConsoleVariable));
    cvars[numCVars] = ConsoleVariable();
    SortCVars();
    return true;
}
void GameConsole::SortCVars(int a, int b)
{
	//yes i know, i don't need to do quicksort since we're only adding one entry at a time.
	//what i should be doing is finding the index where we want to push the entry in O(n)
	//and then push it there. but i'm lazy and this is just not worth doing having in mind
	//how often you're gonna call CreateCVar()
	if(b<=a)
	{
		return;
	}

	int i = a-1;
	int j = b+1;
	while(1)
	{
		while(strcmp_c(cvars[(a+b)/2].name, cvars[++i].name) < 0);
		while(strcmp_c(cvars[(a+b)/2].name, cvars[--j].name) > 0);
		if(i <= j)
		{
			//swap
			ConsoleVariable aux = cvars[i];
			cvars[i] = cvars[j];
			cvars[j] = aux;
		}
		else
			break;
	}
	if(j > a)
		SortCVars(a, j);
	if(i < b)
		SortCVars(i, b);
}
void GameConsole::SortCVars()
{
	SortCVars(0, numCVars-1);
}
void GameConsole::ListCVarsToFile(FILE* f)
{
	for(size_t i=0; i<numCVars; i++)
		fprintf(f, "cvar #%d: %s = %s\n", i, cvars[i].name, cvars[i].data);
}
bool GameConsole::SetCVar(const char* name, const char* value)
{
	ConsoleVariable* v = FindCVar(name);
	if(!v)
	{
		return false;
	}
	else
	{
		*v = ConsoleVariable(name, value);
	}
	return true;
}
char* GameConsole::GetCVar(const char* name)
{
	return FindCVar(name)->data;
}
bool GameConsole::SetCVar(const char* name, int64_t value)
{
	char buf[32];
	memset(buf, 0, 32);
	sprintf(buf, "%lld", value);

	SetCVar(name, buf);
	return true;
}
bool GameConsole::SetCVar(const char* name, int32_t value)
{
	return SetCVar(name, (int64_t)value);
}
bool GameConsole::SetCVar(const char* name, double value)
{
	char buf[32];
	memset(buf, 0, 32);
	sprintf(buf, "%f", value);

	SetCVar(name, buf);
	return true;
}
bool GameConsole::SetCVar(const char* name, float value)
{
	return SetCVar(name, (double)value);
}
int64_t GameConsole::GetCVarI64(const char* name)
{
	return atoll(FindCVar(name)->data);
}
int32_t GameConsole::GetCVarI32(const char* name)
{
	return (int32_t)GetCVarI64(name);
}
double GameConsole::GetCVarF64(const char* name)
{
	return atof(FindCVar(name)->data);
}
float GameConsole::GetCVarF32(const char* name)
{
	return (float)GetCVarF64(name);
}
uint64_t GameConsole::GetCVarUI64(const char* name)
{
	return strtoll(FindCVar(name)->data, NULL, 10);
}
uint32_t GameConsole::GetCVarUI32(const char* name)
{
	return (uint32_t)GetCVarUI64(name);
}

/**
LOG RELATED FUNCTIONS
**/


/**
COMMAND RELATED FUNCTIONS
**/
void GameConsole::SortCommands(int a, int b)
{
	if(b<=a)
	{
		return;
	}

	int i = a-1;
	int j = b+1;
	while(1)
	{
		while(strcmp_c(cmd[(a+b)/2].name, cmd[++i].name) < 0);
		while(strcmp_c(cmd[(a+b)/2].name, cmd[--j].name) > 0);
		if(i <= j)
		{
			//swap
			ConsoleCommand aux = cmd[i];
			cmd[i] = cmd[j];
			cmd[j] = aux;
		}
		else
		{
			break;
		}
	}
	if(j > a)
	{
		SortCommands(a, j);
	}
	if(i < b)
	{
		SortCommands(i, b);
	}
}
void GameConsole::SortCommands()
{
	SortCommands(0, numCommands-1);
}
ConsoleCommand* GameConsole::FindCommand(const char* name)
{
	if(!name)
	{
		return NULL;
	}
	int l = 0;
	int h = numCommands-1;
	int i = 0;
	while(l<=h)
	{
		i = l + (h-l)/2;
		int c = strcmp_c(cmd[i].name, name);
		if(!c)
			return &cmd[i];
		if(c<0)
			h = i-1;
		else
			l = i+1;
	}
	return NULL;
}
bool GameConsole::AddCommand(const char* name, void* ptr)
{
	return AddCommand(name, ptr, NULL);
}
bool GameConsole::AddCommand(const char* name, void* ptr, const char* helpstr)
{
	if(!name || !ptr)
	{
		return false;
	}
	ConsoleCommand* c = FindCommand(name);
	if(c)
	{
		*c = ConsoleCommand(name, ptr, helpstr);
		return true;
	}
	else
	{
		if(numCommands >= GC_MAX_CMD-1)
		{
			return false;
		}
		cmd[numCommands++] = ConsoleCommand(name, ptr, helpstr);
		SortCommands();
		return true;
	}
}
bool GameConsole::RemoveCommand(const char* name)
{
	ConsoleCommand* c = FindCommand(name);
	if(!c)
	{
		return false;
	}
	memmove(c, c+1, ((numCommands--)-(1+c-cmd)) * sizeof(ConsoleCommand));
    cmd[numCommands] = ConsoleCommand();
    return true;
}
void GameConsole::ListCommandsToFile(FILE* f)
{
	for(size_t i=0; i<numCommands; i++)
	{
		fprintf(f, "command #%d: %08X <- %s\n", i, (uint32_t)cmd[i].func, cmd[i].name);
	}
}
void GameConsole::ParseCommand(const char* cmd, char* cmdpart, size_t cmdpart_s, char* argpart, size_t argpart_s)
{
	bool s = false;
	size_t cmdsize = 0;
	size_t argsize = 0;
	char* i = (char*)cmd;
	while(*(i++))
	{
		if(!s)
			cmdsize++;
		else
			argsize++;
		if(*i == ' ' && !s)
		{
			s = true;
			continue;
		}
	}
	memcpy(cmdpart, cmd, ui32min(cmdpart_s, cmdsize));
	memcpy(argpart, &cmd[cmdsize+1], ui32min(argpart_s, argsize));
}
bool GameConsole::ExecuteCommand(const char* cmd)
{
	ConsoleCommand* c;
	if(!cmd)
	{
		printf("error: GameConsole: empty command\n");
		return false;
	}
	int s = strlen(cmd);
	if(!s || !cmd)
	{
		return false;
	}
	char cmdpart[s+1];
	char argpart[s+1];
	memset(cmdpart, 0, s+1);
	memset(argpart, 0, s+1);
	ParseCommand(cmd, cmdpart, s, argpart, s);
	c = FindCommand(cmdpart);
	if(!c)
	{
		printf("error: GameConsole: invalid command\n");
		return false;
	}


	ConsoleCmdFunc f = (ConsoleCmdFunc)FindCommand(cmdpart)->func;
	f(argpart, this);
	return true;
}
