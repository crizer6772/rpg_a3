#pragma once
#include "../incl_full.hpp"

#define DM_FSMODE_WINDOWED 0
#define DM_FSMODE_FULLSCREEN 1
#define DM_FSMODE_FSWINDOW 2


class DisplayMgr
{
	bool VModeAvailable(int w, int h, int fsMode, int rr, int cDepth);
public:
	DisplayMgr();
	~DisplayMgr();

	ALLEGRO_DISPLAY* d;

	bool SetVideoMode(int w, int h, int fsMode, int rr, int cDepth);
};
