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
// object.C
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <SWIFT_config.h>
#include <SWIFT_common.h>
#include <SWIFT_linalg.h>
#include <SWIFT_mesh.h>
#include <SWIFT_object.h>


//////////////////////////////////////////////////////////////////////////////
// SWIFT_Object public functions
//////////////////////////////////////////////////////////////////////////////

SWIFT_Object::SWIFT_Object( )
{
#ifdef SWIFT_HIERARCHY
    meshes = new SWIFT_Array<SWIFT_Tri_Mesh*>[1];
#else
    meshes.Create( 1 );
#endif
    min_bns.Create( 1 );
    max_bns.Create( 1 );
    min_bns[0][0].Set_Is_Max( false ), max_bns[0][0].Set_Is_Max( true ),
    min_bns[0][1].Set_Is_Max( false ), max_bns[0][1].Set_Is_Max( true ),
    min_bns[0][2].Set_Is_Max( false ), max_bns[0][2].Set_Is_Max( true );
    cubes.Create( 1 );
    enlargements.Create( 1 );
    radii.Create( 1 );
    min_vs.Create( 1 );
    max_vs.Create( 1 );

    transform.Identity();
}

SWIFT_Object::SWIFT_Object( int np )
{
    int i;

#ifdef SWIFT_HIERARCHY
    meshes = new SWIFT_Array<SWIFT_Tri_Mesh*>[np];
#else
    meshes.Create( np );
#endif
    min_bns.Create( np );
    max_bns.Create( np );
    for( i = 0; i < np; i++ ) {
        min_bns[i][0].Set_Is_Max( false ), max_bns[i][0].Set_Is_Max( true ),
        min_bns[i][1].Set_Is_Max( false ), max_bns[i][1].Set_Is_Max( true ),
        min_bns[i][2].Set_Is_Max( false ), max_bns[i][2].Set_Is_Max( true );
    }
    cubes.Create( np );
    enlargements.Create( np );
    radii.Create( np );
    min_vs.Create( np );
    max_vs.Create( np );

    transform.Identity();
}

SWIFT_Object::~SWIFT_Object( )
{
    int i;

    // Delete the meshes
    for( i = 0; i < Number_Of_Pieces(); i++ ) {
#ifdef SWIFT_HIERARCHY
        if( meshes[i].Length() != 0 ) {
            if( Finest_Mesh(i)->Ref() == 0 ) {
                int j;
                for( j = 0; j < meshes[i].Length(); j++ ) {
                    delete meshes[i][j];
                }
            } else {
                Finest_Mesh(i)->Decrement_Ref();
            }
        }
#else
        if( Finest_Mesh(i)->Ref() == 0 ) {
            delete Finest_Mesh(i);
        } else {
            Finest_Mesh(i)->Decrement_Ref();
        }
#endif
    }

#ifdef SWIFT_HIERARCHY
    delete [] meshes;
#endif
}

void SWIFT_Object::Get_Box_Nodes( int i,
                            SWIFT_Box_Node** min_0, SWIFT_Box_Node** max_0,
                            SWIFT_Box_Node** min_1, SWIFT_Box_Node** max_1,
                            SWIFT_Box_Node** min_2, SWIFT_Box_Node** max_2 )
{
    *min_0 = min_bns[i];   *max_0 = max_bns[i];
    *min_1 = min_bns[i]+1; *max_1 = max_bns[i]+1;
    *min_2 = min_bns[i]+2; *max_2 = max_bns[i]+2;
}


void SWIFT_Object::Set_Id( int i )
{
    int j;

    id = i;

    for( j = 0; j < Number_Of_Pieces(); j++ ) {
        min_bns[j][0].Set_Id( i+j ); max_bns[j][0].Set_Id( i+j );
        min_bns[j][1].Set_Id( i+j ); max_bns[j][1].Set_Id( i+j );
        min_bns[j][2].Set_Id( i+j ); max_bns[j][2].Set_Id( i+j );
    }
}

void SWIFT_Object::Initialize( SWIFT_Tri_Mesh* m, bool f, bool uc,
                               SWIFT_Real ber, SWIFT_Real bea, bool cp )
{
#ifdef SWIFT_HIERARCHY
    int i;
    meshes[0].Create( m->Num_Levels() );

    // Copy over the mesh pointers from the hierarchy
    for( i = 0; i < Number_Of_Levels(); i++ ) {
        meshes[0][i] = m;
        m = m->Parent();
    }
#else
    meshes[0] = m;
#endif

    fixed = f;
    cubes[0] = uc;
    enlargements[0] = ber * Radius() + bea;

    if( uc ) {
        // Create the cube
        radii[0] = Radius() + enlargements[0];
    } else {
        // Initialize the dynamic bounding box
        min_vs[0][0] = min_vs[0][1] = min_vs[0][2] = max_vs[0][0] = 
        max_vs[0][1] = max_vs[0][2] = Coarsest_Mesh(0)->Vertices()(0);
    }
    Update_Boxes();
}

void SWIFT_Object::Initialize( SWIFT_Tri_Mesh** ms, bool f, const bool* ucs,
                               const SWIFT_Real* ber, const SWIFT_Real* bea,
                               const bool* cps )
{
    int i;

    fixed = f;

    for( i = 0; i < Number_Of_Pieces(); i++ ) {
#ifdef SWIFT_HIERARCHY
        int j;
        SWIFT_Tri_Mesh* m;
        meshes[i].Create( ms[i]->Num_Levels() );

        // Copy over the mesh pointers from the hierarchy
        m = ms[i];
        for( j = 0; j < Number_Of_Levels( i ); j++ ) {
            meshes[i][j] = m;
            m = m->Parent();
        }
#else
        meshes[i] = ms[i];
#endif

        cubes[i] = ucs[i];
        enlargements[i] = (ber == NULL ? 0.0 : ber[i]) * Radius( i ) +
                          (bea == NULL ? 0.0 : bea[i]);

        // Adding the object for the first time
        if( ucs[i] ) {
            // Create the cube
            radii[i] = Radius( i ) + enlargements[i];
        } else {
            // Initialize the dynamic bounding box
            min_vs[i][0] = min_vs[i][1] = min_vs[i][2] = max_vs[i][0] = 
            max_vs[i][1] = max_vs[i][2] = Coarsest_Mesh(i)->Vertices()(0);
        }
    }
    Update_Boxes();
}



