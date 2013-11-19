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
// pair.C
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <SWIFT_config.h>
#include <SWIFT_common.h>
#include <SWIFT_linalg.h>
#include <SWIFT_array.h>
#include <SWIFT_mesh.h>
#include <SWIFT_object.h>
#include <SWIFT_pair.h>


///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////

#ifdef SWIFT_DEBUG
// The number of state transitions before an infinite loop is declared
static const int STATE_TRANS_CYCLE_DECL = 10000;
#endif

// Feature type identifiers
static const int VERTEX = 1;
static const int EDGE = 2;
static const int FACE = 3;


///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////

// These are set by the scene before a query
static SWIFT_Object* obj1;
static SWIFT_Object* obj2;
static int piece1;
static int piece2;


///////////////////////////////////////////////////////////////////////////////
// Local variables
///////////////////////////////////////////////////////////////////////////////

// Helper variables
static SWIFT_Real dist;
static SWIFT_Triple fdir;

// Forwarding variables
static SWIFT_Triple* t1xp = new SWIFT_Triple;
static SWIFT_Triple* h1xp = new SWIFT_Triple;
static SWIFT_Triple* u1xp = new SWIFT_Triple;
static SWIFT_Triple* t2xp = new SWIFT_Triple;
static SWIFT_Triple* h2xp = new SWIFT_Triple;
static SWIFT_Triple* u2xp = new SWIFT_Triple;
static SWIFT_Triple* fnxp = new SWIFT_Triple;
static SWIFT_Real dt12, dh12, dt21, dh21;
static SWIFT_Real dl12, dr12, dl21, dr21;
static SWIFT_Real lam_min1, lam_max1, lam_min2, lam_max2;

// Transformation variables
static SWIFT_Transformation trans12;
static SWIFT_Transformation trans21;
static SWIFT_Transformation* T12 = &trans12;
static SWIFT_Transformation* T21 = &trans21;

// State transition variables
static SWIFT_Tri_Vertex* v1;
static SWIFT_Tri_Vertex* v2;
static SWIFT_Tri_Edge* e1;
static SWIFT_Tri_Edge* e2;
static SWIFT_Tri_Face* f1;
static SWIFT_Tri_Face* f2;

// State saving variables
static RESULT_TYPE save_state;
static SWIFT_Tri_Vertex* save_v1;
static SWIFT_Tri_Vertex* save_v2;
static SWIFT_Tri_Edge* save_e1;
static SWIFT_Tri_Edge* save_e2;
static SWIFT_Tri_Face* save_f1;
static SWIFT_Tri_Face* save_f2;

// State flag for keeping track of the state across global function calls
static RESULT_TYPE state;
static RESULT_TYPE prev_state;


// feat1, feat2, distance, error, pair type lists for holding the
// contacts during traversal
static SWIFT_Array<void*> contact_list1(1);
static SWIFT_Array<void*> contact_list2(1);
static SWIFT_Array<SWIFT_Real> contact_listd(1);
static SWIFT_Array<SWIFT_Real> contact_liste(1);
static SWIFT_Array<RESULT_TYPE> contact_listt(1);

#ifdef SWIFT_HIERARCHY
// Convex hierarchy variables
static int level1;
static int level2;
static int save_level1;
static int save_level2;
#endif






//////////////////////////////////////////////////////////////////////////////
// Function prototypes
//////////////////////////////////////////////////////////////////////////////


// Initialization functions

// Intialize the pair with a random pair of vertices
inline void Initialize_Randomly( RESULT_TYPE& start_state );

// Initialize the pair with a pair of vertices according to the lookup table
// fdir should have already been computed by the calling function to be the
// vector from object 1's center of mass to object 2's center of mass and be
// in object 1's local frame.
inline void Initialize_From_Scratch( RESULT_TYPE& start_state );
// Same thing but the roles of 1 and 2 are reversed (optimization)
inline void Initialize_From_Scratch2( RESULT_TYPE& start_state );

// Initialize the pair with the pair of features from the end of previous query
inline void Initialize_From_Previous( RESULT_TYPE& start_state,
                                      void* feat1, void* feat2 );

// Setup the state for either an intersection query or a distance query
inline void Setup_Pair_Query( SWIFT_Object* o1, SWIFT_Object* o2,
                              int p1, int p2
                              );

// Setup the state for either an intersection query or a distance query
inline void Setup_Piece_Query( RESULT_TYPE& start_state,
                               void* feat1, void* feat2 );


#ifdef SWIFT_HIERARCHY
// Refinement functions for convex hierarchies
RESULT_TYPE Refine( RESULT_TYPE cur_state );
void Grow( SWIFT_Real distance );
#endif

// State transition functions.
inline RESULT_TYPE Vertex_Vertex( );
inline RESULT_TYPE Vertex_Edge( );
inline RESULT_TYPE Vertex_Face( );
       RESULT_TYPE Edge_Edge( );
       RESULT_TYPE Edge_Face( SWIFT_Tri_Edge* edge1, SWIFT_Triple* tx,
                              SWIFT_Triple* hx, SWIFT_Triple* ux,
                              SWIFT_Tri_Edge*& edge2,
                              SWIFT_Tri_Vertex*& vert2, SWIFT_Tri_Face*& face2
                              );

// Intersection and nearest features function for a pair of convex polyhedra
bool Walk_Convex_LC( RESULT_TYPE start_state );
// Distance of the nearest features
inline void Distance_After_Walk_Convex_LC( SWIFT_Real& distance );
inline bool Distance_Convex_LC( RESULT_TYPE start_state, SWIFT_Real& distance );

// Tolerance and distance functions for a pair of objects.  The compile
// flags determine what types of objects are possible
bool Tolerance_LC( RESULT_TYPE start_state, SWIFT_Real tolerance );
bool Distance_LC( RESULT_TYPE start_state,
#ifdef SWIFT_HIERARCHY
                  SWIFT_Real distance_tolerance, SWIFT_Real error_tolerance,
                  SWIFT_Real& grow_dist, SWIFT_Real& distance
#else
                  SWIFT_Real& distance
#endif
                );

//////////////////////////////////////////////////////////////////////////////
// Initialization functions
//////////////////////////////////////////////////////////////////////////////

inline void Initialize_Randomly( RESULT_TYPE& start_state )
{
    const int rint1 = (int)( drand48() *
                                obj1->Coarsest_Mesh( piece1 )->Num_Faces()*3 );
    const int fid1 = rint1 / 3;
    v1 = obj1->Coarsest_Mesh( piece1 )->Faces()[fid1].Edge1().Origin();
    const int rint2 = (int)( drand48() *
                                obj2->Coarsest_Mesh( piece2 )->Num_Faces()*3 );
    const int fid2 = rint2 / 3;
    v2 = obj2->Coarsest_Mesh( piece2 )->Faces()[fid2].Edge1().Origin();
#ifdef SWIFT_HIERARCHY
    level1 = obj1->Coarsest_Level_Id( piece1 );
    level2 = obj2->Coarsest_Level_Id( piece2 );
#endif
    start_state = CONTINUE_VV;
}

inline void Initialize_From_Scratch( RESULT_TYPE& start_state )
{

    // Create center of mass direction vector to be in obj1's local frame
    v1 = obj1->Coarsest_Mesh( piece1 )->Lookup_Vertex( fdir );

    // Move the center of mass direction vector to be in obj2's local frame
    fdir &= trans12;
    fdir.Negate();
    v2 = obj2->Coarsest_Mesh( piece2 )->Lookup_Vertex( fdir );
#ifdef SWIFT_HIERARCHY
    level1 = obj1->Coarsest_Level_Id( piece1 );
    level2 = obj2->Coarsest_Level_Id( piece2 );
#endif
    // Set the state to reflect the fact that we are choosing two vertices.
    start_state = CONTINUE_VV;
}

inline void Initialize_From_Scratch2( RESULT_TYPE& start_state )
{

    // Create center of mass direction vector to be in obj1's local frame
    v2 = obj2->Coarsest_Mesh( piece2 )->Lookup_Vertex( fdir );

    // Move the center of mass direction vector to be in obj2's local frame
    fdir &= trans21;
    fdir.Negate();
    v1 = obj1->Coarsest_Mesh( piece1 )->Lookup_Vertex( fdir );
#ifdef SWIFT_HIERARCHY
    level1 = obj1->Coarsest_Level_Id( piece1 );
    level2 = obj2->Coarsest_Level_Id( piece2 );
#endif
    // Set the state to reflect the fact that we are choosing two vertices.
    start_state = CONTINUE_VV;
}

inline void Initialize_From_Previous( RESULT_TYPE& start_state,
                                      void* feat1, void* feat2 )
{
    switch( start_state ) {
    case CONTINUE_VV:
        v1 = (SWIFT_Tri_Vertex*) feat1;
        v2 = (SWIFT_Tri_Vertex*) feat2;
        break;
    case CONTINUE_VE:
        v1 = (SWIFT_Tri_Vertex*) feat1;
        e2 = (SWIFT_Tri_Edge*) feat2;
        break;
    case CONTINUE_EV:
        e1 = (SWIFT_Tri_Edge*) feat1;
        v2 = (SWIFT_Tri_Vertex*) feat2;
        break;
    case CONTINUE_VF:
        v1 = (SWIFT_Tri_Vertex*) feat1;
        f2 = (SWIFT_Tri_Face*) feat2;
        break;
    case CONTINUE_FV:
        f1 = (SWIFT_Tri_Face*) feat1;
        v2 = (SWIFT_Tri_Vertex*) feat2;
        break;
    case CONTINUE_EE:
        e1 = (SWIFT_Tri_Edge*) feat1;
        e2 = (SWIFT_Tri_Edge*) feat2;
    default:
        break;
    }
}

inline void Setup_Pair_Query( SWIFT_Object* o1, SWIFT_Object* o2,
                              int p1, int p2
                              )
{
    // Set the objects and the pieces
    obj1 = o1; obj2 = o2; piece1 = p1; piece2 = p2;

    // Compute the T12 and T21 transformations
    trans12.Transform_From_To( o1->Transformation(), o2->Transformation() );
    trans21.Invert( trans12 );

#ifdef SWIFT_HIERARCHY
    level1 = Level1();
    level2 = Level2();
#endif
}

inline void Setup_Piece_Query( RESULT_TYPE& start_state,
                               void* feat1, void* feat2 )
{

    if( feat2 == NULL ) {
        // There are no last starting features so initialize the starting
        // features based on the normal map stored in the objects.
        fdir = (trans21 * obj2->Center_Of_Mass( piece2 )) -
               obj1->Center_Of_Mass( piece1 );
        Initialize_From_Scratch( start_state );
    } else {
        // Initialize from the previous pair of features
        // Levels have already been set by the caller
        Initialize_From_Previous( start_state, feat1, feat2 );
    }
}


//////////////////////////////////////////////////////////////////////////////
// Walking/Intersection functions
//////////////////////////////////////////////////////////////////////////////

