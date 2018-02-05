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

	/*
	//might as well do this
	memset(this,0,sizeof(GameConsole));
	*/
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
	/*
	(2018-01-26)
	yes i know, i don't need to do quicksort since we're only adding one entry at a time.
	what i should be doing is finding the index where we want to push the entry in O(n)
	and then push it there. but i'm lazy and this is just not worth doing having in mind
	how often you're gonna call CreateCVar()
	*/
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
STRING PARSING
**/
bool GameConsole::IsReplacementTokenValid(const char* str)
{
	if(strlen(str) < 3)
		return false;
	if(str[1] != ':' && str[2] != ':' && str[3] != ':')
		return false;
	if(str[0] != 'V' && str[0] != 'H' && str[0] != 'R')
		return false;
	return true;
}
size_t GameConsole::ParseReplacementToken(char* out, const char* str, size_t bufsize)
{
	uint32_t sp=0;
	for(int i=1; i<4; i++)
		if(str[i]==':')
		{
			sp=i;
			break;
		}
	char pc[4]={0,0,0,0};
	memcpy(pc,str,ui32min(3,sp));
	if(out == NULL)
	{
		if(pc[0] == 'V')
		{
			char nbuf[64];
			memset(nbuf,0,64);
			memcpy(nbuf,&str[sp+1],strlen(str)-(sp+1));
			return strlen(GetCVar(nbuf));
		}
		if(pc[0] == 'H')
		{
			if(sp==1)
				return 8;
			if(sp>1)
				return 2*atoi(&pc[1]);
		}
		if(pc[0] == 'R')
		{
			return 11; //max 11 digits in an int
		}
		if(pc[0] == '%')
			return 1;
		return 0;
	}
	else
	{
		if(pc[0] == 'V')
		{
			char nbuf[64];
			memset(nbuf,0,64);
			strcpy(nbuf,&str[sp+1]);
			size_t cvl = strlen(GetCVar(nbuf));
			memcpy(out,GetCVar(nbuf),cvl);
			return cvl;
		}
		if(pc[0]=='H')
		{
			int hsize;
			if(sp==1)
			{
				hsize = 4;
			}
			if(sp>1)
			{
				hsize = atoi(&str[1]);
			}
			char nbuf[64];
			char hb[2*hsize+1];
			hb[2*hsize]=0;
			//memset(hb,0,2*hsize+1);
			//memset(nbuf,0,64);
			strcpy(nbuf,&str[sp+1]);
			for(int i=0; i<hsize; i++)
			{
				uint8_t* var = (uint8_t*)FindCVar(nbuf)->data;
				byte2hex(&hb[2*i], var[i]);
			}
			strcpy(out,hb);
			return hsize*2;
		}
		if(pc[0]=='R')
		{
			size_t l = strlen(str);
			char cmdbuf[l];
			memset(cmdbuf,0,l);
			strcpy(cmdbuf,&str[sp+1]);
			char res[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
			sprintf(res, "%d", ExecuteCommand(cmdbuf));
			strcpy(out,res);
			return strlen(res);
		}
		return 0;
	}
}
size_t GameConsole::ParseReplacementTokens(char* out, const char* str, size_t bufsize)
{
	/**
	Available replacement tokens:
	%V, %H, %Hx, %R, %%
	example:
		echo %V:pi;
	output:
		3.1415926535897931

	example:
		echo %H8:pi;
	output:
		332E3134

	example:
		echo %R:importcfg config-new.ini;
	output:
		0
	**/

	uint32_t numrt = 0;

	int32_t rtStart[64];
	int32_t rtEnd[64];
	for(int i=0; i<64; i++)
	{
		rtStart[i] = -1;
		rtEnd[i] = -1;
	}

	int32_t level=0;
	int32_t crt = 0;
	for(char* it=(char*)str;*it;it++)
	{
		int32_t i = it-str;
		if(*it=='%')
		{
			bool dp = false;
			if(i>0)
				if(*(it-1)=='%')
					dp=true;
			if(*(it+1) == '%')
				dp=true;
			if(!dp)
			{
				if(level==0)
				{
					crt = i;
				}
				level++;
			}
		}
		if(*it==';')
		{
			level--;
			if(level==0)
			{
				if(i-crt >= 4)
				{
					size_t rtbs = (i-crt);
					char rtt[rtbs];
					rtt[rtbs-1]=0;
					memcpy(rtt, &str[crt+1], rtbs-1);

					if(IsReplacementTokenValid(rtt))
					{
						rtStart[numrt] = crt;
						rtEnd[numrt] = i;
						numrt++;
					}
				}
			}
		}
	}
	if(numrt == 0)
	{
		if(out)
		{
			strcpy(out,str);
			return bufsize;
		}
		else
		{
			return strlen(str)+1;
		}
	}

	uint32_t rti = 0;
	if(out == NULL)
	{
		size_t strs = strlen(str);
		size_t res = strs;
		uint32_t i=0;
		for(char* it=(char*)str;*it&&(uint32_t)(it-str)<strs;it++)
		{
			i = it-str;
			if(i == rtStart[rti] && rti<numrt)
			{
				size_t rts = 1+rtEnd[rti]-rtStart[rti];
				char rtt[rts-1];
				rtt[rts-2] = 0;
				memcpy(rtt,&it[1],rts-2);
				//printf("rtt=%s,rts=%d\n",rtt,rts);
				it += rts;
				res -= rts;
				res += ParseReplacementToken(NULL,rtt,0);
				rti++;
			}
			if(!(*it))
			{
				break;
			}
		}
		return res+1;
	}
	else
	{
		uint32_t i_out = 0;
		for(char* it=(char*)str;*it;it++)
		{
			uint32_t i = it-str;
			if(i == rtStart[rti] && rti<numrt)
			{
				size_t rts = 1+rtEnd[rti]-rtStart[rti];
				char rtt[rts-1];
				rtt[rts-2] = 0;
				memcpy(rtt,&it[1],rts-2);
				it += rts;
				uint32_t shift = ParseReplacementToken(&out[i_out],rtt,0);
				i_out += shift;
				out[i_out]=0;
				rti++;
			}
			if(!(*it) || i_out >= bufsize)
			{
				break;
			}
			out[i_out++] = *it;
			out[i_out] = 0;
		}
		return 0;
	}
}

/**
LOG RELATED FUNCTIONS
**/
bool GameConsole::LogBufAlloc(uint16_t w, uint16_t h, bool cls)
{
	if(!w || !h)
		return false;

	uint32_t* newbuf = (uint32_t*)malloc(w*h*sizeof(uint32_t));
	memset(newbuf,' ',w*h*sizeof(uint32_t));
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
	return true;
}
bool GameConsole::Log(void* str, bool utf32)
{

}
bool GameConsole::LogLine(void* str, bool utf32)
{
	Log(str,utf32);
	LogLineFeed();
}
void GameConsole::LogLineFeed()
{
	if(!ConsoleLogBuf)
		return;

}
void GameConsole::LogSetColor(uint32_t col)
{

}
void GameConsole::LogSetColorCGA(uint8_t col)
{

}

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
void GameConsole::ParseCommand(const char* cmd, char* cmdpart, size_t cmdpart_s, char* argpart, size_t argpart_s, size_t rtsize)
{
	char realcmd[rtsize+8];
	memset(realcmd,0,rtsize+8);
	ParseReplacementTokens(realcmd, cmd, rtsize);
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
	int s = ParseReplacementTokens(NULL,cmd,0)+1;
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
