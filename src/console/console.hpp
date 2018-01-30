#pragma once
#include "../incl_full.hpp"

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
	uint32_t** ConsoleLogBuf;
    void SortCVars(int a, int b);
	void SortCVars();
	void SortCommands(int a, int b);
	void SortCommands();
public:
	GameConsole();
	size_t numCVars;
	size_t numCommands;
	uint16_t cLogBufW;
	uint16_t cLogBufH;

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

	///LOG RELATED FUNCTIONS
	bool LogBufAlloc(uint16_t w, uint16_t h, bool cls);
	bool LogPush(void* str, bool utf32);
	bool LogPushLine(void* str, bool utf32);

	void LogLineFeed();


	///COMMAND RELATED FUNCTIONS
	ConsoleCommand* FindCommand(const char* name);
	bool AddCommand(const char* name, void* ptr);
	bool AddCommand(const char* name, void* ptr, const char* helpstr);
	bool RemoveCommand(const char* name);
	void ListCommandsToFile(FILE* f);
	void ParseCommand(const char* cmd, char* cmdpart, size_t cmdpart_s, char* argpart, size_t argpart_s);
	bool ExecuteCommand(const char* cmd);
};
