//============================================================================
// Moscow Development Center of Ashtech (Magellan Corp.)
// Written by Fedor Sherstyuk (fns@ashtech.msk.ru)
// Tested by  Alexander Timofeev
// This source code provided as is, without any warranties or responsibilities
//----------------------------------------------------------------------------
// $Workfile:   tcuexc.h  $
// $Date:   17 Aug 1998 15:34:44  $
// $Revision:   1.16  $
// $Author:   FNS  $
//----------------------------------------------------------------------------
//                 Exception handling emulation. Declarations
//============================================================================
#ifndef TCUEXC_H
#define TCUEXC_H

#define TCU__WARN_MODE 1
#include <tcuwarn.h>

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Exception handling is very useful thing, that is unfortunately absent on //
// Windows CE platform. This module provides a facilities for exception     //
// handling emulation. It is implemented based on ideas of Vladimir Belkin, //
// explained in PC Magazine/Russian edition article "C++ Exception: what,   //
// whem, how"                                                               //
//                                                                          //
// Approach presented here is best known to author, but still has some      //
// deficiencies:                                                            //
//                                                                          //
// 1. It is intrusive (i.e. sources should be modified):                    //
//                                                                          //
//      - All exception-sensitive classes (ones that need to be destructed  //
//        during stack unwinding) should be protectedly inherited (directly //
//        or indirectly) from special class tcu__Xsc. Yes, this turns       //
//        single-inheritance into multiple inheritance and may (but not     //
//        must) turn multiple inheritance into virtual inheritance.         //
//                                                                          //
//      - All exception classes (classes of exceptions that could be        //
//        thrown) should be inherited (directly or indirectly) from special //
//        class tcu_Xc. Yes, this means that exception of integer type      //
//        can't be thrown.                                                  //
//                                                                          //
//      - Try, throw, catch constructs should be replaced with special      //
//        macros.                                                           //
//                                                                          //
//    Still this implementation provide some kind of                        //
//    backward-compatibility: implementation of all preserved functionality //
//    through standard exception handling (if it is available) is possible  //
//    by defininng special TCU_X_USE_STANDARD_XH macro.                     //
//                                                                          //
// 2. Presented functionality not quite ARM-compliant.                      //
//      - If exception occured inside constructor of some object then:      //
//                                                                          //
//          - According to ARM destructor for this object should not be     //
//            called                                                        //
//                                                                          //
//          - According to this implementation destructor will be called.   //
//                                                                          //
//        Note that in both cases destructors for base and member objects   //
//        will be called anyway.                                            //
//                                                                          //
//        Note that if macro TCU_X_USE_STANDARD_XH is defined then ARM      //
//        exception handling scheme is used (destructor not called if       //
//        exception occured inside constructor).                            //
//                                                                          //
//      - Exception specification ('throw' clause as part of function       //
//        declaration) is not supported                                     //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// New-style try-block skeleton:                                            //
//                                                                          //
// TCU_X_TRY {                 // Replacement for   try                     //
//     ...                                                                  //
//     TCU_X_THROW(x);         // Replacement for   throw(x);               //
//     ...                                                                  //
// } TCU_X_CATCH (Type, var) { // Replacement for   catch (Type var)        //
//     ...                                                                  //
// } TCU_X_CATCH_TYPE (T) {    // Replacement for   catch (T)               //
//     ...                                                                  //
// } TCU_X_CATCH_ALL {         // Replacement for   catch (...)             //
//     ...                                                                  //
//     TCU_X_RETHROW;          // Replacement for   throw;                  //
//     ...                                                                  //
// } TCU_X_END_TRY             // Should be placed after last catch         //
//                                                                          //
// Naturally in this example TCU_X_THROW and TCU_X_RETHROW may be placed    //
// not only immediately in try/catch block, but also in any function,       //
// called from them.                                                        //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// If proper exception handler was not found during stack unwinding or if   //
// exception is being thrown in destructor during stack unwinding then      //
// tcu_xTerminate() is being called, which in turn calls a last handler,    //
// set by tcu_xSetTerminate() function.                                     //
//                                                                          //
// By default tcu_xTerminate() kills current thread (it couldn't kill the   //
// whole process because of Windows CE limitations).                        //
//                                                                          //
// Note that handler being set by tcu_xSetTerminate() is a global (not      //
// thread-specific) function.                                               //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// Exception-sensitive class declaration (exception sensitive class is one, //
// that need to be destructed during stack unwinding). There are two        //
// requirements imposed on exception-sensitive class:                       //
//                                                                          //
// 1. Class may need to be protectedly derived (directly or indirectly,     //
//    virtually or not) from tcu__Xsc class.                                //
//                                                                          //
//    It is not necessary to make such a derivation if all resources that   //
//    need to be freed at destruction are already contained in members,     //
//    derived from tcu__Xsc. For example, if 'string' class is derived from //
//    tcu__Xsc and some other class 'A' has only string members that need   //
//    to be destructed than it is not necessary to derive class 'A' from    //
//    tcu__Xsc. But if besides string members class 'A' has also members -  //
//    pointers to allocated heap than class 'A' should be derived from      //
//    tcu__Xsc.                                                             //
//                                                                          //
//    If class should be directly derived from tcu__Xsc then one more       //
//    decision should be made - should it be a virtual or nonvirtual        //
//    inheritance. Recipe is simple: for this class there should be a       //
//    unambiguous (const tcu__Xsc *)this conversion. This means that        //
//    if class is multiple derived from from more than one base class,      //
//    derived from tcu__Xsc (not counting tcu__Xsc) then all such base      //
//    classes should be derived from tcu__Xsc virtually. In other words,    //
//    there should be exactly one instance of tcu__Xsc in inheritance       //
//    hierarchy of each class (members are not counted).                    //
//                                                                          //
//    Naturally it is safe to always derive from tcu__Xsc virtually - but   //
//    it is not efficient and may pose some otehr problems.                 //
//                                                                          //
// 2. First operator in constructor body (body is group of statements       //
//    inside curly brackets) of any class, derived from tcu__Xsc should be  //
//    TCU_X_RESET. This holds true even for constructor with empty body.    //
//                                                                          //
//    This statement may be (or may be not) omitted for classes that has no //
//    members, derived from tcu__Xsc.                                       //
//                                                                          //
// Naturally, exception handling system may distinguish automatic variables //
// of exception-sensitive class from other (heap and static ones) and       //
// destroy only former during stack unwinding.                              //
//                                                                          //
// To help declaring classes, derived from tcu__Xsc several macros are      //
// provided:                                                                //
//                                                                          //
// TCU_COLON_XSC  stands for  :protected tcu__Xsc                           //
// TCU_XSC_COMMA  stands for  protected tcu__Xsc,                           //
// TCU_COLON_VXSC stands for  :protected virtual tcu__Xsc                   //
// TCU_VXSC_COMMA stands for  protected virtual tcu__Xsc,                   //
//                                                                          //
// Several examples:                                                        //
//                                                                          //
// class A TCU_COLON_XSC { ..... };                                         //
//                                                                          //
// class B TCU_COLON_VXSC {A a; public B () {TCU_X_RESET; ....}; ....};     //
//                                                                          //
// class C TCU_COLON_VXSC {A b; public B () {TCU_X_RESET; ....}; ....};     //
//                                                                          //
// class D : B, C { ..... };                                                //
//                                                                          //
// class E : A { ..... };                                                   //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// As it was already mentioned, any exception value (a value being thrown)  //
// should belong to class, derived from tcu_Xc.                             //
//                                                                          //
// To declare class, derived from tcu_Xc it is recommended to use           //
// TCU_COLON_XC and TCU_XC_COMMA as follows:                                //
//                                                                          //
//   Write                                                                  //
//       class Xclass TCU_COLON_XC {                                        //
//                                                                          //
//   Instead of                                                             //
//       class Xclass : public tcu_Xc {                                     //
//                                                                          //
//   Write                                                                  //
//       class Xclass : TCU_XC_COMMA AnoterBaseClass {                      //
//                                                                          //
//   Instead of                                                             //
//       class Xclass : public tcu_Xc, AnoterBaseClass {                    //
//                                                                          //
// Usage of TCU_COLON_XC and TCU_XC_COMMA macros allows to easily switch    //
// between this and standard exception handling scheme.                     //
//                                                                          //
// Note that tcu_Xc derived from tcu_Rtti, thus don't forget to write       //
// correspondent TCU_RTTI_DECLARE/TCU_RTTI_IMPLEMENT macros for it. For     //
// immediate descendants of tcu_Xc one can't forget it, because compiler    //
// will complain on undefined pure virtual function '_tcuRttiCheck()'       //
//                                                                          //
// Note that exception value cloned before use (by means of 'new'           //
// operator), thus:                                                         //
//                                                                          //
// - Exception class should have a copy constructor.                        //
//                                                                          //
// - Class of exception value is derived from a type of TCU_X_THROW         //
//   argument (which implemented as a template function). It is this class  //
//   that used during mentioned cloning. So, please, do not cast argument   //
//   of TCU_X_THROW() to something inappropriate.                           //
//                                                                          //
// - It is desirable to (especially for 'out of memory' kind of exceptions) //
//   to have a class-specific 'new', that provides a place in some          //
//   preallocated area.                                                     //
//                                                                          //
//   Note that this preallocated area should be large enough to handle      //
//   several exceptions at once (in concurrent threads).                    //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// As it was already mentioned, if standard exceptions are available then   //
// they could be used (almost) without changing sources - just by defining  //
// TCU_X_USE_STANDARD_XH macro and total recompiling. Of course it should   //
// be kept in mind the deviation from ARM, mentioned in the beginning of    //
// this comment.                                                            //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// Interoperating with ... itself.                                          //
//                                                                          //
// It is quite possible that several instances of this module works inside  //
// single application - e.g. one in core .EXE and some more - in            //
// accompanying DLLs. If they will not know about each other then exception //
// handling will operate incorrectly - some try-blocks will be ignored      //
// (passed through without notice), some exception-sensitive objects will   //
// not be destructed.                                                       //
//                                                                          //
// To organize their proper operation there is a notion of exception        //
// handling token (object of tcu_XToken class). Each instance of this       //
// module creates its own token. But for proper operation one common token  //
// should be used by all instances. To facilitate this class tcu_XToken has //
// following operations (static functions):                                 //
//                                                                          //
// tcu_XToken::get() - returns token, currently used by this instance       //
// tcu_XToken::set() - sets new token for this instance. Previously used    //
//                     token is being destroyed.                            //
//                                                                          //
// It is responsibility of user to properly arrange calling of these        //
// functions.                                                               //
//                                                                          //
// Note that token setting should be performed ASAP (before first use of    //
// try-block and before creation of first exception-sensitive automatic     //
// variables. Otherwise there will be memory leaks, inapropriate stack      //
// unwinding, etc.                                                          //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// This exception handling scheme works well in multithreaded environment.  //
// It allocates one index in TLS (one index per atcually used token, to be  //
// precise).                                                                //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// This module performs a per-thread dynamic allocation of exception        //
// handling system context information. Thus it may (and do) cause a memory //
// leak in following circumstances:                                         //
//                                                                          //
// - Premature exiting from thread - thread is exited (or killed) while     //
//   there are open try-blocks and/or exception-sensitive                   //
//   automatic variables in it.                                             //
//                                                                          //
// - Late call to tcu_XToken::set() - calling this function when there are  //
//   pen try-blocks and/or exception-sensitive automatic variables in some  //
//   thread.                                                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// Use custom exception handling
#ifndef TCU_X_USE_STANDARD_XH

