//============================================================================
// Moscow Development Center of Ashtech (Magellan Corp.)
// Written by Fedor Sherstyuk (fns@ashtech.msk.ru)
// Tested by  Alexander Timofeev
// This source code provided as is, without any warranties or responsibilities
//----------------------------------------------------------------------------
// $Workfile:   tcuxhapi.cpp  $
// $Date:   03 Aug 1998 17:54:08  $
// $Revision:   1.5  $
// $Author:   FNS  $
//----------------------------------------------------------------------------
//        Exception handler interface to Windows CE API. Implementation
//============================================================================

#include <tcuxhapi.h>

#define TCU__WARN_MODE 3
#include <tcuwarn.h>

#include <windows.h>

#define TCU__WARN_MODE 2
#include <tcuwarn.h>



//============================================================================
// Static data, used by this module
//============================================================================

static long s_initCount_ = 0;

static CRITICAL_SECTION s_cs_;


//============================================================================
// Constructor, that performs early initialization
//============================================================================

tcu_XhOsXface::tcu_XhOsXface ()
{
    if (!s_initCount_++) {
        InitializeCriticalSection (&s_cs_);
    }
}



//============================================================================
// The useful functionality
//============================================================================

bool __cdecl tcu_XhOsXface::onCurrentStack (const void *p)
{
    static unsigned long memoryPageSize = 0;
    static const void *highestAddress = 0;

    if (!memoryPageSize) {
        SYSTEM_INFO si;
        GetSystemInfo (&si);
        memoryPageSize = si.dwPageSize;
        highestAddress = si.lpMaximumApplicationAddress;
    }

    if ((reinterpret_cast <unsigned long> (p) ^
         reinterpret_cast <unsigned long> (&p)) < memoryPageSize)
    {
        return true;
    }
    if ((p < &p) || (p > highestAddress)) return false;

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery (&p, &mbi, sizeof (mbi));
    return (p < (static_cast <const char *>(mbi.BaseAddress)+mbi.RegionSize));
}



//----------------------------------------------------------------------------

void tcu_XhOsXface::killCurrentProcess ()
{
    TerminateProcess (GetCurrentProcess (), 0);
}

//----------------------------------------------------------------------------

const tcu_XhOsXface::ThreadDataSlot tcu_XhOsXface::INVALID_SLOT =
                                                           ~((unsigned int)0);

//----------------------------------------------------------------------------

bool tcu_XhOsXface::allocateThreadDataSlot (ThreadDataSlot *p)
{
    if (*p != INVALID_SLOT) return true;
    EnterCriticalSection (&s_cs_);
    if (*p == INVALID_SLOT) *p = TlsAlloc ();
    if (*p >= TLS_MINIMUM_AVAILABLE) *p = INVALID_SLOT;
    LeaveCriticalSection (&s_cs_);
    return (*p != INVALID_SLOT);
}

//----------------------------------------------------------------------------

void tcu_XhOsXface::freeThreadDataSlot (ThreadDataSlot s)
{
    TlsFree (s);
}

//----------------------------------------------------------------------------

void *tcu_XhOsXface::getThreadData (ThreadDataSlot s)
{
    return TlsGetValue (s);
}

//----------------------------------------------------------------------------

void tcu_XhOsXface::setThreadData (ThreadDataSlot s, const void *p)
{
    TlsSetValue (s, (void *)p);
}



//----------------------------------------------------------------------------

long tcu_XhOsXface::atomicIncrement (long *p)
{
    return InterlockedIncrement (p);
}

//----------------------------------------------------------------------------

long tcu_XhOsXface::atomicDecrement (long *p)
{
    return InterlockedDecrement (p);
}



//----------------------------------------------------------------------------

static void defaultComplaintHandler (const char *, const char *)
{}

static tcu_XhOsXface::ComplaintHandler currentComplaintHandler =
                                                      defaultComplaintHandler;

//----------------------------------------------------------------------------

tcu_XhOsXface::ComplaintHandler tcu_XhOsXface::setComplaintHandler
                                           (tcu_XhOsXface::ComplaintHandler f)
{
    EnterCriticalSection (&s_cs_);
    tcu_XhOsXface::ComplaintHandler prev = currentComplaintHandler;
    currentComplaintHandler = f;
    LeaveCriticalSection (&s_cs_);
    return (prev);
}

//----------------------------------------------------------------------------

void tcu_XhOsXface::complaint (const char *complaint, const char *module)
{
    EnterCriticalSection (&s_cs_);
    tcu_XhOsXface::ComplaintHandler h = currentComplaintHandler;
    LeaveCriticalSection (&s_cs_);
    if (h) h (complaint, module);
}



//----------------------------------------------------------------------------

static tcu_XhOsXface::CleanupHandler currentCleanupHandler = 0;

//----------------------------------------------------------------------------

void tcu_XhOsXface::threadCleanup ()
{
    if (currentCleanupHandler) currentCleanupHandler ();
}

//----------------------------------------------------------------------------

void tcu_XhOsXface::setCleanupHandler (tcu_XhOsXface::CleanupHandler h)
{
    currentCleanupHandler  = h;
}

//===========================================================================
// $Log:   tcuxhapi.cpv  $
// 
//    Rev 1.5   03 Aug 1998 17:54:08   FNS
// TCU_WARN_SUPPRESS macro no more necessary for using TCUWARN.H
// 
//    Rev 1.4   31 Jul 1998 19:22:58   FNS
// TCUWARN employed to suppres unnecessary compiler warnings
// 
//    Rev 1.3   23 Jul 1998 23:31:12   FNS
// killCurrentThread() removed
// 
//    Rev 1.2   23 Jul 1998 20:01:58   FNS
// killCurrentProcess() implemented
// 
//    Rev 1.1   23 Jul 1998 17:56:40   FNS
// Got rid of TLS_OUT_OF_INDEXES (not supporte din CE)
// 
//    Rev 1.0   13 May 1998 16:46:38   FNS
// 
