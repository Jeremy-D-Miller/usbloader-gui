/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * demo.cpp
 * Basic template/demonstration of libwiigui capabilities. For a
 * full-featured app using many more extensions, check out Snes9x GX.
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "FreeTypeGX.h"
#include "video.h"
#include "audio.h"
#include "menu.h"
#include "input.h"
#include "filelist.h"
#include "main.h"
#include "http.h"
#include "dns.h"
#include "fatmounter.h"
#include "disc.h"
#include "wbfs.h"
#include "sys.h"
#include "video2.h"
#include "wpad.h"
#include "cfg.h"
#include "language.h"
#include "fat.h"


/* Constants */
#define CONSOLE_XCOORD		260
#define CONSOLE_YCOORD		115
#define CONSOLE_WIDTH		340
#define CONSOLE_HEIGHT		218

FreeTypeGX *fontSystem=0;
FreeTypeGX *fontClock=0;
bool netcheck = false;


/*Networking - Forsaekn*/
int Net_Init(char *ip){

	s32 res;
    while ((res = net_init()) == -EAGAIN)
	{
		usleep(100 * 1000); //100ms
	}

    if (if_config(ip, NULL, NULL, true) < 0) {
		printf("      Error reading IP address, exiting");
		usleep(1000 * 1000 * 1); //1 sec
		return FALSE;
	}
	return TRUE;
}

void
DefaultSettings()
{
	Settings.video = discdefault;
	Settings.vpatch = off;
	Settings.language = ConsoleLangDefault;
	Settings.ocarina = off;
	Settings.hddinfo = hr12;
	Settings.sinfo = ((THEME.showID) ? GameID : Neither);
	Settings.rumble = RumbleOn;
	if (THEME.showRegion)
	{
		Settings.sinfo = ((Settings.sinfo == GameID) ? Both : GameRegion);
	}
	Settings.volume = v80;
	Settings.tooltips = TooltipsOn;
	snprintf(Settings.unlockCode, sizeof(Settings.unlockCode), "ab121b");
	Settings.parentalcontrol = 0;
	Settings.cios = ios249;
	Settings.xflip = no;
	Settings.qboot = no;
	Settings.unicodefix = 0;
	Settings.wiilight = 1;
	Settings.patchcountrystrings = 0;

	CFG_LoadGlobal();
}

// check for libfat.a from 1. Jun 2009
extern int LibFat4USB_Loader_GX;
int fatCheck = LibFat4USB_Loader_GX; // remove when libfat newer than 1. Jun 2009

int
main(int argc, char *argv[])
{

	s32 ret2;

    SDCard_Init(); // mount SD for loading cfg's
	lang_default();
	CFG_Load();

	DefaultSettings();


	SDCard_deInit();// unmount SD for reloading IOS

    /* Load Custom IOS */
    if(Settings.cios == ios222) {
        ret2 = IOS_ReloadIOS(222);
        if (ret2 < 0) {
            Settings.cios = ios249;
            ret2 = IOS_ReloadIOS(249);
        }
	} else {
	    ret2 = IOS_ReloadIOS(249);
	}

	if (ret2 < 0) {
		printf("ERROR: cIOS could not be loaded!");
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}

    SDCard_Init(); // now mount SD:/
    USBDevice_Init(); // and mount USB:/

	Sys_Init();

	InitVideo(); // Initialise video
	InitAudio(); // Initialize audio

    PAD_Init(); // initialize PAD/WPAD
	Wpad_Init();
	WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);

	fontSystem = new FreeTypeGX();
	fontSystem->loadFont(font_ttf, font_ttf_size, 0);
	fontSystem->setCompatibilityMode(FTGX_COMPATIBILITY_DEFAULT_TEVOP_GX_PASSCLR | FTGX_COMPATIBILITY_DEFAULT_VTXDESC_GX_NONE);

	fontClock = new FreeTypeGX();
	fontClock->loadFont(clock_ttf, clock_ttf_size, 0);
	fontClock->setCompatibilityMode(FTGX_COMPATIBILITY_DEFAULT_TEVOP_GX_PASSCLR | FTGX_COMPATIBILITY_DEFAULT_VTXDESC_GX_NONE);

	InitGUIThreads();
	MainMenu(MENU_CHECK);
	return 0;
}