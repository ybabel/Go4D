/*************************************************************************\

  Copyright 2000 The University of North Carolina at Chapel Hill.
  All Rights Reserved.

  Permission to use, copy, modify OR distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
  USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
  OF NORTH CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGES.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
  NORTH CAROLINA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted via:

  US Mail:             S. Ehmann, M. Lin
                       Department of Computer Science
                       Sitterson Hall, CB #3175
                       University of N. Carolina
                       Chapel Hill, NC 27599-3175

  Phone:               (919) 962-1749

  EMail:               geom@cs.unc.edu
                       ehmann@cs.unc.edu
                       lin@cs.unc.edu

\**************************************************************************/


//////////////////////////////////////////////////////////////////////////////
//
// SWIFT_pair.h
//
// Description:
//      Classes to manage a pair of objects.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _SWIFT_PAIR_H_
#define _SWIFT_PAIR_H_

#include <SWIFT_config.h>
#include <SWIFT_common.h>
#include <SWIFT_linalg.h>

class SWIFT_Object;

//////////////////////////////////////////////////////////////////////////////
// SWIFT_Pair
//
// Description:
//      Class to manage a pair of objects.
//////////////////////////////////////////////////////////////////////////////
class SWIFT_Pair {
  public:
    SWIFT_Pair( )
    {
        feat1 = NULL; feat2 = NULL;
        status = 0x7;   // No overlap, not initialized or active
        next = NULL; prev = NULL;
        Set_Active();
    }
    ~SWIFT_Pair( ) { }

    // Get functions
    int Id1( ) const { return id1; }
    int Id2( ) const { return id2; }

#ifdef SWIFT_HIERARCHY
    // Restriction to 32 levels (refer to MAX_LEVELS in SWIFT_mesh.h)
    int Level1( ) const { return (0x7c0 & status) >> 6; }
    int Level2( ) const { return (0xf800 & status) >> 11; }
#endif
    RESULT_TYPE State( ) const { return (RESULT_TYPE)((0x38 & status) >> 3); }
    bool Overlapping( ) const { return !(0x7 & status); }
    // Used if we wish to measure coherence -- revisit
    int Lookup_Table_Id( ) const { return (0x7ff0000 & status) >> 16; }


    bool Active( ) const { return (feat1 != NULL); }
    bool Inactive( ) const { return (feat1 == NULL); }
    bool Initialized( ) const { return (feat2 != NULL); }
    bool Uninitialized( ) const { return (feat2 == NULL); }

    SWIFT_Pair* Next( ) const { return next; }
    SWIFT_Pair* Prev( ) const { return prev; }

    // Set functions
    void Set_Id1( int id ) { id1 = id; }
    void Set_Id2( int id ) { id2 = id; }
#ifdef SWIFT_HIERARCHY
    void Set_Level1( int l ) { status = (0xfffff83f & status) | (l << 6); }
    void Set_Level2( int l ) { status = (0xffff07ff & status) | (l << 11); }
    void Increment_Level1( ) { status += 0x40; }
    void Increment_Level2( ) { status += 0x800; }
    void Decrement_Level1( ) { status -= 0x40; }
    void Decrement_Level2( ) { status -= 0x800; }
#endif
    void Toggle_Overlap( int axis ) { status ^= (1 << axis); }
    void Set_State( RESULT_TYPE state )
                        { status = (0xffffffc7 & status) | (state << 3); }
    // Used if we wish to measure coherence -- revisit
    void Set_Lookup_Table_Id( int id )
                        { status = (0xf800ffff & status) | (id << 16); }
    void Set_Active( )
                { if( feat1 == NULL ) { feat1 = (void*) 0x1; feat2 = NULL; } }
    void Set_Inactive( ) { feat1 = NULL; }
    void Set_Uninitialized( ) { feat2 = NULL; }
    void Set_Features( SWIFT_Tri_Vertex* vert1, SWIFT_Tri_Vertex* vert2 )
    {
        feat1 = (void*)vert1; feat2 = (void*)vert2;
        Set_State( CONTINUE_VV );
    }
    void Set_Next( SWIFT_Pair* n ) { next = n; }
    void Set_Prev( SWIFT_Pair* p ) { prev = p; }

    // Query functions

    // Returns true if there is a tolerance violation, otherwise false.
    bool Tolerance( SWIFT_Object* o1, SWIFT_Object* o2, int p1, int p2,
                    SWIFT_Real tolerance );

    // Returns if there is an intersection, otherwise false.
    // Sets the approximate distance and the error.  It is
    // assumed that the tolerances >= 0.  This function is the same as
    // Exact_Distance if SWIFT_HIERARCHY is off.
    bool Approximate_Distance( SWIFT_Object* o1, SWIFT_Object* o2,
                               int p1, int p2, SWIFT_Real distance_tolerance,
                               SWIFT_Real error_tolerance,
                               SWIFT_Real& distance, SWIFT_Real& error );

    // Returns true if there is an intersection, otherwise false.
    // Sets the exact distance if the tolerance was met.  It is assumed that
    // tolerance >= 0.
    bool Distance( SWIFT_Object* o1, SWIFT_Object* o2, int p1, int p2,
                   SWIFT_Real tolerance,
                   SWIFT_Real& distance );

    // Returns true if there is an intersection, otherwise false.
    // Sets num_contacts to the number of contacts within the given tolerance
    // that were found.  Sets the exact distance if the tolerance was met.
    // It is assumed that tolerance >= 0.
    bool Contacts( SWIFT_Object* o1, SWIFT_Object* o2, int p1, int p2,
                   SWIFT_Real tolerance, SWIFT_Real& distance,
                   int& num_contacts );

    // These will fill the arrays with whatever number of contacts was returned
    // on the last distance call.  Based on the number of contacts and the
    // maximum number of spaces required in the arrays for each call, the
    // caller is responsible to ensure that there is enough room at the end of
    // the arrays.
    void Distances( SWIFT_Array<SWIFT_Real>& dists );
    void Errors( SWIFT_Array<SWIFT_Real>& errs );
    void Contact_Features( SWIFT_Array<int>& pids,
                           SWIFT_Array<int>& ftypes, SWIFT_Array<int>& fids );
    void Contact_Points( SWIFT_Array<SWIFT_Real>& points );
    void Contact_Normals( SWIFT_Array<SWIFT_Real>& normals );


  private:
    inline void Save_State();

    // Per pair data

    // The ids of the pieces.
    int id1;
    int id2;

    // overlap status in the lowest 3 bits then state in the next 3 bits then
    // level of object1 is in the next 5 bits then level of object2 is in the
    // next 5 bits then the index into the normal map is in the next 11 bits
    unsigned int status;

    // if feat1 == NULL then the pair is not active
    void* feat1;
    // if feat2 == NULL then the pair is not initialized
    void* feat2;

    // next and previous pointers for the box overlap list
    SWIFT_Pair* next;
    SWIFT_Pair* prev;
};

#endif


