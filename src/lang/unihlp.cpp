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
	//fuck it i'm not checking for buffer overruns who gives a shit
	size_t s = 0;
	for(size_t i=0;in[i];i++)
		s += CPtoUTF8((out)?out+s:out, in[i]);
	return s;
}

size_t UTF8ToUTF32(uint32_t* out, char* str)
{
	size_t r = 0;
	size_t l = strlen(str);
    for(size_t i=0; i<l; i++)
	{
		uint8_t c = str[i];
		uint32_t cp = 0;
		if(c&0x80) //leading byte
		{
			size_t bGroupSizeX=0, bGroupSize=1;
			while(c & (0x80>>bGroupSizeX)) 		bGroupSizeX++; 	//expected byte group size
			while(str[i+bGroupSize++]&0xD0==0x80);				//actual byte group size
			if(bGroupSizeX != bGroupSize)
				return 0;
			if(bGroupSize < 2 || bGroupSize > 4)
				return 0;
			uint32_t cprStart = 0;
			switch(bGroupSize)
			{
				case 2: cprStart = 0x00000080; break;
				case 3: cprStart = 0x00000800; break;
				case 4: cprStart = 0x00010000; break;
			}
			cp = (c&(0xFF>>(bGroupSize+1))) << (6*(bGroupSize-1));
			for(uint32_t j=1; j<bGroupSize; j++)
				cp += (str[i+j]&0x3F) << (6*(bGroupSize-j-1));
			if(cp < cprStart) //overlong encoding
				return 0;
			if(cp >= 0xD800 && cp <= 0xDFFF)
				return 0;
			i += bGroupSize;
		}
		else
		{
			cp = c;
		}
		if(out)
			out[r] = cp;
		r++;
	}
	return r*sizeof(uint32_t);
}

bool IsValidUTF8(char* str)
{
	return UTF8ToUTF32(NULL, str);
}

uint32_t UTF8ByteGroupCodepoint(char* in)
{
	if(!in) return 0;
	uint32_t res = 0;
	char bGroup[5] = {0,0,0,0,0};
	for(int i=0; i<4 && in[i]; i++)
		bGroup[i] = in[i];
	for(int i=0; i<4; i++)
		if((uint8_t)bGroup[i]<128 || (i!=0&&bGroup[i]&0xD0))
			bGroup[i+1] = 0;
	UTF8ToUTF32(&res, bGroup);
	return res;
}