#ifdef SWIFT_HIERARCHY
// Refine (go to a finer level) on one or both of the objects or signal that
// this is not possible
RESULT_TYPE Refine( RESULT_TYPE cur_state )
{
    int go_down;


    if( level1 == 0 && level2 == 0 ) {
        return DISJOINT;
    }


    if( level1 == 0 ) {
        // Walk down on object2
        go_down = 2;
        level2--;
    } else if( level2 == 0 ) {
        // Walk down on object1
        go_down = 1;
        level1--;
    // Go down 2 children on each object or go down 1 child on
    // each object
    } else {
        // Walk down on both objects
        go_down = 3;
        level1--;
        level2--;
    }

    switch( cur_state ) {
    case CONTINUE_VV:
        if( go_down == 1 ) {
            // Walk down on object1
            v1 = v1->Child();
            if( level1 != 0 ) {
                level1--;
                v1 = v1->Child();
            }
        } else if( go_down == 2 ) {
            // Walk down on object2
            v2 = v2->Child();
            if( level2 != 0 ) {
                level2--;
                v2 = v2->Child();
            }
        } else {
            // Walk down on both objects
            v1 = v1->Child();
            v2 = v2->Child();
            if( level1 != 0 && level2 != 0 ) {
                level1--; level2--;
                v1 = v1->Child();
                v2 = v2->Child();
            } else if( level1 != 0 ) {
                level1--;
                v1 = v1->Child();
            } else if( level2 != 0 ) {
                level2--;
                v2 = v2->Child();
            }
        }
        return CONTINUE_VV;
        break;
    case CONTINUE_VE:
        if( go_down == 1 ) {
            // Walk down on object1
            cur_state = CONTINUE_VE;
            v1 = v1->Child();
            if( level1 != 0 ) {
                level1--;
                v1 = v1->Child();
            }
        } else if( go_down == 2 ) {
            // Walk down on object2
            if( e2->Is_Child_Vertex() ) {
                cur_state = CONTINUE_VV;
                v2 = e2->Vertex_Child();
            } else {
                cur_state = CONTINUE_VE;
                e2 = e2->Child();
            }
            if( level2 != 0 ) {
                level2--;
                if( cur_state == CONTINUE_VV ) {
                    v2 = v2->Child();
                } else {
                    if( e2->Is_Child_Vertex() ) {
                        cur_state = CONTINUE_VV;
                        v2 = e2->Vertex_Child();
                    } else { 
                        e2 = e2->Child();
                    }
                }
            }
        } else {
            v1 = v1->Child();
            if( e2->Is_Child_Vertex() ) {
                v2 = e2->Vertex_Child();
                cur_state = CONTINUE_VV;
            } else {
                e2 = e2->Child();
                cur_state = CONTINUE_VE;
            }
            if( level1 != 0 && level2 != 0 ) {
                level1--; level2--;
                v1 = v1->Child();
                if( cur_state == CONTINUE_VV ) {
                    v2 = v2->Child();
                } else {
                    if( e2->Is_Child_Vertex() ) {
                        v2 = e2->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e2 = e2->Child();
                    }
                }
            } else if( level1 != 0 ) {
                level1--;
                v1 = v1->Child();
            } else if( level2 != 0 ) {
                level2--;
                if( cur_state == CONTINUE_VV ) {
                    v2 = v2->Child();
                } else {
                    if( e2->Is_Child_Vertex() ) {
                        v2 = e2->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e2 = e2->Child();
                    }
                }
            }
        }
        return cur_state;
        break;
    case CONTINUE_EV:
        if( go_down == 1 ) {
            // Walk down on object1
            if( e1->Is_Child_Vertex() ) {
                cur_state = CONTINUE_VV;
                v1 = e1->Vertex_Child();
            } else {
                cur_state = CONTINUE_EV;
                e1 = e1->Child();
            }
            if( level1 != 0 ) {
                level1--;
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                } else {
                    if( e1->Is_Child_Vertex() ) {
                        cur_state = CONTINUE_VV;
                        v1 = e1->Vertex_Child();
                    } else { 
                        e1 = e1->Child();
                    }
                }
            }
        } else if( go_down == 2 ) {
            // Walk down on object2
            cur_state = CONTINUE_EV;
            v2 = v2->Child();
            if( level2 != 0 ) {
                level2--;
                v2 = v2->Child();
            }
        } else {
            v2 = v2->Child();
            if( e1->Is_Child_Vertex() ) {
                v1 = e1->Vertex_Child();
                cur_state = CONTINUE_VV;
            } else {
                e1 = e1->Child();
                cur_state = CONTINUE_EV;
            }
            if( level1 != 0 && level2 != 0 ) {
                level1--; level2--;
                v2 = v2->Child();
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                } else {
                    if( e1->Is_Child_Vertex() ) {
                        v1 = e1->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e1 = e1->Child();
                    }
                }
            } else if( level1 != 0 ) {
                level1--;
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                } else {
                    if( e1->Is_Child_Vertex() ) {
                        v1 = e1->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e1 = e1->Child();
                    }
                }
            } else if( level2 != 0 ) {
                level2--;
                v2 = v2->Child();
            }
        }
        return cur_state;
        break;
    case CONTINUE_VF:
        if( go_down == 1 ) {
            // Walk down on object1
            cur_state = CONTINUE_VF;
            v1 = v1->Child();
            if( level1 != 0 ) {
                level1--;
                v1 = v1->Child();
            }
        } else if( go_down == 2 ) {
            // Walk down on object2
            if( f2->Is_Child_Vertex() ) {
                cur_state = CONTINUE_VV;
                v2 = f2->Vertex_Child();
            } else {
                cur_state = CONTINUE_VF;
                f2 = f2->Child();
            }
            if( level2 != 0 ) {
                level2--;
                if( cur_state == CONTINUE_VV ) {
                    v2 = v2->Child();
                } else {
                    if( f2->Is_Child_Vertex() ) {
                        cur_state = CONTINUE_VV;
                        v2 = f2->Vertex_Child();
                    } else { 
                        f2 = f2->Child();
                    }
                }
            }
        } else {
            v1 = v1->Child();
            if( f2->Is_Child_Vertex() ) {
                v2 = f2->Vertex_Child();
                cur_state = CONTINUE_VV;
            } else {
                f2 = f2->Child();
                cur_state = CONTINUE_VF;
            }
            if( level1 != 0 && level2 != 0 ) {
                level1--; level2--;
                v1 = v1->Child();
                if( cur_state == CONTINUE_VV ) {
                    v2 = v2->Child();
                } else {
                    if( f2->Is_Child_Vertex() ) {
                        v2 = f2->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        f2 = f2->Child();
                    }
                }
            } else if( level1 != 0 ) {
                level1--;
                v1 = v1->Child();
            } else if( level2 != 0 ) {
                level2--;
                if( cur_state == CONTINUE_VV ) {
                    v2 = v2->Child();
                } else {
                    if( f2->Is_Child_Vertex() ) {
                        v2 = f2->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        f2 = f2->Child();
                    }
                }
            }
        }
        return cur_state;
        break;
    case CONTINUE_FV:
        if( go_down == 1 ) {
            // Walk down on object1
            if( f1->Is_Child_Vertex() ) {
                cur_state = CONTINUE_VV;
                v1 = f1->Vertex_Child();
            } else {
                cur_state = CONTINUE_FV;
                f1 = f1->Child();
            }
            if( level1 != 0 ) {
                level1--;
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                } else {
                    if( f1->Is_Child_Vertex() ) {
                        cur_state = CONTINUE_VV;
                        v1 = f1->Vertex_Child();
                    } else { 
                        f1 = f1->Child();
                    }
                }
            }
        } else if( go_down == 2 ) {
            // Walk down on object2
            cur_state = CONTINUE_FV;
            v2 = v2->Child();
            if( level2 != 0 ) {
                level2--;
                v2 = v2->Child();
            }
        } else {
            v2 = v2->Child();
            if( f1->Is_Child_Vertex() ) {
                v1 = f1->Vertex_Child();
                cur_state = CONTINUE_VV;
            } else {
                f1 = f1->Child();
                cur_state = CONTINUE_FV;
            }
            if( level1 != 0 && level2 != 0 ) {
                level1--; level2--;
                v2 = v2->Child();
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                } else {
                    if( f1->Is_Child_Vertex() ) {
                        v1 = f1->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        f1 = f1->Child();
                    }
                }
            } else if( level1 != 0 ) {
                level1--;
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                } else {
                    if( f1->Is_Child_Vertex() ) {
                        v1 = f1->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        f1 = f1->Child();
                    }
                }
            } else if( level2 != 0 ) {
                level2--;
                v2 = v2->Child();
            }
        }
        return cur_state;
        break;
    case CONTINUE_EE:
        if( go_down == 1 ) {
            // Walk down on object1
            if( e1->Is_Child_Vertex() ) {
                v1 = e1->Vertex_Child();
                cur_state = CONTINUE_VE;
            } else {
                e1 = e1->Child();
                cur_state = CONTINUE_EE;
            }
            if( level1 != 0 ) {
                level1--;
                if( cur_state == CONTINUE_VE ) {
                    v1 = v1->Child();
                } else {
                    if( e1->Is_Child_Vertex() ) {
                        cur_state = CONTINUE_VE;
                        v1 = e1->Vertex_Child();
                    } else { 
                        e1 = e1->Child();
                    }
                }
            }
        } else if( go_down == 2 ) {
            // Walk down on object2
            if( e2->Is_Child_Vertex() ) {
                v2 = e2->Vertex_Child();
                cur_state = CONTINUE_EV;
            } else {
                e2 = e2->Child();
                cur_state = CONTINUE_EE;
            }
            if( level2 != 0 ) {
                level2--;
                if( cur_state == CONTINUE_EV ) {
                    v2 = v2->Child();
                } else {
                    if( e2->Is_Child_Vertex() ) {
                        cur_state = CONTINUE_EV;
                        v2 = e2->Vertex_Child();
                    } else { 
                        e2 = e2->Child();
                    }
                }
            }
        } else {
            // Walk down on both objects
            if( e1->Is_Child_Vertex() && e2->Is_Child_Vertex() ) {
                v1 = e1->Vertex_Child();
                v2 = e2->Vertex_Child();
                cur_state = CONTINUE_VV;
            } else if( e1->Is_Child_Vertex() ) {
                v1 = e1->Vertex_Child();
                e2 = e2->Child();
                cur_state = CONTINUE_VE;
            } else if( e2->Is_Child_Vertex() ) {
                e1 = e1->Child();
                v2 = e2->Vertex_Child();
                cur_state = CONTINUE_EV;
            } else {
                e1 = e1->Child();
                e2 = e2->Child();
                cur_state = CONTINUE_EE;
            }
            if( level1 != 0 && level2 != 0 ) {
                level1--; level2--;
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                    v2 = v2->Child();
                } else if( cur_state == CONTINUE_VE ) {
                    v1 = v1->Child();
                    if( e2->Is_Child_Vertex() ) {
                        v2 = e2->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e2 = e2->Child();
                    }
                } else if( cur_state == CONTINUE_EV ) {
                    v2 = v2->Child();
                    if( e1->Is_Child_Vertex() ) {
                        v1 = e1->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e1 = e1->Child();
                    }
                } else {
                    if( e1->Is_Child_Vertex() &&
                        e2->Is_Child_Vertex()
                    ) {
                        v1 = e1->Vertex_Child();
                        v2 = e2->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else if( e1->Is_Child_Vertex() ) {
                        v1 = e1->Vertex_Child();
                        e2 = e2->Child();
                        cur_state = CONTINUE_VE;
                    } else if( e2->Is_Child_Vertex() ) {
                        e1 = e1->Child();
                        v2 = e2->Vertex_Child();
                        cur_state = CONTINUE_EV;
                    } else {
                        e1 = e1->Child();
                        e2 = e2->Child();
                    }
                }
            } else if( level1 != 0 ) {
                level1--;
                if( cur_state == CONTINUE_VV ) {
                    v1 = v1->Child();
                } else if( cur_state == CONTINUE_VE ) {
                    v1 = v1->Child();
                } else if( cur_state == CONTINUE_EV ) {
                    if( e1->Is_Child_Vertex() ) {
                        v1 = e1->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e1 = e1->Child();
                    }
                } else {
                    if( e1->Is_Child_Vertex() ) {
                        v1 = e1->Vertex_Child();
                        cur_state = CONTINUE_VE;
                    } else {
                        e1 = e1->Child();
                    }
                }
            } else if( level2 != 0 ) {
                level2--;
                if( cur_state == CONTINUE_VV ) {
                    v2 = v2->Child();
                } else if( cur_state == CONTINUE_VE ) {
                    if( e2->Is_Child_Vertex() ) {
                        v2 = e2->Vertex_Child();
                        cur_state = CONTINUE_VV;
                    } else {
                        e2 = e2->Child();
                    }
                } else if( cur_state == CONTINUE_EV ) {
                    v2 = v2->Child();
                } else {
                    if( e2->Is_Child_Vertex() ) {
                        v2 = e2->Vertex_Child();
                        cur_state = CONTINUE_EV;
                    } else {
                        e2 = e2->Child();
                    }
                }
            }
        }
        return cur_state;
        break;
    default:
        break;
    }

    return DISJOINT;
}

// After the query is over, Grow is called to attempt to enlarge the objects
// based on their parents if this is at all possible.
void Grow( SWIFT_Real distance )
{
    SWIFT_Tri_Mesh* m1 = obj1->Mesh( piece1, save_level1 );
    SWIFT_Tri_Mesh* m2 = obj2->Mesh( piece2, save_level2 );
    int g1 = 0;
    int g2 = 0;

    while( m1->Parent() != NULL && m2->Parent() != NULL &&
           m1->Parent()->Deviation() - m1->Deviation() <= distance &&
           m2->Parent()->Deviation() - m2->Deviation() <= distance
    ) {
        if( m2->Deviation() * (SWIFT_Real)(m1->Num_Faces()-
                                           m1->Parent()->Num_Faces()) >
            m1->Deviation() * (SWIFT_Real)(m2->Num_Faces()-
                                           m2->Parent()->Num_Faces())
        ) { 
            // Grow mesh 1
            distance -= m1->Parent()->Deviation() - m1->Deviation();
            m1 = m1->Parent();
            save_level1++;
            g1++;
        } else {
            // Grow mesh 2
            distance -= m2->Parent()->Deviation() - m2->Deviation();
            m2 = m2->Parent();
            save_level2++;
            g2++;
        }
    }

    while( m1->Parent() != NULL &&
           m1->Parent()->Deviation() - m1->Deviation() <= distance
    ) {
        // Grow mesh 1
        distance -= m1->Parent()->Deviation() - m1->Deviation();
        m1 = m1->Parent();
        save_level1++;
        g1++;
    }

    while( m2->Parent() != NULL &&
           m2->Parent()->Deviation() - m2->Deviation() <= distance
    ) {
        // Grow mesh 2
        distance -= m2->Parent()->Deviation() - m2->Deviation();
        m2 = m2->Parent();
        save_level2++;
        g2++;
    }

    // Grow the object features of object1
    if( g1 > 0 ) {
        switch( save_state ) {
        case CONTINUE_VV: case CONTINUE_VE: case CONTINUE_VF:
            save_v1 = save_v1->Parent();
            break;
        case CONTINUE_EV:
            save_v1 = (SWIFT_Tri_Vertex*)save_e1->Parent();
            save_state = CONTINUE_VV;
            break;
        case CONTINUE_EE:
            save_v1 = (SWIFT_Tri_Vertex*)save_e1->Parent();
            save_state = CONTINUE_VE;
            break;
        case CONTINUE_FV:
            save_v1 = (SWIFT_Tri_Vertex*)save_f1->Parent();
            save_state = CONTINUE_VV;
            break;
        default:
            break;
        }
        for( g1--; g1 > 0; g1-- ) {
            save_v1 = save_v1->Parent();
        }
    }

    // Grow the object features of object2
    if( g2 > 0 ) {
        switch( save_state ) {
        case CONTINUE_VV: case CONTINUE_EV: case CONTINUE_FV:
            save_v2 = save_v2->Parent();
            break;
        case CONTINUE_VE:
            save_v2 = (SWIFT_Tri_Vertex*)save_e2->Parent();
            save_state = CONTINUE_VV;
            break;
        case CONTINUE_EE:
            save_v2 = (SWIFT_Tri_Vertex*)save_e2->Parent();
            save_state = CONTINUE_EV;
            break;
        case CONTINUE_VF:
            save_v2 = (SWIFT_Tri_Vertex*)save_f2->Parent();
            save_state = CONTINUE_VV;
            break;
        default:
            break;
        }
        for( g2--; g2 > 0; g2-- ) {
            save_v2 = save_v2->Parent();
        }
    }
}
#endif


