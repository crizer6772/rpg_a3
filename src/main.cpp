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

	GameConsole* MainGC = new GameConsole;
	DisplayMgr* MainDM = new DisplayMgr;
	LanguageMgr* MainLM = new LanguageMgr;
	MainGC->CreateCVar("sqrt2", "1.414213562373095");
	MainGC->CreateCVar("pi", "3.141592653589793");
	MainGC->CreateCVar("e", "2.718281828459045");
	MainGC->CreateCVar("i1", "273");
	MainGC->CreateCVar("i2", "38");
	MainGC->CreateCVar("a", "ABC");

	RegisterStandardCommands(MainGC);

	//MainGC->ExecuteCommand("echo ABC(hex)=0x%H3:a; sqrt(2)=%V:sqrt2; %V:i1;+%V:i2;=%R:sum %V:i1; %V:i2;;");
	MainGC->ExecuteCommand("importcfg config-new.ini");

	MainDM->SetVideoMode(MainGC->GetCVarI32("DM_ScrWidth"), MainGC->GetCVarI32("DM_ScrHeight"),
						MainGC->GetCVarI32("DM_FullscreenMode"), MainGC->GetCVarI32("DM_RefreshRate"),
						MainGC->GetCVarI32("DM_ColorDepth"));


	ResourceMgr* MainRM = new ResourceMgr;
	MainRM->SetReleaseTime(MainGC->GetCVarUI32("RM_ReleaseTime"));
	MainRM->SetNoReleaseThreshold(MainGC->GetCVarI32("RM_NoReleaseThreshold"));

	MainLM->LoadLanguageList("lang/langlist.ini");
	MainLM->SelectLanguage(MainGC->GetCVar("LM_Language"));

	MainGC->LinkToDisplayMgr(MainDM);
	MainGC->LinkToLanguageMgr(MainLM);
	MainGC->LinkToResourceMgr(MainRM);
	MainGC->LogBufAlloc(80,25,0);

	MainGC->LogLine("test0",0);
	MainGC->LogLine("test1",0);
	MainGC->LogLine("test2",0);
	MainGC->LogLine("test3",0);

	int bmark;
	double btime=al_get_time();
	for(bmark=0;;bmark++)
	{
		if(!(bmark%768)) if(al_get_time()-btime>=1.0) break;
		MainGC->ExecuteCommand("echo ABC(hex)=0x$H6[a] sqrt(2)=$V[sqrt2] $V[i1]+$V[i2]=$R[sum $V[i1] $V[i2]]");
		//MainGC->ExecuteCommand("echo $V[sqrt2]");
	}
	printf("%d commands per second\n",bmark);

	MainGC->ExecuteCommand("echo ABC(hex)=0x$H6[a] sqrt(2)=$F6[sqrt2] $V[i1]+$V[i2]=$R0[sum $V[i1] $V[i2]] test=$L[TEXT_TEST]");
	al_clear_to_color(al_map_rgb(255,255,255));
	al_rest(0.1);
	bool state = 0;
	for(uint64_t tick=0;;tick++)
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
		al_draw_scaled_bitmap(tb,0,0,bw,bh,0,0,MainGC->GetCVarI32("DM_ScrWidth"), MainGC->GetCVarI32("DM_ScrHeight"),0);
		ALLEGRO_FONT* rf = MainRM->GetFont("data/fonts/nsans.ttf", 26+10*sin(al_get_time()));
		ALLEGRO_FONT* con = MainRM->GetFont("data/fonts/rmono.ttf", 12);
		al_draw_text(rf, al_map_rgb(255,255,255), 10, 10, 0, MainLM->GetString("TEXT_TEST"));
		for(int i=0; i<MainGC->cLogBufH; i++)
		{
			char m[] = "null";
			char* l = MainGC->LogGetLineUTF8(i);
			if(!l) l=m;
			al_draw_text(con, al_map_rgb(255,255,255), 10, 60+12*i, 0, l);
		}

		if(tick%32==0)
		{
			char buf[80];
			sprintf(buf,"tick: %d ",(uint32_t)tick);
			MainGC->LogLine(buf,0);
		}

		al_flip_display();
		MainRM->ReleaseUnusedResources();

		if(al_key_down(&s, ALLEGRO_KEY_F7))
		{
			MainGC->SetCVar("DM_ScrWidth", 1024);
			MainGC->SetCVar("DM_ScrHeight", 768);
			MainDM->SetVideoMode(MainGC->GetCVarI32("DM_ScrWidth"), MainGC->GetCVarI32("DM_ScrHeight"),
						MainGC->GetCVarI32("DM_FullscreenMode"), MainGC->GetCVarI32("DM_RefreshRate"),
						MainGC->GetCVarI32("DM_ColorDepth"));
			MainRM->ReloadAllResources();
		}
	}
}