#ifndef TCURTTI_H
#include <tcurtti.h>
#endif // TCURTTI_H

#ifndef TCUXHAPI_H
#include <tcuxhapi.h>
#endif // TCUXHAPI_H

#include <setjmp.h>


//----------------------------------------------------------------------------
// class tcu_Xc
//----------------------------------------------------------------------------
// Base class for all exceptions

class tcu_Xc : public tcu_Rtti {
private:
    typedef tcu_Rtti Base;

protected:

    // Constructors and assignment - all do nothing
    tcu_Xc () {};
    // tcu_Xc (const tcu_Xc &);              // Implicit
    // tcu_Xc & operator = (const tcu_Xc &); // Implicit

public:
    // Do nothing, just virtual
    virtual ~tcu_Xc ();

    // Necessary for every tcu_Rtti derivative
    TCU_RTTI_DECLARE_ABSTRACT;
};



//----------------------------------------------------------------------------
// TCU_XC macros
//----------------------------------------------------------------------------
// These macros should be used to declare the derivation from tcu_Xc

#define TCU_COLON_XC  : public tcu_Xc
#define TCU_XC_COMMA    public tcu_Xc,



//----------------------------------------------------------------------------
// class tcu__Xsc
//----------------------------------------------------------------------------
// Base class for all exception-sensitive objects

