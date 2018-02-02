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

#ifndef QUEUE_CPP
#define QUEUE_CPP

#include "Array.cpp"

template < class T, int ARRAY_SHIFT_BIT_FULL_SIZE, int BEGIN_POINT_MAX_OFFSET_SHIFT_BIT >
class Queue
{
private:
	
	Array < T, ARRAY_SHIFT_BIT_FULL_SIZE > data;
	unsigned long long int curoffset;
	
public:
	
	inline T * begin() const
	{
		return data.begin()+curoffset;
	}
	
	inline T * end() const
	{
		return data.end();
	}
	
	inline long long int size() const
	{
		return data.size()-curoffset;
	}
	
	template < int a__, int b__ >
	inline T& operator = ( const Queue < T, a__, b__ > & src )
	{
		curoffset = 0;
		data.resize( src.data.size()-src.curoffset );
		memcpy( data.begin(), src.data.begin(), data.size() );
	}
	
	inline T& operator [] ( const unsigned long long int id )
	{
		return data[curoffset+id];
	}
	
	inline T& front()
	{
		return data[curoffset];
	}
	
	inline T& back()
	{
		return data.back();
	}
	
	inline void push( const T& src )
	{
		data.push_back( src );
	}
	
	inline T pop()
	{
		T dst = data[curoffset];
		++curoffset;
		if( curoffset >= (1<<BEGIN_POINT_MAX_OFFSET_SHIFT_BIT) )
		{
			data.erase( 0, curoffset );
			curoffset = 0;
		}
		return dst;
	}
	
	inline void reserve_mem( const unsigned long long int size )
	{
		data.reserve_mem( size );
	}
	
	inline void shrink_to_fit()
	{
		data.shrink_to_fit();
	}
	
	inline void clear()
	{
		data.clear();
	}
	
	Queue()
	{
		data.clear();
		curoffset = 0;
	}
};

#endif