//////////////////////////////////////////////////////////////////////////////
// Feature pair checks and closest feature functions.
//
// First there is a function to check for containment of a point in a vertex
// external voronoi region.
//
// There are four main feature pair functions to test if a given pair of
// features is the closest pair of features between two convex polyhedra.  Each
// one handles a different combination of feature types.  The four functions
// are: Vertex_Vertex, Vertex_Edge, Vertex_Face, and Edge_Edge.  In addition,
// there is a helper function called Edge_Face which resolves a specialized
// edge-face query.  It is called from the Edge_Edge function.
//
// Each of these functions takes as implicit parameters (global variables)
// the two features, two transformation matrices for transforming features from
// one frame to the other and vice versa.  The new pair of features are placed
// in the appropriate global variables.
// T12 is always the transformation matrix from the frame of the first
// feature in the parameter list to the frame of the second feature in the
// parameter list; T21 is the inverse transformation.
//
// If a pair of features processed by a check function is indeed a closest pair
// the distance between these features is placed in the dist variable and the
// separation state of the variables is passed back.  If the features are not a
// closest feature pair, a value is passed back indicating which state to
// proceed to next.  One or possibly both of the closest features are updated
// to form a new pair which is closer or of lower dimension than the previous
// pair.
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Vertex_Vertex:
//
// Checks to see if two vertices are the pair of closest features.
// The two vertices are given in v1 and v2.  If they are not, then either e1
// or e2 is set to an edge to walk to.
//////////////////////////////////////////////////////////////////////////////

inline RESULT_TYPE Vertex_Vertex( )
{
    // Check v1 against v2's region
    SWIFT_Tri_Edge* edge;

    // Transform v1 to v2's local coordinates
    if( prev_state != CONTINUE_VE ) {
        *t1xp = (*T12) * v1->Coords();
        edge = v2->Adj_Edge();
    } else {
        edge = v2->Adj_Edge()->Twin()->Next();
    }

    do {
        if( (dt12 = edge->Distance( *t1xp )) >= 0.0 ) {
            e2 = edge;
            return CONTINUE_VE;
        }
        edge = edge->Twin()->Next();
    } while( edge != v2->Adj_Edge() );


    // Check v2 against v1's region

    // Transform v2 to v1's local coordinates
    if( prev_state != CONTINUE_EV ) {
        *t2xp = (*T21) * v2->Coords();
        edge = v1->Adj_Edge();
    } else {
        edge = v1->Adj_Edge()->Twin()->Next();
    }

    do {
        if( (dt12 = edge->Distance( *t2xp )) >= 0.0 ) {
            e1 = edge;
            return CONTINUE_EV;
        }
        edge = edge->Twin()->Next();
    } while( edge != v1->Adj_Edge() );


    // Set the vector between closest points
    fdir = *t2xp - v1->Coords();

    return DISJOINT;
}


//////////////////////////////////////////////////////////////////////////////
// Vertex_Edge:
//
// Checks to see if the vertex and the edge are the pair of closest features.
// The vertex and the edge are given in v1 and e2.  If they are not, then
// we walk to the v-v, e-e, or v-f cases.  The appropriate v, e, f pointers are
// set for the next state.
//////////////////////////////////////////////////////////////////////////////

inline RESULT_TYPE Vertex_Edge( )
{
// Forward from VV, VF, or EE (not through EF?)
    // Transform v to e's local coordinates
    if( prev_state == DISJOINT ||
        (prev_state == CONTINUE_EE && state == CONTINUE_EV)
    ) {
        *t1xp = (*T12) * v1->Coords();
    }

    if( prev_state != CONTINUE_VF && prev_state != CONTINUE_FV ) {
        dl12 = e2->Face_Distance( *t1xp );
    }
    dr12 = e2->Twin()->Face_Distance( *t1xp );

    // Check v against the edge-face planes of the edge
    if( dl12 >= 0.0 ) {
        e1 = e2;
        f2 = e2->Adj_Face();
        return CONTINUE_VF;
    } else if( dr12 >= 0.0 ) {
        e1 = e2->Twin();
        dl12 = dr12;
        f2 = e1->Adj_Face();
        return CONTINUE_VF;
    }

    // Check v against the vertex-edge planes of the edge

    // Don't have to compute vertex violation of the edge's ve planes if
    // already done in EE state
    if( !(prev_state == CONTINUE_EE && state == CONTINUE_VE) ) {
        // Have to compute dh12
        dh12 = e2->Twin()->Distance( *t1xp );
        if( prev_state != CONTINUE_VV ) {
            dt12 = e2->Distance( *t1xp );
        }
    }

    if( dh12 < 0.0 ) {
        v2 = e2->Next()->Origin();
        v2->Set_Adj_Edge( e2->Twin() );
        return CONTINUE_VV;
    } else if( dt12 < 0.0 ) {
        v2 = e2->Origin();
        v2->Set_Adj_Edge( e2 );
        return CONTINUE_VV;
    }


    // The vertex lies in the edge's v-region.  Check e against the v's planes
    if( prev_state != CONTINUE_EE ) {
        *h2xp = (*T21) * e2->Next()->Origin()->Coords();
        if( !(prev_state == CONTINUE_VV && state == CONTINUE_EV) ) {
            // Only have to transform the tail if we came from VV and it was
            // transformed there already (we are in the EV state).
            *t2xp = (*T21) * e2->Origin()->Coords();
        }
        *u2xp = (*T21) & e2->Direction(); // Normalized
    }

    SWIFT_Tri_Edge* edge;
    SWIFT_Real lambda;
    SWIFT_Tri_Edge* et = NULL;
    SWIFT_Tri_Edge* eh = NULL;

    if( prev_state == CONTINUE_EE ) {
        // lam_min1 and lam_max1 are initialized correctly in EE
        edge = v1->Adj_Edge()->Twin()->Next();
    } else {
        lam_min1 = 0.0; lam_max1 = 1.0;
        edge = v1->Adj_Edge();
    }

    do {
        dt21 = edge->Distance( *t2xp );
        dh21 = edge->Distance( *h2xp );

        if( dt21 < 0.0 ) {
            // The tail is inside the plane
            if( dh21 >= 0.0 && (lambda = dt21 / (dt21 - dh21)) < lam_max1 ) {
                lam_max1 = lambda;
                eh = edge;
                if( lam_min1 > lam_max1 ) {
                    break;
                }
            }
        } else {
            // The tail is not inside the plane
            if( dh21 >= 0.0 ) {
                // The head is not inside the plane
                e1 = edge;
                lam_min1 = 0.0; lam_max1 = 1.0;
                return CONTINUE_EE;
            }
            // The head is inside the plane
            if( (lambda = dt21 / (dt21 - dh21)) > lam_min1 ) {
                lam_min1 = lambda;
                et = edge;
                if( lam_min1 > lam_max1 ) {
                    break;
                }
            }
        }
        edge = edge->Twin()->Next();
    } while( edge != v1->Adj_Edge() );

    // Check the derivatives at the ends of the clipped edge
    if( et != NULL ) {
        if( (*t2xp + (lam_min1 * e2->Length()) * (*u2xp) -
            v1->Coords()) * (*u2xp) > 0.0
        ) {
            e1 = et;
            lam_max1 = lam_min1;
            lam_min1 = 0.0;
            return CONTINUE_EE;
        }
    }
    if( eh != NULL ) {
        if( (*t2xp + (lam_max1 * e2->Length()) * (*u2xp) -
            v1->Coords()) * (*u2xp) < 0.0
        ) {
            e1 = eh;
            lam_min1 = lam_max1;
            lam_max1 = 1.0;
            return CONTINUE_EE;
        }
    }

    const SWIFT_Triple fdir2 = *h2xp - v1->Coords();
    fdir = fdir2 - (fdir2 * (*u2xp)) * (*u2xp);


    return DISJOINT;
}


//////////////////////////////////////////////////////////////////////////////
// Vertex_Face:
//
// Checks to see if the vertex and the face are the pair of closest features.
// The vertex and the face are given in v1 and f2.  If they are not, then
// we walk to the penetration, v-e, e-f->v-f, e-f->e-e, e-f->penetration,
// or e-f->v-f->penetration cases.  The appropriate v, e, f pointers are
// set for the next state.
//
// There is special code to handle the edge-face case that may arise if the
// vertex is in the face region but the corresponding point on the plane is
// not in the vertex region.
//
// When penetration is returned after calling this function, the features
// could either be the original vertex and face or another vertex and the
// original face.
//////////////////////////////////////////////////////////////////////////////

