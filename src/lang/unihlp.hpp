#pragma once
#include "../incl_full.hpp"

size_t CPtoUTF8(char* out, uint32_t codepoint);
size_t UTF32toUTF8(char* out, uint32_t* in);
