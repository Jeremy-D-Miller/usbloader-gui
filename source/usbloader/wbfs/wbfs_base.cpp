#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <ogcsys.h>
#include <errno.h>

#include "Controls/DeviceHandler.hpp"
#include "usbloader/sdhc.h"
#include "usbloader/usbstorage2.h"
#include "usbloader/wbfs.h"
#include "wbfs_rw.h"

#include "wbfs_base.h"

Wbfs::Wbfs(u32 dev, u32 l, u32 s)
    : hdd(NULL), device(dev), lba(l), size(s)
{
}

s32 Wbfs::Init(u32 device)
{
    s32 ret;
	const DISC_INTERFACE * handle = DeviceHandler::GetUSBInterface();

    switch (device)
    {
        case WBFS_DEVICE_USB:
            /* Initialize USB storage */
            ret = handle->startup();
            if (ret)
            {
                currentHandle = handle;
                /* Setup callbacks */
                readCallback = __ReadUSB;
                writeCallback = __WriteUSB;
            }
            else
				return -1;
			break;
        case WBFS_DEVICE_SDHC:
            /* Initialize SDHC */
            ret = SDHC_Init();

            if (ret)
            {
                /* Setup callbacks */
                readCallback = __ReadSDHC;
                writeCallback = __WriteSDHC;
            }
            else return -1;
            break;
    }

    return 0;
}

// Default behavior: can't format
s32 Wbfs::Format()
{
    return -1;
}

s32 Wbfs::CheckGame(u8 *discid)
{
    wbfs_disc_t *disc = NULL;

    /* Try to open game disc */
    disc = OpenDisc(discid);
    if (disc)
    {
        /* Close disc */
        CloseDisc(disc);

        return 1;
    }

    return 0;
}

s32 Wbfs::GameSize(u8 *discid, f32 *size)
{
    wbfs_disc_t *disc = NULL;

    u32 sectors;

    /* Open disc */
    disc = OpenDisc(discid);
    if (!disc) return -2;

    /* Get game size in sectors */
    sectors = wbfs_sector_used(disc->p, disc->header);

    /* Copy value */
    *size = (disc->p->wbfs_sec_sz / GB_SIZE) * sectors;

    /* Close disc */
    CloseDisc(disc);

    return 0;
}

bool Wbfs::ShowFreeSpace(void)
{
    return true;
}