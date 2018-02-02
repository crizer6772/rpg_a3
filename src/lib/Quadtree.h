/*
	Copyright (C) 2017 Marek Zalewski
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.
	
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.
	2.	Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source distribution.
*/

#ifndef QUADTREE_H
#define QUADTREE_H

#include <cstring>
#include <cstdio>

typedef int quadtreePosDataType;
#define QUADTREE_PRINTF_SPECIFIER %d

template < class T >
class QuadtreeNode
{
public:
	QuadtreeNode<T> * node[2][2];
	T * data;
	
	void Destroy();
	
	int GetNodeNumber();
	
	QuadtreeNode();
	~QuadtreeNode();
};

template < class T >
class Quadtree
{
public:
	QuadtreeNode<T> node;
	T defaultValue;
	T defaultValueCopy;
	int lvl;
	
	inline bool Exist( const quadtreePosDataType _x, const quadtreePosDataType _y );
	inline void Set( const quadtreePosDataType _x, const quadtreePosDataType _y, const T& src );
	inline T& Get( const quadtreePosDataType _x, const quadtreePosDataType _y );
	inline T& GetCreate( const quadtreePosDataType _x, const quadtreePosDataType _y );		// creat if not exist
	inline void Erase( const quadtreePosDataType _x, const quadtreePosDataType _y );
	
	inline quadtreePosDataType GetNumberOfNodes();
	
	inline quadtreePosDataType GetSpaceSizeAxes();
	
	inline void Clear();
	
	inline void Init( const quadtreePosDataType levels_, const T defaultValue_ );
	
	Quadtree();
	~Quadtree();
};

#endif

