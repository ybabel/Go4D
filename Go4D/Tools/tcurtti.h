//============================================================================
// Moscow Development Center of Ashtech (Magellan Corp.)
// Written by Fedor Sherstyuk (fns@ashtech.msk.ru)
// Tested by  Alexander Timofeev
// This source code provided as is, without any warranties or responsibilities
//----------------------------------------------------------------------------
// $Workfile:   tcurtti.h  $
// $Date:   27 Aug 1998 19:17:58  $
// $Revision:   1.5  $
// $Author:   FNS  $
//----------------------------------------------------------------------------
//             RTTI emulation (protocol hierarchies). Declarations
//============================================================================
#ifndef TCURTTI_H
#define TCURTTI_H

#define TCU__WARN_MODE 1
#include <tcuwarn.h>

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Protocol hierarchy - mechanism similar to C++ RTTI (useful for           //
// platforms/compilers where there is no RTTI).                             //
//                                                                          //
// In certain form it is resented in MFC by IsKindOf() function and         //
// DECLARE_DYNAMIC, IMPLEMENT_DYNAMIC, STATIC_DOWNCAST, DYNAMIC_DOWNCAST    //
// macros. However it MFC's RTTI is rather heavyweight, pollutes namespace  //
// and present a risk of link-time coupling with lots of something          //
// unnecessary.                                                             //
//                                                                          //
// In here more lightweight version is presented. It is implemented here    //
// per J. Lacos' book "Large-Scale C++ Software design". Besides            //
// compactness it has one more advantage over MFC - provision for multiple  //
// inheritance.                                                             //
//                                                                          //
// Protocol hierarchy is a series of classes, directly or indirectly        //
// derived from tcu_RTTI abstract base class. Multiple inheritance from     //
// this class is allowed.                                                   //
//                                                                          //
// Protocol hierarchy provides following services:                          //
//                                                                          //
// - Class identifiers. Each class, participating in protocol hierarchy has //
//   unique class identifier (of type tcu_RttiTypeId which is in fact void  //
//   *).                                                                    //
//                                                                          //
//   Class identifier could be obtained from an expression (e.g. object):   //
//                                                                          //
//   tcu_rttiTypeId (anExpression)                                          //
//                                                                          //
//   Or from a type name:                                                   //
//                                                                          //
//   TCU_RTTI_TYPE_ID (TypeName)                                            //
//                                                                          //
//   Naturally in former case expression should return an object of type    //
//   that belongs to protocol hierarchy and in latter - type should belong  //
//   to protocol hierarchy. Naturally if this condition is not met there    //
//   will be compile-time error.                                            //
//                                                                          //
// - Safe casts. Pointer to object of class class that belongs to protocol  //
//   hierarchy may be safely converted to pointer to another class that     //
//   belongs to protocol hierarchy. 'Safe' means that this conversion is    //
//   possible only if actual object is belongs to or derived from a         //
//   destination class.                                                     //
//                                                                          //
//   tcu_rttiDynamicCast<DestinationType> (pointerToObject)                 //
//                                                                          //
//   This expression returns pointer to destination type or 0 (if           //
//   conversion impossible). This conversion preserves constness of source  //
//   pointer.                                                               //
//                                                                          //
//   Note that unlike traditional C++ syntax DestinationType is not a type  //
//   of pointer, but rather type of object pointed to (i.e. a type that     //
//   belongs to protocol hierarchy).                                        //
//                                                                          //
// - Checking for belonging to hierarchy:                                   //
//                                                                          //
//   tcu_isKindOf<TypeName> (expr); is true if expression evaluates to      //
//   object, belonging to given type or derived from that type.             //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// To have all these luxuries class should:                                 //
//                                                                          //
// - Be inherited from tcu_Rtti - directly or indirectly, single or         //
//   multiple.                                                              //
//                                                                          //
//                                                                          //
// - Have one of following statements in class declaration:                 //
//                                                                          //
//   TCU_RTTI_DECLARE;                                                      //
//                                                                          //
//   or                                                                     //
//                                                                          //
//   TCU_RTTI_DECLARE_ABSTRACT;                                             //
//                                                                          //
//   Former of this statements could be used in any class, whereas latter - //
//   in abstract ones.                                                      //
//                                                                          //
//   Using TCU_RTTI_DECLARE_ABSTRACT ensures that concrete derived classes  //
//   will have TCU_RTTI_DECLARE in their bodies (because otherwise          //
//   link-time error is occured).                                           //
//                                                                          //
//                                                                          //
//   Note that both of these statements declare a public function, so they  //
//   (just in case) set 'public:' access specifier inside. Therefore it is  //
//   wise to place these statements in a public section of class            //
//   declaration.                                                           //
//                                                                          //
// - Have one of following statements in class functions' definition place: //
//                                                                          //
//   TCU_RTTI_IMPLEMENT(ThisClass);                                         //
//   TCU_RTTI_IMPLEMENT_1(ThisClass,BaseClassName);                         //
//   TCU_RTTI_IMPLEMENT_2(ThisClass,BaseClass1Name,BaseClass2Name);         //
//   TCU_RTTI_IMPLEMENT_3 ...                                               //
//   TCU_RTTI_IMPLEMENT_4 ...                                               //
//   TCU_RTTI_IMPLEMENT_5 ...                                               //
//   TCU_RTTI_IMPLEMENT_6 ...                                               //
//   TCU_RTTI_IMPLEMENT_7 ...                                               //
//   TCU_RTTI_IMPLEMENT_8 ...                                               //
//   TCU_RTTI_IMPLEMENT_9 ...                                               //
//                                                                          //
//   Here ThisClass is a name of class for which RTTI is being implemented. //
//                                                                          //
//   If functions are defined in .H file than statement may need to be      //
//   prefixed with 'inline' and/or 'template <...>' clause. Such a          //
//   prefixing will work fine, because all these macro define just one      //
//   (virtual) function.                                                    //
//                                                                          //
//   Which macro to use depends upon inheritance multiplicity: first two -  //
//   for classes that single inherited from tcu_Rtti, third - for classes   //
//   that twice inherited from tcu_Rtti, etc.                               //
//                                                                          //
//   Parameters of these macros are names of base classes.                  //
//   TCU_RTTI_IMPLEMENT supposes that base class have name 'Base' (it is    //
//   very useful habit to typedef base class to 'Base' inside each class    //
//   declaration).                                                          //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// Example:                                                                 //
//                                                                          //
// class A : public tcu_Rtti {TCU_RTTI_DECLARE_ABSTRACT;};                  //
//                                                                          //
// class B : public A {typedef A Base; TCU_RTTI_DECLARE;};                  //
//                                                                          //
// class C : public A {typedef A Base; TCU_RTTI_DECLARE;};                  //
//                                                                          //
// class D : public B, public C {TCU_RTTI_DECLARE;};                        //
//                                                                          //
// TCU_RTTI_IMPLEMENT_1 (A, tcu_Rtti);                                      //
//                                                                          //
// TCU_RTTI_IMPLEMENT (B);                                                  //
//                                                                          //
// TCU_RTTI_IMPLEMENT (C);                                                  //
//                                                                          //
// TCU_RTTI_IMPLEMENT_2 (D, B, C);                                          //
//                                                                          //
// A a, *pa;                                                                //
//                                                                          //
// B b, *pb;                                                                //
//                                                                          //
// C c, *pc;                                                                //
//                                                                          //
// D d, *pd;                                                                //
//                                                                          //
// pa = &b;                                                                 //
// tcu_RttiDynamicCast <B> (pa);               // Equals &b                 //
// tcu_RttiDynamicCast <C> (pa);               // Equals 0                  //
// tcu_RttiDynamicCast <D> (pa);               // Equals 0                  //
//                                                                          //
// tcu_RttiDynamicCast <A> (&d);               // Equals (A*)&d             //
//                                                                          //
// tcu_rttiTypeId(*pa) == TCU_RTTI_TYPE_ID(B); // Equals true               //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// This module could also be used on compilers that do support true RTTI    //
// and safe casting (there is no any advantage in using this module for     //
// such a situation - on emay need to do it only for compatibility          //
// reasons).                                                                //
//                                                                          //
// To use true RTTI and safe casts one need to define macro                 //
//                                                                          //
// TCU_RTTI_USE_TRUE_RTTI                                                   //
//                                                                          //
// If this macro is defined all functionality declared here will be         //
// implemented through true RTTI and true safe casts. Naturally all sources //
// need to be recompiled.                                                   //
//                                                                          //
// ------------------------------------------------------------------------ //
//                                                                          //
// ATTENTION!                                                               //
//                                                                          //
// The major limitation of this RTTI implementation is a lack DLLs support. //
//                                                                          //
// Indeed, identifier of class T is an address of                           //
// tcu__RttiTypeIdImplementation<T>::s_id, which have separate instances in //
// different modules (core EXE and all DLLs) that use this type. Thus       //
// objects of same type T, but instantiated in different modules will have  //
// different type IDs.                                                      //
//                                                                          //
// This problem in principle may be amended by different implementation of  //
// T::_tcuRttiCheck(0) in classes, that could be instantiated in different  //
// modules. Such amendment may be done without changing of this header -    //
// implementation of T::_tcuRttiCheck() is performed by means of macro and  //
// could be different for particular classes.                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Pure RTTI emulation
//////////////////////////////////////////////////////////////////////////////
#ifndef TCU_RTTI_USE_TRUE_RTTI

//----------------------------------------------------------------------------
// typedef tcu_RttiTypeId
//----------------------------------------------------------------------------
// Type for class Type ID

typedef const void * tcu_RttiTypeId;



//----------------------------------------------------------------------------
// class tcu_Rtti
//----------------------------------------------------------------------------
// Base class for all RTTI objects

class tcu_Rtti {
private:
    typedef void Base;

protected:

    // Default and copy constructors, assignment - all do nothing
    tcu_Rtti () {};
    tcu_Rtti (const tcu_Rtti &) {};
    tcu_Rtti & operator = (const tcu_Rtti &) {return (*this);};

public:

    // CREATORS

    // Do nothing. Just virtual
    virtual ~tcu_Rtti ();



    // ACCESSORS

    // This function is actually dual-purpose. These purposes are gathered in
    // one function to simplify a TCU_RTTI_IMPLEMENT macros usage.
    //
    // When argument is zero this function returns type ID of *this object.
    //
    // When argument is nonzero it is treated as type ID of some class. In
    // this case function returns nonzero if *this belongs to or derived from
    // a class with type ID, passed as argument - otherwise it returns zero.
    virtual tcu_RttiTypeId _tcuRttiCheck (tcu_RttiTypeId) const = 0;

};



//----------------------------------------------------------------------------
// template <class T> class tcu__RttiTypeIdImplementation
//----------------------------------------------------------------------------
// Internal implementation class
//
// Implementation of type ID - a class-unique object
// This template-based perversion allows to avoid static objects in CPP-files
// (such avoidance may be handy for template classes).

template <class T> class tcu__RttiTypeIdImplementation {
public:
    static const void *s_id;
};

template <class T> const void *tcu__RttiTypeIdImplementation<T>::s_id = 0;



//----------------------------------------------------------------------------
// tcu_rttiDynamicCast ()
//----------------------------------------------------------------------------
// Functions that perform dynamic casts, preserving constness
//
// Function parameter - pointer to be casted from
// Template parameter - type of object to be pointed to by resulting pointer
//  (just an object type, no '*', no 'const')

template <class T> inline
T * tcu_rttiDynamicCast (tcu_Rtti *arg)
{
    return ((arg && arg->_tcuRttiCheck
             (&tcu__RttiTypeIdImplementation<T>::s_id)) ?
            reinterpret_cast <T *>(arg) : 0);
}

template <class T> inline
const T * tcu_rttiDynamicCast (const tcu_Rtti *arg)
{
    return ((arg && arg->_tcuRttiCheck
             (&tcu__RttiTypeIdImplementation<T>::s_id)) ?
            reinterpret_cast <const T *>(arg) : 0);
}



//----------------------------------------------------------------------------
// tcu_isKindOf<Type>(expr)
//----------------------------------------------------------------------------
// Returns true if argument belongs to or derived from given type

template <class T> inline
bool tcu_isKindOf (const tcu_Rtti &arg)
{
    return arg._tcuRttiCheck (&tcu__RttiTypeIdImplementation<T>::s_id) != 0;
}



//----------------------------------------------------------------------------
// tcu_rttiTypeId(), TCU_RTTI_TYPE_ID()
//----------------------------------------------------------------------------
// Return type ID of object and class respectively

inline tcu_RttiTypeId tcu_rttiTypeId (const tcu_Rtti &arg)
{
    return (arg._tcuRttiCheck (0));
}

#define TCU_RTTI_TYPE_ID(Type) (&tcu__RttiTypeIdImplementation<Type>::s_id)



//----------------------------------------------------------------------------
// TCU_RTTI_DECLARE, TCU_RTTI_DECLARE_ABSTRACT
//----------------------------------------------------------------------------
// Declare functions, necessary for RTTI object. Forces scope to be 'public
// Former macro may be used in any class, latter - in abstract only

#define TCU_RTTI_DECLARE                                             \
public:                                                              \
    virtual tcu_RttiTypeId _tcuRttiCheck (tcu_RttiTypeId) const      \

#define TCU_RTTI_DECLARE_ABSTRACT                                    \
public:                                                              \
    virtual tcu_RttiTypeId _tcuRttiCheck (tcu_RttiTypeId) const = 0  \



//----------------------------------------------------------------------------
// TCU_RTTI_IMPLEMENT series
//----------------------------------------------------------------------------
// Implement functions, necessary for RTTI class
//
// First parameter of any macro is a name of RTTI class
// TCU_RTTI_IMPLEMENT is for class, that have a 'Base' alias (typedef) for
//   base class
// Other macros in its arguments list a class' immediate RTTI parents

#define TCU_RTTI_IMPLEMENT(ThisClass)                                        \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
        Base::_tcuRttiCheck (id))                                            \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_1(ThisClass,B1)                                   \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
        B1::_tcuRttiCheck (id))                                              \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_2(ThisClass,B1,B2)                                \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
        B1::_tcuRttiCheck (id)                                           ||  \
        B2::_tcuRttiCheck (id))                                              \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_3(ThisClass,B1,B2,B3)                             \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
        B1::_tcuRttiCheck (id)                                           ||  \
        B2::_tcuRttiCheck (id)                                           ||  \
        B3::_tcuRttiCheck (id))                                              \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_4(ThisClass,B1,B2,B3,B4)                          \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
        B1::_tcuRttiCheck (id)                                           ||  \
        B2::_tcuRttiCheck (id)                                           ||  \
        B3::_tcuRttiCheck (id)                                           ||  \
        B4::_tcuRttiCheck (id))                                              \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_5(ThisClass,B1,B2,B3,B4,B5)                       \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
            B1::_tcuRttiCheck (id)                                       ||  \
            B2::_tcuRttiCheck (id)                                       ||  \
            B3::_tcuRttiCheck (id)                                       ||  \
            B4::_tcuRttiCheck (id)                                       ||  \
            B5::_tcuRttiCheck (id))                                          \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_6(ThisClass,B1,B2,B3,B4,B5,B6)                    \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
            B1::_tcuRttiCheck (id)                                       ||  \
            B2::_tcuRttiCheck (id)                                       ||  \
            B3::_tcuRttiCheck (id)                                       ||  \
            B4::_tcuRttiCheck (id)                                       ||  \
            B5::_tcuRttiCheck (id)                                       ||  \
            B6::_tcuRttiCheck (id))                                          \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_7(ThisClass,B1,B2,B3,B4,B5,B6,B7)                 \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
            B1::_tcuRttiCheck (id)                                       ||  \
            B2::_tcuRttiCheck (id)                                       ||  \
            B3::_tcuRttiCheck (id)                                       ||  \
            B4::_tcuRttiCheck (id)                                       ||  \
            B5::_tcuRttiCheck (id)                                       ||  \
            B6::_tcuRttiCheck (id)                                       ||  \
            B7::_tcuRttiCheck (id))                                          \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_8(ThisClass,B1,B2,B3,B4,B5,B6,B7,B8)              \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
            B1::_tcuRttiCheck (id)                                       ||  \
            B2::_tcuRttiCheck (id)                                       ||  \
            B3::_tcuRttiCheck (id)                                       ||  \
            B4::_tcuRttiCheck (id)                                       ||  \
            B5::_tcuRttiCheck (id)                                       ||  \
            B6::_tcuRttiCheck (id)                                       ||  \
            B7::_tcuRttiCheck (id)                                       ||  \
            B8::_tcuRttiCheck (id))                                          \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \

