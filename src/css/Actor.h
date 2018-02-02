
#ifndef ACTOR_H
#define ACTOR_H

#include "../lib/String.h"
#include "../lib/vec2.h"
#include "GraphicBody.h"

class World;

class Actor
{
protected:
	
	friend class World;
	
	String name;
	World * world;
	
	vec2 size;				// from barycenter
	vec2 pos, bpos;			// barycentric
	vec2 vel, bvel;
	vec2 force;
	float rotation;			// rotation
	
	GraphicBody * graphic;
	
public:
	
	inline vec2 GetForwardVector() const;
	inline vec2 GetLeftVector() const;
	inline vec2 GetRightVector() const;
	inline vec2 GetBackVector() const;
	
	
	
	void Draw() const;
	
	virtual void Tick()=0;
	
	Actor();
};

#endif

