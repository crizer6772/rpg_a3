#include "a5init.hpp"

bool Allegro5FullInit()
{
	char* err = NULL;
	if(!al_init())
		err = (char*)"al_init() failed";
	if(!al_install_keyboard())
		err = (char*)"al_install_keyboard() failed";
	if(!al_install_mouse())
		err = (char*)"al_install_mouse() failed";
	if(!al_install_audio())
		err = (char*)"al_install_audio() failed";
	if(!al_init_image_addon())
		err = (char*)"al_init_image_addon() failed";
	if(!al_init_acodec_addon())
		err = (char*)"al_init_acodec_addon() failed";
	if(!al_init_font_addon())
		err = (char*)"al_init_font_addon() failed";
	if(!al_init_primitives_addon())
		err = (char*)"al_init_primitives_addon() failed";
	if(!al_init_ttf_addon())
		err = (char*)"al_init_ttf_addon() failed";
	if(!al_init_video_addon())
		err = (char*)"al_init_video_addon() failed";
	if(!al_init_native_dialog_addon())
		err = (char*)"al_init_native_dialog_addon() failed";
	if(err)
		al_show_native_message_box(NULL, "a5 init failed", err, " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
	return !err;
}
