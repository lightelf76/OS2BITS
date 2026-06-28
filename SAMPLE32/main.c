#include "header.h"

APIRET APIENTRY DriverEntry(REQP_ANY * rp)
{
   APIRET rc = 0;

   switch (rp->header.command)
   {
      case RP_INIT_COMPLETE:
         rc = InitComplete();
         break;

      case RP_IOCTL:
         rc = IOCtl((REQP_IOCTL *)rp);
         break;

      default:
         rc = RPERR_COMMAND;
   }

   if (NO_ERROR != rc)
   {
      rc |= RPERR;
   }
   return rc | RPDONE;

}
