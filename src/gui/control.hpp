#pragma once
#include "../incl_full.hpp"
#include "../hlpfunc.hpp"
#include "../console/console.hpp"

#define GW_CONTROL_MAX_NAME_SIZE 32
#define GW_CONTROL_MAX_PARAMS 32

#define GW_CONTROL_ALIGN_X_LEFT 	0
#define GW_CONTROL_ALIGN_X_CENTER 	1
#define GW_CONTROL_ALIGN_X_RIGHT 	2

#define GW_CONTROL_ALIGN_Y_TOP 		0
#define GW_CONTROL_ALIGN_Y_CENTER 	1
#define GW_CONTROL_ALIGN_Y_BOTTOM 	2

typedef (*GWControlRenderFunc)(GWControl*, GDWindow*, GameDesktop*);
typedef (*GWControlEventFunc)(GWControl*, GDWindow*, GameDesktop*, ALLEGRO_EVENT*);

class GWControlType
{
public:
	char name[GW_CONTROL_MAX_NAME_SIZE];
	GWControlRenderFunc RenderFunc;
	GWControlEventFunc EventFunc;
};

class GWControl
{
public:
	float x,y,w,h;
	int x_align, y_align;
	void Resize(float w, float h);
	void SetPosition(float x, float y, int x_align, int y_align)
};
