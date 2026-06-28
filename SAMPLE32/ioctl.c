#include "header.h"

APIRET APIENTRY IOCtl(REQP_IOCTL * rp)
{
   ULONG rc = ERROR_I24_BAD_COMMAND;

   if (0x80 != rp->category)
   {
      return ERROR_INVALID_CATEGORY;
   }
   switch (rp->function)
   {
      case 0:
      default:
         break;
   }
   return rc;
}
