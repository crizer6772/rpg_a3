
#ifndef VEC2_H
#define VEC2_H

#include <cmath>

class vec2
{
public:
	
	float x, y;
	
public:
	
	inline vec2& operator = ( const vec2& src );
	
	inline vec2 operator + ( const vec2& src ) const;
	inline vec2 operator - ( const vec2& src ) const;
	inline vec2 operator * ( const float src ) const;
	inline vec2 operator / ( const float src ) const;
	
	inline vec2& operator += ( const vec2& src );
	inline vec2& operator -= ( const vec2& src );
	inline vec2& operator *= ( const float src );
	inline vec2& operator /= ( const float src );
	
	inline float GetLength() const;
	inline vec2 GetUnified() const;		// get unitary vector
	inline vec2& Unify();				// make as unitary vector
	
	inline vec2& Rotate( const float angle );
	inline vec2 GetRotated( const float angle ) const;
	
	vec2();
	vec2( const float val );
	vec2( const float x, const float y );
};

#endif

