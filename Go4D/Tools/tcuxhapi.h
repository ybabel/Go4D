//============================================================================
// Moscow Development Center of Ashtech (Magellan Corp.)
// Written by Fedor Sherstyuk (fns@ashtech.msk.ru)
// Tested by  Alexander Timofeev
// This source code provided as is, without any warranties or responsibilities
//----------------------------------------------------------------------------
// $Workfile:   tcuxhapi.h  $
// $Date:   31 Jul 1998 19:23:04  $
// $Revision:   1.3  $
// $Author:   FNS  $
//----------------------------------------------------------------------------
//         Exception handler interface to Windows CE API. Declarations
//============================================================================
#ifndef TCUXHAPI_H
#define TCUXHAPI_H

#define TCU__WARN_MODE 1
#include <tcuwarn.h>

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This module encapsulates all OS-dependent stuff, required by exception   //
// handling emulation.                                                      //
//                                                                          //
// For the most part this class do not need a user's supervision. There     //
// are, however, following exceptions to his rule:                          //
//                                                                          //
// - setComplaintHandler() sets a complaint handler - routine that          //
//   processes complaints (error messages) of exception handling system.    //
//                                                                          //
// - threadCleanup() may need to be called at thread termination. It could  //
//   be safely called for any thread, but it is especially useful for       //
//   prematurely terminated threads (those that terminated not by returning //
//   from thread function, but by some kind of ExitThread()).               //
//                                                                          //
//   Cleanup is actually necessary when thread is abandoned from within     //
//   some open try block or when there are some automatic                   //
//   exception-sensitive objects remain on stack (no matter inside or       //
//   outside try-blocks).                                                   //
//                                                                          //
//   What cleanup is doing: it just deallocates a per-thread data object,   //
//   allocated by exceptio handling system. Failing to deallocate it is a   //
//   memory leak (but not more than that).                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class tcu_XhOsXface {
private:
    typedef void Base;

    // Class is not copyable. It should not be instantiated by user - its only
    // 'legitimate' (and useful) instantiation is performed by itself
    tcu_XhOsXface (const tcu_XhOsXface &);
    tcu_XhOsXface & operator = (const tcu_XhOsXface &);

public:

//----------------------------------------------------------------------------
// Complaint services. Some function may post a complaint (error message) to
// complaint() function. This complaint is processed by current complaint
// handler (default complaint handler do nothing). Complaint is a pair of
// ASCIIZ strings - one text of complaint (error message) another - name of
// module that produced a complaint. Complaint handler is a global (not
// thread-specific function.
//----------------------------------------------------------------------------

    // Prototype for complaint handler
    typedef void(*ComplaintHandler) (const char*complaint, const char*module);

    // Sets new complaint handler. Returns old one
    static ComplaintHandler setComplaintHandler (ComplaintHandler);

    // Make a complaint
    static void complaint (const char *complaint, const char *module);


//----------------------------------------------------------------------------
// Cleanup services
//----------------------------------------------------------------------------

    // This routine should be called by some external thread manager at thread
    // termination. If it will not be called nothing particularly wrong will
    // happened except that for prematurely terminated threads there may be a
    // small memory leak.
    static void threadCleanup ();

    // Prototype for routine, called inside threadCleanup() to perform actual
    // cleanup work
    typedef void (*CleanupHandler) ();

    // Sets new cleanup handler. This routine should be called by exception
    // handling manager
    static void setCleanupHandler (CleanupHandler);


//----------------------------------------------------------------------------
// Stuff, related to thread-specific data. Thread-specific data is void *
// pointer - specific (unique) for each thread. Thread could read from and
// write to this pointer. Thread specific-data is identified by ThreadDataSlot
// value, which should be allocated before using (allocation should be
// performed once during application execution).
//----------------------------------------------------------------------------

    // A type for ThreadDataSlot
    typedef unsigned int ThreadDataSlot;

    // If area, pointed to by passed pointer contain
    // INVALID_SLOT value - slot is being allocated and written to this area.
    // otherwise nothing is performed. Returns true on success.
    static bool allocateThreadDataSlot (ThreadDataSlot *);

    // Releases slot
    static void freeThreadDataSlot (ThreadDataSlot);

    // Gets thread-specific data, identified by given slot. It is guaranteed
    // that before first writing data contain zero.
    static void *getThreadData (ThreadDataSlot);

    // Write thread-specific data, identified by given slot.
    static void setThreadData (ThreadDataSlot, const void *);

    // A constant, that identifies an invalid slot value.
    static const ThreadDataSlot INVALID_SLOT;


//----------------------------------------------------------------------------
// These two functions perform atomic increment/decrement of passed value
//----------------------------------------------------------------------------

    // Both functions return a comparison of resulting value with 0
    static long atomicIncrement (long *p);
    static long atomicDecrement (long *p);


//----------------------------------------------------------------------------
// Other services
//----------------------------------------------------------------------------

    // Returns true if specified address points inside stack of current
    // thread. Function is declared __cdecl to ensure that argument was passed
    // by means of stack (implementation heavily counted on this fact).
    static bool __cdecl onCurrentStack (const void *);

    // Kills (murders!) the current process
    static void killCurrentProcess ();


//----------------------------------------------------------------------------
// Instantiation. Exists for providing early setting up of some internal data
// structures
//----------------------------------------------------------------------------

    tcu_XhOsXface ();
    // ~tcu_XhOsXface (); // Implicit
};

// For sake of early initialization
static tcu_XhOsXface tcu_xhOsXfaceInitializer;

#define TCU__WARN_MODE 0
#include <tcuwarn.h>

#endif // TCUXHAPI_H
//===========================================================================
// $Log:   tcuxhapi.h_v  $
// 
//    Rev 1.3   31 Jul 1998 19:23:04   FNS
// TCUWARN employed to suppres unnecessary compiler warnings
// 
//    Rev 1.2   23 Jul 1998 23:31:04   FNS
// killCurrentThread() removed
// Comments clarified
//
//    Rev 1.1   23 Jul 1998 20:01:50   FNS
// killCurrentProcess() implemented
//
//    Rev 1.0   13 May 1998 16:46:38   FNS
//
