#include <string.h>
#include <unistd.h>

#include "usbloader/wbfs.h"
#include "usbloader/getentries.h"
#include "language/gettext.h"
#include "libwiigui/gui.h"
#include "libwiigui/gui_customoptionbrowser.h"
#include "prompts/PromptWindows.h"
#include "settings/SettingsPrompts.h"
#include "fatmounter.h"
#include "menu.h"
#include "filelist.h"
#include "sys.h"
#define MAXOPTIONS 12

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern bool OpenXMLDatabase(bool openfile, bool loadtitles, bool freemem);

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern GuiSound * bgMusic;
extern GuiImage * bgImg;
extern GuiImageData * pointer[4];
extern GuiImageData * background;
extern u8 shutdown;
extern u8 reset;

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
int MenuSettings()
{
	int menu = MENU_NONE;
	int ret;
	int choice = 0;
	bool exit = false;

	// backup game language setting
	int opt_lang = 0;
	opt_lang = Settings.language;

	enum {
    FADE,
	LEFT,
	RIGHT
    };

	int slidedirection = FADE;

	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND_PCM, Settings.sfxvolume);
	GuiSound btnClick(button_click2_pcm, button_click2_pcm_size, SOUND_PCM, Settings.sfxvolume);
    GuiSound btnClick1(button_click_pcm, button_click_pcm_size, SOUND_PCM, Settings.sfxvolume);

	char imgPath[100];

	snprintf(imgPath, sizeof(imgPath), "%sbutton_dialogue_box.png", CFG.theme_path);
	GuiImageData btnOutline(imgPath, button_dialogue_box_png);
	snprintf(imgPath, sizeof(imgPath), "%ssettings_background.png", CFG.theme_path);
	GuiImageData settingsbg(imgPath, settings_background_png);

	snprintf(imgPath, sizeof(imgPath), "%ssettings_title.png", CFG.theme_path);
	GuiImageData MainButtonImgData(imgPath, settings_title_png);

	snprintf(imgPath, sizeof(imgPath), "%ssettings_title_over.png", CFG.theme_path);
	GuiImageData MainButtonImgOverData(imgPath, settings_title_over_png);

	snprintf(imgPath, sizeof(imgPath), "%spageindicator.png", CFG.theme_path);
	GuiImageData PageindicatorImgData(imgPath, pageindicator_png);

    snprintf(imgPath, sizeof(imgPath), "%sstartgame_arrow_left.png", CFG.theme_path);
	GuiImageData arrow_left(imgPath, startgame_arrow_left_png);

	snprintf(imgPath, sizeof(imgPath), "%sstartgame_arrow_right.png", CFG.theme_path);
	GuiImageData arrow_right(imgPath, startgame_arrow_right_png);

    snprintf(imgPath, sizeof(imgPath), "%scredits_button.png", CFG.theme_path);
	GuiImageData creditsImgData(imgPath, credits_button_png);

    snprintf(imgPath, sizeof(imgPath), "%scredits_button_over.png", CFG.theme_path);
	GuiImageData creditsOver(imgPath, credits_button_over_png);

	GuiImage creditsImg(&creditsImgData);
	GuiImage creditsImgOver(&creditsOver);

    GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
    GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, 0);
	GuiTrigger trigB;
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);
    GuiTrigger trigL;
	trigL.SetButtonOnlyTrigger(-1, WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT, PAD_BUTTON_LEFT);
	GuiTrigger trigR;
	trigR.SetButtonOnlyTrigger(-1, WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT, PAD_BUTTON_RIGHT);
    GuiTrigger trigMinus;
	trigMinus.SetButtonOnlyTrigger(-1, WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS, 0);
	GuiTrigger trigPlus;
	trigPlus.SetButtonOnlyTrigger(-1, WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS, 0);

    GuiText titleTxt(tr("Settings"), 28, (GXColor){0, 0, 0, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

    GuiImage settingsbackground(&settingsbg);

    GuiText backBtnTxt(tr("Back") , 22, (GXColor){THEME.prompttxt_r, THEME.prompttxt_g, THEME.prompttxt_b, 255});
	backBtnTxt.SetMaxWidth(btnOutline.GetWidth()-30);
	GuiImage backBtnImg(&btnOutline);
	if (Settings.wsprompt == yes){
	backBtnTxt.SetWidescreen(CFG.widescreen);
	backBtnImg.SetWidescreen(CFG.widescreen);
	}
	GuiButton backBtn(&backBtnImg,&backBtnImg, 2, 3, -180, 400, &trigA, &btnSoundOver, &btnClick,1);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetTrigger(&trigB);

	GuiButton homo(1,1);
	homo.SetTrigger(&trigHome);

	GuiImage PageindicatorImg1(&PageindicatorImgData);
	GuiText PageindicatorTxt1("1", 22, (GXColor){0, 0, 0, 255});
	GuiButton PageIndicatorBtn1(PageindicatorImg1.GetWidth(), PageindicatorImg1.GetHeight());
	PageIndicatorBtn1.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	PageIndicatorBtn1.SetPosition(200, 400);
	PageIndicatorBtn1.SetImage(&PageindicatorImg1);
	PageIndicatorBtn1.SetLabel(&PageindicatorTxt1);
	PageIndicatorBtn1.SetSoundOver(&btnSoundOver);
	PageIndicatorBtn1.SetSoundClick(&btnClick1);
	PageIndicatorBtn1.SetTrigger(&trigA);
	PageIndicatorBtn1.SetEffectGrow();

	GuiImage PageindicatorImg2(&PageindicatorImgData);
	GuiText PageindicatorTxt2("2", 22, (GXColor){0, 0, 0, 255});
	GuiButton PageIndicatorBtn2(PageindicatorImg2.GetWidth(), PageindicatorImg2.GetHeight());
	PageIndicatorBtn2.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	PageIndicatorBtn2.SetPosition(235, 400);
	PageIndicatorBtn2.SetImage(&PageindicatorImg2);
	PageIndicatorBtn2.SetLabel(&PageindicatorTxt2);
	PageIndicatorBtn2.SetSoundOver(&btnSoundOver);
	PageIndicatorBtn2.SetSoundClick(&btnClick1);
	PageIndicatorBtn2.SetTrigger(&trigA);
	PageIndicatorBtn2.SetEffectGrow();

	GuiImage GoLeftImg(&arrow_left);
	GuiButton GoLeftBtn(GoLeftImg.GetWidth(), GoLeftImg.GetHeight());
	GoLeftBtn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	GoLeftBtn.SetPosition(25, -25);
	GoLeftBtn.SetImage(&GoLeftImg);
	GoLeftBtn.SetSoundOver(&btnSoundOver);
	GoLeftBtn.SetSoundClick(&btnClick);
	GoLeftBtn.SetEffectGrow();
	GoLeftBtn.SetTrigger(&trigA);
	GoLeftBtn.SetTrigger(&trigL);
	GoLeftBtn.SetTrigger(&trigMinus);

	GuiImage GoRightImg(&arrow_right);
	GuiButton GoRightBtn(GoRightImg.GetWidth(), GoRightImg.GetHeight());
	GoRightBtn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	GoRightBtn.SetPosition(-25, -25);
	GoRightBtn.SetImage(&GoRightImg);
	GoRightBtn.SetSoundOver(&btnSoundOver);
	GoRightBtn.SetSoundClick(&btnClick);
	GoRightBtn.SetEffectGrow();
	GoRightBtn.SetTrigger(&trigA);
	GoRightBtn.SetTrigger(&trigR);
	GoRightBtn.SetTrigger(&trigPlus);

    char MainButtonText[50];
    snprintf(MainButtonText, sizeof(MainButtonText), "%s", " ");

    GuiImage MainButton1Img(&MainButtonImgData);
    GuiImage MainButton1ImgOver(&MainButtonImgOverData);
    GuiText MainButton1Txt(MainButtonText, 22, (GXColor){0, 0, 0, 255});
	MainButton1Txt.SetMaxWidth(MainButton1Img.GetWidth());
    GuiButton MainButton1(MainButton1Img.GetWidth(), MainButton1Img.GetHeight());
    MainButton1.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	MainButton1.SetPosition(0, 90);
	MainButton1.SetImage(&MainButton1Img);
	MainButton1.SetImageOver(&MainButton1ImgOver);
	MainButton1.SetLabel(&MainButton1Txt);
	MainButton1.SetSoundOver(&btnSoundOver);
	MainButton1.SetSoundClick(&btnClick1);
	MainButton1.SetEffectGrow();
	MainButton1.SetTrigger(&trigA);

    GuiImage MainButton2Img(&MainButtonImgData);
    GuiImage MainButton2ImgOver(&MainButtonImgOverData);
    GuiText MainButton2Txt(MainButtonText, 22, (GXColor){0, 0, 0, 255});
	MainButton2Txt.SetMaxWidth(MainButton2Img.GetWidth());
    GuiButton MainButton2(MainButton2Img.GetWidth(), MainButton2Img.GetHeight());
    MainButton2.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	MainButton2.SetPosition(0, 160);
	MainButton2.SetImage(&MainButton2Img);
	MainButton2.SetImageOver(&MainButton2ImgOver);
	MainButton2.SetLabel(&MainButton2Txt);
	MainButton2.SetSoundOver(&btnSoundOver);
	MainButton2.SetSoundClick(&btnClick1);
	MainButton2.SetEffectGrow();
	MainButton2.SetTrigger(&trigA);

    GuiImage MainButton3Img(&MainButtonImgData);
    GuiImage MainButton3ImgOver(&MainButtonImgOverData);
    GuiText MainButton3Txt(MainButtonText, 22, (GXColor){0, 0, 0, 255});
	MainButton3Txt.SetMaxWidth(MainButton3Img.GetWidth());
    GuiButton MainButton3(MainButton3Img.GetWidth(), MainButton3Img.GetHeight());
    MainButton3.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	MainButton3.SetPosition(0, 230);
	MainButton3.SetImage(&MainButton3Img);
	MainButton3.SetImageOver(&MainButton3ImgOver);
	MainButton3.SetLabel(&MainButton3Txt);
	MainButton3.SetSoundOver(&btnSoundOver);
	MainButton3.SetSoundClick(&btnClick1);
	MainButton3.SetEffectGrow();
	MainButton3.SetTrigger(&trigA);

    GuiImage MainButton4Img(&MainButtonImgData);
    GuiImage MainButton4ImgOver(&MainButtonImgOverData);
    GuiText MainButton4Txt(MainButtonText, 22, (GXColor){0, 0, 0, 255});
	MainButton4Txt.SetMaxWidth(MainButton4Img.GetWidth());
    GuiButton MainButton4(MainButton4Img.GetWidth(), MainButton4Img.GetHeight());
    MainButton4.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	MainButton4.SetPosition(0, 300);
	MainButton4.SetImage(&MainButton4Img);
	MainButton4.SetImageOver(&MainButton4ImgOver);
	MainButton4.SetLabel(&MainButton4Txt);
	MainButton4.SetSoundOver(&btnSoundOver);
	MainButton4.SetSoundClick(&btnClick1);
	MainButton4.SetEffectGrow();
	MainButton4.SetTrigger(&trigA);

	customOptionList options2(MAXOPTIONS);
	GuiCustomOptionBrowser optionBrowser2(396, 280, &options2, CFG.theme_path, "bg_options_settings.png", bg_options_settings_png, 0, 150);
	optionBrowser2.SetPosition(0, 90);
	optionBrowser2.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);

	GuiWindow w(screenwidth, screenheight);

	int pageToDisplay = 1;
	while ( pageToDisplay > 0) //set pageToDisplay to 0 to quit
	{
	    VIDEO_WaitVSync ();

		menu = MENU_NONE;

		if ( pageToDisplay == 1)
		{
		    /** Standard procedure made in all pages **/
			MainButton1.StopEffect();
			MainButton2.StopEffect();
			MainButton3.StopEffect();
			MainButton4.StopEffect();

            if(slidedirection == RIGHT) {
            MainButton1.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            MainButton2.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            while (MainButton1.GetEffect()>0) usleep(50);
            }
            else if(slidedirection == LEFT) {
            MainButton1.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            MainButton2.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            while (MainButton1.GetEffect()>0) usleep(50);
            }

			HaltGui();

            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("GUI Settings"));
            MainButton1Txt.SetText(MainButtonText);
            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("Game Load"));
            MainButton2Txt.SetText(MainButtonText);
            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("Parental Control"));
            MainButton3Txt.SetText(MainButtonText);
            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("Sound"));
            MainButton4Txt.SetText(MainButtonText);

			mainWindow->RemoveAll();
			mainWindow->Append(&w);
			w.RemoveAll();
            w.Append(&settingsbackground);
            w.Append(&PageIndicatorBtn1);
            w.Append(&PageIndicatorBtn2);
			w.Append(&titleTxt);
			w.Append(&backBtn);
			w.Append(&homo);
			w.Append(&GoRightBtn);
			w.Append(&GoLeftBtn);
			w.Append(&MainButton1);
			w.Append(&MainButton2);
			w.Append(&MainButton3);
			w.Append(&MainButton4);

			PageIndicatorBtn1.SetAlpha(255);
			PageIndicatorBtn2.SetAlpha(50);

			/** Creditsbutton change **/
			MainButton4.SetImage(&MainButton4Img);
			MainButton4.SetImageOver(&MainButton4ImgOver);

			/** Disable ability to click through MainButtons */
			optionBrowser2.SetClickable(false);
			/** Default no scrollbar and reset position **/
			optionBrowser2.SetScrollbar(0);
			optionBrowser2.SetOffset(0);

			MainButton1.StopEffect();
			MainButton2.StopEffect();
			MainButton3.StopEffect();
			MainButton4.StopEffect();

			MainButton1.SetEffectGrow();
			MainButton2.SetEffectGrow();
			MainButton3.SetEffectGrow();
			MainButton4.SetEffectGrow();

            if(slidedirection == FADE) {
            MainButton1.SetEffect(EFFECT_FADE, 20);
			MainButton2.SetEffect(EFFECT_FADE, 20);
            MainButton3.SetEffect(EFFECT_FADE, 20);
            MainButton4.SetEffect(EFFECT_FADE, 20);
            }
            else if(slidedirection == LEFT) {
            MainButton1.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
			MainButton2.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
            }
            else if(slidedirection == RIGHT) {
            MainButton1.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
			MainButton2.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
            }

			mainWindow->Append(&w);

			ResumeGui();

			while(MainButton1.GetEffect() > 0) usleep(50);

		}
		else if ( pageToDisplay == 2 )
		{
			/** Standard procedure made in all pages **/
			MainButton1.StopEffect();
			MainButton2.StopEffect();
			MainButton3.StopEffect();
			MainButton4.StopEffect();

            if(slidedirection == RIGHT) {
            MainButton1.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            MainButton2.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 35);
            while (MainButton1.GetEffect()>0) usleep(50);
            }
            else if(slidedirection == LEFT) {
            MainButton1.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            MainButton2.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 35);
            while (MainButton1.GetEffect()>0) usleep(50);
            }

			HaltGui();

            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("Custom Paths"));
            MainButton1Txt.SetText(MainButtonText);
            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("Update"));
            MainButton2Txt.SetText(MainButtonText);
            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("Default Settings"));
            MainButton3Txt.SetText(MainButtonText);
            snprintf(MainButtonText, sizeof(MainButtonText), "%s", tr("Credits"));
            MainButton4Txt.SetText(MainButtonText);

			mainWindow->RemoveAll();
			mainWindow->Append(&w);
			w.RemoveAll();
            w.Append(&settingsbackground);
            w.Append(&PageIndicatorBtn1);
            w.Append(&PageIndicatorBtn2);
			w.Append(&titleTxt);
			w.Append(&backBtn);
			w.Append(&homo);
			w.Append(&GoRightBtn);
			w.Append(&GoLeftBtn);
			w.Append(&MainButton1);
			w.Append(&MainButton2);
			w.Append(&MainButton3);
			w.Append(&MainButton4);

			PageIndicatorBtn1.SetAlpha(50);
			PageIndicatorBtn2.SetAlpha(255);

			/** Creditsbutton change **/
			MainButton4.SetImage(&creditsImg);
			MainButton4.SetImageOver(&creditsImgOver);

			/** Disable ability to click through MainButtons */
			optionBrowser2.SetClickable(false);
			/** Default no scrollbar and reset position **/
			optionBrowser2.SetScrollbar(0);
			optionBrowser2.SetOffset(0);

			MainButton1.StopEffect();
			MainButton2.StopEffect();
			MainButton3.StopEffect();
			MainButton4.StopEffect();

			MainButton1.SetEffectGrow();
			MainButton2.SetEffectGrow();
			MainButton3.SetEffectGrow();
			MainButton4.SetEffectGrow();

            if(slidedirection == FADE) {
            MainButton1.SetEffect(EFFECT_FADE, 20);
			MainButton2.SetEffect(EFFECT_FADE, 20);
            MainButton3.SetEffect(EFFECT_FADE, 20);
            MainButton4.SetEffect(EFFECT_FADE, 20);
            }
            else if(slidedirection == LEFT) {
            MainButton1.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
			MainButton2.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 35);
            }
            else if(slidedirection == RIGHT) {
            MainButton1.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
			MainButton2.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
            MainButton3.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
            MainButton4.SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 35);
            }

			mainWindow->Append(&w);

			ResumeGui();

			while(MainButton1.GetEffect() > 0) usleep(50);

		}

		while(menu == MENU_NONE)
		{
			VIDEO_WaitVSync ();

			if ( pageToDisplay == 1 )
			{
			    if(MainButton1.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton1.GetEffect() > 0) usleep(50);
                    HaltGui();
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
                    titleTxt.SetText(tr("GUI Settings"));
                    exit = false;
					for(int i = 0; i <= MAXOPTIONS; i++) options2.SetName(i, NULL);
                    options2.SetName(0, "%s",tr("App Language"));
                    options2.SetName(1, "%s",tr("Display"));
                    options2.SetName(2, "%s",tr("Clock"));
                    options2.SetName(3, "%s",tr("Tooltips"));
                    options2.SetName(4, "%s",tr("Flip-X"));
                    options2.SetName(5, "%s",tr("Prompts Buttons"));
                    options2.SetName(6, "%s",tr("Keyboard"));
                    options2.SetName(7, "%s",tr("Wiilight"));
                    options2.SetName(8, "%s",tr("Rumble"));
                    options2.SetName(10, "%s",tr("Titles from XML"));
                    options2.SetName(11, "%s",tr("Screensaver"));
                    for(int i = 0; i <= MAXOPTIONS; i++) options2.SetValue(i, NULL);
                    optionBrowser2.SetScrollbar(1);
                    w.Append(&optionBrowser2);
                    optionBrowser2.SetClickable(true);
                    ResumeGui();

                    VIDEO_WaitVSync ();
                    optionBrowser2.SetEffect(EFFECT_FADE, 20);
			        while(optionBrowser2.GetEffect() > 0) usleep(50);

                    int returnhere = 1;
                    char * languagefile;
                    languagefile = strrchr(Settings.language_path, '/')+1;

                    while(!exit)
                    {
                        VIDEO_WaitVSync ();

                        returnhere = 1;

                        if(Settings.sinfo  >= settings_sinfo_max)
                            Settings.sinfo = 0;
                        if(Settings.hddinfo >= settings_clock_max)
                            Settings.hddinfo = 0; //CLOCK
                        if (Settings.tooltips >= settings_tooltips_max)
                            Settings.tooltips = 0;
                        if ( Settings.xflip >= settings_xflip_max)
                            Settings.xflip = 0;
                        if ( Settings.wsprompt > 1 )
                            Settings.wsprompt = 0;
                        if ( Settings.keyset >= settings_keyset_max)
                            Settings.keyset = 0;
                        if ( Settings.wiilight > 2 )
                            Settings.wiilight = 0;
                        if(Settings.rumble >= settings_rumble_max)
                            Settings.rumble = 0; //RUMBLE
                        if(Settings.screensaver >= settings_screensaver_max)
                            Settings.screensaver = 0; //RUMBLE
						if(Settings.titlesOverride >= 2)
							Settings.titlesOverride = 0;
                        if(!strcmp("notset", Settings.language_path))
                            options2.SetValue(0, "%s", tr("Default"));
                        else
                            options2.SetValue(0, "%s", languagefile);

                        if (Settings.sinfo == GameID) options2.SetValue(1,"%s",tr("Game ID"));
                        else if (Settings.sinfo == GameRegion) options2.SetValue(1,"%s",tr("Game Region"));
                        else if (Settings.sinfo == Both) options2.SetValue(1,"%s",tr("Both"));
                        else if (Settings.sinfo == Neither) options2.SetValue(1,"%s",tr("Neither"));

                        if (Settings.hddinfo == hr12) options2.SetValue(2,"12 %s",tr("Hour"));
                        else if (Settings.hddinfo == hr24) options2.SetValue(2,"24 %s",tr("Hour"));
                        else if (Settings.hddinfo == Off) options2.SetValue(2,"%s",tr("OFF"));

                        if (Settings.tooltips == TooltipsOn) options2.SetValue(3,"%s",tr("ON"));
                        else if (Settings.tooltips == TooltipsOff) options2.SetValue(3,"%s",tr("OFF"));

                        if (Settings.xflip == no) options2.SetValue(4,"%s/%s",tr("Right"),tr("Next"));
                        else if (Settings.xflip == yes) options2.SetValue(4,"%s/%s",tr("Left"),tr("Prev"));
                        else if (Settings.xflip == sysmenu) options2.SetValue(4,"%s", tr("Like SysMenu"));
                        else if (Settings.xflip == wtf) options2.SetValue(4,"%s/%s",tr("Right"),tr("Prev"));
                        else if (Settings.xflip == disk3d) options2.SetValue(4,tr("DiskFlip"));

                        if (Settings.wsprompt == no) options2.SetValue(5,"%s",tr("Normal"));
                        else if (Settings.wsprompt == yes) options2.SetValue(5,"%s",tr("Widescreen Fix"));

                        if (Settings.keyset == us) options2.SetValue(6,"QWERTY");
                        else if (Settings.keyset == dvorak) options2.SetValue(6,"DVORAK");
                        else if (Settings.keyset == euro) options2.SetValue(6,"QWERTZ");
                        else if (Settings.keyset == azerty) options2.SetValue(6,"AZERTY");

                        if (Settings.wiilight == 0) options2.SetValue(7,"%s",tr("OFF"));
                        else if (Settings.wiilight == 1) options2.SetValue(7,"%s",tr("ON"));
                        else if (Settings.wiilight == 2) options2.SetValue(7,"%s",tr("Only for Install"));

                        if (Settings.rumble == RumbleOn) options2.SetValue(8,"%s",tr("ON"));
                        else if (Settings.rumble == RumbleOff) options2.SetValue(8,"%s",tr("OFF"));

                        if (Settings.titlesOverride == 0) options2.SetValue(10,"%s",tr("OFF"));
                        else if (Settings.titlesOverride == 1) options2.SetValue(10,"%s",tr("ON"));

						if (Settings.screensaver == 0) options2.SetValue(11,"%s",tr("OFF"));
                        else if (Settings.screensaver == 1) options2.SetValue(11,"3 min");
						else if (Settings.screensaver == 2) options2.SetValue(11,"5 min");
						else if (Settings.screensaver == 3) options2.SetValue(11,"10 min");
						else if (Settings.screensaver == 4) options2.SetValue(11,"20 min");
						else if (Settings.screensaver == 5) options2.SetValue(11,"30 min");
						else if (Settings.screensaver == 6) options2.SetValue(11,"1 hour");

						if(backBtn.GetState() == STATE_CLICKED)
                        {
                            backBtn.ResetState();
                            exit = true;
							break;
                        }

                        if(shutdown == 1)
                            Sys_Shutdown();
                        else if(reset == 1)
                            Sys_Reboot();

                        else if(menu == MENU_DISCLIST) {
                            w.Remove(&optionBrowser2);
                            w.Remove(&backBtn);
                            WindowCredits();
                            w.Append(&optionBrowser2);
                            w.Append(&backBtn);
                        }
                        else if(homo.GetState() == STATE_CLICKED)
                        {
                            cfg_save_global();
                            optionBrowser2.SetState(STATE_DISABLED);
                            s32 thetimeofbg = bgMusic->GetPlayTime();
                            bgMusic->Stop();
                            choice = WindowExitPrompt(tr("Exit USB Loader GX?"),0, tr("Back to Loader"),tr("Wii Menu"),tr("Back"),0);
                            if(!strcmp("", Settings.oggload_path) || !strcmp("notset", Settings.ogg_path))
                            {
                                bgMusic->Play();
                            } else {
                                bgMusic->PlayOggFile(Settings.ogg_path);
                            }
                            bgMusic->SetPlayTime(thetimeofbg);
                            SetVolumeOgg(255*(Settings.volume/100.0));
                            if(choice == 3) {
                                Sys_LoadMenu(); // Back to System Menu
                            } else if (choice == 2) {
                                Sys_BackToLoader();
                            } else {
                                homo.ResetState();
                            }
                            optionBrowser2.SetState(STATE_DEFAULT);
                        }

                        ret = optionBrowser2.GetClickedOption();

                        switch (ret)
                        {
                            case 0:
                                //if(isSdInserted()) {
								if(isInserted(bootDevice)) {
                                if ( Settings.godmode == 1)
                                {
                                    w.SetEffect(EFFECT_FADE, -20);
                                    while(w.GetEffect()>0) usleep(50);
                                    mainWindow->Remove(&w);
                                    while(returnhere == 1)
                                    returnhere = MenuLanguageSelect();
                                    if(returnhere == 2) {
                                        menu = MENU_SETTINGS;
                                        pageToDisplay = 0;
                                        exit = true;
                                        mainWindow->Append(&w);
                                        break;
                                    } else {
                                    HaltGui();
                                    mainWindow->Append(&w);
                                    w.SetEffect(EFFECT_FADE, 20);
                                    ResumeGui();
                                    while(w.GetEffect()>0) usleep(50);
                                    }
                                } else {
                                    WindowPrompt(tr("Language change:"),tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                }
                                } else {
                                    WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to use this option."), tr("OK"), 0,0,0);
                                }
                                break;
                            case 1:
                                Settings.sinfo++;
                                break;
                            case 2:
                                Settings.hddinfo++;
                                break;
                            case 3:
                                Settings.tooltips++;
                                break;
                            case 4:
                                Settings.xflip++;
                                break;
                            case 5:
                                Settings.wsprompt++;
                                break;
                            case 6:
                                Settings.keyset++;
                                break;
                            case 7:
                                Settings.wiilight++;
                                break;
                            case 8:
                                Settings.rumble++;
                                break;
                            case 9:
                                break;
                            case 10:
                                Settings.titlesOverride++;
                                break;
							case 11:
                                Settings.screensaver++;
                                break;

                           }
                    }
                    optionBrowser2.SetEffect(EFFECT_FADE, -20);
                    while(optionBrowser2.GetEffect() > 0) usleep(50);
                    titleTxt.SetText(tr("Settings"));
                    slidedirection = FADE;
                    if(returnhere != 2)
                    pageToDisplay = 1;
                    MainButton1.ResetState();
                    break;
                }

                if(MainButton2.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton2.GetEffect() > 0) usleep(50);
                    HaltGui();
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
                    titleTxt.SetText(tr("Game Load"));
                    exit = false;
					for(int i = 0; i <= MAXOPTIONS; i++) options2.SetName(i, NULL);
                    options2.SetName(0, "%s",tr("Video Mode"));
                    options2.SetName(1, "%s",tr("VIDTV Patch"));
                    options2.SetName(2, "%s",tr("Game Language"));
                    options2.SetName(3, "%s",tr("Patch Country Strings"));
                    options2.SetName(4, "Ocarina");
                    options2.SetName(5,"%s", tr("Boot/Standard"));
                    options2.SetName(6, "%s",tr("Quick Boot"));
                    options2.SetName(7, "%s",tr("Error 002 fix"));
                    options2.SetName(8, "%s",tr("Anti 002 fix"));
                    for(int i = 0; i <= MAXOPTIONS; i++) options2.SetValue(i, NULL);
                    w.Append(&optionBrowser2);
                    optionBrowser2.SetClickable(true);
                    ResumeGui();

                    VIDEO_WaitVSync ();
                    optionBrowser2.SetEffect(EFFECT_FADE, 20);
			        while(optionBrowser2.GetEffect() > 0) usleep(50);

                    while(!exit)
                    {
                        VIDEO_WaitVSync ();
                        if(Settings.video >= settings_video_max)
                            Settings.video = 0;
                        if(Settings.vpatch >= settings_off_on_max)
                            Settings.vpatch = 0;
                        if ( Settings.patchcountrystrings > 1)
                            Settings.patchcountrystrings = 0;
                        if(Settings.ocarina >= settings_off_on_max)
                            Settings.ocarina = 0;
                        if ( Settings.qboot > 1 )
                            Settings.qboot = 0;
                        if ( Settings.cios >= settings_cios_max)
                            Settings.cios = 0;
                        if ( Settings.language >= settings_language_max)
                            Settings.language = 0;
                        if(Settings.error002 >= settings_off_on_max)
                            Settings.error002 = 0;
                        if(Settings.anti002fix >= settings_off_on_max)
                            Settings.anti002fix = 0;

                        if (Settings.video == discdefault) options2.SetValue(0,"%s",tr("Disc Default"));
                        else if (Settings.video == systemdefault) options2.SetValue(0,"%s",tr("System Default"));
                        else if (Settings.video == patch) options2.SetValue(0,"%s",tr("AutoPatch"));
                        else if (Settings.video == pal50) options2.SetValue(0,"%s PAL50",tr("Force"));
                        else if (Settings.video == pal60) options2.SetValue(0,"%s PAL60",tr("Force"));
                        else if (Settings.video == ntsc) options2.SetValue(0,"%s NTSC",tr("Force"));

                        if (Settings.vpatch == on) options2.SetValue(1,"%s",tr("ON"));
                        else if (Settings.vpatch == off) options2.SetValue(1,"%s",tr("OFF"));

                        if (Settings.language == ConsoleLangDefault) options2.SetValue(2,"%s",tr("Console Default"));
                        else if (Settings.language == jap) options2.SetValue(2,"%s",tr("Japanese"));
                        else if (Settings.language == ger) options2.SetValue(2,"%s",tr("German"));
                        else if (Settings.language == eng) options2.SetValue(2,"%s",tr("English"));
                        else if (Settings.language == fren) options2.SetValue(2,"%s",tr("French"));
                        else if (Settings.language == esp) options2.SetValue(2,"%s",tr("Spanish"));
                        else if (Settings.language == it) options2.SetValue(2,"%s",tr("Italian"));
                        else if (Settings.language == dut) options2.SetValue(2,"%s",tr("Dutch"));
                        else if (Settings.language == schin) options2.SetValue(2,"%s",tr("SChinese"));
                        else if (Settings.language == tchin) options2.SetValue(2,"%s",tr("TChinese"));
                        else if (Settings.language == kor) options2.SetValue(2,"%s",tr("Korean"));

                        if (Settings.patchcountrystrings == 0) options2.SetValue(3,"%s",tr("OFF"));
                        else if (Settings.patchcountrystrings == 1) options2.SetValue(3,"%s",tr("ON"));

                        if (Settings.ocarina == on) options2.SetValue(4,"%s",tr("ON"));
                        else if (Settings.ocarina == off) options2.SetValue(4,"%s",tr("OFF"));

                       if (Settings.godmode != 1) options2.SetValue(5, "********");
                        else if (Settings.cios == ios249) options2.SetValue(5,"cIOS 249");
                        else if (Settings.cios == ios222) options2.SetValue(5,"cIOS 222");

                        if (Settings.qboot == no) options2.SetValue(6,"%s",tr("No"));
                        else if (Settings.qboot == yes) options2.SetValue(6,"%s",tr("Yes"));

                        if (Settings.error002 == no) options2.SetValue(7,"%s",tr("No"));
                        else if (Settings.error002 == yes) options2.SetValue(7,"%s",tr("Yes"));

                        if (Settings.anti002fix == no) options2.SetValue(8,"%s",tr("No"));
                        else if (Settings.anti002fix == yes) options2.SetValue(8,"%s",tr("Yes"));

                        if(backBtn.GetState() == STATE_CLICKED)
                        {
                            backBtn.ResetState();
                            exit = true;
                            break;
                        }

                        if(shutdown == 1)
                            Sys_Shutdown();
                        else if(reset == 1)
                            Sys_Reboot();

                        else if(homo.GetState() == STATE_CLICKED)
                        {
                            cfg_save_global();
                            optionBrowser2.SetState(STATE_DISABLED);
                            s32 thetimeofbg = bgMusic->GetPlayTime();
                            bgMusic->Stop();
                            choice = WindowExitPrompt(tr("Exit USB Loader GX?"),0, tr("Back to Loader"),tr("Wii Menu"),tr("Back"),0);
                            if(!strcmp("", Settings.oggload_path) || !strcmp("notset", Settings.ogg_path))
                            {
                                bgMusic->Play();
                            } else {
                                bgMusic->PlayOggFile(Settings.ogg_path);
                            }
                            bgMusic->SetPlayTime(thetimeofbg);
                            SetVolumeOgg(255*(Settings.volume/100.0));
                            if(choice == 3) {
                                Sys_LoadMenu(); // Back to System Menu
                            } else if (choice == 2) {
                                Sys_BackToLoader();
                            } else {
                                homo.ResetState();
                            }
                            optionBrowser2.SetState(STATE_DEFAULT);
                        }

                        ret = optionBrowser2.GetClickedOption();

                        switch (ret)
                        {
                            case 0:
                                Settings.video++;
                                break;
                            case 1:
                                Settings.vpatch++;
                                break;
                            case 2:
                                Settings.language++;
                                break;
                            case 3:
                                Settings.patchcountrystrings++;
                                break;
                            case 4:
                                Settings.ocarina++;
                                break;
                            case 5:
                                if(Settings.godmode)
                                Settings.cios++;
                                break;
                            case 6:
                                Settings.qboot++;
                                break;
                            case 7:
                                Settings.error002++;
                                break;
                            case 8:
                                Settings.anti002fix++;
                                break;
                        }
                    }
                    optionBrowser2.SetEffect(EFFECT_FADE, -20);
                    while(optionBrowser2.GetEffect() > 0) usleep(50);
                    titleTxt.SetText(tr("Settings"));
                    slidedirection = FADE;
                    pageToDisplay = 1;
                    MainButton2.ResetState();
                    break;
                }

                if(MainButton3.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton3.GetEffect() > 0) usleep(50);
                    HaltGui();
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
                    titleTxt.SetText(tr("Parental Control"));
                    exit = false;
					for(int i = 0; i <= MAXOPTIONS; i++) options2.SetName(i, NULL);
                    options2.SetName(0, "%s",tr("Console"));
                    options2.SetName(1, "%s",tr("Password"));
                    options2.SetName(2, "%s",tr("Controllevel"));
                    for(int i = 0; i <= MAXOPTIONS; i++) options2.SetValue(i, NULL);
                    w.Append(&optionBrowser2);
                    optionBrowser2.SetClickable(true);
                    ResumeGui();

                    VIDEO_WaitVSync ();
                    optionBrowser2.SetEffect(EFFECT_FADE, 20);
			        while(optionBrowser2.GetEffect() > 0) usleep(50);

                    while(!exit)
                    {
                        VIDEO_WaitVSync ();

                        if (Settings.parentalcontrol > 4 )
                            Settings.parentalcontrol = 0;

                        if( Settings.godmode == 1 ) options2.SetValue(0, tr("Unlocked"));
                        else if( Settings.godmode == 0 ) options2.SetValue(0, tr("Locked"));

                        if ( Settings.godmode != 1) options2.SetValue(1, "********");
                        else if (!strcmp("", Settings.unlockCode)) options2.SetValue(1, "%s",tr("not set"));
                        else options2.SetValue(1, Settings.unlockCode);

                        if (Settings.godmode != 1) options2.SetValue(2, "********");
                        else if(Settings.parentalcontrol == 0) options2.SetValue(2, tr("OFF"));
                        else if(Settings.parentalcontrol == 1) options2.SetValue(2, tr("1 (Child 7+)"));
                        else if(Settings.parentalcontrol == 2) options2.SetValue(2, tr("2 (Teen 12+)"));
                        else if(Settings.parentalcontrol == 3) options2.SetValue(2, tr("3 (Mature 16+)"));
                        else if(Settings.parentalcontrol == 4) options2.SetValue(2, tr("4 (Adults Only 18+)"));

                        if(backBtn.GetState() == STATE_CLICKED)
                        {
                            backBtn.ResetState();
                            exit = true;
                            break;
                        }

                        if(shutdown == 1)
                            Sys_Shutdown();
                        else if(reset == 1)
                            Sys_Reboot();

                        else if(homo.GetState() == STATE_CLICKED)
                        {
                            cfg_save_global();
                            optionBrowser2.SetState(STATE_DISABLED);
                            s32 thetimeofbg = bgMusic->GetPlayTime();
                            bgMusic->Stop();
                            choice = WindowExitPrompt(tr("Exit USB Loader GX?"),0, tr("Back to Loader"),tr("Wii Menu"),tr("Back"),0);
                            if(!strcmp("", Settings.oggload_path) || !strcmp("notset", Settings.ogg_path))
                            {
                                bgMusic->Play();
                            } else {
                                bgMusic->PlayOggFile(Settings.ogg_path);
                            }
                            bgMusic->SetPlayTime(thetimeofbg);
                            SetVolumeOgg(255*(Settings.volume/100.0));
                            if(choice == 3) {
                                Sys_LoadMenu(); // Back to System Menu
                            } else if (choice == 2) {
                                Sys_BackToLoader();
                            } else {
                                homo.ResetState();
                            }
                            optionBrowser2.SetState(STATE_DEFAULT);
                        }

                        ret = optionBrowser2.GetClickedOption();

                        switch (ret)
                        {
                            case 0:
                                if (!strcmp("", Settings.unlockCode))
                                {
                                    Settings.godmode = !Settings.godmode;
                                    break;
                                }
                                else if ( Settings.godmode == 0 ) {
                                    //password check to unlock Install,Delete and Format
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[20] = "";
                                    int result = OnScreenKeyboard(entered, 20,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 ) {
                                        if (!strcmp(entered, Settings.unlockCode)) //if password correct
                                        {
                                            if (Settings.godmode == 0) {
                                                WindowPrompt(tr("Correct Password"),tr("All the features of USB Loader GX are unlocked."),tr("OK"),0,0,0);
                                                Settings.godmode = 1;
                                                //__Menu_GetEntries();
                                                menu = MENU_DISCLIST;
                                            }
                                        } else {
                                                WindowPrompt(tr("Wrong Password"),tr("USB Loader GX is protected"),tr("OK"),0,0,0);
                                        }
                                    }
                                } else {
                                    int choice = WindowPrompt (tr("Lock Console"),tr("Are you sure?"),tr("Yes"),tr("No"),0,0);
                                    if(choice == 1) {
                                        WindowPrompt(tr("Console Locked"),tr("USB Loader GX is protected"),tr("OK"),0,0,0);
                                        Settings.godmode = 0;
                                        //__Menu_GetEntries();
                                        menu = MENU_DISCLIST;
                                    }
                                }
                                break;
                            case 1:// Modify Password
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[20] = "";
                                    strncpy(entered, Settings.unlockCode, sizeof(entered));
                                    int result = OnScreenKeyboard(entered, 20,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 ) {
                                        strncpy(Settings.unlockCode, entered, sizeof(Settings.unlockCode));
                                        WindowPrompt(tr("Password Changed"),tr("Password has been changed"),tr("OK"),0,0,0);
                                    }
                                } else {
                                    WindowPrompt(tr("Password Changed"),tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                }
                                break;
                            case 2:
                                if(Settings.godmode)
                                Settings.parentalcontrol++;
                                break;
                        }
                    }
                    optionBrowser2.SetEffect(EFFECT_FADE, -20);
                    while(optionBrowser2.GetEffect() > 0) usleep(50);
                    titleTxt.SetText(tr("Settings"));
                    slidedirection = FADE;
                    pageToDisplay = 1;
                    MainButton3.ResetState();
                    break;
                }

                if(MainButton4.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton4.GetEffect() > 0) usleep(50);
                    HaltGui();
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
                    titleTxt.SetText(tr("Sound"));
                    exit = false;
					for(int i = 0; i <= MAXOPTIONS; i++) options2.SetName(i, NULL);
                    options2.SetName(0, "%s",tr("Backgroundmusic"));
                    options2.SetName(1, "%s",tr("Music Volume"));
                    options2.SetName(2, "%s",tr("SFX Volume"));
                    for(int i = 0; i <= MAXOPTIONS; i++) options2.SetValue(i, NULL);
                    w.Append(&optionBrowser2);
                    optionBrowser2.SetClickable(true);
                    ResumeGui();

                    VIDEO_WaitVSync ();
                    optionBrowser2.SetEffect(EFFECT_FADE, 20);
			        while(optionBrowser2.GetEffect() > 0) usleep(50);


                    char * oggfile;

                    while(!exit)
                    {
                        VIDEO_WaitVSync ();

                        bool returnhere = true;

                        if(!strcmp("notset", Settings.ogg_path))
                            options2.SetValue(0, "%s", tr("Standard"));
                        else {
                            oggfile = strrchr(Settings.ogg_path, '/')+1;
                            options2.SetValue(0, "%s", oggfile);
                        }

                        if(Settings.volume > 0)
                        options2.SetValue(1,"%i", Settings.volume);
                        else
                        options2.SetValue(1,"%s", tr("OFF"));
                        if(Settings.sfxvolume > 0)
                        options2.SetValue(2,"%i", Settings.sfxvolume);
                        else
                        options2.SetValue(2,"%s", tr("OFF"));

                        if(backBtn.GetState() == STATE_CLICKED)
                        {
                            backBtn.ResetState();
                            exit = true;
                            break;
                        }

                        if(shutdown == 1)
                            Sys_Shutdown();
                        else if(reset == 1)
                            Sys_Reboot();

                        else if(homo.GetState() == STATE_CLICKED)
                        {
                            cfg_save_global();
                            optionBrowser2.SetState(STATE_DISABLED);
                            s32 thetimeofbg = bgMusic->GetPlayTime();
                            bgMusic->Stop();
                            choice = WindowExitPrompt(tr("Exit USB Loader GX?"),0, tr("Back to Loader"),tr("Wii Menu"),tr("Back"),0);
                            if(!strcmp("", Settings.oggload_path) || !strcmp("notset", Settings.ogg_path))
                            {
                                bgMusic->Play();
                            } else {
                                bgMusic->PlayOggFile(Settings.ogg_path);
                            }
                            bgMusic->SetPlayTime(thetimeofbg);
                            SetVolumeOgg(255*(Settings.volume/100.0));
                            if(choice == 3) {
                                Sys_LoadMenu(); // Back to System Menu
                            } else if (choice == 2) {
                                Sys_BackToLoader();
                            } else {
                                homo.ResetState();
                            }
                            optionBrowser2.SetState(STATE_DEFAULT);
                        }

                        ret = optionBrowser2.GetClickedOption();

                        switch (ret)
                        {
                            case 0:
                                //if(isSdInserted())
								if(isInserted(bootDevice))
                                {
                                    w.SetEffect(EFFECT_FADE, -20);
                                    while(w.GetEffect()>0) usleep(50);
                                    mainWindow->Remove(&w);
                                    while(returnhere)
                                        returnhere = MenuOGG();
                                    HaltGui();
                                    mainWindow->Append(&w);
                                    w.SetEffect(EFFECT_FADE, 20);
                                    ResumeGui();
                                    while(w.GetEffect()>0) usleep(50);
                                }
                                else
                                    WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to use this option."), tr("OK"), 0,0,0);
                                break;
                            case 1:
                                Settings.volume += 10;
                                if(Settings.volume > 100)
                                    Settings.volume = 0;
                                SetVolumeOgg(255*(Settings.volume/100.0));
                                break;
                            case 2:
                                Settings.sfxvolume += 10;
                                if(Settings.sfxvolume > 100)
                                    Settings.sfxvolume = 0;
                                btnSoundOver.SetVolume(Settings.sfxvolume);
                                btnClick.SetVolume(Settings.sfxvolume);
                                btnClick1.SetVolume(Settings.sfxvolume);
                                break;
                        }
                    }
                    optionBrowser2.SetEffect(EFFECT_FADE, -20);
                    while(optionBrowser2.GetEffect() > 0) usleep(50);
                    titleTxt.SetText(tr("Settings"));
                    slidedirection = FADE;
                    pageToDisplay = 1;
                    MainButton4.ResetState();
                    break;
                }
			}

			if ( pageToDisplay == 2)
			{
                if(MainButton1.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton1.GetEffect() > 0) usleep(50);
                    HaltGui();
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
                    titleTxt.SetText(tr("Custom Paths"));
                    exit = false;
					for(int i = 0; i <= MAXOPTIONS; i++) options2.SetName(i, NULL);
                    if(Settings.godmode)
                    options2.SetName(0, "%s", tr("Cover Path"));
                    options2.SetName(1, "%s", tr("Discimage Path"));
                    options2.SetName(2, "%s", tr("ThemePath"));
                    options2.SetName(3, "%s", tr("titles.txt Path"));
                    options2.SetName(4, "%s", tr("Updatepath"));
                    options2.SetName(5, "%s", tr("Cheatcodes Path"));
                    options2.SetName(6, "%s", tr("Dol Path"));
                    for(int i = 0; i <= MAXOPTIONS; i++) options2.SetValue(i, NULL);
                    w.Append(&optionBrowser2);
                    optionBrowser2.SetClickable(true);
                    ResumeGui();

                    VIDEO_WaitVSync ();
                    optionBrowser2.SetEffect(EFFECT_FADE, 20);
			        while(optionBrowser2.GetEffect() > 0) usleep(50);

			        if(Settings.godmode) {

                    while(!exit)
                    {
                        VIDEO_WaitVSync ();

                        options2.SetValue(0, "%s", Settings.covers_path);
                        options2.SetValue(1, "%s", Settings.disc_path);
                        options2.SetValue(2, "%s", CFG.theme_path);
                        options2.SetValue(3, "%s", Settings.titlestxt_path);
                        options2.SetValue(4, "%s", Settings.update_path);
                        options2.SetValue(5, "%s", Settings.Cheatcodespath);
						options2.SetValue(6, "%s", Settings.dolpath);

                        if(backBtn.GetState() == STATE_CLICKED)
                        {
                            backBtn.ResetState();
                            exit = true;
                            break;
                        }

                        if(shutdown == 1)
                            Sys_Shutdown();
                        else if(reset == 1)
                            Sys_Reboot();

                        else if(homo.GetState() == STATE_CLICKED)
                        {
                            cfg_save_global();
                            optionBrowser2.SetState(STATE_DISABLED);
                            s32 thetimeofbg = bgMusic->GetPlayTime();
                            bgMusic->Stop();
                            choice = WindowExitPrompt(tr("Exit USB Loader GX?"),0, tr("Back to Loader"),tr("Wii Menu"),tr("Back"),0);
                            if(!strcmp("", Settings.oggload_path) || !strcmp("notset", Settings.ogg_path))
                            {
                                bgMusic->Play();
                            } else {
                                bgMusic->PlayOggFile(Settings.ogg_path);
                            }
                            bgMusic->SetPlayTime(thetimeofbg);
                            SetVolumeOgg(255*(Settings.volume/100.0));
                            if(choice == 3) {
                                Sys_LoadMenu(); // Back to System Menu
                            } else if (choice == 2) {
                                Sys_BackToLoader();
                            } else {
                                homo.ResetState();
                            }
                            optionBrowser2.SetState(STATE_DEFAULT);
                        }

                        ret = optionBrowser2.GetClickedOption();

                        switch (ret)
                        {
                            case 0:
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[43] = "";
                                    strncpy(entered, Settings.covers_path, sizeof(entered));
                                    int result = OnScreenKeyboard(entered,43,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 )
                                    {
                                        int len = (strlen(entered)-1);
                                        if(entered[len] !='/')
                                        strncat (entered, "/", 1);
                                        strncpy(Settings.covers_path, entered, sizeof(Settings.covers_path));
                                        WindowPrompt(tr("Coverpath Changed"),0,tr("OK"),0,0,0);
//                                        if(!isSdInserted()) {
										if(!isInserted(bootDevice)) {
                                          WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to save."), tr("OK"), 0,0,0);
                                        }
                                    }
                                } else {
                                    WindowPrompt(tr("Coverpath Changed"),tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                }
                                break;
                            case 1:
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[43] = "";
                                    strncpy(entered, Settings.disc_path, sizeof(entered));
                                    int result = OnScreenKeyboard(entered, 43,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 )
                                    {
                                        int len = (strlen(entered)-1);
                                        if(entered[len] !='/')
                                        strncat (entered, "/", 1);
                                        strncpy(Settings.disc_path, entered, sizeof(Settings.disc_path));
                                        WindowPrompt(tr("Discpath Changed"),0,tr("OK"),0,0,0);
//                                        if(!isSdInserted()) {
										if(!isInserted(bootDevice)) {
                                            WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to save."), tr("OK"), 0,0,0);
                                        }
                                    }
                                } else {
                                    WindowPrompt(tr("Discpath change"),tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                }
                                break;
                            case 2:
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[43] = "";
                                    strncpy(entered, CFG.theme_path, sizeof(entered));
                                    int result = OnScreenKeyboard(entered, 43,0);
                                    HaltGui();
                                    w.RemoveAll();
                                    if ( result == 1 )
                                    {
                                        int len = (strlen(entered)-1);
                                        if(entered[len] !='/')
                                        strncat (entered, "/", 1);
                                        strncpy(CFG.theme_path, entered, sizeof(CFG.theme_path));
                                        WindowPrompt(tr("Themepath Changed"),0,tr("OK"),0,0,0);
//                                        if(!isSdInserted()) {
										if(!isInserted(bootDevice)) {
                                            WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to save."), tr("OK"), 0,0,0);
                                        } else {
                                            cfg_save_global();
                                        }
                                        mainWindow->Remove(bgImg);
                                        CFG_Load();
                                        CFG_LoadGlobal();
                                        menu = MENU_SETTINGS;
                                        #ifdef HW_RVL
                                        snprintf(imgPath, sizeof(imgPath), "%splayer1_point.png", CFG.theme_path);
                                        pointer[0] = new GuiImageData(imgPath, player1_point_png);
                                        snprintf(imgPath, sizeof(imgPath), "%splayer2_point.png", CFG.theme_path);
                                        pointer[1] = new GuiImageData(imgPath, player2_point_png);
                                        snprintf(imgPath, sizeof(imgPath), "%splayer3_point.png", CFG.theme_path);
                                        pointer[2] = new GuiImageData(imgPath, player3_point_png);
                                        snprintf(imgPath, sizeof(imgPath), "%splayer4_point.png", CFG.theme_path);
                                        pointer[3] = new GuiImageData(imgPath, player4_point_png);
                                        #endif
                                        if (CFG.widescreen)
                                        snprintf(imgPath, sizeof(imgPath), "%swbackground.png", CFG.theme_path);
                                            else
                                        snprintf(imgPath, sizeof(imgPath), "%sbackground.png", CFG.theme_path);

                                        background = new GuiImageData(imgPath, CFG.widescreen? wbackground_png : background_png);

                                        bgImg = new GuiImage(background);
                                        mainWindow->Append(bgImg);
                                        mainWindow->Append(&w);
                                    }
                                    w.Append(&settingsbackground);
                                    w.Append(&titleTxt);
                                    w.Append(&backBtn);
                                    w.Append(&optionBrowser2);
                                    ResumeGui();
                                } else {
                                    WindowPrompt(tr("Themepath change"),tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                }
                                break;
                            case 3:
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[43] = "";
                                    strncpy(entered, Settings.titlestxt_path, sizeof(entered));
                                    int result = OnScreenKeyboard(entered,43,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 )
                                    {
                                        int len = (strlen(entered)-1);
                                        if(entered[len] !='/')
                                        strncat (entered, "/", 1);
                                        strncpy(Settings.titlestxt_path, entered, sizeof(Settings.titlestxt_path));
                                        WindowPrompt(tr("Path of titles.txt changed."),0,tr("OK"),0,0,0);
//                                        if(isSdInserted()) {
										if(isInserted(bootDevice)) {
                                            cfg_save_global();
                                            CFG_Load();
                                        } else {
                                            WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to save."), tr("OK"), 0,0,0);
                                        }
                                    }
                                }
                                else
                                {
                                    WindowPrompt(tr("Path of titles.txt change"),tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                }
                                break;
                            case 4:
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[43] = "";
                                    strncpy(entered, Settings.update_path, sizeof(entered));
                                    int result = OnScreenKeyboard(entered,43,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 )
                                    {
                                        int len = (strlen(entered)-1);
                                        if(entered[len] !='/')
                                        strncat (entered, "/", 1);
                                        strncpy(Settings.update_path, entered, sizeof(Settings.update_path));
                                        WindowPrompt(tr("Updatepath changed."),0,tr("OK"),0,0,0);
                                    }
                                }
                                else
                                    WindowPrompt(0,tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                break;
                            case 5:
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[43] = "";
                                    strncpy(entered, Settings.Cheatcodespath, sizeof(entered));
                                    int result = OnScreenKeyboard(entered,43,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 )
                                    {
                                        int len = (strlen(entered)-1);
                                        if(entered[len] !='/')
                                        strncat (entered, "/", 1);
                                        strncpy(Settings.Cheatcodespath, entered, sizeof(Settings.Cheatcodespath));
                                        WindowPrompt(tr("Cheatcodes Path changed"),0,tr("OK"),0,0,0);
                                    }
                                }
                                else
                                    WindowPrompt(0,tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                break;
							case 6:
                                if ( Settings.godmode == 1)
                                {
                                    w.Remove(&optionBrowser2);
                                    w.Remove(&backBtn);
                                    char entered[43] = "";
                                    strncpy(entered, Settings.dolpath, sizeof(entered));
                                    int result = OnScreenKeyboard(entered,43,0);
                                    w.Append(&optionBrowser2);
                                    w.Append(&backBtn);
                                    if ( result == 1 )
                                    {
                                        int len = (strlen(entered)-1);
                                        if(entered[len] !='/')
                                        strncat (entered, "/", 1);
                                        strncpy(Settings.dolpath, entered, sizeof(Settings.dolpath));
                                        WindowPrompt(tr("Dolpath Changed"),0,tr("OK"),0,0,0);
//                                        if(!isSdInserted()) {
										if(!isInserted(bootDevice)) {
                                          WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to save."), tr("OK"), 0,0,0);
                                        }
                                    }
                                } else {
                                    WindowPrompt(tr("Dolpath change"),tr("Console should be unlocked to modify it."),tr("OK"),0,0,0);
                                }
                                break;

                        }
                    }
                    /** If not godmode don't let him inside **/
                    } else {
                        WindowPrompt(tr("Console Locked"), tr("Unlock console to use this option."), tr("OK"), 0, 0, 0);
                    }
                    optionBrowser2.SetEffect(EFFECT_FADE, -20);
                    while(optionBrowser2.GetEffect() > 0) usleep(50);
                    titleTxt.SetText(tr("Settings"));
                    slidedirection = FADE;
                    pageToDisplay = 2;
                    MainButton1.ResetState();
                    break;
                }

                if(MainButton2.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton2.GetEffect() > 0) usleep(50);
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
//                    if(isSdInserted() && Settings.godmode) {
					if(isInserted(bootDevice) && Settings.godmode) {
                    w.Remove(&optionBrowser2);
                    w.Remove(&backBtn);
                    int ret = ProgressUpdateWindow();
                    if(ret < 0) {
                        WindowPrompt(tr("Update failed"),0,tr("OK"),0,0,0);
                    }
                    w.Append(&optionBrowser2);
                    w.Append(&backBtn);
                    } else {
                        WindowPrompt(tr("Console Locked"), tr("Unlock console to use this option."), tr("OK"), 0,0,0);
                    }
                    slidedirection = FADE;
                    pageToDisplay = 2;
                    MainButton2.ResetState();
                    break;
                }

                if(MainButton3.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton3.GetEffect() > 0) usleep(50);
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
                    w.Remove(&backBtn);
                    w.Remove(&optionBrowser2);
                    if(Settings.godmode) {
                    int choice = WindowPrompt(tr("Are you sure?"), 0, tr("Yes"), tr("Cancel"), 0, 0);
                    if(choice == 1) {
//							if(isSdInserted())
							if(isInserted(bootDevice))
							{
								char GXGlobal_cfg[26];
								sprintf(GXGlobal_cfg, "%s/config/GXGlobal.cfg", bootDevice);
								remove(GXGlobal_cfg);
							}
							gettextCleanUp();
							//lang_default();
							CFG_Load();
							menu = MENU_SETTINGS;
							pageToDisplay = 0;
                    }
                    } else {
                        WindowPrompt(tr("Console Locked"), tr("Unlock console to use this option."), tr("OK"), 0, 0, 0);
                    }
                    w.Append(&backBtn);
                    w.Append(&optionBrowser2);
                    slidedirection = FADE;
                    pageToDisplay = 2;
                    MainButton3.ResetState();
                    break;
                }

                if(MainButton4.GetState() == STATE_CLICKED) {
                    MainButton1.SetEffect(EFFECT_FADE, -20);
                    MainButton2.SetEffect(EFFECT_FADE, -20);
                    MainButton3.SetEffect(EFFECT_FADE, -20);
                    MainButton4.SetEffect(EFFECT_FADE, -20);
                    while(MainButton4.GetEffect() > 0) usleep(50);
                    w.Remove(&PageIndicatorBtn1);
                    w.Remove(&PageIndicatorBtn2);
                    w.Remove(&GoRightBtn);
                    w.Remove(&GoLeftBtn);
                    w.Remove(&MainButton1);
                    w.Remove(&MainButton2);
                    w.Remove(&MainButton3);
                    w.Remove(&MainButton4);
                    WindowCredits();
                    slidedirection = FADE;
                    pageToDisplay = 2;
                    MainButton4.ResetState();
                    break;
                }
			}

			if(shutdown == 1)
				Sys_Shutdown();
			if(reset == 1)
				Sys_Reboot();

			if(backBtn.GetState() == STATE_CLICKED)
			{
				//Add the procedure call to save the global configuration
//				if(isSdInserted()) {
				if(isInserted(bootDevice)) {
				cfg_save_global();
				}
				menu = MENU_DISCLIST;
				pageToDisplay = 0;
				break;
			}

			if(GoLeftBtn.GetState() == STATE_CLICKED)
			{
                pageToDisplay--;
                /** Change direction of the flying buttons **/
                if(pageToDisplay < 1)
                pageToDisplay = 2;
                slidedirection = LEFT;
                GoLeftBtn.ResetState();
                break;
			}

			if(GoRightBtn.GetState() == STATE_CLICKED)
			{
                pageToDisplay++;
                /** Change direction of the flying buttons **/
                if(pageToDisplay > 2)
                pageToDisplay = 1;
                slidedirection = RIGHT;
                GoRightBtn.ResetState();
                break;
			}

			if(PageIndicatorBtn1.GetState() == STATE_CLICKED)
			{
			    if(pageToDisplay == 2) {
                slidedirection = LEFT;
                pageToDisplay = 1;
                PageIndicatorBtn1.ResetState();
                break;
                }
                PageIndicatorBtn1.ResetState();
			}
			else if(PageIndicatorBtn2.GetState() == STATE_CLICKED)
			{
                if(pageToDisplay == 1) {
                slidedirection = RIGHT;
                pageToDisplay = 2;
                PageIndicatorBtn2.ResetState();
                break;
                } else
                PageIndicatorBtn2.ResetState();
			}

			if(homo.GetState() == STATE_CLICKED)
			{
			    cfg_save_global();
				optionBrowser2.SetState(STATE_DISABLED);
				s32 thetimeofbg = bgMusic->GetPlayTime();
				bgMusic->Stop();
				choice = WindowExitPrompt(tr("Exit USB Loader GX?"),0, tr("Back to Loader"),tr("Wii Menu"),tr("Back"),0);
				if(!strcmp("", Settings.oggload_path) || !strcmp("notset", Settings.ogg_path))
				{
					bgMusic->Play();
				} else {
					bgMusic->PlayOggFile(Settings.ogg_path);
				}
				bgMusic->SetPlayTime(thetimeofbg);
				SetVolumeOgg(255*(Settings.volume/100.0));

				if(choice == 3)
				{
					Sys_LoadMenu(); // Back to System Menu
				} else if (choice == 2) {
					Sys_BackToLoader();
				} else {
					homo.ResetState();
				}
                optionBrowser2.SetState(STATE_DEFAULT);
			}
		}
	}

	w.SetEffect(EFFECT_FADE, -20);
	while(w.GetEffect()>0) usleep(50);


	// if language has changed, reload titles
	int opt_langnew = 0;
	opt_langnew = Settings.language;
	if (Settings.titlesOverride==1 && opt_lang != opt_langnew) {
		OpenXMLDatabase(Settings.titlestxt_path, Settings.db_language, Settings.db_JPtoEN, true, true, false); // open file, reload titles, do not keep in memory
		menu = MENU_DISCLIST;
	}

	HaltGui();

	mainWindow->RemoveAll();
	mainWindow->Append(bgImg);

	ResumeGui();
	return menu;
}


