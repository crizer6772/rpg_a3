#pragma once
#include "../incl_full.hpp"
#include "../display/display.hpp"
#include "control.hpp"

class GDWindow
{
	GWControl* control;
	ALLEGRO_BITMAP* bmp;
	bool redraw_requested;
public:
	float x,y,w,h;
	bool Init(GameDesktop* desktop, float x, float y, float w, float h);
	bool Redraw();
	bool ForceRedraw();
	bool SendEvent(ALLEGRO_EVENT* ev);
	bool CreateControl(const char* type, const char* name);
	bool RemoveControl(const char* name);
	bool SetControlParam();
};