inline RESULT_TYPE Vertex_Face( )
{

    if( prev_state < CONTINUE_VF ) {
        // Transform v to f's local coordinates
        if( prev_state == DISJOINT ) {
            *t1xp = (*T12) * v1->Coords();
        }

        // Not necessary to use dl12 here...  But remember that it is a
        // forwarding variable TO here.

        // It is important to note that the e-f plane which is maximally
        // violated must be chosen and not simply the first one (incorrect).
        if( prev_state >= CONTINUE_VE ) {
            dr21 = f2->Edge2P() == e1 ? dl12 :
                                       f2->Edge2().Face_Distance( *t1xp );
            dr12 = f2->Edge3P() == e1 ? dl12 :
                                       f2->Edge3().Face_Distance( *t1xp );
            dl12 = f2->Edge1P() == e1 ? dl12 :
                                        f2->Edge1().Face_Distance( *t1xp );
        } else {
            dl12 = f2->Edge1().Face_Distance( *t1xp );
            dr21 = f2->Edge2().Face_Distance( *t1xp );
            dr12 = f2->Edge3().Face_Distance( *t1xp );
        }

        // Check v against the edge-face planes of the face
        e2 = f2->Edge1P();
        if( dr21 < dl12 ) {
            dl12 = dr21;
            e2 = f2->Edge2P();
        }
        if( dr12 < dl12 ) {
            dl12 = dr12;
            e2 = f2->Edge3P();
        }

        // Check if v is in f's v-region
        if( dl12 < 0.0 ) {
            return CONTINUE_VE;
        }

        // No edge-face plane was violated.

        // Transform f's normal to v's local coordinates
        *fnxp = (*T21) & f2->Normal();

        if( prev_state != LOCAL_MINIMUM ) {
            // Take the distance to the plane.
            dist = f2->Distance( *t1xp );

            // Take the distance from the vertex to the face plane and negate
            // the normal if necessary.
            if( dist < 0.0 ) {
                fnxp->Negate();
            }
        }
    } else if( prev_state >= CONTINUE_EE ) {
        // Transform f's normal to v's local coordinates
        *fnxp = (*T21) & f2->Normal();

        // The distance is already computed and is passed in the dist variable
        if( dist < 0.0 ) {
            fnxp->Negate();
        }
    }

    // Look for an edge that points into the face.
    SWIFT_Tri_Edge* edge = v1->Adj_Edge();
    SWIFT_Real minfd;
    minfd = SWIFT_INFINITY;
    if( prev_state >= CONTINUE_VF ) {
        edge = edge->Twin()->Next();
    }

    do {
        dr21 = (*fnxp) * edge->Direction();
        if( dr21 < minfd ) {
            minfd = dr21;
            e1 = edge;
        }
        edge = edge->Twin()->Next();
    } while( edge != v1->Adj_Edge() );

    if( minfd >= 0.0 ) {
        // The distance is already in the dist variable.

        // Check for local minimum
        if( dist < 0.0 ) {
            return LOCAL_MINIMUM;
        } else {
            return DISJOINT;
        }
    }

    // Resolve the edge-face case.  We have found an edge that is pointing into
    // the face.  minfd holds the dot product of the edge and the face normal.

    // Transform the head of the edge to f's local coordinates and compute the
    // distance of it to the first and second edge-face planes.
    *h1xp = (*T12) * e1->Next()->Origin()->Coords();

    dr21 = f2->Edge1().Face_Distance( *h1xp );
    dr12 = f2->Edge2().Face_Distance( *h1xp );

    // Find out which edge-face planes the head of the edge violates.
    // e2 will holds first one (if possible), edge holds possible second one.
    edge = NULL;
    e2 = NULL;
    if( dr21 < 0.0 ) {
        e2 = f2->Edge1P();
        if( dr12 < 0.0 ) {
            edge = f2->Edge2P();
        } else {
            if( (dr12 = f2->Edge3().Face_Distance( *h1xp )) < 0.0 ) {
                edge = f2->Edge3P();
            }
        }
    } else if( dr12 < 0.0 ) {
        dr21 = dr12;
        e2 = f2->Edge2P();
        if( (dr12 = f2->Edge3().Face_Distance( *h1xp )) < 0.0 ) {
            edge = f2->Edge3P();
        }
    } else {
        if( (dr21 = f2->Edge3().Face_Distance( *h1xp )) < 0.0 ) {
            e2 = f2->Edge3P();
        }
    }

    // Check if any of the edge-face planes were violated.
    if( e2 == NULL ) {
        // Check for penetration
        minfd = dist;
        dist = f2->Distance( *h1xp );
        if( (dist < 0.0 && minfd > 0.0) || (dist > 0.0 && minfd < 0.0) ) {
            // Previous continuation was CONTINUE_VF
            return PENETRATION;
        }

        v1 = e1->Next()->Origin();
        v1->Set_Adj_Edge( e1->Twin() );

        // Swap the head and the tail
        SWIFT_Triple* temp_xp = t1xp; t1xp = h1xp; h1xp = temp_xp;

        return CONTINUE_VF;
    }

    // One or more of the edge-face planes is pierced by the edge.
    // Find out which one is pierced first.

    *u1xp = (*T12) & e1->Direction();
    SWIFT_Real ux_dot = *u1xp * e2->Face_Normal();

    if( edge == NULL ) {
        dr12 = dr21;
    } else {
        SWIFT_Real ux_dot2 = *u1xp * edge->Face_Normal();
        // There were two edge-face planes pierced.
        if( dr12 * ux_dot > dr21 * ux_dot2 ) {
            // the other edge clips first
            e2 = edge;
            ux_dot = ux_dot2;
        } else {
            dr12 = dr21;
        }
    }

    dr21 = f2->Distance( *h1xp );

    // minfd contains the dot product of the edge direction and the face norm.
    // dr12 contains the distance that the head is from the nearest clip edge
    // ux_dot contains the dot product of the edge direction and the nearest
    //      clip edge edge-face normal.
    // dr21 contains the distance that the head is from the face plane.
    // dist contains the distance that the tail is from the face plane.

    if( (dr21 > 0.0 && dist < 0.0 && -dr12 * minfd > dr21 * ux_dot) ||
        (dr21 < 0.0 && dist > 0.0 && dr12 * minfd < dr21 * ux_dot)
    ) {
        return PENETRATION;
    }

    // Continue onto the E-E case.  e1 is set to the edge coming from the
    // input vertex and e2 is the edge belonging to the input face so
    // the parameters are set correctly.
    return CONTINUE_EE;
}


//////////////////////////////////////////////////////////////////////////////
// Edge_Edge:
//
// Checks to see if the two edges are the pair of closest features.
// The edges are given in e1 and e2.  If they are not the closest, then
// we walk to the v-v, v-e, e-v, e-f->v-f, e-f->penetration cases.
// The appropriate v, e, f pointers are set for the next state.
//
// A special function is called to handle the edge face cases that arise when
// the closest points on the edges are on the interiors and violate each
// other's regions.
//
// When penetration is returned after calling this function, the features
// could either be the original edges or one of the edges the same and the
// other different.
//////////////////////////////////////////////////////////////////////////////

// Computes the closest LOCAL points on the edges.  The vector that spans the
// two points is also computed in EDGE2's LOCAL coordinates pointing from edge1
// to edge2.
void Compute_Closest_Points_Edge_Edge(
    SWIFT_Tri_Edge* edge1, SWIFT_Tri_Edge* edge2, SWIFT_Triple& tri1,
    //SWIFT_Triple& tri2, SWIFT_Transformation& T_1_2, SWIFT_Triple* sv = NULL )
    SWIFT_Triple& tri2, SWIFT_Transformation& T_1_2, SWIFT_Real* dist = NULL )
{
    SWIFT_Triple tx = T_1_2 * edge1->Origin()->Coords();
    SWIFT_Triple ux = T_1_2 & edge1->Direction();   // Vector transform
    tri2 = edge2->Origin()->Coords() - tx;
    SWIFT_Real d1_dot_v1 = ux * tri2;
    SWIFT_Real d2_dot_v1 = edge2->Direction() * tri2;
    SWIFT_Real d = edge2->Direction() * ux;
    // t is distance along e1 and u is distance along e2
    SWIFT_Real t, u = 1.0 - d * d;  // denom

    if( u == 0.0 ) {
        t = 0.0;
        u = -d2_dot_v1;
    } else {
        t = (d1_dot_v1 - d2_dot_v1 * d) / u;
        if( t < 0.0 ) {
            t = 0.0;
            u = -d2_dot_v1;
        } else if( t > edge1->Length() ) {
            t = edge1->Length();
            u = t*d - d2_dot_v1;
        } else {
            u = t*d - d2_dot_v1;
        }
    }

    if( u < 0.0 ) {
        tri2 = edge2->Origin()->Coords();
        if( d1_dot_v1 < 0.0 ) {
            t = 0.0;
            tri1 = edge1->Origin()->Coords();
        } else if( d1_dot_v1 > edge1->Length() ) {
            t = edge1->Length();
            tri1 = edge1->Next()->Origin()->Coords();
        } else {
            t = d1_dot_v1;
            tri1 = edge1->Origin()->Coords() + t * edge1->Direction();
        }
    } else if( u > edge2->Length() ) {
        t = edge2->Length() * d + d1_dot_v1;
        tri2 = edge2->Next()->Origin()->Coords();
        if( t < 0.0 ) {
            t = 0.0;
            tri1 = edge1->Origin()->Coords();
        } else if( t > edge1->Length() ) {
            t = edge1->Length();
            tri1 = edge1->Next()->Origin()->Coords();
        } else {
            tri1 = edge1->Origin()->Coords() + t * edge1->Direction();
        }
    } else { 
        // The points lie on the interiors of the edges
        tri1 = edge1->Origin()->Coords() + t * edge1->Direction();
        tri2 = edge2->Origin()->Coords() + u * edge2->Direction();
    }

    if( dist != NULL ) {
        *dist = tri2.Dist( tx + t * ux );
    }
}