/********************************************************************************
*Game specific settings
*********************************************************************************/
int GameSettings(struct discHdr * header)
{
	bool exit = false;
	int ret;
	int retVal = 0;

	char gameName[31];

	if (strlen(get_title(header)) < (27 + 3)) {
		sprintf(gameName, "%s", get_title(header));
	}
	else {
		strncpy(gameName, get_title(header),  27);
		gameName[27] = '\0';
		strncat(gameName, "...", 3);
	}

	customOptionList options3(12);
	options3.SetName(0,"%s", tr("Video Mode"));
	options3.SetName(1,"%s", tr("VIDTV Patch"));
	options3.SetName(2,"%s", tr("Game Language"));
	options3.SetName(3, "Ocarina");
	options3.SetName(4, "IOS");
	options3.SetName(5,"%s", tr("Parental control"));
	options3.SetName(6,"%s", tr("Error 002 fix"));
	options3.SetName(7,"%s", tr("Patch Country Strings"));
	options3.SetName(8,"%s", tr("Alternate DOL"));
	options3.SetName(9,"%s", tr("Block IOS Reload"));
	options3.SetName(10,"%s", tr("Reset Playcounter"));
	options3.SetName(11,"%s", tr("Default Gamesettings"));

	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND_PCM, Settings.sfxvolume);
	GuiSound btnClick(button_click2_pcm, button_click2_pcm_size, SOUND_PCM, Settings.sfxvolume);

	char imgPath[100];

	snprintf(imgPath, sizeof(imgPath), "%sbutton_dialogue_box.png", CFG.theme_path);
	GuiImageData btnOutline(imgPath, button_dialogue_box_png);
	snprintf(imgPath, sizeof(imgPath), "%sgamesettings_background.png", CFG.theme_path);
	GuiImageData settingsbg(imgPath, settings_background_png);

    GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
    GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, 0);
	GuiTrigger trigB;
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

    GuiText titleTxt(get_title(header), 28, (GXColor){0, 0, 0, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(12,40);
	titleTxt.SetMaxWidth(356, GuiText::SCROLL);

    GuiImage settingsbackground(&settingsbg);
	GuiButton settingsbackgroundbtn(settingsbackground.GetWidth(), settingsbackground.GetHeight());
	settingsbackgroundbtn.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	settingsbackgroundbtn.SetPosition(0, 0);
	settingsbackgroundbtn.SetImage(&settingsbackground);

    GuiText saveBtnTxt(tr("Save"), 22, (GXColor){THEME.prompttxt_r, THEME.prompttxt_g, THEME.prompttxt_b, 255});
	saveBtnTxt.SetMaxWidth(btnOutline.GetWidth()-30);
	GuiImage saveBtnImg(&btnOutline);
	if (Settings.wsprompt == yes){
	saveBtnTxt.SetWidescreen(CFG.widescreen);
	saveBtnImg.SetWidescreen(CFG.widescreen);}
	GuiButton saveBtn(&saveBtnImg,&saveBtnImg, 2, 3, -180, 400, &trigA, &btnSoundOver, &btnClick,1);
	saveBtn.SetScale(0.9);
	saveBtn.SetLabel(&saveBtnTxt);

    GuiText cancelBtnTxt(tr("Back"), 22, (GXColor){THEME.prompttxt_r, THEME.prompttxt_g, THEME.prompttxt_b, 255});
	cancelBtnTxt.SetMaxWidth(btnOutline.GetWidth()-30);
	GuiImage cancelBtnImg(&btnOutline);
	if (Settings.wsprompt == yes){
	cancelBtnTxt.SetWidescreen(CFG.widescreen);
	cancelBtnImg.SetWidescreen(CFG.widescreen);}
	GuiButton cancelBtn(&cancelBtnImg,&cancelBtnImg, 2, 3, 180, 400, &trigA, &btnSoundOver, &btnClick,1);
	cancelBtn.SetScale(0.9);
	cancelBtn.SetLabel(&cancelBtnTxt);
	cancelBtn.SetTrigger(&trigB);

	GuiText deleteBtnTxt(tr("Uninstall"), 22, (GXColor){THEME.prompttxt_r, THEME.prompttxt_g, THEME.prompttxt_b, 255});
	deleteBtnTxt.SetMaxWidth(btnOutline.GetWidth()-30);
	GuiImage deleteBtnImg(&btnOutline);
	if (Settings.wsprompt == yes){
	deleteBtnTxt.SetWidescreen(CFG.widescreen);
	deleteBtnImg.SetWidescreen(CFG.widescreen);}
	GuiButton deleteBtn(&deleteBtnImg,&deleteBtnImg, 2, 3, 0, 400, &trigA, &btnSoundOver, &btnClick,1);
	deleteBtn.SetScale(0.9);
	deleteBtn.SetLabel(&deleteBtnTxt);

	GuiCustomOptionBrowser optionBrowser3(396, 280, &options3, CFG.theme_path, "bg_options_gamesettings.png", bg_options_settings_png, 1, 200);
	optionBrowser3.SetPosition(0, 90);
	optionBrowser3.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);

    HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&settingsbackgroundbtn);
    w.Append(&titleTxt);
	w.Append(&deleteBtn);
	w.Append(&saveBtn);
	w.Append(&cancelBtn);
    w.Append(&optionBrowser3);

    mainWindow->Append(&w);

	struct Game_CFG* game_cfg = CFG_get_game_opt(header->id);

	if (game_cfg)//if there are saved settings for this game use them
	{
		videoChoice = game_cfg->video;
		languageChoice = game_cfg->language;
		ocarinaChoice = game_cfg->ocarina;
		viChoice = game_cfg->vipatch;
		iosChoice = game_cfg->ios;
		parentalcontrolChoice = game_cfg->parentalcontrol;
		fix002 = game_cfg->errorfix002;
		countrystrings = game_cfg->patchcountrystrings;
		alternatedol = game_cfg->loadalternatedol;
		reloadblock = game_cfg->iosreloadblock;
	}
	else
	{
		videoChoice = Settings.video;
		languageChoice = Settings.language;
		ocarinaChoice = Settings.ocarina;
		viChoice = Settings.vpatch;
		if(Settings.cios == ios222) {
        iosChoice = i222;
		} else {
		iosChoice = i249;
		}
		parentalcontrolChoice = 0;
		fix002 = Settings.error002;
		countrystrings = Settings.patchcountrystrings;
		alternatedol = off;
		reloadblock = off;
	}

	int opt_lang = languageChoice; // backup language setting

	ResumeGui();

	while(!exit)
	{
		VIDEO_WaitVSync();

		if (videoChoice == discdefault) options3.SetValue(0,"%s",tr("Disc Default"));
		else if (videoChoice == systemdefault) options3.SetValue(0,"%s",tr("System Default"));
		else if (videoChoice == patch) options3.SetValue(0,"%s",tr("AutoPatch"));
		else if (videoChoice == pal50) options3.SetValue(0,"%s PAL50",tr("Force"));
		else if (videoChoice == pal60) options3.SetValue(0,"%s PAL60",tr("Force"));
		else if (videoChoice == ntsc) options3.SetValue(0,"%s NTSC",tr("Force"));

        if (viChoice == on) options3.SetValue(1,"%s",tr("ON"));
		else if (viChoice == off) options3.SetValue(1,"%s",tr("OFF"));

		if (languageChoice == ConsoleLangDefault) options3.SetValue(2,"%s",tr("Console Default"));
		else if (languageChoice == jap) options3.SetValue(2,"%s",tr("Japanese"));
		else if (languageChoice == ger) options3.SetValue(2,"%s",tr("German"));
		else if (languageChoice == eng) options3.SetValue(2,"%s",tr("English"));
		else if (languageChoice == fren) options3.SetValue(2,"%s",tr("French"));
		else if (languageChoice == esp) options3.SetValue(2,"%s",tr("Spanish"));
        else if (languageChoice == it) options3.SetValue(2,"%s",tr("Italian"));
		else if (languageChoice == dut) options3.SetValue(2,"%s",tr("Dutch"));
		else if (languageChoice == schin) options3.SetValue(2,"%s",tr("SChinese"));
		else if (languageChoice == tchin) options3.SetValue(2,"%s",tr("TChinese"));
		else if (languageChoice == kor) options3.SetValue(2,"%s",tr("Korean"));

        if (ocarinaChoice == on) options3.SetValue(3,"%s",tr("ON"));
		else if (ocarinaChoice == off) options3.SetValue(3,"%s",tr("OFF"));

		if (iosChoice == i249) options3.SetValue(4,"249");
		else if (iosChoice == i222) options3.SetValue(4,"222");
		else if (iosChoice == i223) options3.SetValue(4,"223");

		if (parentalcontrolChoice == 0) options3.SetValue(5, tr("0 (Everyone)"));
		else if (parentalcontrolChoice == 1) options3.SetValue(5, tr("1 (Child 7+)"));
		else if (parentalcontrolChoice == 2) options3.SetValue(5, tr("2 (Teen 12+)"));
		else if (parentalcontrolChoice == 3) options3.SetValue(5, tr("3 (Mature 16+)"));
		else if (parentalcontrolChoice == 4) options3.SetValue(5, tr("4 (Adults Only 18+)"));

        if (fix002 == on) options3.SetValue(6,tr("ON"));
		else if (fix002 == off) options3.SetValue(6,tr("OFF"));

        if (countrystrings == on) options3.SetValue(7,tr("ON"));
		else if (countrystrings == off) options3.SetValue(7,tr("OFF"));

        if (alternatedol == on) options3.SetValue(8,tr("ON"));
		else if (alternatedol == off) options3.SetValue(8,tr("OFF"));

        if (reloadblock == on) options3.SetValue(9,tr("ON"));
		else if (reloadblock == off) options3.SetValue(9,tr("OFF"));

        options3.SetValue(10, NULL);
        options3.SetValue(11, NULL);

		if(shutdown == 1)
			Sys_Shutdown();
		if(reset == 1)
			Sys_Reboot();

		ret = optionBrowser3.GetClickedOption();

		switch (ret)
		{
			case 0:
				videoChoice = (videoChoice + 1) % CFG_VIDEO_COUNT;
				break;
			case 1:
				viChoice = (viChoice + 1) % 2;
				break;
            case 2:
				languageChoice = (languageChoice + 1) % CFG_LANG_COUNT;
				break;
            case 3:
				ocarinaChoice = (ocarinaChoice + 1) % 2;
				break;
			case 4:
				iosChoice = (iosChoice + 1) % 3;
				break;
			case 5:
				parentalcontrolChoice = (parentalcontrolChoice + 1) % 5;
				break;
            case 6:
                fix002 = (fix002+1) % 2;
                break;
            case 7:
                countrystrings = (countrystrings+1) % 2;
                break;
            case 8:
                alternatedol = (alternatedol+1) % 2;
                break;
            case 9:
                reloadblock = (reloadblock+1) % 2;
                break;
            case 10:
                int result;
				result = WindowPrompt(tr("Are you sure?"),0,tr("Yes"),tr("Cancel"),0,0);
				if(result == 1) {
					if(isInserted(bootDevice)) {
					struct Game_NUM* game_num = CFG_get_game_num(header->id);
					if (game_num) {
						favoritevar = game_num->favorite;
						playcount = game_num->count;
					} else {
						favoritevar = 0;
						playcount = 0;
					}
					playcount = 0;
					CFG_save_game_num(header->id);
                }
				}
                break;
            case 11:
                int choice = WindowPrompt(tr("Are you sure?"),0,tr("Yes"),tr("Cancel"),0,0);
                if(choice == 1) {
                    videoChoice = Settings.video;
                    viChoice = Settings.vpatch;
                    languageChoice = Settings.language;
                    ocarinaChoice = Settings.ocarina;
                    fix002 = Settings.error002;
                    countrystrings = Settings.patchcountrystrings;
                    alternatedol = off;
                    reloadblock = off;
                    if(Settings.cios == ios222) {
                        iosChoice = i222;
                    } else {
                        iosChoice = i249;
                    }
                    parentalcontrolChoice = 0;
                    CFG_forget_game_opt(header->id);
					// if default language is different than language from main settings, reload titles
					int opt_langnew = 0;
					opt_langnew = Settings.language;
					if (Settings.titlesOverride==1 && opt_lang != opt_langnew)
						OpenXMLDatabase(Settings.titlestxt_path, Settings.db_language, Settings.db_JPtoEN, true, true, false); // open file, reload titles, do not keep in memory
						// titles are refreshed in menu.cpp as soon as this function returns
                }
                break;
		}

		if(saveBtn.GetState() == STATE_CLICKED)
		{
//			if(isSdInserted()) {
			if(isInserted(bootDevice)) {
                if (CFG_save_game_opt(header->id))
				{
					// if language has changed, reload titles
					int opt_langnew = 0;
					game_cfg = CFG_get_game_opt(header->id);
					if (game_cfg) opt_langnew = game_cfg->language;
					if (Settings.titlesOverride==1 && opt_lang != opt_langnew)
						OpenXMLDatabase(Settings.titlestxt_path, Settings.db_language, Settings.db_JPtoEN, true, true, false); // open file, reload titles, do not keep in memory
						// titles are refreshed in menu.cpp as soon as this function returns
					WindowPrompt(tr("Successfully Saved"), 0, tr("OK"), 0,0,0);
				}
				else
				{
					WindowPrompt(tr("Save Failed"), 0, tr("OK"), 0,0,0);
				}
		    } else {
                WindowPrompt(tr("No SD-Card inserted!"), tr("Insert an SD-Card to save."), tr("OK"), 0,0,0);
		    }

			saveBtn.ResetState();
			optionBrowser3.SetFocus(1);
		}

		if (cancelBtn.GetState() == STATE_CLICKED)
		{
			exit = true;
			break;
		}

		if (deleteBtn.GetState() == STATE_CLICKED)
		{
			int choice = WindowPrompt(
					tr("Do you really want to delete:"),
					gameName,
					tr("Yes"),tr("Cancel"),0,0);

			if (choice == 1)
			{
				CFG_forget_game_opt(header->id);
				CFG_forget_game_num(header->id);
				ret = WBFS_RemoveGame(header->id);
				if (ret < 0)
				{
					WindowPrompt(
					tr("Can't delete:"),
					gameName,
					tr("OK"),0,0,0);
				}
				else {
					//__Menu_GetEntries();
					WindowPrompt(
					tr("Successfully deleted:"),
					gameName,
					tr("OK"),0,0,0);
					retVal = 1;
				}
				break;
			}
			else if (choice == 0)
			{
				deleteBtn.ResetState();
				optionBrowser3.SetFocus(1);
			}

		}
	}

	HaltGui();
	mainWindow->Remove(&w);
	ResumeGui();

	return retVal;
}
