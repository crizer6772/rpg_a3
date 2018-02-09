#pragma once

#include "../incl_full.hpp"
#include "console.hpp"

int GCCMD_sum(void* arg, void* con);
int GCCMD_echo(void* arg, void* con);
int GCCMD_importcfg(void* arg, void* con);
int GCCMD_listcvars(void* arg, void* con);
int GCCMD_listcommands(void* arg, void* con);

bool RegisterStandardCommands(GameConsole* con);