RESULT_TYPE Edge_Edge( )
{
    SWIFT_Real d1, d2;

    // Handle the swap of the previously computed lambda values
    if( prev_state == CONTINUE_VE ) {
        lam_min2 = lam_min1; lam_max2 = lam_max1;
    }

    // Don't have to transform the first edge if it was already done in EV
    // Don't have to clip against the tail if it was already done in EV
    if( prev_state != CONTINUE_EV ) {
        // Transform e1 to e2's coordinates
        if( prev_state != CONTINUE_VF && prev_state != CONTINUE_EF ) {
            *h1xp = (*T12) * e1->Next()->Origin()->Coords();
            *u1xp = (*T12) & e1->Direction();
            if( prev_state != CONTINUE_VE ) {
                *t1xp = (*T12) * e1->Origin()->Coords();
            }
        }
        if( prev_state != CONTINUE_VE ) {
            dt12 = e2->Distance( *t1xp );
        }
        d1 = e2->Distance( *h1xp );
        lam_min1 = 0.0; lam_max1 = 1.0;
        // Test the vertices of edge1 against edge2

        // Test against the vertex-edge planes
        if( dt12 < 0.0 ) {
            if( d1 < 0.0 ) {
                // Edge is fully clipped out
                v2 = e2->Origin();
                v2->Set_Adj_Edge( e2 );
                lam_min1 = 0.0; lam_max1 = 1.0;
                prev_state = CONTINUE_EE;
                return CONTINUE_EV;
            }
            lam_min1 = dt12 / (dt12 - d1);
            // Check the derivative
            if( (*t1xp + (lam_min1 * e1->Length()) * (*u1xp) -
                e2->Origin()->Coords()) * (*u1xp) > 0.0
            ) {
                v2 = e2->Origin();
                v2->Set_Adj_Edge( e2 );
                lam_max1 = lam_min1; lam_min1 = 0.0;
                prev_state = CONTINUE_EE;
                return CONTINUE_EV;
            }
        } else if( d1 < 0.0 ) {
            lam_max1 = dt12 / (dt12 - d1);
            // Check the derivative
            if( (*t1xp + (lam_max1 * e1->Length()) * (*u1xp) -
                e2->Origin()->Coords()) * (*u1xp) < 0.0
            ) {
                v2 = e2->Origin();
                v2->Set_Adj_Edge( e2 );
                lam_min1 = lam_max1; lam_max1 = 1.0;
                prev_state = CONTINUE_EE;
                return CONTINUE_EV;
            }
        }
    } else {
        dt21 = dt12; dh21 = dh12;
    }

    if( prev_state != CONTINUE_VE ) {
        dh12 = e2->Twin()->Distance( *t1xp );
    }
    d2 = e2->Twin()->Distance( *h1xp );
    if( dh12 < 0.0 ) {
        if( d2 < 0.0 ) {
            // Edge is fully clipped out
            v2 = e2->Next()->Origin();
            v2->Set_Adj_Edge( e2->Twin() );
            lam_min1 = 0.0; lam_max1 = 1.0;
            prev_state = CONTINUE_EE;
            return CONTINUE_EV;
        }
        lam_min1 = dh12 / (dh12 - d2);
        // Check the derivative
        if( (*t1xp + (lam_min1 * e1->Length()) * (*u1xp) -
            e2->Next()->Origin()->Coords()) * (*u1xp) > 0.0
        ) {
            v2 = e2->Next()->Origin();
            v2->Set_Adj_Edge( e2->Twin() );
            lam_max1 = lam_min1; lam_min1 = 0.0;
            prev_state = CONTINUE_EE;
            return CONTINUE_EV;
        }
    } else if( d2 < 0.0 ) {
        lam_max1 = dh12 / (dh12 - d2);
        // Check the derivative
        if( (*t1xp + (lam_max1 * e1->Length()) * (*u1xp) -
            e2->Next()->Origin()->Coords()) * (*u1xp) < 0.0
        ) {
            v2 = e2->Next()->Origin();
            v2->Set_Adj_Edge( e2->Twin() );
            lam_min1 = lam_max1; lam_max1 = 1.0;
            prev_state = CONTINUE_EE;
            return CONTINUE_EV;
        }
    }

    // Test the vertices of edge2 against edge1

    // Don't have to transform the second edge if it was already done in VE
    // Don't have to clip against the tail if it was already done in VE
    // Have to us the d?21 variables here because the d?12 variables hold
    // information for if we go to VE.
    if( prev_state != CONTINUE_VE ) {
        // Transform e2 to e1's coordinates

        if( prev_state != CONTINUE_FV && prev_state != CONTINUE_FE ) {
            *h2xp = (*T21) * e2->Next()->Origin()->Coords();
            *u2xp = (*T21) & e2->Direction();
            if( prev_state != CONTINUE_EV ) {
                *t2xp = (*T21) * e2->Origin()->Coords();
            }
        }
        if( prev_state != CONTINUE_EV ) {
            dt21 = e1->Distance( *t2xp );
        }
        // Use the variable dr21 here since it is not needed
        dr21 = e1->Distance( *h2xp );
        lam_min2 = 0.0; lam_max2 = 1.0;
        // Test against the vertex-edge planes
        if( dt21 < 0.0 ) {
            if( dr21 < 0.0 ) {
                // Edge is fully clipped out
                v1 = e1->Origin();
                v1->Set_Adj_Edge( e1 );
                lam_min1 = 0.0; lam_max1 = 1.0;
                prev_state = CONTINUE_EE;
                return CONTINUE_VE;
            }
            lam_min2 = dt21 / (dt21 - dr21);
            // Check the derivative
            if( (*t2xp + (lam_min2 * e2->Length()) * (*u2xp) -
                e1->Origin()->Coords()) * (*u2xp) > 0.0
            ) {
                v1 = e1->Origin();
                v1->Set_Adj_Edge( e1 );
                lam_max1 = lam_min2; lam_min1 = 0.0;
                prev_state = CONTINUE_EE;
                return CONTINUE_VE;
            }
        } else if( dr21 < 0.0 ) {
            lam_max2 = dt21 / (dt21 - dr21);
            // Check the derivative
            if( (*t2xp + (lam_max2 * e2->Length()) * (*u2xp) -
                e1->Origin()->Coords()) * (*u2xp) < 0.0
            ) {
                v1 = e1->Origin();
                v1->Set_Adj_Edge( e1 );
                lam_min1 = lam_max2; lam_max1 = 1.0;
                prev_state = CONTINUE_EE;
                return CONTINUE_VE;
            }
        }
    }

    if( prev_state != CONTINUE_EV ) {
        dh21 = e1->Twin()->Distance( *t2xp );
    }
    dl21 = e1->Twin()->Distance( *h2xp );
    if( dh21 < 0.0 ) {
        if( dl21 < 0.0 ) {
            // Edge is fully clipped out

            // Swap the head and the tail
            SWIFT_Triple* temp_xp = t1xp; t1xp = h1xp; h1xp = temp_xp;

            if( prev_state == CONTINUE_EV ) {
                // Have to compute dt12
                dt12 = e2->Distance( *t1xp );
            } else {
                dt12 = d1;
            }
            dh12 = d2;

            v1 = e1->Next()->Origin();
            v1->Set_Adj_Edge( e1->Twin() );
            lam_min1 = 0.0; lam_max1 = 1.0;
            prev_state = CONTINUE_EE;
            return CONTINUE_VE;
        }
        lam_min2 = dh21 / (dh21 - dl21);
        // Check the derivative
        if( (*t2xp + (lam_min2 * e2->Length()) * (*u2xp) -
            e1->Next()->Origin()->Coords()) * (*u2xp) > 0.0
        ) {
            // Swap the head and the tail
            SWIFT_Triple* temp_xp = t1xp; t1xp = h1xp; h1xp = temp_xp;

            if( prev_state == CONTINUE_EV ) {
                // Have to compute dt12
                dt12 = e2->Distance( *t1xp );
            } else {
                dt12 = d1;
            }
            dh12 = d2;

            v1 = e1->Next()->Origin();
            v1->Set_Adj_Edge( e1->Twin() );
            lam_max1 = lam_min2; lam_min1 = 0.0;
            prev_state = CONTINUE_EE;
            return CONTINUE_VE;
        }
    } else if( dl21 < 0.0 ) {
        lam_max2 = dh21 / (dh21 - dl21);
        // Check the derivative
        if( (*t2xp + (lam_max2 * e2->Length()) * (*u2xp) -
            e1->Next()->Origin()->Coords()) * (*u2xp) < 0.0
        ) {
            // Swap the head and the tail
            SWIFT_Triple* temp_xp = t1xp; t1xp = h1xp; h1xp = temp_xp;

            if( prev_state == CONTINUE_EV ) {
                // Have to compute dt12
                dt12 = e2->Distance( *t1xp );
            } else {
                dt12 = d1;
            }
            dh12 = d2;

            v1 = e1->Next()->Origin();
            v1->Set_Adj_Edge( e1->Twin() );
            lam_min1 = lam_max2; lam_max1 = 1.0;
            prev_state = CONTINUE_EE;
            return CONTINUE_VE;
        }
    }

    SWIFT_Tri_Edge* et2 = NULL;
    SWIFT_Tri_Edge* eh2 = NULL;
    SWIFT_Tri_Edge* et1 = NULL;
    SWIFT_Tri_Edge* eh1 = NULL;
    SWIFT_Real lambda;
    bool fully_clipped1 = false;
    bool fully_clipped2 = false;
    bool done = false;

    // Test against the edge face planes

    if( prev_state == CONTINUE_EV || prev_state == CONTINUE_FV
                                  || prev_state == CONTINUE_FE
    ) {
        dl21 = dl12; dr21 = dr12;
    }

    if( prev_state != CONTINUE_VE && prev_state != CONTINUE_EF ) {
        dl12 = e2->Face_Distance( *t1xp );
    }
    if( prev_state == CONTINUE_VF || prev_state == CONTINUE_EF ) {
        dt12 = dr12;
    } else {
        dt12 = e2->Face_Distance( *h1xp );
    }
    if( dl12 >= 0.0 ) {
        if( dt12 >= 0.0 ) {
            // Edge is fully clipped out
            done = true;
            et2 = eh2 = e2;
            fully_clipped2 = true;
        } else if( (lambda = dl12 / (dl12 - dt12)) > lam_min1 ) {
            lam_min1 = lambda;
            et2 = e2;
            done = lam_max1 < lam_min1;
        }
    } else if( dt12 >= 0.0 && (lambda = dl12 / (dl12 - dt12)) < lam_max1 ) {
        lam_max1 = lambda;
        eh2 = e2;
        done = lam_max1 < lam_min1;
    }
    if( !done ) {
        if( prev_state != CONTINUE_VE ) {
            dr12 = e2->Twin()->Face_Distance( *t1xp );
        }
        dh12 = e2->Twin()->Face_Distance( *h1xp );
        if( dr12 >= 0.0 ) {
            if( dh12 >= 0.0 ) {
                // Edge is fully clipped out
                et2 = eh2 = e2->Twin();
                fully_clipped2 = true;
            } else if( (lambda = dr12 / (dr12 - dh12)) > lam_min1 ) {
                lam_min1 = lambda;
                et2 = e2->Twin();
            }
        } else if( dh12 >= 0.0 && (lambda = dr12 / (dr12 - dh12)) < lam_max1 ) {
            lam_max1 = lambda;
            eh2 = e2->Twin();
        }
    }

    // Test against the edge face planes
    if( prev_state != CONTINUE_EV &&  prev_state != CONTINUE_FE ) {
        dl21 = e1->Face_Distance( *t2xp );
    }
    if( prev_state == CONTINUE_FV || prev_state == CONTINUE_FE ) {
        dt21 = dr21;
    } else {
        dt21 = e1->Face_Distance( *h2xp );
    }
    done = false;
    if( dl21 >= 0.0 ) {
        if( dt21 >= 0.0 ) {
            // Edge is fully clipped out
            et1 = eh1 = e1;
            done = true;
            fully_clipped1 = true;
        } else if( (lambda = dl21 / (dl21 - dt21)) > lam_min2 ) {
            lam_min2 = lambda;
            et1 = e1;
            done = lam_max2 < lam_min2;
        }
    } else if( dt21 >= 0.0 && (lambda = dl21 / (dl21 - dt21)) < lam_max2 ) {
        lam_max2 = lambda;
        eh1 = e1;
        done = lam_max2 < lam_min2;
    }
    if( !done ) {
        if( prev_state != CONTINUE_EV ) {
            dr21 = e1->Twin()->Face_Distance( *t2xp );
        }
        dh21 = e1->Twin()->Face_Distance( *h2xp );
        if( dr21 >= 0.0 ) {
            if( dh21 >= 0.0 ) {
                // Edge is fully clipped out
                et1 = eh1 = e1->Twin();
                fully_clipped1 = true;
            } else if( (lambda = dr21 / (dr21 - dh21)) > lam_min2 ) {
                lam_min2 = lambda;
                et1 = e1->Twin();
            }
        } else if( dh21 >= 0.0 && (lambda = dr21 / (dr21 - dh21)) < lam_max2 ) {
            lam_max2 = lambda;
            eh1 = e1->Twin();
        }
    }

    // Check the derivatives crossing to faces if the edges were clipped by the
    // edge-face planes
    if( et2 != NULL ) {
        if( !fully_clipped2 ) {
            d1 = et2->Adj_Face()->Distance(
                                *t1xp + (lam_min1 * e1->Length()) * (*u1xp) );
        }
        d2 = et2->Adj_Face()->Normal() * (*u1xp);

        if( fully_clipped2 || d1 < 0.0 && d2 < 0.0 || d1 > 0.0 && d2 > 0.0 ) {
            if( !fully_clipped2 || (et2 == e2 && dt12 < dl12)
                                || (et2 != e2 && dh12 < dr12)
            ) {
                SWIFT_Triple* temp_xp = h1xp;
                h1xp = t1xp;
                t1xp = temp_xp;
                u1xp->Negate();
                e1 = e1->Twin();
                dt12 = (et2 == e2) ? dt12 : dh12;
                dr21 = -d2;
            } else {
                dt12 = (et2 == e2) ? dl12 : dr12;
                dr21 = d2;
            }
            e2 = et2;
            f2 = e2->Adj_Face();
            prev_state = CONTINUE_EF;
            RESULT_TYPE result = Edge_Face( e1, t1xp, h1xp, u1xp, e2, v1, f2
                                                                );
            if( result == CONTINUE_VF ) {
                // Swap the head and the tail
                SWIFT_Triple* temp_xp = t1xp;
                t1xp = h1xp;
                h1xp = temp_xp;
            }
            return result;
        }
    }
    if( eh2 != NULL ) {
        d1 = eh2->Adj_Face()->Distance(
                                *t1xp + (lam_max1 * e1->Length()) * (*u1xp) );
        d2 = eh2->Adj_Face()->Normal() * (*u1xp);

        if( d1 < 0.0 && d2 > 0.0 || d1 > 0.0 && d2 < 0.0 ) {
            dt12 = (eh2 == e2) ? dl12 : dr12;
            dr21 = d2;
            e2 = eh2;
            f2 = e2->Adj_Face();
            prev_state = CONTINUE_EF;
            RESULT_TYPE result = Edge_Face( e1, t1xp, h1xp, u1xp, e2, v1, f2
                                                                );
            if( result == CONTINUE_VF ) {
                // Swap the head and the tail
                SWIFT_Triple* temp_xp = t1xp;
                t1xp = h1xp;
                h1xp = temp_xp;
            }
            return result;
        }
    }
    if( et1 != NULL ) {
        if( !fully_clipped1 ) {
            d1 = et1->Adj_Face()->Distance(
                                *t2xp + (lam_min2 * e2->Length()) * (*u2xp) );
        }
        d2 = et1->Adj_Face()->Normal() * (*u2xp);

        if( fully_clipped1 || d1 < 0.0 && d2 < 0.0 || d1 > 0.0 && d2 > 0.0 ) {
            if( !fully_clipped1 || (et1 == e1 && dt21 < dl21)
                                || (et1 != e1 && dh21 < dr21)
            ) {
                SWIFT_Triple* temp_xp = h2xp;
                h2xp = t2xp;
                t2xp = temp_xp;
                u2xp->Negate();
                e2 = e2->Twin();
                dt12 = (et1 == e1) ? dt21 : dh21;
                dr21 = -d2;
            } else {
                dt12 = (et1 == e1) ? dl21 : dr21;
                dr21 = d2;
            }
            e1 = et1;
            f1 = e1->Adj_Face();
            prev_state = CONTINUE_FE;
            RESULT_TYPE result = Edge_Face( e2, t2xp, h2xp, u2xp, e1, v2, f1
                                                                );
            if( result == CONTINUE_VF ) {
                // Swap the head and the tail
                SWIFT_Triple* temp_xp = t2xp;
                t2xp = h2xp;
                h2xp = temp_xp;
                result = CONTINUE_FV;
            }
            return result;
        }
    }
    if( eh1 != NULL ) {
        d1 = eh1->Adj_Face()->Distance(
                                *t2xp + (lam_max2 * e2->Length()) * (*u2xp) );
        d2 = eh1->Adj_Face()->Normal() * (*u2xp);

        if( d1 < 0.0 && d2 > 0.0 || d1 > 0.0 && d2 < 0.0 ) {
            dt12 = (eh1 == e1) ? dl21 : dr21;
            dr21 = d2;
            e1 = eh1;
            f1 = e1->Adj_Face();
            prev_state = CONTINUE_FE;
            RESULT_TYPE result = Edge_Face( e2, t2xp, h2xp, u2xp, e1, v2, f1
                                                                );
            if( result == CONTINUE_VF ) {
                // Swap the head and the tail
                SWIFT_Triple* temp_xp = t2xp;
                t2xp = h2xp;
                h2xp = temp_xp;
                result = CONTINUE_FV;
            }
            return result;
        }
    }
    prev_state = CONTINUE_EE;

    return DISJOINT;
}


