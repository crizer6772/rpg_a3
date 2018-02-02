
#ifndef WORLD_H
#define WORLD_H

#include "../lib/Map.cpp"
#include "../lib/String.h"
#include "../lib/QuadTree.h"

#include "GraphicBody.h"
#include "Actor.h"

class World
{
private:
	
	Map < String, Actor*, 8 > actor;
	Map < String, GraphicBody*, 8 > model;
	
	vec2 colliderOffset;
	vec2 colliderSize;
	Quadtree < Array < Actor*, 8 > > collider;
	
	float deltaTime;
	
public:
	
	inline void GetDeltaTime() const;
	
	void Init( const vec2& offset, const vec2& size );
	
	inline void UpdatePhysics( const Actor* actor );
	void UpdateCollider();
	void UpdateActors();
	void Update( const float deltaTime );
	
	int SpawnActor( const String& name, const Actor* actor );		// return 0 if spawn can not be done
	void DeleteActor( const String& name );
	
	World();
	~World();
};

#endif

