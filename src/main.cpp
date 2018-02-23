#include "incl_full.hpp"
#include "a5init.hpp"
#include "resmgr/resmgr.hpp"
#include "lang/langmgr.hpp"
#include "display/display.hpp"
#include "console/console.hpp"
#include "console/commoncmd.hpp"

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

	int bmark;
	double btime=al_get_time();
	for(bmark=0;;bmark++)
	{
		if(!(bmark%768)) if(al_get_time()-btime>=1.0) break;
		MainGC->ExecuteCommand("echo ABC(hex)=0x$H6[a] sqrt(2)=$V[sqrt2] $V[i1]+$V[i2]=$R[sum $V[i1] $V[i2]]");
		//MainGC->ExecuteCommand("echo $V[sqrt2]");
	}
	MainGC->CreateCVar("benchresult", "0");
	MainGC->SetCVar("benchresult", bmark);
	MainGC->ExecuteCommand("echo $V[benchresult] commands per second");
	MainGC->RemoveCVar("benchresult");


	ALLEGRO_EVENT ev;
	ALLEGRO_EVENT_QUEUE* eq = al_create_event_queue();
	al_register_event_source(eq, al_get_keyboard_event_source());
	uint32_t cmdlinebuf[80];
	memset(cmdlinebuf,0,80*sizeof(uint32_t));
	uint32_t cmdlinecur = 0;

	al_clear_to_color(al_map_rgb(255,255,255));
	al_rest(0.1);
	bool state = 0;
	for(MainGC->CreateCVar("tick", "0");;MainGC->SetCVar("tick", MainGC->GetCVarI32("tick")+1))
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

		int fsize = 24;

		al_draw_scaled_bitmap(tb,0,0,bw,bh,0,0,MainGC->GetCVarI32("DM_ScrWidth"), MainGC->GetCVarI32("DM_ScrHeight"),0);
		ALLEGRO_FONT* rf = MainRM->GetFont("data/fonts/nsans.ttf", 26+10*sin(al_get_time()));
		ALLEGRO_FONT* con = MainRM->GetFont("data/fonts/rmono.ttf", fsize);
		al_draw_text(rf, al_map_rgb(255,255,255), 10, 10, 0, MainLM->GetString("TEXT_TEST"));

		uint32_t con_x1=10,con_y1=60,con_x2=16+al_get_text_width(con,"0")*80,con_y2=fsize*24+60;
		al_draw_filled_rectangle(con_x1,con_y1,con_x2,con_y2,al_map_rgba(63,63,63,127));
		al_draw_filled_rectangle(con_x1,con_y2,con_x2,con_y2+fsize,al_map_rgba(0,0,0,127));
		for(int i=0; i<MainGC->cLogBufH; i++)
		{
			char m[] = "null";
			char* l = MainGC->LogGetLineUTF8(i);
			if(!l) l=m;
			al_draw_text(con, al_map_rgb(255,255,255), 10, 60+fsize*i, 0, l);
		}
		char cmdlinebuf8[320];
		memset(cmdlinebuf8, 0, 320);
		UTF32toUTF8(cmdlinebuf8, cmdlinebuf);
		al_draw_text(con, al_map_rgb(255,255,0), 10, 60+fsize*24, 0, cmdlinebuf8);

		while(al_get_next_event(eq, &ev))
		{
			if(ev.type == ALLEGRO_EVENT_KEY_CHAR)
			{
				if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER && *cmdlinebuf && *cmdlinebuf8)
				{
					MainGC->ExecuteCommand(cmdlinebuf8);
					memset(cmdlinebuf,0,80*sizeof(uint32_t));
					cmdlinecur = 0;
				}
				else if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && cmdlinecur)
					cmdlinebuf[cmdlinecur--] = 0;
				else if(ev.keyboard.keycode == ALLEGRO_KEY_DELETE)
					memmove(&cmdlinebuf[cmdlinecur], &cmdlinebuf[cmdlinecur+1], 79-cmdlinecur);
				else if(cmdlinecur < 79 && ev.keyboard.unichar)
						cmdlinebuf[cmdlinecur++] = (uint32_t)ev.keyboard.unichar;
			}
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