#define TCU_RTTI_IMPLEMENT_9(ThisClass,B1,B2,B3,B4,B5,B6,B7,B8,B9)           \
tcu_RttiTypeId ThisClass::_tcuRttiCheck (tcu_RttiTypeId id) const            \
{                                                                            \
    if ((id==0) || (id==&tcu__RttiTypeIdImplementation<ThisClass>::s_id) ||  \
            B1::_tcuRttiCheck (id)                                       ||  \
            B2::_tcuRttiCheck (id)                                       ||  \
            B3::_tcuRttiCheck (id)                                       ||  \
            B4::_tcuRttiCheck (id)                                       ||  \
            B5::_tcuRttiCheck (id)                                       ||  \
            B6::_tcuRttiCheck (id)                                       ||  \
            B7::_tcuRttiCheck (id)                                       ||  \
            B8::_tcuRttiCheck (id)                                       ||  \
            B9::_tcuRttiCheck (id))                                          \
    {                                                                        \
        return (&tcu__RttiTypeIdImplementation<ThisClass>::s_id);            \
    }                                                                        \
    return (0);                                                              \
}                                                                            \



//////////////////////////////////////////////////////////////////////////////
// Implementation through standard C++ RTTI
//////////////////////////////////////////////////////////////////////////////
#else // TCU_RTTI_USE_TRUE_RTTI

