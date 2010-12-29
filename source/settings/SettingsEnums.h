#ifndef SETTINGS_ENUMS_H_
#define SETTINGS_ENUMS_H_

#include "libs/libwbfs/wiidisc.h"

enum
{
    JAPANESE = 0,
    ENGLISH,
    GERMAN,
    FRENCH,
    SPANISH,
    ITALIAN,
    DUTCH,
    S_CHINESE,
    T_CHINESE,
    KOREAN,
    CONSOLE_DEFAULT,
    MAX_LANGUAGE,
};

enum
{
    VIDEO_MODE_SYSDEFAULT = 0,
    VIDEO_MODE_DISCDEFAULT,
    VIDEO_MODE_PAL50,
    VIDEO_MODE_PAL60,
    VIDEO_MODE_NTSC,
    VIDEO_MODE_PATCH,
    VIDEO_MODE_MAX
};

enum
{
    OFF = 0,
    ON,
    MAX_ON_OFF
};

enum
{
    CLOCK_HR12 = 1, // use OFF for clock off
    CLOCK_HR24,
    CLOCK_MAX
};


enum
{
    WIILIGHT_OFF = 0,
    WIILIGHT_ON,
    WIILIGHT_INSTALL,
    WIILIGHT_MAX

};

enum
{
    GAMEINFO_ID,
    GAMEINFO_REGION,
    GAMEINFO_BOTH,
    GAMEINFO_NONE,
    GAMEINFO_MAX
};

enum
{
    SCREENSAVER_3_MIN = 1,
    SCREENSAVER_5_MIN,
    SCREENSAVER_10_MIN,
    SCREENSAVER_20_MIN,
    SCREENSAVER_30_MIN,
    SCREENSAVER_60_MIN,
    SCREENSAVER_MAX
};

enum
{
    XFLIP_NO = 0,
    XFLIP_YES,
    XFLIP_SYSMENU,
    XFLIP_WTF,
    XFLIP_DISK3D,
    XFLIP_MAX
};

enum
{
    //! Sorting should be used as AND to allow favorite ABC/RANK
    SORT_ABC = 0x01,
    SORT_PLAYCOUNT = 0x02,
    SORT_RANKING = 0x04,
    SORT_FAVORITE = 0x08,
};
enum
{
    KEYBOARD_QWERTY = 0,
    KEYBOARD_DVORAK,
    KEYBOARD_QWERTZ,
    KEYBOARD_AZERTY,
    KEYBOARD_QWERTY2,
    KEYBOARD_MAX
};

enum
{
    INSTALL_TO_NO_DIR,
    INSTALL_TO_GAMEID_NAME,
    INSTALL_TO_NAME_GAMEID,
    INSTALL_TO_MAX
};

enum
{
    GAMESPLIT_NONE,
    GAMESPLIT_2GB,
    GAMESPLIT_4GB,
    GAMESPLIT_MAX
};

enum
{
    LIST_MODE,
    GRID_MODE,
    CAROUSEL_MODE
};

#endif
