#include <gctypes.h>

#include "IosLoader.h"
#include "Controls/DeviceHandler.hpp"
#include "usbloader/usbstorage2.h"
#include "usbloader/disc.h"
#include "usbloader/wbfs.h"
#include "usbloader/wdvd.h"
#include "wad/nandtitle.h"
#include "mload/mload_modules.h"
#include "settings/CSettings.h"
#include "wad/nandtitle.h"
#include "mload/mload.h"
#include "mload/modules/ehcmodule_5.h"
#include "mload/modules/dip_plugin_249.h"
#include "mload/modules/odip_frag.h"
#include "utils/tools.h"
#include "gecko.h"


/******************************************************************************
 * Public Methods:
 ******************************************************************************/
/*
 * Check if the ios passed is a Hermes ios.
 */
bool IosLoader::IsHermesIOS(s32 ios)
{
    return (ios == 222 || ios == 223 || ios == 224 || ios == 202);
}

/*
 * Check if the ios passed is a Waninkoko ios.
 */
bool IosLoader::IsWaninkokoIOS(s32 ios)
{
    if(ios < 200 || ios > 255)
        return false;

    return !IsHermesIOS(ios);
}

/*
 * Check if the ios passed is a d2x ios.
 */
bool IosLoader::IsD2X(s32 ios)
{
    iosinfo_t *info = GetIOSInfo(ios);
    if(!info)
        return false;

    bool res = (strncasecmp(info->name, "d2x", 3) == 0);

    free(info);

    return res;
}

/*
 * Loads CIOS (If possible the one from the settings file).
 * @return 0 if a cios has been successfully loaded. Else a value below 0 is returned.
 */
s32 IosLoader::LoadAppCios()
{
    u32 activeCios = IOS_GetVersion();
    s32 ret = -1;

    // We have what we need
    if((int) activeCios == Settings.cios)
        return 0;

    u32 ciosLoadPriority[] = { Settings.cios, 222, 249, 250, 245, 246, 247, 248 }; // Ascending.


    for (u32 i = 0; i < (sizeof(ciosLoadPriority)/sizeof(ciosLoadPriority[0])); ++i)
    {
        u32 cios = ciosLoadPriority[i];

        if (activeCios == cios)
        {
            ret = 0;
            break;
        }

        if ((ret = ReloadIosSafe(cios)) > -1)
        {
            // Remember working cios.
            Settings.cios = cios;
            break;
        }
    }

    return ret;
}


/*
 * Loads a CIOS before a game start.
 * @return 0 if a cios has been successfully loaded. Else a value below 0 is returned.
 */
s32 IosLoader::LoadGameCios(s32 ios)
{
    if(ios == IOS_GetVersion())
        return 0;

    s32 ret = -1;

    // Unmount fat before reloading IOS.
    WBFS_CloseAll();
    WDVD_Close();
    DeviceHandler::DestroyInstance();

    ret = ReloadIosSafe(ios);

    // Remount devices after reloading IOS.
    DeviceHandler::Instance()->MountAll();
    Disc_Init();

    return ret;
}

/*
 * Reloads a certain IOS under the condition, that an appropriate version of the IOS is installed.
 * @return a negative value if a safe reload of the ios was not possible.
 */
s32 IosLoader::ReloadIosSafe(s32 ios)
{
    if(IsHermesIOS(ios))
    {
        s32 iosRev = NandTitles.VersionOf(TITLE_ID(1, ios));
        if((iosRev < 4 || iosRev > 6) && iosRev != 65535)
            return -11;
    }
    else if(IsWaninkokoIOS(ios))
    {
        s32 iosRev = NandTitles.VersionOf(TITLE_ID(1, ios));
        if((iosRev < 9 || iosRev > 30000) && iosRev != 65535)  //let's see if Waninkoko actually gets to 30
            return -22;
    }
    else
    {
        return -33;
    }

    s32 r = IOS_ReloadIOS(ios);
    if (r >= 0) WII_Initialize();

    IosLoader::LoadIOSModules(IOS_GetVersion(), IOS_GetRevision());

    return r;
}

/******************************************************************************
 * Private/Protected Methods:
 ******************************************************************************/
void IosLoader::LoadIOSModules(s32 ios, s32 ios_rev)
{
    //! Hermes IOS
    if(IsHermesIOS(ios))
    {
        const u8 * ech_module = NULL;
        int ehc_module_size = 0;
        const u8 * dip_plugin = NULL;
        int dip_plugin_size = 0;

        ech_module = ehcmodule_5;
        ehc_module_size = size_ehcmodule_5;
        dip_plugin = odip_frag;
        dip_plugin_size = odip_frag_size;
        gprintf("Loading ehc v5 and opendip module\n");

        load_modules(ech_module, ehc_module_size, dip_plugin, dip_plugin_size);
    }
    //! Waninkoko IOS
    else if(IsWaninkokoIOS(ios))
    {
        iosinfo_t *info = GetIOSInfo(ios);
        if(ios_rev >= 18 && (!info || info->version < 6))
        {
            if(mload_init() < 0)
                return;

            gprintf("Loading dip module for Waninkoko's cios\n");
            mload_module((u8 *) dip_plugin_249, dip_plugin_249_size);
            mload_close();
        }

        if(info) free(info);
    }
}

/*
 * Reads the ios info struct from the .app file.
 * @return pointer to iosinfo_t on success else NULL. The user is responsible for freeing the buffer.
 */
iosinfo_t *IosLoader::GetIOSInfo(s32 ios)
{
	char filepath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(0x20);
	u64 TicketID = ((((u64) 1) << 32) | ios);
	u32 TMD_Length;

	s32 ret = ES_GetStoredTMDSize(TicketID, &TMD_Length);
	if (ret < 0)
		return NULL;

	signed_blob *TMD = (signed_blob*) memalign(32, ALIGN32(TMD_Length));
	if (!TMD)
		return NULL;

	ret = ES_GetStoredTMD(TicketID, TMD, TMD_Length);
	if (ret < 0)
	{
		free(TMD);
		return NULL;
	}

	sprintf(filepath, "/title/%08x/%08x/content/%08x.app", 0x00000001, ios, *(u8 *)((u32)TMD+0x1E7));

    free(TMD);

    u8 *buffer = NULL;
    u32 filesize = 0;

    NandTitle::LoadFileFromNand(filepath, &buffer, &filesize);

    if(!buffer)
		return NULL;

    iosinfo_t *iosinfo = (iosinfo_t *) buffer;

    if(iosinfo->magicword != 0x1ee7c105 || iosinfo->magicversion != 1)
    {
        free(buffer);
        return NULL;
    }

    return iosinfo;
}
