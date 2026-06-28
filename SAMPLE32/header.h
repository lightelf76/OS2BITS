#define INCL_LONGLONG
#include <os2def.h>
#define INCL_ERRORS
#include <bseerr.h>
#include <bsekee.h>
#include <devreqp.h>

APIRET APIENTRY InitComplete(void);
APIRET APIENTRY IOCtl(REQP_IOCTL * rp);

