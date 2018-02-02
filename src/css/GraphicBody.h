
#ifndef GRAPHIC_BODY_H
#define GRAPHIC_BODY_H

#include "../lib/String.h"
#include "../lib/vec2.h"

class GraphicBody
{
private:
	
	String name;
	
public:
	
	void Draw( const vec2& pos, const vec2& size, const float rotation );
	
	GraphicBody();
	~GraphicBody();
};

#endif

