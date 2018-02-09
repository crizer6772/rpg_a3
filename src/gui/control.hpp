#pragma once
#include "../incl_full.hpp"
#include "../hlpfunc.hpp"
#include "../console/console.hpp"

#define GW_CONTROL_MAX_NAME_SIZE 32
#define GW_CONTROL_MAX_PARAMS 32

typedef (*GWControlRenderFunc)(GWControl*, GDWindow*, GameDesktop*, ALLEGRO_DISPLAY*);
typedef (*GWControlEventFunc)(GWControl*, GDWindow*, GameDesktop*, ALLEGRO_EVENT*);

class GWControlType
{
public:
	char name[GW_CONTROL_MAX_NAME_SIZE];
	float x,y,w,h;
	GWControlRenderFunc RenderFunc;
	GWControlEventFunc EventFunc;
};

class GWControl
{
public:

};