//////////////////////////////////////////////////////////////////////////////
// Edge_Face:
//
// Figure out which state to transition to when given an edge (edge1) and a
// face (edge2->face).
// This is called from the edge-edge case where the pair of nearest points
// are on the interiors of the two edges.  It can transition to v-f, e-e, or
// penetration cases.  In the e-e case, edge2 is set to the edge that whose e-f
// plane is pierced so the new pair is edge1 and edge2.
//
// We just have to decide to step to the vertex-face case if the head of
// the edge is closest to the face or to the edge-edge case if the head of
// the edge is not in the face's V-region.  Or decide that penetration exists.
//////////////////////////////////////////////////////////////////////////////
RESULT_TYPE Edge_Face( SWIFT_Tri_Edge* edge1, SWIFT_Triple* tx,
                       SWIFT_Triple* hx, SWIFT_Triple* ux,
                       SWIFT_Tri_Edge*& edge2,
                       SWIFT_Tri_Vertex*& vert1, SWIFT_Tri_Face*& face2
                       )
{
    // From knowing which edge of the face we are entering from, we set e4 to
    // be the entering edge and e3 and edge2 to be the other edges in their
    // order around the face.  We also compute the distance of the tail of the
    // input edge from the entering edge-face plane (dt12) to later determine if
    // the tail is on the face side or not.  We compute the distance of the
    // input edge's head from the two (non-entering) edges of the face
    // (dh1/dh2).

    SWIFT_Tri_Edge* e3 = edge2->Next();
    SWIFT_Tri_Edge* e4 = edge2->Prev();
    SWIFT_Real dh1 = e3->Face_Distance( *hx );
    SWIFT_Real dh2 = e4->Face_Distance( *hx );
    SWIFT_Real dhf = dist = face2->Distance( *hx );
    SWIFT_Real dt1 = e3->Face_Distance( *tx );
    SWIFT_Real dt2 = e4->Face_Distance( *tx );
    SWIFT_Real un1, un2, unf;
    bool clipped_out = false;

    if( dr21 > 0.0 ) {
        dhf = -dhf;
        unf = -dr21;
    } else {
        unf = dr21;
    }

    // Check results of clipping the head against the other two edges
    // If neither edge clipped the head, then proceed to VF.
    // Set e3 to be the edge that clipped the head first.
    // Set dh1 to be distance of the head from the first clipping plane.
    // Set un1 to the dot product of the first edge face plane normal and
    // the direction vector of the entering edge.
    if( dh1 < 0.0 ) {
        // Head was clipped by the first edge
        if( dh2 < 0.0 ) {
            if( dt1 > 0.0 && dt2 > 0.0 ) {
                // Head was also clipped by either the second or
                // third edge
                un1 = *ux * e3->Face_Normal();
                un2 = *ux * e4->Face_Normal();
                if( dh1 * un2 < dh2 * un1 ) {
                    // The second edge clipped it first
                    SWIFT_Tri_Edge* tempe = e3; e3 = e4; e4 = tempe;
                    // Use un1 as a temp
                    un1 = dt1; dt1 = dt2; dt2 = un1;
                    un1 = dh1; dh1 = dh2; dh2 = un1;
                    un1 = un2;
                //} else {
                    // Head was not clipped by the second edge
                    // Everything is set correctly
                }
            } else if( dt2 > 0.0 ) {
                // The first edge fully clipped the clip edge
                edge2 = e3;
                dl12 = dt1; dr12 = dh1;
                return CONTINUE_EE;
            } else if( dt1 > 0.0 ) {
                // The second edge fully clipped the clip edge
                edge2 = e4;
                dl12 = dt2; dr12 = dh2;
                return CONTINUE_EE;
            } else {
                // The head and the tail were both clipped out by both edges
                clipped_out = true;
            }
        } else {
            // The head was only clipped out by the first edge
            if( dt1 > 0.0 && dt2 > 0.0 ) {
                un1 = *ux * e3->Face_Normal();
            } else if( dt2 > 0.0 ) {
                // The tail was clipped out by only the first edge
                // Walk to the first edge
                edge2 = e3;
                dl12 = dt1; dr12 = dh1;
                return CONTINUE_EE;
            } else if( dt1 > 0.0 ) {
                // The tail was clipped out by only the second edge
                un1 = *ux * e3->Face_Normal();
                un2 = *ux * e4->Face_Normal();

                // Figure out if the whole edge was clipped out
                if( dt2 * un1 < dt1 * un2 ) {
                    // The entire edge was not clipped out

                    // Check for penetration
                    if( dh2 * unf < dhf * un2 ) {
                        if( dh1 * unf < dhf * un1 ) {
                            return PENETRATION;
                        } else {
                            edge2 = e3;
                            dl12 = dt1; dr12 = dh1;
                            return CONTINUE_EE;
                        }
                    } else {
                        edge2 = e4;
                        dl12 = dt2; dr12 = dh2;
                        return CONTINUE_EE;
                    }
                } else {
                    // The entire edge was clipped out
                    clipped_out = true;
                }
            } else {
                // The tail was clipped out by both edges.  Go to the 1st edge.
                edge2 = e3;
                dl12 = dt1; dr12 = dh1;
                return CONTINUE_EE;
            }
        }
    } else {
        // Head was not clipped by the first edge
        if( dh2 < 0.0 ) {
            // Head was clipped by the second edge
            if( dt1 > 0.0 && dt2 > 0.0 ) {
                SWIFT_Tri_Edge* tempe = e3; e3 = e4; e4 = tempe;
                // Use un1 as a temp
                un1 = dt1; dt1 = dt2; dt2 = un1;
                un1 = dh1; dh1 = dh2; dh2 = un1;
                un1 = *ux * e3->Face_Normal();
            } else if( dt2 > 0.0 ) {
                // The tail was clipped out by only the first edge
                un1 = *ux * e3->Face_Normal();
                un2 = *ux * e4->Face_Normal();

                // Figure out if the whole edge was clipped out
                if( dt1 * un2 < dt2 * un1 ) {
                    // The entire edge was not clipped out.

                    // Check for penetration
                    if( dh1 * unf < dhf * un1 ) {
                        if( dh2 * unf < dhf * un2 ) {
                            return PENETRATION;
                        } else {
                            edge2 = e4;
                            dl12 = dt2; dr12 = dh2;
                            return CONTINUE_EE;
                        }
                    } else {
                        edge2 = e3;
                        dl12 = dt1; dr12 = dh1;
                        return CONTINUE_EE;
                    }
                } else {
                    // The entire edge was clipped out
                    clipped_out = true;
                }
            } else if( dt1 > 0.0 ) {
                // The tail was clipped out by only the second edge
                // Walk to the second edge
                edge2 = e4;
                dl12 = dt2; dr12 = dh2;
                return CONTINUE_EE;
            } else {
                // The tail was clipped out by both edges.  Go to the 2nd edge.
                edge2 = e4;
                dl12 = dt2; dr12 = dh2;
                return CONTINUE_EE;
            }
        } else {
            // Head was not clipped at all.  It is not possible for the tail
            // to be clipped out by both edges.

            if( dt1 < 0.0 ) {
                // Tail clipped out by first edge only
                if( dhf > 0.0 ) {
                    // Nearest point to the face is the head of the edge
                    vert1 = edge1->Next()->Origin();
                    vert1->Set_Adj_Edge( edge1->Twin() );
                    return CONTINUE_VF;
                } else {
                    // Head is under the face
                    un1 = *ux * e3->Face_Normal();

                    // Check for penetration or go to the first edge
                    if( dh1 * unf < dhf * un1 ) {
                        return PENETRATION;
                    }

                    // Walk to the first edge
                    edge2 = e3;
                    dl12 = dt1; dr12 = dh1;
                    return CONTINUE_EE;
                }
            } else if( dt2 < 0.0 ) {
                // Tail clipped out by second edge only
                if( dhf > 0.0 ) {
                    // Nearest point to the face is the head of the edge
                    vert1 = edge1->Next()->Origin();
                    vert1->Set_Adj_Edge( edge1->Twin() );
                    return CONTINUE_VF;
                } else {
                    // Head is under the face
                    un2 = *ux * e4->Face_Normal();

                    // Check for penetration or go to the second edge
                    if( dh2 * unf < dhf * un2 ) {
                        return PENETRATION;
                    }

                    // Walk to the second edge
                    edge2 = e4;
                    dl12 = dt2; dr12 = dh2;
                    return CONTINUE_EE;
                }
#ifdef SWIFT_DEBUG
            } else if( dt1 < 0.0 && dt2 < 0.0 ) {
cerr << "********* Error: Head not clipped but tail clipped by both" << endl;
#endif
            } else {
                // Check for penetration
                dt1 = dr21 > 0.0 ? -face2->Distance(*tx) : face2->Distance(*tx);
                if( dhf < 0.0 && dt1 > 0.0 ) {
                    return PENETRATION;
                }

                // Walk to head.
                vert1 = edge1->Next()->Origin();
                vert1->Set_Adj_Edge( edge1->Twin() );
                return CONTINUE_VF;
            }
        }
    }

    if( clipped_out ) {
        un1 = e4->Distance( *tx );
        un2 = e4->Distance( *hx );

        if( dt2 < 0.0 && un1 > 0.0 ) {
            if( un2 < 0.0 ) {
                if( e3->Twin()->Distance( *hx ) > 0.0 ) {
                    // Decide which edge to go to.
                    edge2 = ((*tx + (un1 / (un1-un2)) * (*ux) -
                            edge2->Origin()->Coords()) * (*ux) < 0.0) ? e3 : e4;
                } else {
                    edge2 = e4;
                }
            } else {
                edge2 = e4;
            }
        } else {
            if( dh2 < 0.0 && un2 > 0.0 ) {
                if( e3->Twin()->Distance( *tx ) > 0.0 ) {
                    // Decide which edge to go to.
                    edge2 = ((*tx + (un1 / (un1-un2)) * (*ux) -
                            edge2->Origin()->Coords()) * (*ux) > 0.0) ? e3 : e4;
                } else {
                    edge2 = e4;
                }
            } else {
                // Go to e3 or to the vertex but since the vertex is a
                // subset of e3 then we simply go to e3.
                edge2 = e3;
            }
        }

        if( edge2 == e3 ) {
            dl12 = dt1; dr12 = dh1;
        } else {
            dl12 = dt2; dr12 = dh2;
        }
        return CONTINUE_EE;
    }

    // Check for penetration.  For this second test, dhf < 0.0, dh1 < 0.0,
    // unf < 0.0, un1 > 0.0.  Check that the tail is not also under the face.
    un2 = face2->Distance( *tx );
    if( dhf < 0.0 && (dr21 > 0.0 && un2 < 0.0 || dr21 < 0.0 && un2 > 0.0) &&
        dh1 * unf < dhf * un1
    ) {
        // The face is pierced by the edge.  edge2 is set the original
        // edge across which we entered which is correct.
        //dist = dhf;
        return PENETRATION;
    }

    // Walk to the edge whose edge-face plane clipped the head first
    edge2 = e3;
    dl12 = dt1; dr12 = dh1;

    return CONTINUE_EE;
}


///////////////////////////////////////////////////////////////////////////////
// Intersection and distance functions for a pair of convex polyhedra
///////////////////////////////////////////////////////////////////////////////