class tcu__Xsc {
private:
    typedef void Base;

    friend class tcu__XTryBlock;

    tcu__Xsc *d_prev;
protected:

    // CREATORS

    // These two constructors do exactly the same thing (i.e. latter do not
    // use its argument)
    tcu__Xsc ();
    tcu__Xsc (const tcu__Xsc &);

    virtual ~tcu__Xsc ();



    // ACCESSORS

    // Called from TCU_X_RESET macro (which should be used in constructors
    // of objects that has members, derived from tcu__Xsc.
    void xscReset () const;



    // MANIPULATORS

    // Assignment
    tcu__Xsc & operator = (const tcu__Xsc &) {return *this;};

    // Destroys itself and subsequent objects in destroy chain
    void xscDestroyObjects ();

};



//----------------------------------------------------------------------------
// TCU_XSC macros
//----------------------------------------------------------------------------
// These macros should be used to declare the derivation from tcu__Xsc

#define TCU_COLON_XSC  :protected tcu__Xsc
#define TCU_XSC_COMMA  protected tcu__Xsc,
#define TCU_COLON_VXSC :protected virtual tcu__Xsc
#define TCU_VXSC_COMMA protected virtual tcu__Xsc,

#define TCU_X_RESET  xscReset ()



//----------------------------------------------------------------------------
// class tcu__XTryBlock
//----------------------------------------------------------------------------
// Try block descriptor

