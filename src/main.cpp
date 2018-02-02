#include "incl_full.hpp"
#include "a5init.hpp"
#include "resmgr/resmgr.hpp"
#include "lang/langmgr.hpp"
#include "display/display.hpp"
#include "console/console.hpp"
#include "console/command.hpp"

int main(int argc, char** argv)
{
	if(!Allegro5FullInit())
		return 0;
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

	GameConsole* test = new GameConsole;
	DisplayMgr* MainDM = new DisplayMgr;
	LanguageMgr* MainLM = new LanguageMgr;
	test->CreateCVar("sqrt2", "1.414213562373095");
	test->CreateCVar("pi", "3.141592653589793");
	test->CreateCVar("e", "2.718281828459045");
	RegisterStandardCommands(test);
	test->ExecuteCommand("importcfg config-new.ini");
	test->ExecuteCommand("echo %V:e;");

	MainDM->SetVideoMode(test->GetCVarI32("DM_ScrWidth"), test->GetCVarI32("DM_ScrHeight"),
						test->GetCVarI32("DM_FullscreenMode"), test->GetCVarI32("DM_RefreshRate"),
						test->GetCVarI32("DM_ColorDepth"));


	ResourceMgr* MainRM = new ResourceMgr;
	MainRM->SetReleaseTime(test->GetCVarUI32("RM_ReleaseTime"));
	MainRM->SetNoReleaseThreshold(test->GetCVarI32("RM_NoReleaseThreshold"));

	MainLM->LoadLanguageList("lang/langlist.ini");
	MainLM->SelectLanguage(test->GetCVar("LM_Language"));
	al_clear_to_color(al_map_rgb(255,255,255));
	al_rest(0.1);
	bool state = 0;
	while(1)
	{
		ALLEGRO_KEYBOARD_STATE s;
		al_get_keyboard_state(&s);
		if(al_key_down(&s, ALLEGRO_KEY_LSHIFT))
			state = 1;
		if(al_key_down(&s, ALLEGRO_KEY_RSHIFT))
			state = 0;
		if(al_key_down(&s, ALLEGRO_KEY_ESCAPE))
			break;
		al_clear_to_color(al_map_rgb(0,0,0));
		ALLEGRO_BITMAP* tb;
		if(state)
			tb = MainRM->GetBitmap("s311.png");
		else
			tb = MainRM->GetBitmap("s2.png");
		int bw = al_get_bitmap_width(tb);
		int bh = al_get_bitmap_height(tb);
		al_draw_scaled_bitmap(tb,0,0,bw,bh,0,0,test->GetCVarI32("DM_ScrWidth"), test->GetCVarI32("DM_ScrHeight"),0);
		al_draw_text(MainRM->GetFont("data/fonts/rmono.ttf", 26+10*sin(al_get_time()*8)), al_map_rgb(255,255,255), 10, 10, 0, MainLM->GetString("TEXT_TEST"));
		al_flip_display();
		MainRM->ReleaseUnusedResources();

		if(al_key_down(&s, ALLEGRO_KEY_F7))
		{
			test->SetCVar("DM_ScrWidth", 1024);
			test->SetCVar("DM_ScrHeight", 768);
			MainDM->SetVideoMode(test->GetCVarI32("DM_ScrWidth"), test->GetCVarI32("DM_ScrHeight"),
						test->GetCVarI32("DM_FullscreenMode"), test->GetCVarI32("DM_RefreshRate"),
						test->GetCVarI32("DM_ColorDepth"));
			MainRM->ReleaseAllResources();
		}
	}
}
