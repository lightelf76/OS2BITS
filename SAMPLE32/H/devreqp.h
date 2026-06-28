/*
 * Some type definitions for OS/2 2.x/3.x  driver request packets.
 */

#ifdef __cplusplus
extern "C" {
#endif
#if __WATCOMC__ >= 1000
#pragma pack(push,1);
#else
#pragma pack(1);
#endif

/*
 * Look at \WATCOM\H\OS2\DEVSYM.INC for a description
 * of the Device Driver Request Packet Definitions
 */

typedef struct REQP_HEADER REQP_HEADER;
struct REQP_HEADER
{
    BYTE                length;         // Length of request packet
    BYTE                unit;           // Unit code (B)
    BYTE                command;        // Command code
    USHORT              status;         // Status code
    ULONG               res1;           // Flags
    REQP_HEADER* _Seg16 next;           // Link to next request packet in queue
};

typedef struct
{
  REQP_HEADER header;
  union
  {
    struct
    {
        BYTE    res;            // Unused
        ULONG   devhlp;         // Address of Dev Help entry point
        PCHAR16 parms;          // Command-line arguments
        BYTE    drive;          // Drive number of first unit
    } in;
    struct
    {
        BYTE    units;          // Number of supported units
        USHORT  finalcs;        // Offset to end of code
        USHORT  finalds;        // Offset of end of data
        PVOID16 bpb;            // BIOS parameter block
    } out;
  };
} REQP_INIT;

typedef struct
{
    REQP_HEADER header;
    BYTE        media;          // Media descriptor
    ULONG       transaddr;      // Transfer physical address
    USHORT      count;          // bytes/sectors count
    ULONG       start;          // Starting sector for block device
    SHANDLE     fileid;         // System file number
} REQP_RWV;

typedef struct
{
    REQP_HEADER header;
    SHANDLE     fileid;         // System file number
} REQP_OPENCLOSE;

typedef struct {
    REQP_HEADER header;
    BYTE        category;         // category code
    BYTE        function;         // function code
    PVOID16     parms;            // &parameters
    PVOID16     buffer;           // &buffer
    SHANDLE     fileid;           // System file number
    USHORT      parmsize;
    USHORT      buffersize;
} REQP_IOCTL;


/*
 *  ADD-type device drivers Init request packet extensions
 */

typedef struct
{
    USHORT      cache_parm_list;        /* addr of InitCache_Parameter List  */
    USHORT      disk_config_table;      /* addr of disk_configuration table  */
    USHORT      init_req_vec_table;     /* addr of IRQ_Vector_Table          */
    USHORT      cmd_line_args;          /* addr of Command Line Args         */
    USHORT      machine_config_table;   /* addr of Machine Config Info       */
} DDD_PARM_LIST;

typedef struct
{
    ULONG       cache_size;             /* size of caching memory           */
    ULONG       cache_addr;             /* phys addr of caching memory      */
    USHORT      cache_thresh;
    USHORT      cache_msg_id;           /* Msg ID returned by cache init    */
    USHORT      cache_handle;           /* handle for cache buffers         */
} CACHEINIT_INPUT;

typedef struct
{
    USHORT      Length;                 /* Length of table                  */
    USHORT      BusInfo;                /* Machine Bus Type                 */
    USHORT      CpuInfo;                /* Machine CPU Type                 */
    UCHAR       SubModel;               /* Machine Submodel ID              */
    UCHAR       Model;                  /* Machine Model ID                 */
    USHORT      ABIOSRevision;          /* Abios Revision Level             */
    USHORT      HardDriveCount;         /* # of BIOS Fixed Drives           */
    UCHAR       Reserved1[20];
} MACHINE_CONFIG_INFO, far *PMACHINE_CONFIG_INFO;

#define CPUINFO_286     0
#define CPUINFO_386     1
#define CPUINFO_486     2
#define CPUINFO_PENTIUM 3

#define BUSINFO_MCA             0x01
#define BUSINFO_EISA            0x02
#define BUSINFO_ABIOS_SUPPORTED 0x04  /* ABIOS is supported  */
#define BUSINFO_ABIOS_PRESENT   0x08  /* ABIOS is present    */
#define BUSINFO_PCI             0x10  /* PCI BIOS detected   */
#define BUSINFO_OEM_ABIOS       0x20  /* Mfr is OEM, valid if ABIOS supported */
#define BUSINFO_PENTIUM_CPU     0x40  /* Pentium CPU detected */


typedef enum {
    RPERR               =   0x8000,     // Error
    RPDEV               =   0x4000,     // Device-specific error code
    RPBUSY              =   0x0200,     // Device is busy
    RPDONE              =   0x0100,     // Command complete
    RPERR_PROTECT       =   0x8000,     // Write-protect error
    RPERR_UNIT          =   0x8001,     // Unknown unit
    RPERR_READY         =   0x8002,     // Not ready
    RPERR_COMMAND       =   0x8003,     // Unknown command
    RPERR_CRC           =   0x8004,     // CRC error
    RPERR_LENGTH        =   0x8005,     // Bad request length
    RPERR_SEEK          =   0x8006,     // Seek error
    RPERR_FORMAT        =   0x8007,     // Unknown media
    RPERR_SECTOR        =   0x8008,     // Sector not found
    RPERR_PAPER         =   0x8009,     // Out of paper
    RPERR_WRITE         =   0x800A,     // Write fault
    RPERR_READ          =   0x800B,     // Read fault
    RPERR_GENERAL       =   0x800C,     // General failure
    RPERR_DISK          =   0x800D,     // Disk change
    RPERR_MEDIA         =   0x8010,     // Uncertain media
    RPERR_INTERRUPTED   =   0x8011,     // Call interrupted (character)
    RPERR_MONITOR       =   0x8012,     // Monitors unsupported
    RPERR_PARAMETER     =   0x8013,     // Invalid parameter
    RPERR_USE           =   0x8014,     // Device in use
    RPERR_INIT          =   0x8015,     // Non-critical init failure
    RPERR_FORCE_WORD    =   0xffff
} REQP_status;

typedef union {
    REQP_HEADER         header;
    REQP_INIT           init;
    REQP_RWV            rwv;
    REQP_OPENCLOSE      openclose;
    REQP_IOCTL          ioctl;
} REQP_ANY;

/*
 * Look at \WATCOM\H\OS2\DEVCMD.INC for a list
 * of the Device Driver Request Packet Commands
 */

typedef enum {
     RP_INIT            =   0x00,
     RP_MEDIA_CHECK     =   0x01,
     RP_BUILD_BPB       =   0x02,
     RP_READ            =   0x04,
     RP_READ_NO_WAIT    =   0x05,
     RP_INPUT_STATUS    =   0x06,
     RP_INPUT_FLUSH     =   0x07,
     RP_WRITE           =   0x08,
     RP_WRITE_VERIFY    =   0x09,
     RP_OUTPUT_STATUS   =   0x0a,
     RP_OUTPUT_FLUSH    =   0x0b,
     RP_OPEN            =   0x0d,
     RP_CLOSE           =   0x0e,
     RP_REMOVABLE       =   0x0f,
     RP_IOCTL           =   0x10,
     RP_RESET           =   0x11,
     RP_GET_DRIVE_MAP   =   0x12,
     RP_SET_DRIVE_MAP   =   0x13,
     RP_DEINSTALL       =   0x14,
     RP_PARTITIONABLE   =   0x16,
     RP_GET_FIXED_MAP   =   0x17,
     RP_BASEDEV_INIT    =   0x1b,
     RP_SHUTDOWN        =   0x1c,
     RP_GET_DRIVER_CAPS =   0x1d,
     RP_INIT_COMPLETE   =   0x1f,
     RP_END             =   0xff
} REQP_command;

#if __WATCOMC__ >= 1000
#pragma pack(pop);
#else
#pragma pack();
#endif
#ifdef __cplusplus
};
#endif
