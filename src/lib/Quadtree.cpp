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

#ifndef QUADTREE_CPP
#define QUADTREE_CPP

#include "Quadtree.h"

template < class T >
void QuadtreeNode<T>::Destroy()
{
	if( node[0][0] != NULL )
	{
		node[0][0] -> Destroy();
		delete node[0][0];
		node[0][0] = NULL;
	}
	if( node[0][1] != NULL )
	{
		node[0][1] -> Destroy();
		delete node[0][1];
		node[0][1] = NULL;
	}
	if( node[1][0] != NULL )
	{
		node[1][0] -> Destroy();
		delete node[1][0];
		node[1][0] = NULL;
	}
	if( node[1][1] != NULL )
	{
		node[1][1] -> Destroy();
		delete node[1][1];
		node[1][1] = NULL;
	}
	if( data != NULL )
	{
		delete data;
		data = NULL;
	}
}

template < class T >
int QuadtreeNode<T>::GetNodeNumber()
{
	int i = 1;
	if( node[0][0] != NULL )
	{
		i += (node[0][0] -> GetNodeNumber());
	}
	if( node[0][1] != NULL )
	{
		i += (node[0][1] -> GetNodeNumber());
	}
	if( node[1][0] != NULL )
	{
		i += (node[1][0] -> GetNodeNumber());
	}
	if( node[1][1] != NULL )
	{
		i += (node[1][1] -> GetNodeNumber());
	}
	return i;
}

template < class T >
QuadtreeNode<T>::QuadtreeNode()
{
	node[0][0] = NULL;
	node[0][1] = NULL;
	node[1][0] = NULL;
	node[1][1] = NULL;
	data = NULL;
}

template < class T >
QuadtreeNode<T>::~QuadtreeNode()
{
	Destroy();
}

template < class T >
inline bool Quadtree<T>::Exist( const quadtreePosDataType _x, const quadtreePosDataType _y )
{
	defaultValue = defaultValueCopy;
	quadtreePosDataType i, fx, fy;
	
	QuadtreeNode<T> * currentNode = &node;
	
	for( i = 0; i < lvl; ++i )
	{
		fx = quadtreePosDataType(_x>>(lvl-i))&1;
		fy = quadtreePosDataType(_y>>(lvl-i))&1;
		if( currentNode -> node[fx][fy] != NULL )
		{
			currentNode = currentNode -> node[fx][fy];
		}
		else
		{
			return false;
		}
	}
	
	if( currentNode -> data != NULL )
	{
		return true;
	}
	
	return false;
}

template < class T >
inline void Quadtree<T>::Set( const quadtreePosDataType _x, const quadtreePosDataType _y, const T& src )
{
	defaultValue = defaultValueCopy;
	quadtreePosDataType i, fx, fy;
	
	QuadtreeNode<T> * currentNode = &node;
	
	for( i = 0; i < lvl; ++i )
	{
		fx = quadtreePosDataType(_x>>(lvl-i))&1;
		fy = quadtreePosDataType(_y>>(lvl-i))&1;
		if( currentNode -> node[fx][fy] == NULL )
		{
			currentNode -> node[fx][fy] = new QuadtreeNode<T>;
		}
		currentNode = currentNode -> node[fx][fy];
	}
	
	if( currentNode -> data == NULL )
	{
		currentNode -> data = new T;
	}
	
	*(currentNode -> data) = src;
}

template < class T >
inline T& Quadtree<T>::Get( const quadtreePosDataType _x, const quadtreePosDataType _y )
{
	defaultValue = defaultValueCopy;
	quadtreePosDataType i, fx, fy;
	
	QuadtreeNode<T> * currentNode = &node;
	
	for( i = 0; i < lvl; ++i )
	{
		fx = quadtreePosDataType(_x>>(lvl-i))&1;
		fy = quadtreePosDataType(_y>>(lvl-i))&1;
		if( currentNode -> node[fx][fy] != NULL )
		{
			currentNode = currentNode -> node[fx][fy];
		}
		else
		{
			return defaultValue;
		}
	}
	
	if( currentNode -> data == NULL )
	{
		return defaultValue;
	}
	
	return *(currentNode -> data);
}

template < class T >
inline T& Quadtree<T>::GetCreate( const quadtreePosDataType _x, const quadtreePosDataType _y )
{
	defaultValue = defaultValueCopy;
	quadtreePosDataType i, fx, fy;
	
	QuadtreeNode<T> * currentNode = &node;
	
	for( i = 0; i < lvl; ++i )
	{
		fx = quadtreePosDataType(_x>>(lvl-i))&1;
		fy = quadtreePosDataType(_y>>(lvl-i))&1;
		if( currentNode -> node[fx][fy] == NULL )
		{
			currentNode -> node[fx][fy] = new QuadtreeNode<T>;
		}
		currentNode = currentNode -> node[fx][fy];
	}
	
	if( currentNode -> data == NULL )
	{
		currentNode -> data = new T;
	}
	
	return *(currentNode -> data);
}

template < class T >
inline void Quadtree<T>::Erase( const quadtreePosDataType _x, const quadtreePosDataType _y )
{
	defaultValue = defaultValueCopy;
	quadtreePosDataType i, j, k, fx, fy, fast, last = 0, lx = 0, ly = 0;
	
	QuadtreeNode<T> * currentNode = &node;
	QuadtreeNode<T> * lastNode = &node;
	
	for( i = 0; i < lvl; ++i )
	{
		fx = quadtreePosDataType(_x>>(lvl-i))&1;
		fy = quadtreePosDataType(_y>>(lvl-i))&1;
		
		fast = 0;
		fast += ( currentNode->node[0][0] != NULL ) ? 1 : 0;
		fast += ( currentNode->node[0][1] != NULL ) ? 1 : 0;
		fast += ( currentNode->node[1][0] != NULL ) ? 1 : 0;
		fast += ( currentNode->node[1][1] != NULL ) ? 1 : 0;
		if( fast > 1 )
		{
			last = i;
			lx = fx;
			ly = fy;
			lastNode = currentNode;
		}
		
		if( currentNode -> node[fx][fy] != NULL )
		{
			currentNode = currentNode -> node[fx][fy];
		}
		else
		{
			currentNode = NULL;
			break;
		}
	}
	
	if( lastNode != NULL )
	{
		if( lastNode -> node[lx][ly] != NULL )
		{
			lastNode -> node[lx][ly] -> Destroy();
			delete lastNode -> node[lx][ly];
			lastNode -> node[lx][ly] = NULL;
		}
	}
}

template < class T >
inline quadtreePosDataType Quadtree<T>::GetNumberOfNodes()
{
	return node.GetNodeNumber();
}

template < class T >
inline quadtreePosDataType Quadtree<T>::GetSpaceSizeAxes()
{
	return ((quadtreePosDataType)(1))<<lvl;
}

template < class T >
inline void Quadtree<T>::Clear()
{
	node.Destroy();
}

template < class T >
inline void Quadtree<T>::Init( const quadtreePosDataType levels_, const T defaultValue_ )
{
	lvl = levels_;
	defaultValue = defaultValue_;
	defaultValueCopy = defaultValue;
}

template < class T >
Quadtree<T>::Quadtree()
{
	memset( &defaultValue, 0, sizeof(T) );
	lvl = 1;
}

template < class T >
Quadtree<T>::~Quadtree()
{
	memset( &defaultValue, 0, sizeof(T) );
	node.Destroy();
	lvl = 0;
}

#endif