class tcu__XTryBlock {
private:
    typedef void Base;

    jmp_buf d_jmpBuf;             // For long jump to this try block
    tcu__Xsc *d_prevXscTop;       // Saved pointer to top of
                                  //   exception-sensitive objects' stack
    tcu__XTryBlock *d_prevTryTop; // Saved pointer to top of try blocks'
                                  //   stack
    tcu_Xc *d_exceptionValue_p;   // Pointer to copy of thrown value. 0 means
                                  //   no exception

    // Descriptors are not copyable in any way
    tcu__XTryBlock (const tcu__XTryBlock &);
    tcu__XTryBlock & operator = (const tcu__XTryBlock &);

public:

    // CREATORS

    tcu__XTryBlock ();
    ~tcu__XTryBlock ();



    // MANIPULATORS

    // Provides an access to jmp buf
    jmp_buf &jmpBuf () {return d_jmpBuf;};

    // Provides an access to pointer to thrown value
    tcu_Xc * &valuePtr () {return d_exceptionValue_p;};

    // Destroy all exception-sensitive objects up to nearest try block and
    // jump there
    void unwind (tcu_Xc *);

    // Perform context saving, find appropriate try-block descriptor (if
    // any) and call its 'unwind()'
    static void doThrow (tcu_Xc *);

    // Rerises a current exception
    static void reThrow ();
};



//----------------------------------------------------------------------------
// TCU_X_TRY, TCU_X_CATCH, TCU_X_THROW series of macros
//----------------------------------------------------------------------------
// These macros form a replacement for try/catch/throw constructs

