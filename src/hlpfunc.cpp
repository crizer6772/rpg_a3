#include "hlpfunc.hpp"

//helpers

//case insensitive strcmp
int strcmp_c(const char* a, const char* b)
{
	char ca, cb;
	for(int i=0;;i++)
	{
		ca = (a[i]>='A'&&a[i]<='Z')?a[i]+32:a[i];
		cb = (b[i]>='A'&&b[i]<='Z')?b[i]+32:b[i];
		if(ca!=cb)
			return cb-ca;
		if(!a[i] && !b[i])
			return 0;
	}
}
uint32_t ui32max(uint32_t a, uint32_t b)
{
	return (a>b)?a:b;
}
uint32_t ui32min(uint32_t a, uint32_t b)
{
	return (a<b)?a:b;
}
uint32_t ui32clip(uint32_t a, uint32_t n, uint32_t b)
{
	return ui32max(a, ui32min(n, b));
}
int32_t i32max(int32_t a, int32_t b)
{
	return (a>b)?a:b;
}
int32_t i32min(int32_t a, int32_t b)
{
	return (a<b)?a:b;
}
int32_t i32clip(int32_t a, int32_t n, int32_t b)
{
	return i32max(a, i32min(n, b));
}
uint32_t pow2round(uint32_t n)
{
	if(n<=1)
		return 1;
	else
		return pow2round(n>>1)<<1;
}
void byte2hex(char* out, uint8_t b)
{
	uint8_t n1 = b/16;
	uint8_t n2 = b%16;
	out[0] = (n1<10)?(n1+'0'):(n1+'A'-10);
	out[1] = (n2<10)?(n2+'0'):(n2+'A'-10);
}
