#include <stdint.h>
// based on the SECPACK secure.h,  with some Long File support findings
// SecHlp functions are 32-bit flat with 16-bit stack (SS != DS)

#define SECCALL             _System
#define LARGE_FILE_SUPPORT          

#define DEVHLP_SECURITY     0x44
#define DHSEC_GETEXPORT     0x48a78df8

#define SEC_EXPORT_MAJOR_VERSION 0x0001

#ifdef LARGE_FILE_SUPPORT
#define SEC_EXPORT_MINOR_VERSION 0x0001
#else
#define SEC_EXPORT_MINOR_VERSION 0x0000
#endif

#pragma pack(push, 2)
typedef struct {
    PSZ    pszPath;      // well formed path
    ULONG  ulHandle;     // search handle
    ULONG  rc;           // rc user got from findfirst
    PUSHORT pResultCnt;  // count of found files
    USHORT usReqCnt;     // count user requested
    USHORT usLevel;      // search level
    USHORT usBufSize;    // user buffer size
    USHORT fPosition;    // use position information?
    PCHAR  pcBuffer;     // ptr to user buffer
    ULONG  Position;     // Position for restarting search
    PSZ    pszPosition;  // file to restart search with
}  FINDPARMS, *PFINDPARMS;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct sec_export_s 
{
    USHORT  seVersionMajor;
    USHORT  seVersionMinor;
    // p16Addr must be a valid 16:16 pBuffer alias (if available) or NULL
    // Offset is byte offset within file or -1 for current offset
    ULONG   (* SECCALL SecHlpRead) (ULONG SFN, PULONG pcbBytes, PUCHAR pBuffer, ULONG p16Addr, ULONG Offset);
    ULONG   (* SECCALL SecHlpWrite) (ULONG SFN, PULONG pcbBytes, PUCHAR pBuffer, ULONG p16Addr, ULONG Offset);
    // ulOpenFlags and ulOpenMode are same as used for DosOpen
    ULONG   (* SECCALL SecHlpOpen) (PSZ pszFileName, PULONG pSFN, ULONG ulOpenFlag, ULONG ulOpenMode);
    ULONG   (* SECCALL SecHlpClose) (ULONG SFN);
    ULONG   (* SECCALL SecHlpQFileSize) (ULONG SFN, PULONG pSize);
    // TYPE can be 0, 1, 2 as for DosSetFilePtr
    ULONG   (* SECCALL SecHlpChgFilePtr) (ULONG SFN, LONG Offset, ULONG TYPE, PULONG pAbs);
    ULONG   (* SECCALL SecHlpSFFromSFN) (ULONG SFN);
    ULONG   (* SECCALL SecHlpFindNext) (PFINDPARMS pParms);
    ULONG   (* SECCALL SecHlpPathFromSFN) (ULONG SFN);
    // array of DOS emulation handlers
    ULONG   *apDemSVC; 
#ifdef LARGE_FILE_SUPPORT
    // parameters are just deduced, to be checked!
    ULONG   (* SECCALL SecHlpReadL) (ULONG SFN, PLONGLONG pcbBytes, PUCHAR pBuffer, ULONG p16Addr, LONGLONG Offset);
    ULONG   (* SECCALL SecHlpWriteL) (ULONG SFN, PLONGLONG pcbBytes, PUCHAR pBuffer, ULONG p16Addr, LONGLONG Offset);
    ULONG   (* SECCALL SecHlpQFileSizeL) (ULONG SFN, PLONGLONG pSize);
    ULONG   (* SECCALL SecHlpChgFilePtrL) (ULONG SFN, LONGLONG Offset, ULONG TYPE, PLONGLONG pAbs);
#endif
} sec_export_t;
#pragma pack(pop)

/* How to get exports
   0) Reserve or allocate enough space for sec_export_t structure (size depends on the LargeFile support expected)
   1) Set seVersionMajor to SEC_EXPORT_MAJOR_VERSION
      Set seVersionMinor to SEC_EXPORT_MINOR_VERSION
   2)
      EAX = DHSEC_GETEXPORT
      ECX = FLAT offset of the sec_export_t structure
      DL  = DEVHLP_SECURITY
      call [Dev_Help]
      AX - error code (0 if OK)
*/
