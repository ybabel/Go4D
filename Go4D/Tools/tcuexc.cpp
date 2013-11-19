//============================================================================
// Moscow Development Center of Ashtech (Magellan Corp.)
// Written by Fedor Sherstyuk (fns@ashtech.msk.ru)
// Tested by  Alexander Timofeev
// This source code provided as is, without any warranties or responsibilities
//----------------------------------------------------------------------------
// $Workfile:   tcuexc.cpp  $
// $Date:   03 Aug 1998 17:54:20  $
// $Revision:   1.9  $
// $Author:   FNS  $
//----------------------------------------------------------------------------
//                 Exception handling emulation. Implementation
//============================================================================

#ifndef TCU_X_USE_STANDARD_XH

#include <tcuexc.h>
#include <tcurtti.h>
#include <tcuxhapi.h>

#include <setjmp.h>

#define TCU__WARN_MODE 2
#include <tcuwarn.h>


//============================================================================
// class tcu_Xc
//============================================================================

tcu_Xc::~tcu_Xc ()
{}

//----------------------------------------------------------------------------

TCU_RTTI_IMPLEMENT (tcu_Xc);



//============================================================================
// struct InstanceData_
//============================================================================
// This structure contains data, belonged to one instance of exception
// handling system (tere could be several of them - e.g. on in core EXE,
// whereas others in DLLs.

struct InstanceData_ {
    // Thread-data slot, used by instance
    tcu_XhOsXface::ThreadDataSlot d_slot;

    // Number of contexts, created using this instance's slot
    long d_numOfContexts;

    // Number of instances that use this instance data
    long d_numOfInstances;

    // Address of routine for complaining
    void (*d_complaintsRoutine) (const char *, const char *);
};


//============================================================================
// class tcu_InstanceDataMgr_
//============================================================================
// It is desirable to have exactly one instance of instance data to be used by
// all instances of exception handling system. User may achieve this by means
// of tcu_Token class.
//
// This class maintains a pointer to currently used instance data, handles
// switching to another instance data, sets/retrieves the pointer to
// per-thread data of exception handling system

class tcu_XContext_; // Forward declaration of context class that presents a
                     // per-thread data of exception handling system

class tcu_InstanceDataMgr_ {
private:
    typedef void Base;

    // Class is not instantiable
    tcu_InstanceDataMgr_ ();
    tcu_InstanceDataMgr_ (const tcu_InstanceDataMgr_ &);
    tcu_InstanceDataMgr_ & operator = (const tcu_InstanceDataMgr_ &);
    ~tcu_InstanceDataMgr_ ();

    // Became true once cleanup handler was set. Made class-static (not
    // function-static) to facilitate inline expansion of
    // setCleanupExpansion() routine
    static bool s_cleanupHandlerWasSet;

    // Initial instance data of this instance
    static InstanceData_ s_thisInstanceData;

    // Pointer to TLS index placeholder, being actually used currently
    static InstanceData_ *s_curInsData_p;

    // Allocates thread data slot. Return true on success
    static bool allocateThreadDataSlot ();

    // Routine for cleaning up per-thread data in cases of premature thread
    // exiting
    static void cleanupHandler ();

    // Sets cleanup handler
    static void setCleanupHandler ();

public:

    // Returns data to bear in token (pointer to currently used TLS index
    // placeholder
    static void *getTokenData ();

    // Sets new TLS index placeholder
    static void setNewTokenData (void *);

    // Fills data, pointed by argument, with address of exception handling
    // system context for current thread (argument must be nonzero). Returns
    // true on success
    static bool getContextPtr (tcu_XContext_ **);

    // Sets new address of exception handling system context for current
    // thread
    static void setContextPtr (const tcu_XContext_ *);

    // Make a complaint (error message)
    static void complaint (const char *complaint);
};

//----------------------------------------------------------------------------

InstanceData_ tcu_InstanceDataMgr_::s_thisInstanceData =
                {tcu_XhOsXface::INVALID_SLOT, 0, 1, tcu_XhOsXface::complaint};

InstanceData_ *tcu_InstanceDataMgr_::s_curInsData_p = &s_thisInstanceData;

