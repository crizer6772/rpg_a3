#pragma once
#include "../incl_full.hpp"
#include "../display/display.hpp"
#include "control.hpp"

class GDWindow
{
	GWDesktop* desktop;
	GWControl* control;
	ALLEGRO_BITMAP* bmp;
	bool redraw_requested;
public:
	float x,y,w,h;
	bool Init(GameDesktop* desktop, float x, float y, float w, float h);
	bool Redraw();
	bool ForceRedraw();
	bool SendEvent(ALLEGRO_EVENT* ev);

	GWControl* GetControl(const char* name);
	bool CreateControl(const char* type, const char* name, float x, float y, float w, float h);
	bool RemoveControl(const char* name);
	void Clear();
};
