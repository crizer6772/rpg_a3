#pragma once
#include "incl_full.hpp"

int strcmp_c(const char* a, const char* b);
uint32_t ui32max(uint32_t a, uint32_t b);
uint32_t ui32min(uint32_t a, uint32_t b);
uint32_t ui32clip(uint32_t a, uint32_t n, uint32_t b);
int32_t i32max(int32_t a, int32_t b);
int32_t i32min(int32_t a, int32_t b);
int32_t i32clip(int32_t a, int32_t n, int32_t b);
uint32_t pow2round(uint32_t n);
void byte2hex(char* out, uint8_t b);
