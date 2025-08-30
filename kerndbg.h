#ifndef __H_KERNDBG__
#define __H_KERNDBG__

/* Based on DDK's devhlp.inc and some additional findings */

/* Register Driver with kernel debugger */
#define DH_REGISTER_KDD                83H
/* The only defined subfunction (passed in EAX) */
#define KDD_REGISTER_COMM              1
/* structure, pointer passed in ES:DI */
#pragma pack(push,1)
typedef struct KDDC
{
    ULONG          Flags;
    ULONG          IRQ;
    VOID   FAR *   pfnRouter;
    ULONG  FAR *   pStatus;
} KDDC;
#pragma pack(pop)

/* KDDC_Flags value */
/* Only test for KDD support */
#define KDDC_fTEST                     0x00000001 
/* Seems like not supported: pfnRouter and pStatus are 0:32 */
#define KDDC_fFLAT                     0x00000010 
/* Reboot if not KDD debugable */
#define KDDC_fREBOOT                   0x00000020 
/* Dump if not KDD debugable */
#define KDDC_fDUMP                     0x00000040 
/* Valid flag Mask */
#define KDDC_fVALID_MASK               0x00000060 

/* KDDC_IRQ is the number of IRQ needed for driver or -1 if driver needs no IRQ.
   IRQ-less driver is required for SMP Debug!
   Router can't use any Kernel services (DevHelp or so). All resources must be
   allocated before calling REGISTER_KDD.
   Router must implement basic byte-oriented send/recv/flush, much like UART,
   parameters for router call are passed in registers.
*/

/* Router commands - at AX register */
/* Send character */
#define KDDC_rcSEND                    0x00000001 
/* Entry:  AX = KDDC_rcSEND 
           CL = character to send
   Return: AX = 0
*/

/* Receive character */
#define KDDC_rcRECV                    0x00000002 
/* Entry:  AX = KDDC_rcRECV
           CX = 0 or KDDC_rmRECV_WAIT
   Return: if character is available
               AX = 0
               CL = <character>
           else if CX = 0
               AX = 1
           else
               wait indefinitely for character
*/

/* Flush buffer */
#define KDDC_rcFLUSH                   0x00000003 
/* Entry:  AX = KDDC_rcFLUSH
           CX = KDDC_rmFLUSH_SEND or 
           CX = KDDC_rmFLUSH_RECV
   Return: AX = 0 
   Remark: seems like both KDDC_rmFLUSH_SEND and CX = KDDC_rmFLUSH_RECV are never set
           simultaneously. Send or recv buffer must be flushed before return
*/

/* Router command modifier - at CX register */
/* Wait for character */
#define KDDC_rmRECV_WAIT               0x00000001 
/* Flush send buffer */
#define KDDC_rmFLUSH_SEND              0x00000001 
/* Flush recv buffer */
#define KDDC_rmFLUSH_RECV              0x00000002 

/* Status flags, located within driver */
/* KDD Comm Active - set/cleared by driver? */
#define KDDC_sfACTIVE                  0x00000001 
/* KDB Active - set/cleared by Kernel */
#define KDDC_sfKDB_ACTIVE              0x00000002 

#endif /* __H_KERNDBG__ */
