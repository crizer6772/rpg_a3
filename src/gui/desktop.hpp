#pragma once
#include "../incl_full.hpp"
#include "../display/display.hpp"

class GameDesktop
{
	DisplayMgr* dm;
	ResourceMgr* rm;
	LanguageMgr* lm;
	GameConsole* gc;
	float scale;

	//GDControl* control;
	//size_t numControls;
public:
	void Link(DisplayMgr* dm, ResourceMgr* rm, LanguageMgr* lm, GameConsole* gc);
	void SetScale(float scale);
	void SendEvent(ALLEGRO_EVENT* ev);

	float GetScale();
};
