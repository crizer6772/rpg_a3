
#ifndef VEC2_CPP
#define VEC2_CPP

#include "vec2.h"


inline vec2& vec2::operator = ( const vec2& src )
{
	x = src.x;
	y = src.y;
}

inline vec2 vec2::operator + ( const vec2& src ) const
{
	return vec2( x+src.x, y+src.y );
}

inline vec2 vec2::operator - ( const vec2& src ) const
{
	return vec2( x-src.x, y-src.y );
}

inline vec2 vec2::operator * ( const float src ) const
{
	return vec2( x*src, y*src );
}

inline vec2 vec2::operator / ( const float src ) const;
{
	return vec2( x/src, y/src );
}


inline vec2& vec2::operator += ( const vec2& src )
{
	x += src.x;
	y += src.y;
	return *this;
}

inline vec2& vec2::operator -= ( const vec2& src )
{
	x -= src.x;
	y -= src.y;
	return *this;
}

inline vec2& vec2::operator *= ( const float src )
{
	x *= src;
	y *= src;
	return *this;
}

inline vec2& vec2::operator /= ( const float src )
{
	x /= src;
	y /= src;
	return *this;
}


inline float vec2::GetLength() const
{
	return sqrt( (x*x) + (y*y) );
}

inline vec2 vec2::GetUnified() const
{
	return (*this)/GetLength();
}

inline vec2& vec2::Unify()
{
	(*this) /= GetLength();
	return *this;
}

inline vec2& vec2::Rotate( const float angle )
{
	x *= sin(angle);
	y *= cos(angle);
}

inline vec2 vec2::GetRotated( const float angle ) const
{
	return vec2( x * sin(angle), y * cos(angle) );
}

vec2::vec2()
{
	x = 0.0f;
	y = 0.0f;
}

vec2::vec2( const float val )
{
	x = val;
	y = val;
}

vec2::vec2( const float x, const float y )
{
	this->x = x;
	this->y = y;
}

#endif