// Replacement for 'try'
#define TCU_X_TRY                                 \
{tcu__XTryBlock __tcuXtb__; tcu_Xc *__tcuXcPtr__; \
    __tcuXcPtr__ = 0;                             \
    if (!setjmp (__tcuXtb__.jmpBuf())) {

// Replacement for 'catch (T v)'
#define TCU_X_CATCH(T,v)                                                     \
}else if((__tcuXcPtr__=tcu_rttiDynamicCast<T>(__tcuXtb__.valuePtr())) != 0){ \
    T &v = *reinterpret_cast<T *>(__tcuXcPtr__); __tcuXtb__.valuePtr() = 0;

// Replacement for 'catch (T)'
#define TCU_X_CATCH_TYPE(T)                                  \
} else if(tcu_rttiDynamicCast <T> (__tcuXtb__.valuePtr ())) { \
    __tcuXtb__.valuePtr() = 0;

// Replacement for 'catch (...)'
#define TCU_X_CATCH_ALL } else { __tcuXtb__.valuePtr() = 0;

// Should be placed after last catch-block
#define TCU_X_END_TRY }}

// Replacement for 'throw(x)'
#define TCU_X_THROW(x) tcu_xThrow (x)

// Replacement for 'throw'
#define TCU_X_RETHROW tcu__XTryBlock::reThrow()



//----------------------------------------------------------------------------
// class tcu_XToken
//----------------------------------------------------------------------------
// Identifier of exception-handling subsystem to use

class tcu_XToken {
private:
    typedef void Base;

    void *d_tokenData;  // Data that token bears

public:

    // CREATORS

    tcu_XToken ();

    // tcu_XToken (const tcu_XToken &);              // Implicit
    // ~tcu_XToken ();                               // Implicit
    // tcu_XToken & operator = (const tcu_XToken &); // Implicit


    // ACCESSORS

    // Comparison
    bool operator == (const tcu_XToken &) const;
    bool operator != (const tcu_XToken &) const;


    // STATIC FUNCTIONS

    // Sets token to use
    static void set (tcu_XToken);

    // Returns currently used token
    static tcu_XToken get ();
};



//----------------------------------------------------------------------------
// void tcu_xThrow (T value)
//----------------------------------------------------------------------------
// Actually perform throw(x)

template <class T> void tcu_xThrow (T value)
{
    tcu__XTryBlock::doThrow (new T (value));
}



//----------------------------------------------------------------------------
// tcu_xTerminate series
//----------------------------------------------------------------------------
// These functions emulate 'terminate()' and 'set_terminate()' functionality

// Type for terminate handler
typedef void (*tcu_xTerminatePrototype) ();

// Sets new terminate handler, return old one
extern tcu_xTerminatePrototype tcu_xSetTerminate (tcu_xTerminatePrototype);

// Calls terminate handler. Note that default handler terminate thread, not
// entire process
extern void tcu_xTerminate ();



// Use standard exception handling
#else // TCU_X_USE_STANDARD_XH

#define TCU_RTTI_USE_TRUE_RTTI
#ifndef TCURTTI_H
#include <tcurtti.h>
#endif // TCURTTI_H

#ifndef TCUXHAPI_H
#include <tcuxhapi.h>
#endif // TCUXHAPI_H

#define TCU_COLON_XC
#define TCU_XC_COMMA

#define TCU_COLON_XSC
#define TCU_XSC_COMMA
#define TCU_COLON_VXSC
#define TCU_VXSC_COMMA
#define TCU_X_RESET

#define TCU_X_TRY           try
#define TCU_X_CATCH(T,v)    catch (T v)
#define TCU_X_CATCH_TYPE(T) catch (T)
#define TCU_X_CATCH_ALL     catch (...)
#define TCU_X_END_TRY
#define TCU_X_THROW(x)      throw (x)
#define TCU_X_RETHROW       throw

class tcu_XToken {
private:
    typedef void Base;

public:

    tcu_XToken () {};

    // tcu_XToken (const tcu_XToken &);
    // ~tcu_XToken ();
    // tcu_XToken & operator = (const tcu_XToken &);


    static void set (tcu_XToken) {}
    static tcu_XToken get ()     {return tcu_XToken ();}

};


typedef void (*tcu_xTerminatePrototype) ();
#define tcu_xSetTerminate set_terminate
#define tcu_xTerminate    terminate


#endif // TCU_X_USE_STANDARD_XH

#define TCU__WARN_MODE 0
#include <tcuwarn.h>

#endif // TCUEXC_H
//===========================================================================
// $Log:   tcuexc.h_v  $
//
//    Rev 1.16   17 Aug 1998 15:34:44   FNS
// TCU_RTTI_DECLARE=0 changed back to TCU_RTTI_DECLARE_ABSTRACT
//
//    Rev 1.15   31 Jul 1998 19:23:16   FNS
// TCUWARN employed to suppres unnecessary compiler warnings
//
//    Rev 1.14   31 Jul 1998 15:09:38   AAT
// No changes
//
//    Rev 1.13   29 Jul 1998 17:03:50   FNS
// Some comments added
//
//    Rev 1.12   29 Jul 1998 12:00:36   AAT
//
//    Rev 1.11   27 Jul 1998 16:03:32   AAT
// Fixed some bugs
//
//    Rev 1.10   24 Jul 1998 16:49:36   FNS
// Usage of __tcuXcPtr__ imitated
//
//    Rev 1.9   24 Jul 1998 15:19:38   FNS
// TCU_RTTI_DECLARE_ABSTRACT; replaced with TCU_RTTI_DECLARE=0;
//
//    Rev 1.8   24 Jul 1998 15:11:04   FNS
// TCU_COLON_XC fixed
// TCU_XC_COMMA introduced
//
//    Rev 1.7   24 Jul 1998 14:38:28   FNS
// TCU_COLON_XC macro introduced
//
//    Rev 1.6   23 Jul 1998 23:31:26   FNS
// // Comments clarified
//
//    Rev 1.5   23 Jul 1998 19:41:48   FNS
// Some more typos fixed
//
//    Rev 1.4   23 Jul 1998 19:33:22   FNS
// Parentheses ...
//
//    Rev 1.3   23 Jul 1998 19:21:22   FNS
// Parentheses fixed again
//
//    Rev 1.2   23 Jul 1998 19:14:42   FNS
// Parentheses fixed in TCU_X_ macros
//
//    Rev 1.1   23 Jul 1998 17:33:08   FNS
// Both tcu__Xsc constructors made outlined
// Got rid of new.h
//
//    Rev 1.0   13 May 1998 16:46:38   FNS
//
