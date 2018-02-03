/*
This source code is part of Integrated Libraries project

Copyright (C) 2018 Marek Zalewski

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#ifndef FILE_CPP
#define FILE_CPP

#include <cstdio>
#include "String.cpp"

class File
{
private:
	
	FILE * file;
	String filename;
	
public:
	
	inline bool open( const char * filename, const char * format )
	{
		close();
		this->filename = filename;
		file = fopen( filename, format );
		return (bool)file;
	}
	
	inline void close()
	{
		if( file )
		{
			fclose( file );
			file = NULL;
			filename = "";
		}
	}
	
	inline bool good()
	{
		return (bool)file;
	}
	
	inline unsigned long long int getfilesize()
	{
		if( file )
		{
			unsigned long long int size, temp;
			temp = ftell( file );
			fseek( file, 0L, SEEK_END );
			size = ftell( file );
			fseek( file, temp, SEEK_SET );
			return size;
		}
		return 0;
	}
	
	inline void setpos( const unsigned long long int pos )
	{
		if( file )
		{
			unsigned long long int filesize = getfilesize();
			if( pos >= filesize )
			{
				fseek( file, 0L, SEEK_END );
			}
			else
			{
				fseek( file, pos, SEEK_SET );
			}
		}
	}
	
	inline unsigned long long int getpos() const
	{
		if( file )
		{
			return ftell( file );
		}
		return 0;
	}
	
	template < class T >
	inline unsigned long long int read( T * data, const unsigned long long int maxlen )
	{
		if( file )
		{
			memset( data, 0, maxlen*sizeof(T) );
			return (unsigned long long int)fread( data, sizeof(T), maxlen, file );
		}
		return 0;
	}
	
	template < class T >
	inline bool write( const T * data, const unsigned long long int count )
	{
		if( file )
		{
			fwrite( data, sizeof(T), count, file );
		}
		return true;
	}
	
	inline bool write( const String& str )
	{
		if( file )
		{
			fwrite( str.c_str(), str.size(), sizeof(char), file );
		}
		return true;
	}
	
	void getline( String& dst )
	{
		if( file )
		{
			dst = "";
			char sign;
			while( true )
			{
				sign = 0;
				fread( &sign, sizeof(char), 1, file );
				if( sign == 0 || sign == '\n' )
					break;
				dst += sign;
			}
		}
	}
	
	void getword( String& dst )
	{
		if( file )
		{
			char sign = 1;
			while( ( (unsigned char)sign <= ' ' || sign == 127 || (unsigned char)sign == 255 ) && sign != 0 )
			{
				sign = 0;
				fread( &sign, sizeof(char), 1, file );
			}
			dst = String(sign);
			while( true )
			{
				sign = 0;
				fread( &sign, sizeof(char), 1, file );
				if( (unsigned char)sign <= ' ' || sign == 127 || (unsigned char)sign == 255 )
					break;
				dst += sign;
			}
		}
	}
	
	inline bool eof() const
	{
		if( file )
			return feof( file );
		return false;
	}
	
	double getfloat()
	{
		if( file )
		{
			long long int pos = 0;
			String str;
			while( true )
			{
				getword( str );
				if( str[pos] == '-' )
					++pos;
				if( str[pos] >= '0' && str[pos] <= '9' )
				{
					while( str[pos] >= '0' && str[pos] <= '9' )
					{
						++pos;
					}
					
					if( str[pos] == '.' )
					{
						++pos;
						while( str[pos] >= '0' && str[pos] <= '9' )
						{
							++pos;
						}
					}
					
					str[pos] = 0;
					
					return atof( str.c_str() );
				}
				if( eof() )
					break;
			}
		}
		return 0;
	}
	
	long long int getll()
	{
		if( file )
		{
			long long int val = 0;
			long long int pos = 0;
			String str;
			while( true )
			{
				pos = 0;
				getword( str );
				if( str[pos] == '-' )
					++pos;
				if( str[pos] >= '0' && str[pos] <= '9' )
				{
					while( str[pos] >= '0' && str[pos] <= '9' )
					{
						val *= 10;
						val += (long long int)(str[pos] - '0');
						++pos;
					}
					if( str[0] == '-' )
						return val * -1;
					else
						return val;
				}
				if( eof() )
					break;
			}
		}
		return 0;
	}
	
	unsigned long long int getull()
	{
		if( file )
		{
			unsigned long long int val = 0;
			long long int pos = 0;
			String str;
			while( true )
			{
				pos = 0;
				getword( str );
				if( str[pos] >= '0' && str[pos] <= '9' )
				{
					while( str[pos] >= '0' && str[pos] <= '9' )
					{
						val *= 10;
						val += (unsigned long long int)(str[pos] - '0');
						++pos;
					}
					return val;
				}
				if( eof() )
					break;
			}
		}
		return 0;
	}
	
	inline char getchar()
	{
		char sign = 0;
		if( file )
		{
			fread( &sign, sizeof(char), 1, file );
		}
		return sign;
	}
	
	inline void putchar( const char src )
	{
		if( file )
		{
			fwrite( &src, sizeof(char), 1, file );
		}
	}
	
	File()
	{
		file = NULL;
	}
	
	File( const char * filename, const char * format )
	{
		open( filename, format );
	}
	
	~File()
	{
		close();
	}
};

#endif

