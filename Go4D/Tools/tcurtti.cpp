//============================================================================
// Moscow Development Center of Ashtech (Magellan Corp.)
// Written by Fedor Sherstyuk (fns@ashtech.msk.ru)
// Tested by  Alexander Timofeev
// This source code provided as is, without any warranties or responsibilities
//----------------------------------------------------------------------------
// $Workfile:   tcurtti.cpp  $
// $Date:   03 Aug 1998 17:54:00  $
// $Revision:   1.2  $
// $Author:   FNS  $
//----------------------------------------------------------------------------
//            RTTI emulation (protocol hierarchies). Implementation
//============================================================================

#include <tcurtti.h>

#define TCU__WARN_MODE 2
#include <tcuwarn.h>

tcu_Rtti::~tcu_Rtti ()
{}

#ifndef TCU_RTTI_USE_TRUE_RTTI

tcu_RttiTypeId tcu_Rtti::_tcuRttiCheck (tcu_RttiTypeId id) const
{
    if ((id == 0) || (id == &tcu__RttiTypeIdImplementation<tcu_Rtti>::s_id)) {
        return (&tcu__RttiTypeIdImplementation<tcu_Rtti>::s_id);
    }
    return (0);
}

#endif // TCU_RTTI_USE_TRUE_RTTI

//===========================================================================
// $Log:   tcurtti.cpv  $
// 
//    Rev 1.2   03 Aug 1998 17:54:00   FNS
// TCU_WARN_SUPPRESS macro no more necessary for using TCUWARN.H
// 
//    Rev 1.1   31 Jul 1998 19:22:34   FNS
// TCUWARN employed to fight unnecessary compiler warnings
// 
//    Rev 1.0   13 May 1998 16:46:38   FNS
// 
