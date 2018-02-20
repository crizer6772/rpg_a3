#pragma once
#include "../incl_full.hpp"

size_t CPtoUTF8(char* out, uint32_t codepoint);
size_t UTF32toUTF8(char* out, uint32_t* in);
uint32_t UTF8ByteGroupCodepoint(char* in);
size_t UTF8ToUTF32(uint32_t* out, char* str);
bool IsValidUTF8(char* str);
