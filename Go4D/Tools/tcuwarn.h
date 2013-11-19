//============================================================================
// Moscow Development Center of Ashtech (Magellan Corp.)
// Written by Fedor Sherstyuk (fns@ashtech.msk.ru)
// Tested by  Alexander Timofeev
// This source code provided as is, without any warranties or responsibilities
//----------------------------------------------------------------------------
// $Workfile:   tcuwarn.h  $
// $Date:   03 Aug 1998 17:53:52  $
// $Revision:   1.1  $
// $Author:   FNS  $
//----------------------------------------------------------------------------
//                       Supression of compiler warnings
//============================================================================

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Purpose of this file is to suppress most annoying and senseless          //
// warnings, thus allowing sensible compilation at maximum warning level.   //
// It do this by setting appropriate warning-suppression pragmas.           //
//                                                                          //
// This file  by default do  nothing. It is compiler-specific and currently //
// support following compilers:                                             //
//                                                                          //
// - Visual C++ v5.0                                                        //
//                                                                          //
// Behavior of this file is controlled by following macros:                 //
//                                                                          //
// TCU__WARN_MODE = 0 - Restore default warning settings                    //
//                                                                          //
//                  1 - Set pragmas for TCU header                          //
//                                                                          //
//                  2 - Set pragmas for TCU source                          //
//                                                                          //
//                  3 - Set pragmas for external header (e.g. windows.h)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#ifdef TCU__WARN_MODE

#  if TCU__WARN_MODE == 1

//----------------------------------------------------------------------------
// Pragmas for TCU header files

#ifdef _MSC_VER
# if _MSC_VER == 1100

#pragma warning( disable: 4097 ) //typedef-name used as synonym for class-name
#pragma warning( default: 4201 ) //nonstandard extension:nameless struct/union
#pragma warning( default: 4514 ) //unreferenced inline function removed

# endif // _MSC_VER == 1100
#endif

// End of pragmas for TCU header files
//----------------------------------------------------------------------------

#      elif TCU__WARN_MODE == 3

//----------------------------------------------------------------------------
// Pragmas for external header files

#ifdef _MSC_VER
# if _MSC_VER == 1100

#pragma warning( default: 4097 ) //typedef-name used as synonym for class-name
#pragma warning( disable: 4201 ) //nonstandard extension:nameless struct/union
#pragma warning( default: 4514 ) //unreferenced inline function removed

# endif // _MSC_VER == 1100
#endif

// End of pragmas for external header files
//----------------------------------------------------------------------------

#      elif TCU__WARN_MODE == 2

//----------------------------------------------------------------------------
// Pragmas for TCU source

#ifdef _MSC_VER
# if _MSC_VER == 1100

#pragma warning( disable: 4097 ) //typedef-name used as synonym for class-name
#pragma warning( default: 4201 ) //nonstandard extension:nameless struct/union
#pragma warning( disable: 4514 ) //unreferenced inline function removed

# endif // _MSC_VER == 1100
#endif

// End of pragmas for TCU source
//----------------------------------------------------------------------------

#      elif TCU__WARN_MODE == 0

//----------------------------------------------------------------------------
// Pragmas for restoring default warning processing

#ifdef _MSC_VER
# if _MSC_VER == 1100

#pragma warning( default: 4097 ) //typedef-name used as synonym for class-name
#pragma warning( default: 4201 ) //nonstandard extension:nameless struct/union
#pragma warning( default: 4514 ) //unreferenced inline function removed

# endif // _MSC_VER == 1100
#endif

// End of pragmas for restoring default warning processing
//----------------------------------------------------------------------------

#  else // TCU__WARN_MODE
#    undef TCU__WARN_MODE
#    error TCU__WARN_MODE has improper value
#  endif

#else // def TCU__WARN_MODE
#  error TCU__WARN_MODE must be defined
#endif // def TCU__WARN_MODE

#ifdef TCU__WARN_MODE
#undef TCU__WARN_MODE
#endif // def TCU__WARN_MODE
//===========================================================================
// $Log:   tcuwarn.h_v  $
// 
//    Rev 1.1   03 Aug 1998 17:53:52   FNS
// TCU_WARN_SUPPRESS macro no more necessary for using TCUWARN.H
// 
//    Rev 1.0   31 Jul 1998 17:22:14   FNS
// 
