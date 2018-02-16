#include "unihlp.hpp"

size_t CPtoUTF8(char* out, uint32_t codepoint)
{
	codepoint &= 0x1FFFFF;
    if (codepoint <= 0x7F)
	{
		if(out)
		{
			out[0] = codepoint;
		}
        return 1;
    }
    if (codepoint <= 0x7FF)
    {
    	if(out)
		{
			out[0] = 0xC0 | (codepoint >> 6);
			out[1] = 0x80 | (codepoint & 0x3F);
		}
        return 2;
    }
    if (codepoint <= 0xFFFF)
    {
    	if(out)
		{
			out[0] = 0xE0 | (codepoint >> 12);
			out[1] = 0x80 | ((codepoint >> 6) & 0x3F);
			out[2] = 0x80 | (codepoint & 0x3F);
		}

        return 3;
    }
    if (codepoint <= 0x10FFFF)
    {
    	if(out)
		{
			out[0] = 0xF0 | (codepoint >> 18);
			out[1] = 0x80 | ((codepoint >> 12) & 0x3F);
			out[2] = 0x80 | ((codepoint >> 6) & 0x3F);
			out[3] = 0x80 | (codepoint & 0x3F);
		}
        return 4;
    }
    return 0;
}

size_t UTF32toUTF8(char* out, uint32_t* in)
{
	size_t s = 0;
	for(size_t i=0;in[i];i++)
		s += CPtoUTF8((out)?out+s:out, in[i]);
	return s;
}

uint32_t UTF8ByteGroupCodepoint(char* in)
{
	uint32_t res = 0;
	if(in[0])
	{
		if(in&0x80)
		{
            //TODO
		}
	}
	else
	{
		return 0;
	}
}
