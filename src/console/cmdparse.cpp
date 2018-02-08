#include "console.hpp"
#include "../hlpfunc.hpp"

bool GameConsole::IsRTTypeValid(char* rttype)
{
	size_t l = strlen(rttype);
	bool v = false;
	bool n = true;
	char ts[] = {'f','v','h','r','l'};
	if(l>3 || !l)
		return false;
	char f = rttype[0];
	if(f>='A'&&f<='Z') f+=32;
	for(size_t i=0; i<sizeof(ts); i++)
		if(f==ts[i]) v=true;
	for(size_t i=1; i<l; i++)
		if(rttype[i]<'0'||rttype[i]>'9') n=false;
	return v&&n;
}
int32_t GameConsole::ParseReplacementToken(char* out, char type, int param, const char* value)
{
	ConsoleVariable* cv = FindCVar(value);
	size_t vl = strlen(value);
	char* dot = NULL;
	if(!value || !vl)
		return -1;
	if(!out)
	{
		switch(type)
		{
		case 'f':
		case 'F':
			if(!cv)
				return -1;
			dot = strchr(cv->data, '.');
			if(!dot)
				return strlen(cv->data)+param;
			return param+1+(dot - cv->data);
			break;
		case 'v':
		case 'V':
			if(!cv)
				return -1;
			return strlen(cv->data);
			break;
		case 'h':
		case 'H':
			if(!cv || !param)
				return -1;
			return param;
			break;
		case 'r':
		case 'R':
			if(param == 0)
				return 8;
			return 11;
			break;
		case 'l':
		case 'L':
			if(!lm)
				return vl;
			return strlen(lm->GetString(value));
			break;
		}
	}
	else
	{
		char bufv[12]={0,0,0,0,0,0,0,0,0,0,0,0};
		char buff[128];
		int32_t rv;
		switch(type)
		{
		case 'f':
		case 'F':
			memset(buff,0,128);
			sprintf(buff,"%c.%df", '%', param);
			sprintf(out, buff, atof(cv->data));
			break;
		case 'v':
		case 'V':
			strcat(out,cv->data);
			break;
		case 'h':
		case 'H':
			for(int i=0; i<param; i++)
			{
				uint8_t n = cv->data[i/2];
				if(i&1)
					n = n&0x0F;
				else
					n = n>>4;
				char buf[2]={0,0};
				if(n<10)
					buf[0] = n+'0';
				else
					buf[0] = n+'A';
				strcat(out,buf);
			}
			break;
		case 'r':
		case 'R':
			rv = ExecuteCommand(value);
			if(param == 0)
				sprintf(bufv, "%08X", rv);
			else
				sprintf(bufv, "%d", rv);
			strcat(out,bufv);
			break;
		case 'l':
		case 'L':
			if(!lm)
				strcat(out,value);
			else
				strcat(out,lm->GetString(value));
			break;
		}
		return strlen(out);
	}
	return -1;
}
int32_t GameConsole::ParseReplacementTokens(char* out, const char* str)
{
	/**
	Available replacement tokens:
	$F(n) - float with n decimals (defaults to 6)
	$V - console variable value
	$H(n) - hexadecimal value, n digits (big endian)
	$R - return value of a console command (int32)
	$Rh - return value of a console command, in 8 hex digits
	$L - string from linked langmgr
	**/
	size_t cmdlen = strlen(str);
	size_t newsize = 0;
	uint32_t pLevel = 0;
	int32_t d = -1;
	for(size_t i=0; i<cmdlen; i++)
	{
		if(d>=0)
		{
			int32_t td = d;
			d = -1;
			if(str[i] == '$')
			{
				char dd[3]={'$','$',0};
				memcpy(out+newsize,dd,3);
				newsize += 2;
				continue;
			}
			char rttype[4]={0,0,0,0};
			for(size_t j=0; j<3&&j<cmdlen&&str[j+i]!='[';j++)
				rttype[j] = str[j+i];
			size_t rts = strlen(rttype);
			i += rts;
			if(!IsRTTypeValid(rttype) || str[i] != '[')
			{
				memcpy(&out[newsize], &str[td], i-td);
				newsize += i-td;
				i--;
				continue;
			}

			size_t rtStart = i;
			size_t rtEnd;

			for(rtEnd=i; rtEnd<cmdlen; rtEnd++)
			{
				if(str[rtEnd]=='[') pLevel++;
				if(str[rtEnd]==']') pLevel--;
				if(pLevel == 0)
				{
					break;
				}
			}
			if(rtEnd==cmdlen) rtEnd--;
			if(pLevel)
			{
				size_t s = rtEnd-td+1;
				if(out)
					memcpy(&out[newsize], &str[td], s);
				newsize += s;
				return newsize+1;
			}
			if(rtEnd-rtStart < 2)
			{
				size_t s = rtEnd-td+1;
				if(out)
					memcpy(&out[newsize], &str[td], s);
				newsize += s;
				i = rtEnd;
				continue;
			}
			int param = -1;
			if(rts>1)
				param = atoi(&rttype[1]);
			else
			{
				switch(rttype[0])
				{
				case 'f':
				case 'F':
					param = 6;
					break;
				case 'h':
				case 'H':
					param = 8;
					break;
				}
			}

			size_t rtvs = rtEnd-rtStart-1;
			char rtv[rtvs+1];
			rtv[rtvs] = 0;
			memcpy(rtv,&str[rtStart+1],rtvs);
			int32_t pr = ParseReplacementToken(NULL, rttype[0], param, rtv);
			if(pr == -1)
			{
				size_t s = rtEnd-td+1;
				if(out)
					memcpy(&out[newsize], &str[td], s);
				newsize += s;
				i = rtEnd;
				continue;
			}
			else
			{
				int32_t r2;
				if(out)
				{
					r2=ParseReplacementToken(&out[newsize], rttype[0], param, rtv);
				}
				else
					r2=pr;
				newsize += r2;
				i = rtEnd;
				continue;
			}
		}
		if(str[i] == '$')
			d = i;
		else
		{
			d = -1;
			if(out)
				out[newsize] = str[i];
			newsize++;
		}
	}
	return newsize+1;
}