bool tcu_InstanceDataMgr_::s_cleanupHandlerWasSet = false;

//----------------------------------------------------------------------------

bool tcu_InstanceDataMgr_::allocateThreadDataSlot ()
{
    if (!tcu_XhOsXface::allocateThreadDataSlot (&s_curInsData_p->d_slot)) {
        complaint ("Unable to allocate TLS slot");
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------

void tcu_InstanceDataMgr_::cleanupHandler ()
{
    if (s_curInsData_p->d_slot == tcu_XhOsXface::INVALID_SLOT) return;
    void *p = (tcu_XhOsXface::getThreadData (s_curInsData_p->d_slot));
    delete p;
}

//----------------------------------------------------------------------------

inline void tcu_InstanceDataMgr_::setCleanupHandler ()
{
    if (!s_cleanupHandlerWasSet) {
        tcu_XhOsXface::setCleanupHandler (cleanupHandler);
        s_cleanupHandlerWasSet = true;
    }
}

//----------------------------------------------------------------------------

void *tcu_InstanceDataMgr_::getTokenData ()
{
    setCleanupHandler ();
    allocateThreadDataSlot ();
    return s_curInsData_p;
}

//----------------------------------------------------------------------------

void tcu_InstanceDataMgr_::setNewTokenData (void *p)
{
    setCleanupHandler ();
    if (p == s_curInsData_p) return;
    InstanceData_ *prev = s_curInsData_p;
    s_curInsData_p = reinterpret_cast <InstanceData_ *> (p);
    if ((--prev->d_numOfInstances == 0) &&
        (prev->d_slot != tcu_XhOsXface::INVALID_SLOT))
    {
        tcu_XhOsXface::freeThreadDataSlot (prev->d_slot);
        prev->d_slot = tcu_XhOsXface::INVALID_SLOT;
    }
    if ((!prev->d_numOfInstances) && prev->d_numOfContexts) {
        complaint ("Dangling automatic exception-sensitive variables and/or"
                   " try-blocks appeared after setting new token");
    }
}

//----------------------------------------------------------------------------

bool tcu_InstanceDataMgr_::getContextPtr (tcu_XContext_ **p)
{
    setCleanupHandler ();
    if (!allocateThreadDataSlot ()) return false;
    *p = static_cast <tcu_XContext_ *> (tcu_XhOsXface::getThreadData
                                                    (s_curInsData_p->d_slot));
    return true;
}

//----------------------------------------------------------------------------

void tcu_InstanceDataMgr_::setContextPtr (const tcu_XContext_ *p)
{
    if (p) {
        tcu_XhOsXface::atomicIncrement (&s_curInsData_p->d_numOfContexts);
    } else {
        tcu_XhOsXface::atomicDecrement (&s_curInsData_p->d_numOfContexts);
    }
    tcu_XhOsXface::setThreadData (s_curInsData_p->d_slot, p);
}

//----------------------------------------------------------------------------

void tcu_InstanceDataMgr_::complaint (const char *complaint)
{
    s_curInsData_p->d_complaintsRoutine (complaint, __FILE__);
}


//============================================================================
// class tcu_Token
//============================================================================

tcu_XToken::tcu_XToken ()
: d_tokenData (tcu_InstanceDataMgr_::getTokenData ())
{}

//----------------------------------------------------------------------------

bool tcu_XToken::operator == (const tcu_XToken &t) const
{
    return t.d_tokenData == d_tokenData;
}

//----------------------------------------------------------------------------

bool tcu_XToken::operator != (const tcu_XToken &t) const
{
    return t.d_tokenData != d_tokenData;
}

//----------------------------------------------------------------------------

void tcu_XToken::set (tcu_XToken t)
{
    tcu_InstanceDataMgr_::setNewTokenData (t.d_tokenData);
}

//----------------------------------------------------------------------------

tcu_XToken tcu_XToken::get ()
{
    tcu_XToken t;
    return t;
}


//============================================================================
// class tcu_XContext_
//============================================================================
// Placeholder of per-thread context information. Contain pointers to tops of
// try-blocks stack and exception-sensitive objects stack and information
// about currently processed exception.

class tcu_XContext_ {
protected:
    typedef void Base;

    // Contexts are not copyable
    tcu_XContext_ (const tcu_XContext_ &);
    tcu_XContext_ & operator = (const tcu_XContext_ &);

    // Default constructor
    tcu_XContext_ ();

    // ~tcu_XContext_ (); // Implicit

public:

    tcu__Xsc *d_lastXsc;          // Pointer to top of exception-sensitive
                                  // objects' stack
    tcu__XTryBlock *d_lastTry;    // Pointer to top of try blocks' stack
    tcu_Xc *d_currentException_p; // Pointer to currently processed exception
                                  // data


    // Returns pointer to filled context of current thread or 0 (if some error
    // occured)
    static tcu_XContext_ * get ();

    // Destroys context, passed as parameter
    static void destroy (tcu_XContext_ *);
};


//----------------------------------------------------------------------------

inline tcu_XContext_::tcu_XContext_ ()
: d_lastXsc(0), d_lastTry(0), d_currentException_p(0)
{}

//----------------------------------------------------------------------------

tcu_XContext_ * tcu_XContext_::get ()
{
    tcu_XContext_ *p;
    if (!tcu_InstanceDataMgr_::getContextPtr (&p)) return 0;
    if (!p) {
        p = new tcu_XContext_;
        if (p) {
            tcu_InstanceDataMgr_::setContextPtr (p);
        } else {
            tcu_InstanceDataMgr_::complaint("Out of memory creating context");
        }
    }
    return p;
}

//----------------------------------------------------------------------------

void tcu_XContext_::destroy (tcu_XContext_ *c)
{
    if (c) {
        delete c;
        tcu_InstanceDataMgr_::setContextPtr (0);
    }
}


//============================================================================
// class tcu__Xsc
//============================================================================

// Helper function that actually implements constructors. First parameter -
// pointer to created object (i.e. 'this'), second - &(this->d_prev).
static void _xscConstructor (tcu__Xsc **this_p, tcu__Xsc **d_prev_p);

//----------------------------------------------------------------------------

tcu__Xsc::tcu__Xsc ()
{
    tcu__Xsc *thisLvalue = this; // 'this' is not lvalue and can't be '&'
    _xscConstructor (&thisLvalue, &d_prev);
}

//----------------------------------------------------------------------------

tcu__Xsc::tcu__Xsc (const tcu__Xsc &)
{
    tcu__Xsc *thisLvalue = this; // 'this' is not lvalue and can't be '&'
    _xscConstructor (&thisLvalue, &d_prev);
}

//----------------------------------------------------------------------------

static void _xscConstructor (tcu__Xsc **this_p, tcu__Xsc **d_prev_p)
{
    if (!tcu_XhOsXface::onCurrentStack (*this_p)) return;
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    *d_prev_p = context->d_lastXsc;
    if (context->d_lastXsc == *this_p) {
        tcu_InstanceDataMgr_::complaint ("Multiple instances of tcu__Xsc in "
                                         "single inheritance hierarchy.");
    }
    context->d_lastXsc = *this_p;
}

//----------------------------------------------------------------------------

tcu__Xsc::~tcu__Xsc ()
{
    if (!tcu_XhOsXface::onCurrentStack (this)) return;
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    if (d_prev != this) context->d_lastXsc = d_prev;
    if ((0 == context->d_lastXsc) && (0 == context->d_lastTry)) {
        tcu_XContext_::destroy (context);
    }
}

//----------------------------------------------------------------------------

void tcu__Xsc::xscReset () const
{
    if (!tcu_XhOsXface::onCurrentStack (this)) return;
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    context->d_lastXsc = const_cast <tcu__Xsc *> (this);
}

//----------------------------------------------------------------------------

void tcu__Xsc::xscDestroyObjects ()
{
    this->~tcu__Xsc ();
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (context && context->d_lastXsc) {
        context->d_lastXsc->xscDestroyObjects ();
    }
}



//===========================================================================
// class tcu__XTryBlock
//===========================================================================

tcu__XTryBlock::tcu__XTryBlock ()
: d_exceptionValue_p (0)
{
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    d_prevTryTop = context->d_lastTry;
    context->d_lastTry = this;
    d_prevXscTop = context->d_lastXsc;
    context->d_lastXsc = 0;
}

//----------------------------------------------------------------------------

tcu__XTryBlock::~tcu__XTryBlock ()
{
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    context->d_lastTry = d_prevTryTop;
    context->d_lastXsc = d_prevXscTop;
    if (d_exceptionValue_p) {
        doThrow (d_exceptionValue_p);
    } else {
        delete context->d_currentException_p;
        context->d_currentException_p = 0;
    }
    if ((0 == context->d_lastXsc) && (0 == context->d_lastTry)) {
        tcu_XContext_::destroy (context);
    }
}

//----------------------------------------------------------------------------

void tcu__XTryBlock::unwind (tcu_Xc *xp)
{
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    d_exceptionValue_p = xp;
    context->d_lastTry = d_prevTryTop;
    context->d_lastXsc = d_prevXscTop;
    longjmp (d_jmpBuf, ~0);
}

//----------------------------------------------------------------------------

void tcu__XTryBlock::doThrow (tcu_Xc *xp)
{
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    context->d_currentException_p = xp;
    if (context->d_lastXsc) {
        tcu__Xsc *p = context->d_lastXsc;
        context->d_lastXsc = context->d_lastXsc->d_prev;
        p->xscDestroyObjects ();
    }
    if (!context->d_lastTry) {
        tcu_xTerminate ();
    } else {
        context->d_lastTry->unwind (xp);
    }
}

//----------------------------------------------------------------------------

void tcu__XTryBlock::reThrow ()
{
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (!context) return;
    if (context->d_currentException_p) {
        tcu_Xc *currentException_p = context->d_currentException_p;
        context->d_currentException_p = 0;
        doThrow (currentException_p);
    }
}



//===========================================================================
// tcu_xTerminate() series
//===========================================================================

// Tefault terminate handler
static void defaultTerminateHandler_ ();

// Points to current terminate handler
static tcu_xTerminatePrototype terminateHandler_ = defaultTerminateHandler_;

//----------------------------------------------------------------------------

void tcu_xTerminate ()
{
    tcu_XContext_ *context = tcu_XContext_::get ();
    if (context) {
        delete context->d_currentException_p;
        tcu_XContext_::destroy (context);
    }
    if (terminateHandler_) terminateHandler_ ();
    defaultTerminateHandler_ ();
}

//----------------------------------------------------------------------------

tcu_xTerminatePrototype tcu_xSetTerminate (tcu_xTerminatePrototype f)
{
    tcu_xTerminatePrototype prev = terminateHandler_;
    terminateHandler_ = f;
    return (prev);
}

//----------------------------------------------------------------------------

static void defaultTerminateHandler_ ()
{
    tcu_XhOsXface::killCurrentProcess ();
}

#endif // TCU_X_USE_STANDARD_XH

//===========================================================================
// $Log:   tcuexc.cpv  $
// 
//    Rev 1.9   03 Aug 1998 17:54:20   FNS
// TCU_WARN_SUPPRESS macro no more necessary for using TCUWARN.H
// 
//    Rev 1.8   31 Jul 1998 19:23:10   FNS
// TCUWARN employed to suppres unnecessary compiler warnings
// 
//    Rev 1.7   27 Jul 1998 15:22:06   FNS
// Conditional compilation on TCU_X_USE_STANDARD_XH added
// 
//    Rev 1.6   24 Jul 1998 15:18:40   FNS
// 
// 
//    Rev 1.5   24 Jul 1998 14:52:16   FNS
// Bug fixed in reThrow()
//
//    Rev 1.4   23 Jul 1998 20:02:20   FNS
// killCurrentProcess() implemented
//
//    Rev 1.3   23 Jul 1998 18:03:34   FNS
// d_cleanupHandlerWasSet renamed to s_cleanupHandlerWasSet
//
//    Rev 1.2   23 Jul 1998 18:00:38   FNS
// Typo fixed
//
//    Rev 1.1   23 Jul 1998 17:33:56   FNS
// Both tcu__Xsc constructors made outlined: _xscConstructor() introduced
// Got rid of new.h
//
//    Rev 1.0   13 May 1998 16:46:38   FNS
//
