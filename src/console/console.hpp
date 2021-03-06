#pragma once
#include "../incl_full.hpp"
#include "../lang/langmgr.hpp"
#include "../display/display.hpp"
#include "../resmgr/resmgr.hpp"
#include "../lang/unihlp.hpp"

#define GC_CVAR_MAX_NAME_SIZE 32
#define GC_CVAR_MAX_DATA_SIZE 64
#define GC_CMD_MAX_NAME_SIZE 32
#define GC_CMD_MAX_HELP_SIZE 140 //stfu twitter did that for 12 years
#define GC_MAX_CVARS 1024
#define GC_MAX_CMD 1024

typedef int (*ConsoleCmdFunc)(void*, void*); //void* arg, void* console

struct ConsoleVariable
{
	ConsoleVariable();
	ConsoleVariable(const char* name, const char* value);
	char name[GC_CVAR_MAX_NAME_SIZE];
	char data[GC_CVAR_MAX_DATA_SIZE];
};

struct ConsoleCommand
{
	ConsoleCommand();
	ConsoleCommand(const char* name, void* ptr);
	ConsoleCommand(const char* name, void* ptr, const char* help);
	void* func;
	char name[GC_CMD_MAX_NAME_SIZE];
	char help[GC_CMD_MAX_HELP_SIZE];
};

class GameConsole
{
	ConsoleVariable cvars[GC_MAX_CVARS];
	ConsoleCommand cmd[GC_MAX_CMD];
	uint32_t* ConsoleLogBuf;
	char* UTF8LineBuf;
	LanguageMgr* lm;
	DisplayMgr* dm;
	ResourceMgr* rm;
public:
	GameConsole();
	size_t numCVars;
	size_t numCommands;
	uint16_t cLogBufW;
	uint16_t cLogBufH;
	uint16_t cLogCursorX;
	uint16_t cLogCursorY;

	///CVAR RELATED FUNCTIONS
	ConsoleVariable* FindCVar(const char* name);
	void ListCVarsToFile(FILE* f);
	bool CreateCVar(const char* name, const char* value);
	bool RemoveCVar(const char* name);
	bool SetCVar(const char* name, const char* value);
	char* GetCVar(const char* name);
	bool SetCVar(const char* name, int32_t value);
	bool SetCVar(const char* name, int64_t value);
	bool SetCVar(const char* name, float value);
	bool SetCVar(const char* name, double value);
	int32_t GetCVarI32(const char* name);
	int64_t GetCVarI64(const char* name);
	float GetCVarF32(const char* name);
	double GetCVarF64(const char* name);
	uint32_t GetCVarUI32(const char* name);
	uint64_t GetCVarUI64(const char* name);

	///LM/RM/DM INTEGRATION
	void LinkToLanguageMgr(LanguageMgr* lm);
	void LinkToDisplayMgr(DisplayMgr* dm);
	void LinkToResourceMgr(ResourceMgr* rm);


	///STRING PARSING
	bool IsRTTypeValid(char* rttype);
	int32_t ParseReplacementToken(char* out, char type, int param, const char* value);
	int32_t ParseReplacementTokens(char* out, const char* str);

	///LOG RELATED FUNCTIONS
	bool LogBufAlloc(uint16_t w, uint16_t h, bool cls);
	bool Log(const void* str, bool utf32);
	bool LogLine(const void* str, bool utf32);
	void LogLineFeed();
	void LogSetColor(uint32_t col); //24-bit RGB
	void LogSetColorCGA(uint8_t col); //4-bit RGBI
	char* LogGetLineUTF8(uint16_t y);

	///COMMAND RELATED FUNCTIONS
	ConsoleCommand* FindCommand(const char* name);
	bool AddCommand(const char* name, void* ptr);
	bool AddCommand(const char* name, void* ptr, const char* helpstr);
	bool RemoveCommand(const char* name);
	void ListCommandsToFile(FILE* f);
	void ParseCommand(const char* cmd, char* cmdpart, size_t cmdpart_s, char* argpart, size_t argpart_s, size_t rtsize);
	int ExecuteCommand(const char* cmd);
};
