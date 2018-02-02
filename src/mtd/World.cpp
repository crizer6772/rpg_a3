
#ifndef WORLD_CPP
#define WORLD_CPP

#include "../css/World.h"


void World::GetDeltaTime() const
{
	return this->deltaTime;
}

void World::Init( const vec2& offset, const vec2& size )
{
	this->colliderOffset = offset;
	this->colliderSize = size;
}

inline void World::UpdatePhysics( const Actor* actor )
{
	
}

void World::UpdateCollider()
{
	this->collider.Clear();
	int bx, by, ex, ey, x, y;
	for( auto it = this->actor.begin(); it < this->actor.end(); ++it )
	{
		if( it->val )
		{
			bx = (it->val->pos.x - it->val->size.x)/5.0f;
			by = (it->val->pos.y - it->val->size.y)/5.0f;
			ex = (it->val->pos.x + it->val->size.x)/5.0f;
			ey = (it->val->pos.y + it->val->size.y)/5.0f;
			for( x = bx; x <= ex; ++x )
			{
				for( y = by; y <= ey; ++y )
				{
					this->collider.GetCreate( x, y ).push_back( it->val );
				}
			}
		}
		else
		{
			this->actor.erase( it );
			--it;
		}
	}
}

void World::UpdateActors()
{
	this->UpdateCollider();
	
	for( auto it = actor.begin(); it < actor.end(); ++it )
	{
		it->val->Tick();
	}
	
	for( auto it = actor.begin(); it < actor.end(); ++it )
	{
		this->UpdatePhysics( it->val );
	}
}

void World::Update( const float deltaTime )
{
	this->deltaTime = deltaTime;
	this->UpdateActors();
}

int World::SpawnActor( const String& name, const Actor* actor )
{
	actor->world = this;
	actor->name = name;
	Actor& temp = this->actor[name];
	if( temp == NULL )
	{
		temp = actor;
		return 1;
	}
	return 0;
}

void World::DeleteActor( const String& name )
{
	this->actor.erase( name );
}

World::World()
{
	this->collider.Init( 10, Array<Actor*,8>() );
}

World::~World()
{
	this->collider.Clear();
}

#endif

