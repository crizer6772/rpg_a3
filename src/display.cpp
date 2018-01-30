#include "display.hpp"

DisplayMgr::DisplayMgr()
{
	d = NULL;
}

DisplayMgr::~DisplayMgr()
{

}

bool DisplayMgr::VModeAvailable(int w, int h, int fsmode, int rr, int cDepth)
{
	if(fsmode != DM_FSMODE_FULLSCREEN)
	{
		return true;
	}
	bool res = false;
	int vmodes = al_get_num_display_modes();
	ALLEGRO_DISPLAY_MODE dm;
	for(int i=0; i<vmodes; i++)
	{
		al_get_display_mode(i, &dm);
		if(dm.width == w && dm.height == h && dm.refresh_rate == rr && al_get_pixel_size(dm.format)*8 == cDepth)
		{
			res = true;
		}
	}
	return res;
}

bool DisplayMgr::SetVideoMode(int w, int h, int fsmode, int rr, int cDepth)
{
	al_set_new_display_flags(ALLEGRO_DIRECT3D);
	if(d)
	{
		al_destroy_display(d);
	}
	if(!VModeAvailable(w, h, fsmode, rr, cDepth))
	{
		d = al_create_display(800, 600);
	}
	int flags = 0;

	flags |= ALLEGRO_DIRECT3D;

	if(fsmode == DM_FSMODE_FULLSCREEN)
		flags |= ALLEGRO_FULLSCREEN;

	if(fsmode == DM_FSMODE_FSWINDOW)
		flags |= ALLEGRO_FULLSCREEN_WINDOW;

	al_set_new_display_flags(flags);
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, cDepth, ALLEGRO_REQUIRE);
	al_set_new_display_refresh_rate(rr);

	d = al_create_display(w,h);

	if(!d)
	{
		al_set_new_display_option(ALLEGRO_COLOR_SIZE, 32, ALLEGRO_SUGGEST);
		al_set_new_display_refresh_rate(0);
		d = al_create_display(800, 600);
		return (bool)d;
	}
	else
	{
		return true;
	}
}
DisplayMgr MainDM;
