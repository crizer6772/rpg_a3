#pragma once
#include "../incl_full.hpp"
#include "../display/display.hpp"
#include "window.hpp"

class GameDesktop
{
	DisplayMgr* dm;
	ResourceMgr* rm;
	LanguageMgr* lm;
	GameConsole* gc;
	float scale;

	GDWindow* window;
public:
	void Link(DisplayMgr* dm, ResourceMgr* rm, LanguageMgr* lm, GameConsole* gc);
	void SetScale(float scale);
	void SendEvent(ALLEGRO_EVENT* ev);

	float GetScale();

	GDWindow* GetWindow(const char* name);
	bool DestroyWindow(const char* name);
	bool CreateWindow(const char* name, float x, float y, float w, float h, uint16_t style);
};