//----------------------------------------------------------------------------
// typedef tcu_RttiTypeId
//----------------------------------------------------------------------------
// Type for class Type ID

typedef const void * tcu_RttiTypeId;



//----------------------------------------------------------------------------
// class tcu_Rtti
//----------------------------------------------------------------------------
// Base class for all RTTI objects. Pretty empty

class tcu_Rtti {
private:
    typedef void Base;

protected:

    // Default and copy constructors, assignment - all do nothing
    tcu_Rtti () {};
    tcu_Rtti (const tcu_Rtti &) {};
    tcu_Rtti & operator = (const tcu_Rtti &) {return (*this);};

public:

    // CREATORS

    // Do nothing. Just virtual
    virtual ~tcu_Rtti ();
};



//----------------------------------------------------------------------------
// tcu_rttiDynamicCast ()
//----------------------------------------------------------------------------
// Functions that perform dynamic casts, preserving constness
//
// Function parameter - pointer to be casted from
// Template parameter - type of object to be pointed to by resulting pointer
//  (just an object type, no '*', no 'const')

template <class T> inline
T * tcu_rttiDynamicCast (tcu_Rtti *arg)
{
    return (dynamic_cast <T *> (arg));
}

template <class T> inline
const T * tcu_rttiDynamicCast (const tcu_Rtti *arg)
{
    return (dynamic_cast <const T *> (arg));
}