// Determines nearest features and intersection between two convex polyhedra.
// Walk to the nearest features.  Uses an improved LC closest features alg.
// If there is intersection returns true otherwise returns false.
bool Walk_Convex_LC( RESULT_TYPE start_state )
{
    // For looping and swapping
    int i;
#ifdef SWIFT_DEBUG
    int r = 0;
#endif


    prev_state = DISJOINT;

    do {
        switch( state = start_state ) {
        case CONTINUE_VV:
            start_state = Vertex_Vertex();
            prev_state = CONTINUE_VV;
            break;
        case CONTINUE_EV:
            T21 = &trans12;
            T12 = &trans21;
            { SWIFT_Triple* temp_xp = t1xp; t1xp = t2xp; t2xp = temp_xp;
              temp_xp = h1xp; h1xp = h2xp; h2xp = temp_xp;
              temp_xp = u1xp; u1xp = u2xp; u2xp = temp_xp; }
            e2 = e1;
            v1 = v2;
        case CONTINUE_VE:
            start_state = Vertex_Edge();
            prev_state = state;

            if( state == CONTINUE_EV ) {
                // Unswap the results
                SWIFT_Triple* temp_xp = t1xp; t1xp = t2xp; t2xp = temp_xp;
                temp_xp = h1xp; h1xp = h2xp; h2xp = temp_xp;
                temp_xp = u1xp; u1xp = u2xp; u2xp = temp_xp;
                switch( start_state ) {
                case CONTINUE_VV:
                    { SWIFT_Tri_Vertex* tempv = v1; v1 = v2; v2 = tempv;
                    }
                    break;
                case CONTINUE_EE:
                    { SWIFT_Tri_Edge* tempe = e1; e1 = e2; e2 = tempe;
                    }
                    break;
                case CONTINUE_VF:
                    start_state = CONTINUE_FV;
                    f1 = f2;
                    v2 = v1;
                    break;
                default: // DISJOINT
                    // Swap the edge back (vertex is ok) since Vertex_Edge
                    // may have set e1
                    e1 = e2;
                    break;
                }
                T12 = &trans12;
                T21 = &trans21;
            }
            break;
        case CONTINUE_FV:
            T21 = &trans12;
            T12 = &trans21;
            { SWIFT_Triple* temp_xp = t1xp; t1xp = t2xp; t2xp = temp_xp;
              temp_xp = h1xp; h1xp = h2xp; h2xp = temp_xp;
              temp_xp = u1xp; u1xp = u2xp; u2xp = temp_xp; }
            f2 = f1;
            v1 = v2;
        case CONTINUE_VF:
            start_state = Vertex_Face();
            prev_state = state;

            if( state == CONTINUE_FV ) {
                // Unswap the results
                SWIFT_Triple* temp_xp = t1xp; t1xp = t2xp; t2xp = temp_xp;
                temp_xp = h1xp; h1xp = h2xp; h2xp = temp_xp;
                temp_xp = u1xp; u1xp = u2xp; u2xp = temp_xp;
                switch( start_state ) {
                case CONTINUE_VV:
                    { SWIFT_Tri_Vertex* tempv = v1; v1 = v2; v2 = tempv;
                    }
                    break;
                case CONTINUE_VE:
                    start_state = CONTINUE_EV;
                    e1 = e2;
                    v2 = v1;
                    break;
                case CONTINUE_EE:
                    { SWIFT_Tri_Edge* tempe = e1; e1 = e2; e2 = tempe;
                    }
                    break;
                case CONTINUE_VF:
                    start_state = CONTINUE_FV;
                    f1 = f2;
                    v2 = v1;
                    break;
                case PENETRATION:
                    f1 = f2;
                    v2 = v1;
#ifdef SWIFT_HIERARCHY
                    start_state = Refine( state );
                    if( start_state == DISJOINT ) {
                        // Could not walk to a finer level
                        start_state = PENETRATION;
                    }
#else
#endif
                    break;
                case LOCAL_MINIMUM:
                  { // Handle the local minimum
                    v2 = v1;
#ifdef SWIFT_HIERARCHY
                    SWIFT_Tri_Mesh* m = obj1->Mesh( piece1, level1 );
#else
                    SWIFT_Tri_Mesh* m = obj1->Mesh( piece1 );
#endif
                    SWIFT_Real dist2;
                    dist = m->Faces()[0].Distance( *t2xp );
                    f1 = m->Faces()(0);
                    for( i = 1; i < m->Num_Faces(); i++ ) {
                        dist2 = m->Faces()[i].Distance( *t2xp );
                        if( dist2 > dist ) {
                            dist = dist2;
                            f1 = m->Faces()(i);
                        }
                    }
                    // Check for penetration
                    if( dist < 0.0 ) {
#ifdef SWIFT_HIERARCHY
                        // Try to walk to a finer level
                        start_state = Refine( state );
                        if( start_state == DISJOINT ) {
                            // Could not walk to a finer level
                            start_state = PENETRATION;
                        }
#else
                        start_state = PENETRATION;
#endif
                    } else {
                        prev_state = LOCAL_MINIMUM;
                        start_state = CONTINUE_FV;
                    }
                  } break;
                default: // DISJOINT
                    break;
                }
                T12 = &trans12;
                T21 = &trans21;
            } else if( start_state == PENETRATION ) {
#ifdef SWIFT_HIERARCHY
                start_state = Refine( state );
                if( start_state == DISJOINT ) {
                    // Could not walk to a finer level
                    start_state = PENETRATION;
                }
#else
#endif
            } else if( start_state == LOCAL_MINIMUM ) {
                // Handle the local minimum
#ifdef SWIFT_HIERARCHY
                SWIFT_Tri_Mesh* m = obj2->Mesh( piece2, level2 );
#else
                SWIFT_Tri_Mesh* m = obj2->Mesh( piece2 );
#endif
                SWIFT_Real dist2;
                dist = m->Faces()[0].Distance( *t1xp );
                f2 = m->Faces()(0);
                for( i = 1; i < m->Num_Faces(); i++ ) {
                    dist2 = m->Faces()[i].Distance( *t1xp );
                    if( dist2 > dist ) {
                        dist = dist2;
                        f2 = m->Faces()(i);
                    }
                }
                // Check for penetration
                if( dist < 0.0 ) {
#ifdef SWIFT_HIERARCHY
                    // Try to walk to a finer level
                    start_state = Refine( state );
                    if( start_state == DISJOINT ) {
                        // Could not walk to a finer level
                        start_state = PENETRATION;
                    }
#else
                    start_state = PENETRATION;
#endif
                } else {
                    prev_state = LOCAL_MINIMUM;
                    start_state = CONTINUE_VF;
                }
            }
            break;
        case CONTINUE_EE:
            // It is the responsibility of Edge_Edge() to set prev_state.
            start_state = Edge_Edge();

#ifdef SWIFT_HIERARCHY
            if( start_state == PENETRATION ) {
                start_state = Refine( CONTINUE_EE );
                if( start_state == DISJOINT ) {
                    // Could not walk to a finer level
                    start_state = PENETRATION;
                }
            }
#endif
            break;
        default:
            break;
        }

#ifdef SWIFT_DEBUG
        if( r++ == STATE_TRANS_CYCLE_DECL ) {
            cerr << "SWIFT infinite loop detected: Exiting..." << endl;
            exit( -1 );
        }
#endif
    } while( start_state != DISJOINT && start_state != PENETRATION );

    return (start_state == PENETRATION);
}

inline void Distance_After_Walk_Convex_LC( SWIFT_Real& distance )
{
    // Compute the distance of the objects based on the set globals from walk
    if( state == CONTINUE_FV || state == CONTINUE_VF ) {
        // The distance is stored in the dist variable
        distance = dist;
    } else if( state == CONTINUE_EE ) {
        // The fdir vector is not set at all
        SWIFT_Triple tri1, tri2;
        Compute_Closest_Points_Edge_Edge( e1, e2, tri1, tri2, trans12,
                                          &distance );
        //distance = fdir.Length();
    } else {
        // The fdir vector spans the nearest points
        distance = fdir.Length();
    }
}

inline bool Distance_Convex_LC( RESULT_TYPE start_state, SWIFT_Real& distance )
{
    if( Walk_Convex_LC( start_state ) ) {
        // There is penetration
        distance = -1.0;
        return true;
    }

    Distance_After_Walk_Convex_LC( distance );

    return false;

}


///////////////////////////////////////////////////////////////////////////////
// Global Query functions
///////////////////////////////////////////////////////////////////////////////

inline void Set_Contact_List_Single( void* feat1, void* feat2,
     RESULT_TYPE feature_types, SWIFT_Real distance, SWIFT_Real error = 0.0 )
{
    contact_list1[0] = feat1;
    contact_list2[0] = feat2;
    contact_listt[0] = feature_types;
    contact_listd[0] = distance;
    contact_liste[0] = error;
}



// returns intersection: true or false
inline
bool Tolerance_LC( RESULT_TYPE start_state, SWIFT_Real tolerance )
{
#ifdef SWIFT_HIERARCHY
    // This will refine until there is no penetration or until the finest
    // level is reached.
#endif
    return Walk_Convex_LC( start_state );
}


// returns intersection: true or false
inline
bool Distance_LC( RESULT_TYPE start_state,
#ifdef SWIFT_HIERARCHY
                  SWIFT_Real distance_tolerance, SWIFT_Real error_tolerance,
                  SWIFT_Real& grow_dist, SWIFT_Real& distance
#else
                  SWIFT_Real& distance
#endif
                )
{
#ifdef SWIFT_HIERARCHY
    // This will refine until there is no penetration or until the finest
    // level is reached
    bool result = Distance_Convex_LC( start_state, grow_dist );
    distance = grow_dist;

    // Save the features to the save variables
    save_v1 = v1;
    save_v2 = v2;
    save_e1 = e1;
    save_e2 = e2;
    save_f1 = f1;
    save_f2 = f2;
    save_state = state;
    save_level1 = level1;
    save_level2 = level2;

    if( result ) {
        return true;
    }

    // Compare distance against tolerance for early exit
    while( distance <= distance_tolerance &&
           obj1->Mesh( piece1, level1 )->Deviation() +
           obj2->Mesh( piece2, level2 )->Deviation() > error_tolerance
    ) {
        // Proceed to go down the hierarchy until the distance tolerance is
        // exceed, or the error tolerance is met.
        start_state = Refine( state );

        // Compute the distance at the new levels.
        Distance_Convex_LC( start_state, distance );
    }

    return false;
#else
    return Distance_Convex_LC( start_state, distance );
#endif
}


///////////////////////////////////////////////////////////////////////////////
// SWIFT_Pair public functions
///////////////////////////////////////////////////////////////////////////////

//bool SWIFT_Pair::Intersection( SWIFT_Object* o1, SWIFT_Object* o2,
bool SWIFT_Pair::Tolerance( SWIFT_Object* o1, SWIFT_Object* o2,
                            int p1, int p2, SWIFT_Real tolerance )
{
    RESULT_TYPE start_state;
    bool result;
#ifdef SWIFT_HIERARCHY
    SWIFT_Real grow_dist;
#endif

    // Setup the pair specific query stuff
    Setup_Pair_Query( o1, o2, p1, p2
                      );

    start_state = State();
    Setup_Piece_Query( start_state, feat1, feat2 );

// ------------------------- Start tolerance call -------------------------
#ifdef SWIFT_HIERARCHY
    result = Distance_LC( start_state, 0.0, false, grow_dist, grow_dist );
    Grow( grow_dist );
#else
    result = Tolerance_LC( start_state, tolerance );
#endif
// ------------------------- End tolerance call -------------------------


#ifndef SWIFT_HIERARCHY
    // Save the features into the save variables
    save_v1 = v1;
    save_v2 = v2;
    save_e1 = e1;
    save_e2 = e2;
    save_f1 = f1;
    save_f2 = f2;
    save_state = state;
#endif

    Save_State();

    return result;
}

bool SWIFT_Pair::Approximate_Distance( SWIFT_Object* o1, SWIFT_Object* o2,
                                int p1, int p2, SWIFT_Real distance_tolerance,
                                SWIFT_Real error_tolerance,
                                SWIFT_Real& distance, SWIFT_Real& error )

{
    RESULT_TYPE start_state = State();
    bool result;
#ifdef SWIFT_HIERARCHY
    SWIFT_Real grow_dist;
#endif

    // Setup the pair specific query stuff
    Setup_Pair_Query( o1, o2, p1, p2
                      );


// ------------------------- Start approx dist call -------------------------
#ifdef SWIFT_HIERARCHY
    result = Distance_LC( start_state, distance_tolerance, error_tolerance,
                          grow_dist, distance );
    Grow( grow_dist );
#else
    result = Distance_LC( start_state, distance );
    error = 0.0;
#endif
// ------------------------- End approx dist call -------------------------


#ifndef SWIFT_HIERARCHY
    // Save the features to the save variables.
    save_v1 = v1;
    save_v2 = v2;
    save_e1 = e1;
    save_e2 = e2;
    save_f1 = f1;
    save_f2 = f2;
    save_state = state;
#endif

    Save_State();

    if( distance <= distance_tolerance ) {
#ifdef SWIFT_HIERARCHY
        error = obj1->Mesh( piece1, level1 )->Deviation() +
                obj2->Mesh( piece2, level2 )->Deviation();
#else
        error = 0.0;
#endif
    }

    return result;
}



bool SWIFT_Pair::Distance( SWIFT_Object* o1, SWIFT_Object* o2,
                           int p1, int p2, SWIFT_Real tolerance,
                           SWIFT_Real& distance )
{
    RESULT_TYPE start_state;
    bool result;
#ifdef SWIFT_HIERARCHY
    SWIFT_Real grow_dist;
#endif

    // Setup the pair specific query stuff
    Setup_Pair_Query( o1, o2, p1, p2
                      );

    start_state = State();
    Setup_Piece_Query( start_state, feat1, feat2 );

// ------------------------- Start dist call -------------------------
#ifdef SWIFT_HIERARCHY
    result = Distance_LC( start_state, tolerance, 0.0, grow_dist, distance );
    Grow( grow_dist );
#else
    result = Distance_LC( start_state, distance );
#endif
// ------------------------- End dist call -------------------------


#ifndef SWIFT_HIERARCHY
    // Save the features to the save variables.
    save_v1 = v1;
    save_v2 = v2;
    save_e1 = e1;
    save_e2 = e2;
    save_f1 = f1;
    save_f2 = f2;
    save_state = state;
#endif

    Save_State();


    return result;
}

