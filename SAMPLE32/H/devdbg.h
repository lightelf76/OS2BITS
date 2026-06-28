/*DDK*************************************************************************/
/*                                                                           */
/* COPYRIGHT    Copyright (C) 1992 IBM Corporation                           */
/*                                                                           */
/*    The following IBM OS/2 source code is provided to you solely for       */
/*    the purpose of assisting you in your development of OS/2 device        */
/*    drivers. You may use this code in accordance with the IBM License      */
/*    Agreement provided in the IBM Developer Connection Device Driver       */
/*    Source Kit for OS/2. This Copyright statement may not be removed.      */
/*                                                                           */
/*****************************************************************************/
/******************************************************************************
*                 Pro AudioSpectrum16 Physical Device Driver
*                     Production code and toolkit sample
*
*
* DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
* sample code created by IBM Corporation and Media Vision Corporation.
* It is provided to you solely for the purpose of assisting you in the
* development of your applications.
* The code is provided "AS IS", without warranty of any kind.
* IBM and Media Vision shall not be liable for any damages arising out of
* your use of the sample code, even if they have been advised of the
* possibility of such damages.
*
*******************************************************************************
*
* commdbg.h
*
******************************************************************************/

#ifdef DEBUG
void APIENTRY CharOut( char );
void APIENTRY StringOut( PBYTE szString);
void APIENTRY PrintfOut( PBYTE DbStr, ...);
#define DBG_LOG()  PrintfOut("%s %s %d\n", ((PBYTE)__FILE__),((PBYTE)__FUNCTION__), __LINE__)
#define Assert(b, s)    if (!(b)) StringOut(s);
#define dprintf(...)  PrintfOut(__VA_ARGS__)
#define DPRINTF(s,...)  PrintfOut("%s %s %d:"s,((PBYTE)__FILE__),((PBYTE)__FUNCTION__), __LINE__, __VA_ARGS__)
#else

#define CharOut(char)
#define StringOut(szString)
#define PrintfOut(...)
#define DBG_LOG()
#define Assert(b, s)
#define dprintf(...)
#define DPRINTF(s,...)
#endif