//----------------------------------------------------------------------------
// tcu_isKindOf<Type>(expr)
//----------------------------------------------------------------------------
// Returns true if argument belongs to or derived from given type

template <class T> inline
bool tcu_isKindOf (const tcu_Rtti &arg)
{
    return dynamic_cast <const T *> (&arg) != 0;
}



//----------------------------------------------------------------------------
// tcu_rttiTypeId, TCU_RTTI_TYPE_ID
//----------------------------------------------------------------------------
// Declare functions, necessary for RTTI object. Forces scope to be 'public
// Former macro may be used in any class, latter - in abstract only

#define tcu_rttiTypeId(X)   (&typeid(X))
#define TCU_RTTI_TYPE_ID(X) (&typeid(X))



//----------------------------------------------------------------------------
// TCU_RTTI_DECLARE, TCU_RTTI_IMPLEMENT series
//----------------------------------------------------------------------------
#define TCU_RTTI_DECLARE
#define TCU_RTTI_DECLARE_ABSTRACT
#define TCU_RTTI_IMPLEMENT(ThisClass)
#define TCU_RTTI_IMPLEMENT_1(ThisClass,B1)
#define TCU_RTTI_IMPLEMENT_2(ThisClass,B1,B2)
#define TCU_RTTI_IMPLEMENT_3(ThisClass,B1,B2,B3)
#define TCU_RTTI_IMPLEMENT_4(ThisClass,B1,B2,B3,B4)
#define TCU_RTTI_IMPLEMENT_5(ThisClass,B1,B2,B3,B4,B5)
#define TCU_RTTI_IMPLEMENT_6(ThisClass,B1,B2,B3,B4,B5,B6)
#define TCU_RTTI_IMPLEMENT_7(ThisClass,B1,B2,B3,B4,B5,B6,B7)
#define TCU_RTTI_IMPLEMENT_8(ThisClass,B1,B2,B3,B4,B5,B6,B7,B8)
#define TCU_RTTI_IMPLEMENT_9(ThisClass,B1,B2,B3,B4,B5,B6,B7,B8,B9)

#endif // TCU_RTTI_USE_TRUE_RTTI

#define TCU__WARN_MODE 0
#include <tcuwarn.h>

#endif // TCURTTI_H
//===========================================================================
// $Log:   tcurtti.h_v  $
// 
//    Rev 1.5   27 Aug 1998 19:17:58   FNS
// Comment on possible problems with DLL added.
// 
//    Rev 1.4   17 Aug 1998 15:34:00   FNS
// TCU_RTTI_DECLARE_ABSTRACT reintroduced
// tcu_isKindOf<Type>(expr) introduced
// 
//    Rev 1.3   31 Jul 1998 19:22:48   FNS
// TCUWARN employed to suppres unnecessary compiler warnings
// 
//    Rev 1.2   29 Jul 1998 12:15:08   AAT
//
//    Rev 1.1   24 Jul 1998 15:20:16   FNS
// TCU_RTTI_DECLARE_ABSTRACT removed (TCU_RTTI_DECLARE could be used instead)
//
//    Rev 1.0   13 May 1998 16:46:36   FNS
//