bool SWIFT_Pair::Contacts( SWIFT_Object* o1, SWIFT_Object* o2, int p1, int p2,
                           SWIFT_Real tolerance, SWIFT_Real& distance,
                           int& num_contacts )
{
    RESULT_TYPE start_state = State();
    bool result;
#ifdef SWIFT_HIERARCHY
    SWIFT_Real grow_dist;
#endif

    // Setup the pair specific query stuff
    Setup_Pair_Query( o1, o2, p1, p2
                      );

    start_state = State();
    Setup_Piece_Query( start_state, feat1, feat2 );

// ------------------------- Start contacts call -------------------------
#ifdef SWIFT_HIERARCHY
    result = Distance_LC( start_state, tolerance, 0.0, grow_dist, distance );
    Grow( grow_dist );
#else
    result = Distance_LC( start_state, distance );
#endif
// ------------------------- End contacts call -------------------------


#ifndef SWIFT_HIERARCHY
    // Save the features to the save variables.
    save_v1 = v1;
    save_v2 = v2;
    save_e1 = e1;
    save_e2 = e2;
    save_f1 = f1;
    save_f2 = f2;
    save_state = state;
#endif

    Save_State();

    num_contacts = (distance <= tolerance ? 1 : 0);
    if( num_contacts == 1 ) {
        Set_Contact_List_Single( feat1, feat2, state, distance );
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////
// List Fillers
///////////////////////////////////////////////////////////////////////////////

void SWIFT_Pair::Distances( SWIFT_Array<SWIFT_Real>& dists )
{
    int i;
    for( i = 0; i < contact_listd.Length(); i++ ) {
        dists.Add( contact_listd[i] );
    }
}

void SWIFT_Pair::Errors( SWIFT_Array<SWIFT_Real>& errs )
{
    int i;
    for( i = 0; i < contact_liste.Length(); i++ ) {
        errs.Add( contact_liste[i] );
    }
}

void SWIFT_Pair::Contact_Features( SWIFT_Array<int>& pids,
                                   SWIFT_Array<int>& ftypes,
                                   SWIFT_Array<int>& fids )
{
    int i;

    for( i = 0; i < contact_listt.Length(); i++ ) {
        pids.Add( piece1 );
        pids.Add( piece2 );
        switch( contact_listt[i] ) {
        case CONTINUE_VV:
            ftypes.Add( VERTEX );
            ftypes.Add( VERTEX );
            fids.Add( obj1->Mesh( piece1 )->Orig_Vertex_Id( v1 ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Vertex_Id( v2 ) );
            break;
        case CONTINUE_VE:
            ftypes.Add( VERTEX );
            ftypes.Add( EDGE );
            fids.Add( obj1->Mesh( piece1 )->Orig_Vertex_Id( v1 ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Vertex_Id( e2->Origin() ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Vertex_Id(
                                                    e2->Next()->Origin() ) );
            break;
        case CONTINUE_EV:
            ftypes.Add( EDGE );
            ftypes.Add( VERTEX );
            fids.Add( obj1->Mesh( piece1 )->Orig_Vertex_Id( e1->Origin() ) );
            fids.Add( obj1->Mesh( piece1 )->Orig_Vertex_Id(
                                                    e1->Next()->Origin() ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Vertex_Id( v2 ) );
            break;
        case CONTINUE_VF:
            ftypes.Add( VERTEX );
            ftypes.Add( FACE );
            fids.Add( obj1->Mesh( piece1 )->Orig_Vertex_Id( v1 ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Face_Id( f2 ) );
            break;
        case CONTINUE_FV:
            ftypes.Add( FACE );
            ftypes.Add( VERTEX );
            fids.Add( obj1->Mesh( piece1 )->Orig_Face_Id( f1 ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Vertex_Id( v2 ) );
            break;
        case CONTINUE_EE:
            ftypes.Add( EDGE );
            ftypes.Add( EDGE );
            fids.Add( obj1->Mesh( piece1 )->Orig_Vertex_Id( e1->Origin() ) );
            fids.Add( obj1->Mesh( piece1 )->Orig_Vertex_Id(
                                                    e1->Next()->Origin() ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Vertex_Id( e2->Origin() ) );
            fids.Add( obj2->Mesh( piece2 )->Orig_Vertex_Id(
                                                    e2->Next()->Origin() ) );
            break;
        default:
            break;
        }
    }
}

void SWIFT_Pair::Contact_Points( SWIFT_Array<SWIFT_Real>& points )
{
    int i;
    SWIFT_Triple tri;
    SWIFT_Real* points_ptr = points.Data() + points.Length();

    for( i = 0; i < contact_listt.Length(); i++, points_ptr += 6 ) {
        switch( contact_listt[i] ) {
        case CONTINUE_VV:
            ((SWIFT_Tri_Vertex*)contact_list1[i])->Coords().
                                                    Get_Value( points_ptr );
            ((SWIFT_Tri_Vertex*)contact_list2[i])->Coords().
                                                    Get_Value( points_ptr+3 );
            break;
        case CONTINUE_VE:
          { const SWIFT_Tri_Edge* e = ((SWIFT_Tri_Edge*)contact_list2[i]);
            const SWIFT_Tri_Vertex* v = ((SWIFT_Tri_Vertex*)contact_list1[i]);
            v->Coords().Get_Value( points_ptr );
            tri = e->Origin()->Coords() + ((trans12 * v->Coords() -
                  e->Origin()->Coords()) * e->Direction()) * e->Direction();
            tri.Get_Value( points_ptr+3 );
          } break;
        case CONTINUE_EV:
          { const SWIFT_Tri_Edge* e = ((SWIFT_Tri_Edge*)contact_list1[i]);
            const SWIFT_Tri_Vertex* v = ((SWIFT_Tri_Vertex*)contact_list2[i]);
            tri = e->Origin()->Coords() + ((trans21 * v->Coords() -
                  e->Origin()->Coords()) * e->Direction()) * e->Direction();
            tri.Get_Value( points_ptr );
            v->Coords().Get_Value( points_ptr+3 );
          } break;
        case CONTINUE_VF:
          { const SWIFT_Tri_Face* f = ((SWIFT_Tri_Face*)contact_list2[i]);
            const SWIFT_Tri_Vertex* v = ((SWIFT_Tri_Vertex*)contact_list1[i]);
            v->Coords().Get_Value( points_ptr );
            tri = trans12 * v->Coords();
            tri -= ((tri - f->Coords1()) * f->Normal()) * f->Normal();
            tri.Get_Value( points_ptr+3 );
          } break;
        case CONTINUE_FV:
          { const SWIFT_Tri_Face* f = ((SWIFT_Tri_Face*)contact_list1[i]);
            const SWIFT_Tri_Vertex* v = ((SWIFT_Tri_Vertex*)contact_list2[i]);
            tri = trans21 * v->Coords();
            tri -= ((tri - f->Coords1()) * f->Normal()) * f->Normal();
            tri.Get_Value( points_ptr );
            v->Coords().Get_Value( points_ptr+3 );
          } break;
        case CONTINUE_EE:
          { SWIFT_Tri_Edge* e = ((SWIFT_Tri_Edge*)contact_list1[i]);
            SWIFT_Tri_Edge* ee = ((SWIFT_Tri_Edge*)contact_list2[i]);
            SWIFT_Triple tri2;
            Compute_Closest_Points_Edge_Edge( e, ee, tri, tri2, trans12 );
            tri.Get_Value( points_ptr );
            tri2.Get_Value( points_ptr+3 );
          } break;
        default:
            break;
        }
    }

    // Fix up the length of the points array
    points.Set_Length( points.Length() + 6*contact_listt.Length() );
}

void SWIFT_Pair::Contact_Normals( SWIFT_Array<SWIFT_Real>& normals )
{
    int i;
    SWIFT_Triple tri;
    SWIFT_Real* normals_ptr = normals.Data() + normals.Length();

    for( i = 0; i < contact_listt.Length(); i++, normals_ptr += 3 ) {
        switch( contact_listt[i] ) {
        case CONTINUE_VV:
            tri = (obj2->Transformation() &
                   ((SWIFT_Tri_Vertex*)contact_list2[i])->Gathered_Normal()) -
                  (obj1->Transformation() &
                   ((SWIFT_Tri_Vertex*)contact_list1[i])->Gathered_Normal());
#ifdef SWIFT_USE_FLOAT
            if( tri.Length_Sq() < EPSILON7 ) {
#else
            if( tri.Length_Sq() < EPSILON13 ) {
#endif
                // Use the edge method
                tri = (obj1->Transformation() &
                 ((SWIFT_Tri_Vertex*)contact_list1[i])->Gathered_Direction()) -
                      (obj2->Transformation() &
                 ((SWIFT_Tri_Vertex*)contact_list2[i])->Gathered_Direction());
            }
            tri.Normalize();
            break;
        case CONTINUE_VE:
          { SWIFT_Tri_Edge* e = ((SWIFT_Tri_Edge*)contact_list2[i]);
#ifdef SWIFT_USE_FLOAT
            if( contact_listd[i] < EPSILON7 ) {
#else
            if( contact_listd[i] < EPSILON13 ) {
#endif
                // Just take the average of the face normals
                e = e->Origin()->Adj_Edge( e->Next()->Origin() );
                tri = e->Adj_Face()->Normal() + e->Twin()->Adj_Face()->Normal();
#ifdef SWIFT_USE_FLOAT
                if( tri.Length_Sq() < EPSILON7 ) {
#else
                if( tri.Length_Sq() < EPSILON13 ) {
#endif
                    // Do the cross product method
                    tri = e->Twin()->Adj_Face()->Normal() % e->Direction();
                }
            } else {
                // Compute weighted combination of vertex' angle about the edge
                tri = trans12 * ((SWIFT_Tri_Vertex*)contact_list1[i])->Coords();
                tri -= e->Origin()->Coords() +
                        (((tri - e->Origin()->Coords()) * e->Direction()) *
                         e->Direction());
            }
            tri.Normalize();
            tri &= obj2->Transformation();
          } break;
        case CONTINUE_EV:
          { SWIFT_Tri_Edge* e = ((SWIFT_Tri_Edge*)contact_list1[i]);
#ifdef SWIFT_USE_FLOAT
            if( contact_listd[i] < EPSILON7 ) {
#else
            if( contact_listd[i] < EPSILON13 ) {
#endif
                // Just take the average of the face normals
                e = e->Origin()->Adj_Edge( e->Next()->Origin() );
                tri = -e->Adj_Face()->Normal() -e->Twin()->Adj_Face()->Normal();
#ifdef SWIFT_USE_FLOAT
                if( tri.Length_Sq() < EPSILON7 ) {
#else
                if( tri.Length_Sq() < EPSILON13 ) {
#endif
                    // Do the cross product method
                    tri = e->Adj_Face()->Normal() % e->Direction();
                }
            } else {
                // Compute weighted combination of vertex' angle about the edge
                tri = -(trans12 *
                      ((SWIFT_Tri_Vertex*)contact_list1[i])->Coords());
                tri += e->Origin()->Coords() +
                        (((-tri - e->Origin()->Coords()) * e->Direction()) *
                         e->Direction());
            }
            tri.Normalize();
            tri &= obj1->Transformation();
          } break;
        case CONTINUE_VF:
          { // Transform the face normal to world coordinates
            const SWIFT_Tri_Face* f = ((SWIFT_Tri_Face*)contact_list2[i]);
            tri = obj2->Transformation() & f->Normal();
          } break;
        case CONTINUE_FV:
          { // Transform the face normal to world coordinates
            const SWIFT_Tri_Face* f = ((SWIFT_Tri_Face*)contact_list1[i]);
            tri = -(obj1->Transformation() & f->Normal());
          } break;
        case CONTINUE_EE:
          { SWIFT_Tri_Edge* e = ((SWIFT_Tri_Edge*)contact_list1[i]);
            SWIFT_Tri_Edge* ee = ((SWIFT_Tri_Edge*)contact_list2[i]);
            // Compute the cross product of the two edge directions
            tri = e->Direction() % (trans21 & ee->Direction());
#ifdef SWIFT_USE_FLOAT
            if( tri.Length_Sq() < EPSILON7 ) {
#else
            if( tri.Length_Sq() < EPSILON13 ) {
#endif
                // Handle nearly parallel edges by averaging face normals
                e = e->Origin()->Adj_Edge( e->Next()->Origin() );
                ee = ee->Origin()->Adj_Edge( ee->Next()->Origin() );
                tri = - (obj1->Transformation() & (e->Adj_Face()->Normal() +
                                            e->Twin()->Adj_Face()->Normal()))
                      + (obj2->Transformation() & (ee->Adj_Face()->Normal() +
                                            ee->Twin()->Adj_Face()->Normal()));
#ifdef SWIFT_USE_FLOAT
                if( tri.Length_Sq() < EPSILON7 ) {
#else
                if( tri.Length_Sq() < EPSILON13 ) {
#endif
                    // Use the cross product method
                    tri = ee->Twin()->Adj_Face()->Normal() % ee->Direction();
                    tri &= obj2->Transformation();
                }
            } else {
                // Determine which way the vector should point
                if( (obj1->Center_Of_Mass( piece1 ) - e->Origin()->Coords()) *
                    tri < 0.0
                ) {
                    tri.Negate();
                }
                tri &= obj1->Transformation();
            }
            tri.Normalize();
          } break;
        default:
            break;
        }
        tri.Get_Value( normals_ptr );
    }

    // Fix up the length of the normals array
    normals.Set_Length( normals.Length() + 3*contact_listt.Length() );
}



///////////////////////////////////////////////////////////////////////////////
// SWIFT_Pair private functions
///////////////////////////////////////////////////////////////////////////////

// Takes the last known state before penetration or disjointness was detected
// and saves those features as well as their type encoded as a CONTINUE_* value
// encoded in the status bits of the pair.
inline void SWIFT_Pair::Save_State( )
{
    // Save the features
    switch( save_state ) {
    case CONTINUE_VV:
        feat1 = (void*) save_v1;
        feat2 = (void*) save_v2;
        break;
    case CONTINUE_VE:
        feat1 = (void*) save_v1;
        feat2 = (void*) save_e2;
        break;
    case CONTINUE_EV:
        feat1 = (void*) save_e1;
        feat2 = (void*) save_v2;
        break;
    case CONTINUE_VF:
        feat1 = (void*) save_v1;
        feat2 = (void*) save_f2;
        break;
    case CONTINUE_FV:
        feat1 = (void*) save_f1;
        feat2 = (void*) save_v2;
        break;
    case CONTINUE_EE:
        feat1 = (void*) save_e1;
        feat2 = (void*) save_e2;
    default:
        break;
    }

#ifdef SWIFT_HIERARCHY
    Set_Level1( save_level1 );
    Set_Level2( save_level2 );
#endif

    // Save the state id
    Set_State( save_state );
}


