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
	cLogCursorX = 0;
	cLogCursorY = 0;
	ConsoleLogBuf = NULL;
	lm = NULL;
	rm = NULL;
	dm = NULL;
	UTF8LineBuf = NULL;
	/*
	//might as well do this
	memset(this,0,sizeof(GameConsole));
	*/
}

int qsCVarCompare(const void* a, const void* b)
{
	ConsoleVariable* va = (ConsoleVariable*)a;
	ConsoleVariable* vb = (ConsoleVariable*)b;
	return strcmp_c(vb->name, va->name);
}
int qsCmdCompare(const void* a, const void* b)
{
	ConsoleCommand* va = (ConsoleCommand*)a;
	ConsoleCommand* vb = (ConsoleCommand*)b;
	return strcmp_c(vb->name, va->name);
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
	qsort(cvars, numCVars, sizeof(ConsoleVariable), qsCVarCompare);
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
    qsort(cvars, numCVars, sizeof(ConsoleVariable), qsCVarCompare);
    return true;
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
	char buf[64];
	memset(buf, 0, 64);
	sprintf(buf, "%.16f", value);

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
LM/DM/RM INTEGRATION
**/

void GameConsole::LinkToLanguageMgr(LanguageMgr* lm)
{
	this->lm = lm;
}
void GameConsole::LinkToDisplayMgr(DisplayMgr* dm)
{
	this->dm = dm;
}
void GameConsole::LinkToResourceMgr(ResourceMgr* rm)
{
	this->rm = rm;
}

/**
LOG RELATED FUNCTIONS
**/
bool GameConsole::LogBufAlloc(uint16_t w, uint16_t h, bool cls)
{
	if(!w || !h)
		return false;

	uint32_t* newbuf = (uint32_t*)malloc(w*h*sizeof(uint32_t));
	if(!newbuf)
		return false;
	for(size_t i=0; i<w*h; i++)
		newbuf[i] = (uint32_t)' ';
	if(!cls && ConsoleLogBuf)
	{
		int32_t offset = i32max(0,cLogBufH-h);
		int32_t height = i32min(cLogBufH,h);
		for(uint16_t i=offset; i<offset+height; i++)
		{
			memcpy(&newbuf[w*i],&ConsoleLogBuf[cLogBufW*i],ui32min(cLogBufW,w)*sizeof(uint32_t));
		}
		free(ConsoleLogBuf);
	}
	cLogBufW = w;
	cLogBufH = h;
	ConsoleLogBuf = newbuf;

	if(UTF8LineBuf)
		free(UTF8LineBuf);
	UTF8LineBuf = (char*)calloc(w*h*4+1, 1);
	return true;
}
bool GameConsole::Log(const void* str, bool utf32)
{
	if(!ConsoleLogBuf || !str)
		return false;
	uint32_t* str32 = (uint32_t*)str;
	char* str8 = (char*)str;
	for(size_t i=0;;i++)
	{
		uint32_t ch=0;
		if(utf32)
			ch = str32[i];
		else
			ch = str8[i];
		if(!ch)
			break;
		if(cLogCursorX >= cLogBufW)
		{
			cLogCursorY++;
			cLogCursorX=0;
		}
		while(cLogCursorY >= cLogBufH)
		{
			LogLineFeed();
			cLogCursorY--;
		}
		ConsoleLogBuf[cLogCursorY*cLogBufW+cLogCursorX] = ch;
		cLogCursorX++;
	}
	return true;
}
bool GameConsole::LogLine(const void* str, bool utf32)
{
	bool r = Log(str,utf32);
	LogLineFeed();
	return r;
}
void GameConsole::LogLineFeed()
{
	if(!ConsoleLogBuf)
		return;
	cLogCursorX = 0;
	cLogCursorY++;

	while(cLogCursorY >= cLogBufH)
	{
		cLogCursorY--;
		for(uint16_t i=0; i<cLogBufH-1; i++)
			memcpy(&ConsoleLogBuf[cLogBufW*i], &ConsoleLogBuf[cLogBufW*(i+1)], cLogBufW*sizeof(uint32_t));
		for(uint16_t i=0; i<cLogBufW; i++)
			ConsoleLogBuf[cLogBufW*(cLogBufH-1)+i] = (uint32_t)' ';
	}

}
void GameConsole::LogSetColor(uint32_t col)
{

}
void GameConsole::LogSetColorCGA(uint8_t col)
{

}
char* GameConsole::LogGetLineUTF8(uint16_t y)
{
	if(y >= cLogBufH || !ConsoleLogBuf)
		return NULL;
	size_t s=0;
	for(size_t i=0; i<cLogBufW; i++)
		s += CPtoUTF8(&UTF8LineBuf[s],ConsoleLogBuf[y*cLogBufW+i]);
	return UTF8LineBuf;
}

/**
COMMAND RELATED FUNCTIONS
**/
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
		qsort(cmd, numCommands, sizeof(ConsoleCommand), qsCmdCompare);
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
void GameConsole::ParseCommand(const char* cmd, char* cmdpart, size_t cmdpart_s, char* argpart, size_t argpart_s, size_t rtsize)
{
	char realcmd[rtsize+8];
	memset(realcmd,0,rtsize+8);
	ParseReplacementTokens(realcmd, cmd);
	bool s = false;
	size_t cmdsize = 0;
	size_t argsize = 0;

	char* i = (char*)realcmd;
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
	memcpy(cmdpart, realcmd, ui32min(cmdpart_s, cmdsize));
	memcpy(argpart, &realcmd[cmdsize+1], ui32min(argpart_s, argsize));
	//delete[] realcmd;
}
int GameConsole::ExecuteCommand(const char* cmd)
{
	ConsoleCommand* c;
	if(!cmd || !(*cmd))
	{
		printf("error: GameConsole: empty command\n");
		return -1;
	}
	int s = ParseReplacementTokens(NULL,cmd);
	//int s = 1024;
	char cmdpart[s];
	char argpart[s];
	memset(cmdpart, 0, s);
	memset(argpart, 0, s);
	ParseCommand(cmd, cmdpart, s, argpart, s, s-1);
	c = FindCommand(cmdpart);
	if(!c)
	{
		printf("error: GameConsole: invalid command\n");
		return -1;
	}


	ConsoleCmdFunc f = (ConsoleCmdFunc)FindCommand(cmdpart)->func;
	return f(argpart, this);
}
