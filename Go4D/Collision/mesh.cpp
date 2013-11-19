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
// mesh.C
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <iostream.h>

#include <SWIFT_config.h>
#include <SWIFT_common.h>
#include <SWIFT_linalg.h>
#include <SWIFT_array.h>
#include <SWIFT_mesh.h>
#include <SWIFT_fileio.h>
#include <SWIFT_mesh_utils.h>

// Feature type identifiers
static const int SWIFT_VERTEX = 1;
static const int SWIFT_EDGE = 2;
static const int SWIFT_FACE = 3;

#ifdef SWIFT_DEBUG
#ifdef SWIFT_USE_FLOAT
static const SWIFT_Real REL_TOL1 = EPSILON6;
static const SWIFT_Real REL_TOL2 = EPSILON7;
static const SWIFT_Real REL_TOL_SQRT = EPSILON3;
#else
static const SWIFT_Real REL_TOL1 = EPSILON11;
static const SWIFT_Real REL_TOL2 = EPSILON12;
static const SWIFT_Real REL_TOL_SQRT = EPSILON6;
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
// QSlim Hierarchy Creation Local functions
//////////////////////////////////////////////////////////////////////////////
#ifdef SWIFT_HIERARCHY
#ifdef SWIFT_QSLIM_HIER
#include <stdmix.h>
#include <MxQSlim.h>

// This code was derived from the QSlim package since it uses the mix lib.
// Credit goes to Michael Garland.
static void SWIFT_QSlim( int ft, MxStdModel* mdl )
{
    unsigned int i;
    MxQSlim* slim = new MxEdgeQSlim(*mdl);

    slim->placement_policy = MX_PLACE_OPTIMAL;
    slim->boundary_weight = 1000.0;
    slim->weighting_policy = MX_WEIGHT_AREA;
    slim->compactness_ratio = 0.0;
    slim->meshing_penalty = 1.0;
    slim->will_join_only = false;

    slim->initialize();

    slim->decimate( ft );

    // First, mark stray vertices for removal
    for( i = 0; i < mdl->vert_count(); i++ ) {
        if( mdl->vertex_is_valid( i ) && mdl->neighbors( i ).length() == 0 ) {
            mdl->vertex_mark_invalid( i );
        }
    }

    // Compact vertex array so only valid vertices remain
    mdl->compact_vertices();

    delete slim;
}

// Read the vertices from the model into the array.  Allocates the array.
static void Read( MxStdModel* mdl, SWIFT_Real*& vs, int& vn )
{
    unsigned int i, j;
    vs = new SWIFT_Real[3*mdl->vert_count()];

    vn = mdl->vert_count();
    for( i = 0, j = 0; i < mdl->vert_count(); i++ ) {
        vs[j++] = mdl->vertex(i)[0];
        vs[j++] = mdl->vertex(i)[1];
        vs[j++] = mdl->vertex(i)[2];
    }
}

// Write the geometry from the arrays into the model.  Allocates the model.
static void Write( MxStdModel** mdl, const SWIFT_Real* vs, const int* fs,
                              int vn, int fn )
{
    int i, j;
    *mdl = new MxStdModel(vn,fn);
    for( i = 0, j = 0; i < vn; i++, j += 3 ) {
        (*mdl)->add_vertex( vs[j], vs[j+1], vs[j+2] );
    }
    for( i = 0, j = 0; i < fn; i++, j += 3 ) {
        (*mdl)->add_face( fs[j], fs[j+1], fs[j+2] );
    }

}
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
// SWIFT_Tri_Vertex functions
//////////////////////////////////////////////////////////////////////////////
int SWIFT_Tri_Vertex::Valence( ) const
{
    // The number of edges adjacent to the vertex is equivalent to the
    // number of faces that are adjacent since the mesh should be closed.
    SWIFT_Tri_Edge* e = edge->Twin()->Next();
    int valence;
    for( valence = 1; e != edge; e = e->Twin()->Next(), valence++ )
#ifdef SWIFT_DEBUG
    {
        if( valence == 1000 ) {
            cerr << "Warning: touched 1000 edges while computing valence"
                 << endl;
            break;
        }
    }
#else
    ;
#endif
    return valence;
}

#ifdef SWIFT_DEBUG
bool SWIFT_Tri_Vertex::Verify_Topology( int pos ) const
{
    bool result = true;

    if( edge == NULL ) {
        cerr << "Vertex at position " << pos << " does not have edge" << endl;
        result = false;
    }

    // Verify that the edges around this vertex loop around
    int count = 0;
    SWIFT_Tri_Edge* e = edge->Twin()->Next();

    for( ; e != edge && count != 1000; e = e->Twin()->Next(), count++ );
    if( count == 1000 ) {
        cerr << "Vertex at position " << pos
             << " next loop exceeded 1000 iterations" << endl;
        result = false;
    }

    e = edge->Prev()->Twin();
    count = 0;
    for( ; e != edge && count != 1000; e = e->Prev()->Twin(), count++ );
    if( count == 100 ) {
        cerr << "Vertex at position " << pos
             << " prev loop exceeded 1000 iterations" << endl;
        result = false;
    }

    return result;
}
#endif


//////////////////////////////////////////////////////////////////////////////
// SWIFT_Tri_Edge functions
//////////////////////////////////////////////////////////////////////////////
#ifdef SWIFT_DEBUG
bool SWIFT_Tri_Edge::Verify_Topology( int pos1, int pos2 ) const
{
    bool result = true;

    if( orig == NULL ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not have vertex" << endl;
        result = false;
    }

    if( face == NULL ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not have face" << endl;
        result = false;
    } else if( face->Edge1P() != this && face->Edge2P() != this &&
               face->Edge3P() != this
    ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not point to the face that points to it" << endl;
        result = false;
    }

    if( next == NULL ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not have next" << endl;
        result = false;
    } else if( next->Next()->Next() != this ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not point to next correctly" << endl;
        result = false;
    }

    if( twin == NULL ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not have twin" << endl;
        result = false;
    } else if( twin->Twin() != this ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " is not its twin's twin" << endl;
        result = false;
    }

    if( Marked() ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " is marked" << endl;
    }

    return result;
}

// The tests are formulated so as to fail if any of the numbers are NAN
// Since NAN (compare) x = FALSE for all x and all comparison operators.
bool SWIFT_Tri_Edge::Verify_Geometry( int pos1, int pos2 ) const
{
    bool result = true;

    if( twin != NULL ) {
        if( !(len == twin->Length()) ) {
            cerr << "Edge at position " << pos1 << ", " << pos2
                 << " does not have twin len" << endl;
            result = false;
        }

        if( !(-u == twin->Direction()) ) {
            cerr << "Edge at position " << pos1 << ", " << pos2
                 << " does not have twin -u" << endl;
            result = false;
        }
    }

    // Make sure that the direction and length match with the set endpoints
    SWIFT_Triple tempu = u;
    SWIFT_Triple temp1 = next->Origin()->Coords() - orig->Coords();
    SWIFT_Real lsq = temp1.Length_Sq();
    SWIFT_Real length_sq = len * len;

    if( !(length_sq / lsq < 1.0 + REL_TOL1 && length_sq / lsq > 1.0 - REL_TOL1)
    ) {
        cerr << "Edge at position " << pos1 << ", " << pos2 << " length = "
             << len << " does not match with endpoints length = "
             << sqrt( lsq ) << endl;
        result = false;
    }

    if( len < REL_TOL2 ) {
        cerr << "Edge at position " << pos1 << ", " << pos2 << " length = "
             << len << " is too short!" << endl;
        result = false;
    }

    tempu.Normalize();
    temp1.Normalize();
    length_sq = tempu * temp1;

    if( !(length_sq < 1.0 + REL_TOL2 && length_sq > 1.0 - REL_TOL2) ) {
        cerr << "Edge at position " << pos1 << ", " << pos2 << " direction = "
             << tempu << " does not match with endpoints direction = "
             << temp1 << endl;
        result = false;
    }

    // Attempt to make sure that the edge direction vector is unit length
    if( !(u.Length_Sq() < 1.0 + REL_TOL2 && u.Length_Sq() > 1.0 - REL_TOL2)
    ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not have a unit length u vector = " << u << endl;
        result = false;
    }

    length_sq = Distance( orig->Coords() );

    // Make sure that the origin is on the edge plane.
    if( !(length_sq < REL_TOL_SQRT && length_sq > -REL_TOL_SQRT) ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " origin is a distance away from the edge plane = "
             << length_sq << endl;
        result = false;
    }

    length_sq = Face_Distance( orig->Coords() );

    // Make sure that the origin is on the edge-face plane.
    if( !(length_sq < REL_TOL_SQRT && length_sq > -REL_TOL_SQRT) ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " origin is a distance away from the edge-face plane = "
             << length_sq << endl;
        result = false;
    }

    // Make sure that the edge-face plane is correct
    tempu = face->Normal() % u;
    length_sq = tempu * fn;
    if( !(length_sq < 1.0 + REL_TOL2 && length_sq > 1.0 - REL_TOL2) ) {
        cerr << "Edge at position " << pos1 << ", " << pos2
             << " does not have correct edge-face normal = " << fn
             << " should be = " << tempu << endl;
        result = false;
    }


    return result;
}
#endif


//////////////////////////////////////////////////////////////////////////////
// SWIFT_Tri_Face functions
//////////////////////////////////////////////////////////////////////////////
void SWIFT_Tri_Face::Compute_Plane_From_Edges( )
{
    // It is important for the Fill_Holes function that this use the directions
    // of e2 and e3 only!
    normal = e2.Direction() % e3.Direction();
    normal.Normalize();
    d = e1.Origin()->Coords() * normal;
}

void SWIFT_Tri_Face::Compute_Plane_From_Edges( SWIFT_Tri_Edge* edge1 )
{
    normal = edge1->Direction() % edge1->Next()->Direction();
    normal.Normalize();
    d = edge1->Origin()->Coords() * normal;
}

#ifdef SWIFT_DEBUG
bool SWIFT_Tri_Face::Verify_Topology( int pos ) const
{
    if( Marked() ) {
        cerr << "Face at position " << pos << " is marked" << endl;
    }

    return true;
}

// The tests are formulated so as to fail if any of the numbers are NAN
// Since NAN (compare) x = FALSE for all x and all comparison operators.
bool SWIFT_Tri_Face::Verify_Geometry( int pos ) const
{
    bool result = true;

    // Make sure that the edges that are set compute the same normal
    SWIFT_Triple temp1 = e1.Direction() % e2.Direction();

    temp1.Normalize();
    if( !((temp1 * normal) < 1.0 + REL_TOL2 &&
         (temp1 * normal) > 1.0 - REL_TOL2)
    ) {
        cerr << "Face at position " << pos << " normal = " << normal
             << " does not match with that computed using e1 and e2 = "
             << temp1 << endl;
        cerr << "The edges are e1 = " << e1.Direction() << ", e2 = "
             << e2.Direction() << endl;
        result = false;
    }

    temp1 = e2.Direction() % e3.Direction();
    temp1.Normalize();
    if( !((temp1 * normal) < 1.0 + REL_TOL2 &&
         (temp1 * normal) > 1.0 - REL_TOL2)
    ) {
        cerr << "Face at position " << pos << " normal = " << normal
             << " does not match with that computed using e2 and e3 = "
             << temp1 << endl;
        cerr << "The edges are e2 = " << e2.Direction() << ", e3 = "
             << e3.Direction() << endl;
        result = false;
    }

    temp1 = e3.Direction() % e1.Direction();
    temp1.Normalize();
    if( !((temp1 * normal) < 1.0 + REL_TOL2 &&
         (temp1 * normal) > 1.0 - REL_TOL2)
    ) {
        cerr << "Face at position " << pos << " normal = " << normal
             << " does not match with that computed using e3 and e1 = "
             << temp1 << endl;
        cerr << "The edges are e3 = " << e3.Direction() << ", e1 = "
             << e1.Direction() << endl;
        result = false;
    }

    temp1 = (e1.Origin()->Coords() - e3.Origin()->Coords()) %
            (e2.Origin()->Coords() - e1.Origin()->Coords());
    temp1.Normalize( );
    if( !((temp1 * normal) < 1.0 + REL_TOL2 &&
         (temp1 * normal) > 1.0 - REL_TOL2)
    ) {
        cerr << "Face at position " << pos << " normal = " << normal
             << " does not match with that computed using vertices 1 = "
             << temp1 << endl;
        result = false;
    }

    temp1 = (e2.Origin()->Coords() - e1.Origin()->Coords()) %
            (e3.Origin()->Coords() - e2.Origin()->Coords());
    temp1.Normalize( );
    if( !((temp1 * normal) < 1.0 + REL_TOL2 &&
         (temp1 * normal) > 1.0 - REL_TOL2)
    ) {
        cerr << "Face at position " << pos << " normal = " << normal
             << " does not match with that computed using vertices 2 = "
             << temp1 << endl;
        result = false;
    }

    temp1 = (e3.Origin()->Coords() - e2.Origin()->Coords()) %
            (e1.Origin()->Coords() - e3.Origin()->Coords());
    temp1.Normalize( );
    if( !((temp1 * normal) < 1.0 + REL_TOL2 &&
         (temp1 * normal) > 1.0 - REL_TOL2)
    ) {
        cerr << "Face at position " << pos << " normal = " << normal
             << " does not match with that computed using vertices 3 = "
             << temp1 << endl;
        result = false;
    }

    // Attempt to make sure that the face normal vector is unit length
    if( !(normal.Length_Sq() < 1.0 + REL_TOL2 &&
          normal.Length_Sq() > 1.0 - REL_TOL2)
    ) {
        cerr << "Face at position " << pos
             << " does not have a unit length normal vector = " << normal
             << endl;
        result = false;
    }

    return result;
}
#endif


//////////////////////////////////////////////////////////////////////////////
// SWIFT_Tri_Mesh public functions
//////////////////////////////////////////////////////////////////////////////

bool SWIFT_Tri_Mesh::Create( const SWIFT_Real* vs, const int* fs,
                             int vn, int fn, const SWIFT_Orientation& orient,
                             const SWIFT_Translation& trans, SWIFT_Real sc,
                             const int* fv
#ifdef SWIFT_HIERARCHY
                             , int tcount
#ifdef SWIFT_QSLIM_HIER
                             , SWIFT_Real tratio
#endif
#endif
                             )
{
#if defined(SWIFT_HIERARCHY) && defined(SWIFT_QSLIM_HIER)
    // Create a QSlim simplification hierarchy
    int i, j;
    MxStdModel* mdl;
    const SWIFT_Real* qverts[MAX_LEVELS];
    const int* qfaces[MAX_LEVELS];
    int qvert_lens[MAX_LEVELS];
    int qface_lens[MAX_LEVELS];
    SWIFT_Orientation* qorient;
    SWIFT_Translation* qtrans;
    SWIFT_Real* qsc;
    int face_target;
    int vnn, tn;
    const SWIFT_Real* new_vs;
    const int* new_fs;
    SWIFT_Real* read_qverts;
    int* chull_faces;
    SWIFT_Array<int> triang_edges;
    SWIFT_Array<int> vmap; 
    SWIFT_Array<int> vnewi;

    // Initialize the other level arrays.
    for( i = 1; i < MAX_LEVELS; i++ ) {
        qverts[i] = NULL;
        qfaces[i] = NULL;
    }

    // Initialize the transformation arrays
    qorient = new SWIFT_Orientation[MAX_LEVELS];
    for( i = 0; i < MAX_LEVELS; i++ ) {
        for( j = 0; j < 9; j++ ) {
            qorient[i][j] = orient[j];
        }
    }

    qtrans = new SWIFT_Translation[MAX_LEVELS];
    for( i = 0; i < MAX_LEVELS; i++ ) {
        for( j = 0; j < 3; j++ ) {
            qtrans[i][j] = trans[j];
        }
    }

    qsc = new SWIFT_Real[MAX_LEVELS];
    for( i = 0; i < MAX_LEVELS; i++ ) {
        qsc[i] = sc;
    }

    // No need to transform the vertices because that will be done by the
    // other create call.  Duplicate vertices are removed here because we
    // want QSlim to behave properly.
    Process_Vertices( vs, vn, vnn, new_vs, vmap, vnewi );

    // Triangulate the faces and delete the faces array that is created
    Process_Faces( fs, fn, fv, tn, new_fs, triang_edges );

    // Take care of the vertices and the faces as a result of handling
    // duplication and triangulation
    qvert_lens[0] = vnn;
    qverts[0] = new_vs;
    qface_lens[0] = tn;
    if( no_dup_verts ) {
        // There were no duplicate vertices.  Simply assign the original verts.
        // And assign the new faces.
        qfaces[0] = new_fs;
    } else {
        // Remap the qfaces array
        chull_faces = new int[tn*3];
        for( i = 0, j = 0; i < tn; i++, j += 3 ) {
            chull_faces[j] = vnewi[ vmap[ new_fs[j] ] ];
            chull_faces[j+1] = vnewi[ vmap[ new_fs[j+1] ] ];
            chull_faces[j+2] = vnewi[ vmap[ new_fs[j+2] ] ];
        }
        qfaces[0] = chull_faces;
#ifdef WIN32
        delete (void*)new_fs;
#else
        delete new_fs;
#endif
    }

    // Write the arrays to the model
    //Write( &mdl, qverts[0], qfaces[0], qvert_lens[0], qface_lens[0] );

    // Compute the face target
    face_target = (int)(tratio*(SWIFT_Real)qface_lens[0]);
    for( j = 1; j < MAX_LEVELS && face_target > tcount; j++ ) {
        // Write the arrays to the model
        Write( &mdl, qverts[j-1], qfaces[j-1],
                     qvert_lens[j-1], qface_lens[j-1] );

        // Simplify the model to the desired face target
        SWIFT_QSlim( face_target, mdl );

        // Read the simplified model into the arrays
        Read( mdl, read_qverts, qvert_lens[j] );
        qverts[j] = read_qverts;

        // Make the model convex.  qfaces are allocated here.
        Compute_Convex_Hull( qverts[j], qvert_lens[j],
                             chull_faces, qface_lens[j]  );
        qfaces[j] = chull_faces;

        // Compute the next face target
        face_target = (int)(tratio*(SWIFT_Real)face_target);

        delete mdl;
    }

    // Call the hierarchy-given creation function.  fv is given as NULL since
    // all the faces produced by qhull are triangles.
    Create( qverts, qfaces, qvert_lens, qface_lens, j, true, &triang_edges,
            qorient, qtrans, qsc );

    // Delete everything
    if( !no_dup_verts ) {
        delete qverts[0];
    }

    if( !only_tris || !no_dup_verts ) {
        delete qfaces[0];
    }

    for( i = 1; i < j; i++ ) {
        delete qverts[i];
        delete qfaces[i];
    }

    delete qorient;
    delete qtrans;
    delete qsc;

    return true;
#else
    int i, j;
    int e, f;
    int vnn, tn;
    const SWIFT_Real* new_vs;
    const int* new_fs;
    SWIFT_Tri_Vertex* tv;
    SWIFT_Tri_Edge* te;
    SWIFT_Array<SWIFT_Tri_Edge*> sort_edges;
    SWIFT_Array<SWIFT_Tri_Face> temp_faces;
    SWIFT_Array<int> triang_edges;
    SWIFT_Array<int> vmap; 
    SWIFT_Array<int> vnewi;

    // Remove duplicate vertices
    Process_Vertices( vs, vn, vnn, new_vs, vmap, vnewi );

    // Create the vertices
    verts.Create( vnn );

    // Transform the vertices
    Transform_Vertices( new_vs, vnn, orient, trans, sc );

    // Delete the vertex array copy if there were duplicate vertices
    if( !no_dup_verts ) {
#ifdef WIN32
        delete (void*)new_vs;
#else
        delete new_vs;
#endif
    }

    // Triangulate faces.
    Process_Faces( fs, fn, fv, tn, new_fs, triang_edges );

    // Create the faces
    faces.Create( tn );

    // Create the connecting faces and edge ptrs.
    temp_faces.Create( tn );
    sort_edges.Create( tn*6 );

    // For each face, create one oriented as given and one oriented the
    // opposite direction
    for( e = 0, f = 0, i = 0; i < tn; i++ ) {
        // Create the face oriented as given
        tv = verts( vnewi[ vmap[ new_fs[f++] ] ] );
        tv->Set_Adj_Edge( faces[i].Edge1P() );
        faces[i].Edge1().Set_Origin( tv );
        faces[i].Edge1().Set_Twin( temp_faces[i].Edge2P() );
        sort_edges[e++] = faces[i].Edge1P();

        tv = verts( vnewi[ vmap[ new_fs[f++] ] ] );
        tv->Set_Adj_Edge( faces[i].Edge2P() );
        faces[i].Edge2().Set_Origin( tv );
        faces[i].Edge2().Set_Twin( temp_faces[i].Edge1P() );
        sort_edges[e++] = faces[i].Edge2P();

        tv = verts( vnewi[ vmap[ new_fs[f++] ] ] );
        tv->Set_Adj_Edge( faces[i].Edge3P() );
        faces[i].Edge3().Set_Origin( tv );
        faces[i].Edge3().Set_Twin( temp_faces[i].Edge3P() );
        sort_edges[e++] = faces[i].Edge3P();

        // Create the reversed face
        temp_faces[i].Edge1().Set_Origin( faces[i].Edge3().Origin() );
        temp_faces[i].Edge1().Set_Twin( faces[i].Edge2P() );
        sort_edges[e++] = temp_faces[i].Edge1P();

        temp_faces[i].Edge2().Set_Origin( faces[i].Edge2().Origin() );
        temp_faces[i].Edge2().Set_Twin( faces[i].Edge1P() );
        sort_edges[e++] = temp_faces[i].Edge2P();

        temp_faces[i].Edge3().Set_Origin( faces[i].Edge1().Origin() );
        temp_faces[i].Edge3().Set_Twin( faces[i].Edge3P() );
        sort_edges[e++] = temp_faces[i].Edge3P();
    }

    // Don't need the new_fs array anymore
    if( !only_tris ) {
#ifdef WIN32
        delete (void*)new_fs;
#else
        delete new_fs;
#endif
    }

    // Sort the edges lexicographically
    Quicksort( sort_edges, 0, tn*6-1 );


    // Run through the sorted list and connect the mesh.
    tn *= 6;
    for( i = 0; i < tn; ) {
        for( ; i < tn && sort_edges[i]->Marked(); i++ ) {
            sort_edges[i]->Unmark();
        }

        if( i == tn ) break;

        j = i + 1;
        if( j < tn && sort_edges[i]->Origin() == sort_edges[j]->Origin() &&
            sort_edges[i]->Twin()->Origin() == sort_edges[j]->Twin()->Origin()
        ) {
            sort_edges[i]->Twin()->Mark();
            sort_edges[j]->Twin()->Mark();

            // Found a pair of edges that are equal.  But are there three?
            if( j < tn-1 &&
                sort_edges[j+1]->Origin() == sort_edges[j]->Origin() &&
                sort_edges[j+1]->Twin()->Origin() ==
                                                sort_edges[j]->Twin()->Origin()
            ) {
                cerr << "Error: The mesh is not a 2-manifold --" << endl
                     << "       all edges must belong to exactly two triangles"
                     << endl;
                return false;
            } else {
                // Connect the pair appropriately
                sort_edges[i]->Twin()->Set_Twin( sort_edges[j] );
                sort_edges[j]->Twin()->Set_Twin( sort_edges[i] );
                te = sort_edges[i]->Twin();
                sort_edges[i]->Set_Twin( sort_edges[j]->Twin() );
                sort_edges[j]->Set_Twin( te );
                i += 2;
                j += 2;
            }
        } else {
            // Found a lone edge.  It must be on the boundary so snip it.
            cerr << "Error: The mesh is not closed --" << endl
                 << "       all edges must belong to exactly two triangles"
                 << endl;
            return false;
        }
    }

    Compute_Geometry();
    Compute_Center_Of_Mass();
    Compute_Radius();
    Compute_Volume();

    // If there are new edges resulting from face triangulation, then fix the
    // edge-face planes on them to be twin-identical
    for( i = 0; i < tn/6 - fn; i++ ) {
        faces[triang_edges[i]].Edge3().Twin()->Set_Face_Distance(
                            -faces[triang_edges[i]].Edge3().Face_Distance() );
        faces[triang_edges[i]].Edge3().Twin()->Set_Face_Normal(
                            -faces[triang_edges[i]].Edge3().Face_Normal() );
    }

#ifdef SWIFT_DEBUG
    cerr << "******* SWIFT_Tri_Mesh::Create()'1 Verification *******" << endl;
    Verify();
#endif

#ifdef SWIFT_HIERARCHY
    deviation = 0.0;
    Create_Bounding_Volume_Hierarchy( tcount );
#endif

#ifdef SWIFT_HIERARCHY
    Coarsest_Level()->Create_Lookup_Table();
#else
    Create_Lookup_Table();
#endif

#endif
    return true;
}

SWIFT_Tri_Mesh* SWIFT_Tri_Mesh::Clone( const SWIFT_Orientation& orient,
                                       const SWIFT_Translation& trans,
                                       SWIFT_Real sc )
{
    int i, j;
    SWIFT_Tri_Mesh* result = new SWIFT_Tri_Mesh;

    result->Vertices().Create( Num_Vertices() );
    result->Faces().Create( Num_Faces() );

    result->Transform_Vertices_And_COM( this, orient, trans, sc );

    // Make the vertices point to the correct edges and give them coordinates
    for( i = 0; i < Num_Vertices(); i++ ) {
        j = Edge_Id( verts[i].Adj_Edge() );
        result->Vertices()[i].Set_Adj_Edge( result->EdgeP( j ) );
    }

    // Make the edges point to the correct vertices, edges, and faces
    for( i = 0; i < Num_Faces(); i++ ) {
        j = Edge_Id( faces[i].Edge1().Twin() );
        result->Faces()[i].Edge1().Set_Twin( result->EdgeP( j ) );
        result->Faces()[i].Edge1().Set_Origin( result->Vertices()(
                                    Vertex_Id( faces[i].Edge1().Origin() ) ) );

        j = Edge_Id( faces[i].Edge2().Twin() );
        result->Faces()[i].Edge2().Set_Twin( result->EdgeP( j ) );
        result->Faces()[i].Edge2().Set_Origin( result->Vertices()(
                                    Vertex_Id( faces[i].Edge2().Origin() ) ) );

        j = Edge_Id( faces[i].Edge3().Twin() );
        result->Faces()[i].Edge3().Set_Twin( result->EdgeP( j ) );
        result->Faces()[i].Edge3().Set_Origin( result->Vertices()(
                                    Vertex_Id( faces[i].Edge3().Origin() ) ) );
    }

    // Compute the geometry
    for( i = 0; i < Num_Faces(); i++ ) {
        result->Faces()[i].Edge1().Compute_Direction_Length_Twin();
        result->Faces()[i].Edge2().Compute_Direction_Length_Twin();
        result->Faces()[i].Edge3().Compute_Direction_Length_Twin();
        result->Faces()[i].Compute_Plane_From_Edges();
        result->Faces()[i].Edge1().Compute_Voronoi_Planes();
        result->Faces()[i].Edge2().Compute_Voronoi_Planes();
        result->Faces()[i].Edge3().Compute_Voronoi_Planes();
    }

    result->Set_Radius( sc * radius );
    result->Set_Volume( sc * sc * sc * volume );
#ifdef SWIFT_HIERARCHY
    result->Set_Deviation( sc * deviation );

    // Recursively clone the child and take care of mesh child/parent pointers
    if( result->Child() != NULL ) {
        result->Set_Child( result->Child()->Clone( orient, trans, sc ) );
        result->Child()->Set_Parent( this );

        // Fix up the feature child/parent pointers
        for( i = 0; i < Num_Vertices(); i++ ) {
            result->Vertices()[i].Set_Child( result->Child()->Vertices()(
                                child->Vertex_Id( verts[i].Child() ) ) );
        }
        for( i = 0; i < child->Num_Vertices(); i++ ) {
            result->Child()->Vertices()[i].Set_Parent( result->Vertices()(
                                Vertex_Id( child->Vertices()[i].Parent() ) ) );
        }

        for( i = 0; i < Num_Faces(); i++ ) {
            if( faces[i].Is_Child_Vertex() ) {
                result->Faces()[i].Set_Vertex_Child(
                    result->Child()->Vertices()( child->Vertex_Id(
                                        faces[i].Vertex_Child() ) ) );
            } else {
                result->Faces()[i].Set_Child( result->Child()->Faces()(
                                        child->Face_Id( faces[i].Child() ) ) );
            }
            if( faces[i].Edge1().Is_Child_Vertex() ) {
                result->Faces()[i].Edge1().Set_Vertex_Child(
                    result->Child()->Vertices()( child->Vertex_Id(
                                        faces[i].Edge1().Vertex_Child() ) ) );
            } else {
                result->Faces()[i].Edge1().Set_Child(
                    result->Child()->EdgeP( child->Edge_Id( 
                                        faces[i].Edge1().Child() ) ) );
            }
            if( faces[i].Edge2().Is_Child_Vertex() ) {
                result->Faces()[i].Edge2().Set_Vertex_Child(
                    result->Child()->Vertices()( child->Vertex_Id(
                                        faces[i].Edge2().Vertex_Child() ) ) );
            } else {
                result->Faces()[i].Edge2().Set_Child(
                    result->Child()->EdgeP( child->Edge_Id( 
                                        faces[i].Edge2().Child() ) ) );
            }
            if( faces[i].Edge3().Is_Child_Vertex() ) {
                result->Faces()[i].Edge3().Set_Vertex_Child(
                    result->Child()->Vertices()( child->Vertex_Id(
                                        faces[i].Edge3().Vertex_Child() ) ) );
            } else {
                result->Faces()[i].Edge3().Set_Child(
                    result->Child()->EdgeP( child->Edge_Id( 
                                        faces[i].Edge3().Child() ) ) );
            }
        }

        for( i = 0; i < child->Num_Faces(); i++ ) {
            result->Child()->Faces()[i].Set_Parent(
                    (SWIFT_Tri_Face*)result->Vertices()( Vertex_Id(
                    (SWIFT_Tri_Vertex*)child->Faces()[i].Parent() ) ) );
            result->Child()->Faces()[i].Edge1().Set_Parent(
                    (SWIFT_Tri_Edge*)result->Vertices()( Vertex_Id(
                    (SWIFT_Tri_Vertex*)child->Faces()[i].Edge1().Parent() ) ) );
            result->Child()->Faces()[i].Edge2().Set_Parent(
                    (SWIFT_Tri_Edge*)result->Vertices()( Vertex_Id(
                    (SWIFT_Tri_Vertex*)child->Faces()[i].Edge2().Parent() ) ) );
            result->Child()->Faces()[i].Edge3().Set_Parent(
                    (SWIFT_Tri_Edge*)result->Vertices()( Vertex_Id(
                    (SWIFT_Tri_Vertex*)child->Faces()[i].Edge3().Parent() ) ) );
        }
    }
#endif

#ifdef SWIFT_DEBUG
    cerr << "******* SWIFT_Tri_Mesh::Clone() Verification *******" << endl;
    Verify(
#ifdef SWIFT_HIERARCHY
            true
#endif
            );
#endif

    return result;
}


SWIFT_Tri_Vertex* SWIFT_Tri_Mesh::Close_Vertex( const SWIFT_Triple& pt,
                                                SWIFT_Tri_Vertex*& startv )
{
    int type;
    SWIFT_Real dist;
    void* near_f;

    if( startv == NULL ) {
        startv = verts(0);
    }

    // Use Extremal_Vertex to get close
    Extremal_Vertex( pt - com, startv );

    // Then use Closest_Feature to get even closer
    near_f = Closest_Feature( pt, startv, type, dist );

    if( near_f == NULL ) {
        cerr << "Error (SWIFT_Tri_Mesh::Close_Vertex): Could not find "
             << "Closest_Feature: inside mesh" << endl;
        return verts(0);
    }

    switch( type ) {
    case SWIFT_VERTEX:
        startv = (SWIFT_Tri_Vertex*)near_f;
        return (SWIFT_Tri_Vertex*)near_f;
        break;
    case SWIFT_EDGE:
        startv = ((SWIFT_Tri_Edge*)near_f)->Origin();
        dist = ((SWIFT_Tri_Edge*)near_f)->Origin()->Coords().Dist_Sq( pt );
        if( ((SWIFT_Tri_Edge*)near_f)->Next()->Origin()->Coords().
                                                         Dist_Sq( pt ) > dist
        ) {
            return ((SWIFT_Tri_Edge*)near_f)->Origin();
        } else {
            return ((SWIFT_Tri_Edge*)near_f)->Next()->Origin();
        }
        break;
    case SWIFT_FACE:
        SWIFT_Real dist2, dist3;
        dist = ((SWIFT_Tri_Face*)near_f)->Edge1().Origin()->Coords().
                                                                Dist_Sq( pt );
        dist2 = ((SWIFT_Tri_Face*)near_f)->Edge2().Origin()->Coords().
                                                                Dist_Sq( pt );
        dist3 = ((SWIFT_Tri_Face*)near_f)->Edge3().Origin()->Coords().
                                                                Dist_Sq( pt );
        startv = ((SWIFT_Tri_Face*)near_f)->Edge1().Origin();
        if( dist < dist2 ) {
            if( dist < dist3 ) {
                // First one closest
                return ((SWIFT_Tri_Face*)near_f)->Edge1().Origin();
            } else {
                // Third one closest
                return ((SWIFT_Tri_Face*)near_f)->Edge3().Origin();
            }
        } else {
            if( dist2 < dist3 ) {
                // Second one closest
                return ((SWIFT_Tri_Face*)near_f)->Edge2().Origin();
            } else {
                // Third one closest
                return ((SWIFT_Tri_Face*)near_f)->Edge3().Origin();
            }
        }
        break;
    default:
        cerr << "Error (SWIFT_Tri_Mesh::Close_Vertex): Could not find "
             << "Closest_Feature: bad feature type" << endl;
        return verts(0);
        break;
    }

}

// Does not visit vertices that have already been visited.
SWIFT_Real SWIFT_Tri_Mesh::Extremal_Vertex(
                        const SWIFT_Triple& dir, SWIFT_Tri_Vertex*& startv )
{
    SWIFT_Real d1, d2; 
    SWIFT_Tri_Edge* e = startv->Adj_Edge()->Twin();
    SWIFT_Tri_Edge* nexte = NULL;
    SWIFT_Tri_Edge* ende = e;
    
    d1 = dir * startv->Coords();

    d2 = dir * e->Origin()->Coords();
    if( d2 > d1 ) {
        d1 = d2;
        nexte = e;
    }
    e = e->Next()->Twin();
    
    while( true ) { 
        // Find the neighbor that has a greater distance
        for( ; e != ende; e = e->Next()->Twin() ) {
            d2 = dir * e->Origin()->Coords();
            if( d2 > d1 ) {
                d1 = d2;
                nexte = e;
            }
        }
        if( nexte == NULL ) {
            break;
        }
        // Set up the edges for the next iteration
        e = nexte->Twin()->Next()->Twin()->Next()->Twin();
        ende = nexte->Prev();
        startv = nexte->Origin();
        nexte = NULL;
    }
    return d1;
}

void* SWIFT_Tri_Mesh::Closest_Feature( const SWIFT_Triple& c,
                                       SWIFT_Tri_Vertex* startv,
                                       int& type, SWIFT_Real& dist )
{
    int state;  // VV = 1, VE = 2, VF = 3
    SWIFT_Tri_Vertex* v;
    SWIFT_Tri_Edge* e;
    SWIFT_Tri_Face* f;
    SWIFT_Tri_Vertex* last_v;
    SWIFT_Tri_Edge* last_e;
    SWIFT_Tri_Face* last_f;
    SWIFT_Real d1, d2;

    last_v = NULL;
    last_e = NULL;
    last_f = NULL;
    v = (startv == NULL) ? verts(0) : v = startv;
    state = 1;

    // Compute the nearest feature on the child level to each vertex
    while( true ) {
        if( state == 1 ) {
            if( last_v != v ) {
                SWIFT_Tri_Edge* edge = v->Adj_Edge();

                // d1 is the max plane violation
                d1 = edge->Distance( c );
                last_v = v;
                e = edge;
                for( edge = edge->Twin()->Next(); edge != v->Adj_Edge();
                     edge = edge->Twin()->Next()
                ) {
                    d2 = edge->Distance( c );
                    if( d2 > d1 ) {
                        d1 = d2;
                        e = edge;
                    }
                }

                if( d1 > 0.0 ) {
                    state = 2;
                    continue;
                }
            }

            // Are closest to this vertex
            dist = v->Coords().Dist_Sq( c );
            type = SWIFT_VERTEX;
            return (void*)v;
        } else if( state == 2 ) {
            if( last_e != e ) {
                d1 = e->Face_Distance( c );
                d2 = e->Twin()->Face_Distance( c );
                last_e = e;

                if( d1 >= 0.0 ) {
                    f = e->Adj_Face();
                }

                if( d2 > d1 ) {
                    d1 = d2;
                    f = e->Twin()->Adj_Face();
                }

                // compute closest point on e to v
                dist = e->Distance( c );

                if( dist < 0.0 ) {
                    // The first endpoint is closest to the vertex
                    if( d1 <= -dist ) {
                        v = e->Origin();
                        state = 1;
                        continue;
                    } else {
                        state = 3;
                        continue;
                    }
                } else if( dist > e->Length() ) {
                    // The second endpoint is closest to the vertex
                    if( d1 <= dist - e->Length() ) {
                        v = e->Next()->Origin();
                        state = 1;
                        continue;
                    } else {
                        state = 3;
                        continue;
                    }
                } else if( d1 >= 0.0 ) {
                    state = 3;
                    continue;
                }
            }

            // Are closest to this edge
            dist = c.Dist_Sq( e->Origin()->Coords() + (dist * e->Direction()) );
            type = SWIFT_EDGE;
            return (void*)e;
        } else {
            if( last_f != f ) {
                // Find the edge-face plane that is maximally violated.
                d1 = f->Edge1().Face_Distance( c );
                d2 = f->Edge2().Face_Distance( c );
                last_f = f;

                e = f->Edge1P();
                if( d2 < d1 ) {
                    d1 = d2;
                    e = f->Edge2P();
                }

                d2 = f->Edge3().Face_Distance( c );
                if( d2 < d1 ) {
                    d1 = d2;
                    e = f->Edge3P();
                }
                if( d1 < 0.0 ) {
                    state = 2;
                    continue;
                }

                // Check to see if we are on the positive side
                if( f->Distance( c ) < 0.0 ) {
                    // Check all faces
                    d1 = faces[0].Distance( c );
                    f = faces(0);
                    for( int j = 1; j < Num_Faces(); j++ ) {
                        d2 = faces[j].Distance( c );
                        if( d2 > d1 ) {
                            d1 = d2;
                            f = faces(j);
                        }
                    }
                    if( d1 > 0.0 ) {
                        continue;
                    } else {
                        f = NULL;
                    }
                }
            }

            // Are closest to this face
            dist = f->Distance( c );
            type = SWIFT_FACE;
            return (void*)f;
        }
    }
}

#ifdef SWIFT_DEBUG
void SWIFT_Tri_Mesh::Verify(
#ifdef SWIFT_HIERARCHY
                            bool bounding
#endif
                            )
{
    int i;

cerr << "------------- Beginning Verification -------------" << endl;

    for( i = 0; i < Num_Vertices(); i++ ) {
        // Verify the vertex connectivity
        verts[i].Verify_Topology( i );
    }

    for( i = 0; i < Num_Faces(); i++ ) {
        // Verify the face connectivity
        if( faces[i].Verify_Topology( i ) ) {
            faces[i].Verify_Geometry( i );
        }

        // Verify the edge connectivity
        if( faces[i].Edge1().Verify_Topology( i, 1 ) ) {
            faces[i].Edge1().Verify_Geometry( i, 1 );
        }
        if( faces[i].Edge2().Verify_Topology( i, 2 ) ) {
            faces[i].Edge2().Verify_Geometry( i, 2 );
        }
        if( faces[i].Edge3().Verify_Topology( i, 3 ) ) {
            faces[i].Edge3().Verify_Geometry( i, 3 );
        }
    }

    // Verify that the mesh is convex by checking each edge
    for( i = 0; i < Num_Faces(); i++ ) {
        if( !faces[i].Inside_Tol(
                    faces[i].Edge1().Twin()->Prev()->Origin(), REL_TOL2 )
        ) {
            cerr << "Face edge 1 is non-convex for face " << i
                 << " by more than a relative amount of 1e-12" << endl;
            cerr << faces[i].Distance(
                    faces[i].Edge1().Twin()->Prev()->Origin() ) << endl;
        }
        if( !faces[i].Inside_Tol(
                    faces[i].Edge2().Twin()->Prev()->Origin(), REL_TOL2 )
        ) {
            cerr << "Face edge 2 is non-convex for face " << i
                 << " by more than a relative amount of 1e-12" << endl;
            cerr << faces[i].Distance(
                    faces[i].Edge2().Twin()->Prev()->Origin() ) << endl;
        }
        if( !faces[i].Inside_Tol(
                    faces[i].Edge3().Twin()->Prev()->Origin(), REL_TOL2 )
        ) {
            cerr << "Face edge 3 is non-convex for face " << i
                 << " by more than a relative amount of 1e-12" << endl;
            cerr << faces[i].Distance(
                    faces[i].Edge3().Twin()->Prev()->Origin() ) << endl;
        }
    }

#ifdef SWIFT_HIERARCHY
    // Verify that this mesh bounds the finest mesh

    // First find the radius of the largest sphere centered at the center of
    // mass which is contained in this mesh
    if( bounding && child != NULL ) {
        SWIFT_Tri_Mesh* finest;
        SWIFT_Real rmin = SWIFT_INFINITY;
        SWIFT_Real r;
        int j;

        // Find a pointer to the finest mesh
        finest = child;
        while( finest->Child() != NULL ) {
            finest = finest->Child();
        }

        for( i = 0; i < Num_Faces(); i++ ) {
            r = (faces[i].Edge1().Origin()->Coords() - com) * faces[i].Normal();
            if( r < rmin ) {
                rmin = r;
            }
        }
        rmin *= rmin;

        // Second check all the vertices for containment in this sphere...
        // if any fall outside of the sphere then test it against all the faces.
        for( i = 0; i < finest->Num_Vertices(); i++ ) {
            if( finest->Vertices()[i].Coords().Dist_Sq( com ) > rmin ) {
                for( j = 0; j < Num_Faces(); j++ ) {
                    if( faces[j].Distance( finest->Vertices()(i) ) > REL_TOL2
                    ) {
                        cerr << "Finest vertex at position " << i
                             << " is outside of face at position " << j
                             << " with amount "
                             << faces[j].Distance( finest->Vertices()(i) )
                             << endl;
                    }
                }
            }
        }
    }

    // Verify that each coarser level has a child for every feature.  And
    // verify that the coarser level's child actually references one of the
    // finer level's features.
    if( child != NULL ) {
        for( i = 0; i < Num_Vertices(); i++ ) {
            if( verts[i].Child() == NULL ) {
                cerr << "Vertex at position " << i << " has no child" << endl;
            } else if( !child->Vertex_In_Range( verts[i].Child() ) ) {
                cerr << "Vertex at position " << i
                     << " has child not existing on finer level" << endl;
            }
        }
        for( i = 0; i < Num_Faces(); i++ ) {
            if( faces[i].Child() == NULL ) {
                cerr << "Face at position " << i << " has no child" << endl;
            } else if( (faces[i].Is_Child_Vertex() && 
                        !child->Vertex_In_Range( faces[i].Vertex_Child() ))
                       || (!faces[i].Is_Child_Vertex() &&
                           !child->Face_In_Range( faces[i].Child() ))
            ) {
                cerr << "Face at position " << i
                     << " has child not existing on finer level" << endl;
            }
            if( faces[i].Edge1().Child() == NULL ) {
                cerr << "Edge at position " << i << ", 1 has no child" << endl;
            } else {
                if( (faces[i].Edge1().Is_Child_Vertex() && 
                     !child->Vertex_In_Range( faces[i].Edge1().Vertex_Child() ))
                     || (!faces[i].Edge1().Is_Child_Vertex() &&
                         !child->Edge_In_Range( faces[i].Edge1().Child() ))
                ) {
                    cerr << "Edge at position " << i
                         << ", 1 has child not existing on finer level" << endl;
                }
                if( !faces[i].Edge1().Is_Child_Vertex() &&
                    faces[i].Edge1().Child()->Adj_Face()->Edge1P() !=
                        faces[i].Edge1().Child() &&
                    faces[i].Edge1().Child()->Adj_Face()->Edge2P() !=
                        faces[i].Edge1().Child() &&
                    faces[i].Edge1().Child()->Adj_Face()->Edge3P() !=
                        faces[i].Edge1().Child()
                ) {
                    cerr << "Edge at position " << i
                         << ", 1 has a messed up child" << endl;
                }
            }
            if( faces[i].Edge2().Child() == NULL ) {
                cerr << "Edge at position " << i << ", 2 has no child" << endl;
            } else {
                if( (faces[i].Edge2().Is_Child_Vertex() && 
                     !child->Vertex_In_Range( faces[i].Edge2().Vertex_Child() ))
                     || (!faces[i].Edge2().Is_Child_Vertex() &&
                         !child->Edge_In_Range( faces[i].Edge2().Child() ))
                ) {
                    cerr << "Edge at position " << i
                         << ", 2 has child not existing on finer level" << endl;
                }
                if( !faces[i].Edge2().Is_Child_Vertex() &&
                    faces[i].Edge2().Child()->Adj_Face()->Edge1P() !=
                        faces[i].Edge2().Child() &&
                    faces[i].Edge2().Child()->Adj_Face()->Edge2P() !=
                        faces[i].Edge2().Child() &&
                    faces[i].Edge2().Child()->Adj_Face()->Edge3P() !=
                        faces[i].Edge2().Child()
                ) {
                    cerr << "Edge at position " << i
                         << ", 2 has a messed up child" << endl;
                }
            }
            if( faces[i].Edge3().Child() == NULL ) {
                cerr << "Edge at position " << i << ", 3 has no child" << endl;
            } else {
                if( (faces[i].Edge3().Is_Child_Vertex() && 
                     !child->Vertex_In_Range( faces[i].Edge3().Vertex_Child() ))
                     || (!faces[i].Edge3().Is_Child_Vertex() &&
                         !child->Edge_In_Range( faces[i].Edge3().Child() ))
                ) {
                    cerr << "Edge at position " << i
                         << ", 3 has child not existing on finer level" << endl;
                }
                if( !faces[i].Edge3().Is_Child_Vertex() &&
                    faces[i].Edge3().Child()->Adj_Face()->Edge1P() !=
                        faces[i].Edge3().Child() &&
                    faces[i].Edge3().Child()->Adj_Face()->Edge2P() !=
                        faces[i].Edge3().Child() &&
                    faces[i].Edge3().Child()->Adj_Face()->Edge3P() !=
                        faces[i].Edge3().Child()
                ) {
                    cerr << "Edge at position " << i
                         << ", 3 has a messed up child" << endl;
                }
            }
        }
    }

    // Verify that each finer level has a parent for every feature.  And
    // verify that the finer level's child actually references one of the
    // coarser level's features.
    if( parent != NULL ) {
        for( i = 0; i < Num_Vertices(); i++ ) {
            if( verts[i].Parent() == NULL ) {
                cerr << "Vertex at position " << i << " has no parent" << endl;
            } else if( !parent->Vertex_In_Range( verts[i].Parent() ) ) {
                cerr << "Vertex at position " << i
                     << " has parent not existing on coarser level" << endl;
            }
        }
        for( i = 0; i < Num_Faces(); i++ ) {
            if( faces[i].Parent() == NULL ) {
                cerr << "Face at position " << i << " has no parent" << endl;
            } else if( !parent->Vertex_In_Range(
                                    (SWIFT_Tri_Vertex*)faces[i].Parent() )
            ) {
                cerr << "Face at position " << i
                     << " has parent not existing on coarser level" << endl;
            }
            if( faces[i].Edge1().Parent() == NULL ) {
                cerr << "Edge at position " << i << ", 1 has no parent" << endl;
            } else if( !parent->Vertex_In_Range(
                            (SWIFT_Tri_Vertex*)faces[i].Edge1().Parent() )
            ) {
                cerr << "Edge at position " << i
                     << ", 1 has parent not existing on coarser level" << endl;
            }
            if( faces[i].Edge2().Parent() == NULL ) {
                cerr << "Edge at position " << i << ", 2 has no parent" << endl;
            } else if( !parent->Vertex_In_Range(
                            (SWIFT_Tri_Vertex*)faces[i].Edge2().Parent() )
            ) {
                cerr << "Edge at position " << i
                     << ", 2 has parent not existing on coarser level" << endl;
            }
            if( faces[i].Edge3().Parent() == NULL ) {
                cerr << "Edge at position " << i << ", 3 has no parent" << endl;
            } else if( !parent->Vertex_In_Range(
                            (SWIFT_Tri_Vertex*)faces[i].Edge3().Parent() )
            ) {
                cerr << "Edge at position " << i
                     << ", 3 has parent not existing on coarser level" << endl;
            }
        }
    }
#endif

cerr << "------------- Done Verification -------------" << endl;
}
#endif


///////////////////////////////////////////////////////////////////////////////
// SWIFT_Tri_Mesh private functions
///////////////////////////////////////////////////////////////////////////////
void SWIFT_Tri_Mesh::Quicksort( SWIFT_Array<SWIFT_Tri_Edge*>& es, int p, int r )
{
    if( p < r ) {
        // Compute a random element to use as the pivot
        int rn = (int) ((SWIFT_Real)(r-p+1) * drand48()) + p;
        int i = p-1;
        int j = r+1;
        SWIFT_Tri_Edge* x = es[rn];
        SWIFT_Tri_Edge* te;

        // Swap the random element into the first position
        es[rn] = es[p];
        es[p] = x;

        while( true ) {
            j--;
            while( es[j]->Origin() > x->Origin() ||
                   (es[j]->Origin() == x->Origin() &&
                    es[j]->Twin()->Origin() > x->Twin()->Origin())
            ) {
                j--;
            }
            i++;
            while( es[i]->Origin() < x->Origin() ||
                   (es[i]->Origin() == x->Origin() &&
                    es[i]->Twin()->Origin() < x->Twin()->Origin())
            ) {
                i++;
            }
            if( i < j ) {
                te = es[i];
                es[i] = es[j];
                es[j] = te;
            } else {
                break;
            }
        }

        Quicksort( es, p, j );
        Quicksort( es, j+1, r );
    }
}


// All edges should be unmarked when this function is called.  When this
// function terminates, they will still be unmarked.
void SWIFT_Tri_Mesh::Compute_Geometry( )
{
    int i;

    // Compute edge lengths, directions, vertex-edge planes, and face planes
    for( i = 0; i < Num_Faces(); i++ ) {
        if( faces[i].Edge1().Unmarked() ) {
                // Compute direction and length of the edge and its twin
                faces[i].Edge1().Compute_Direction_Length_Twin();
                // Compute edge distance
                faces[i].Edge1().Set_Origin_On_Plane_Twin();
                faces[i].Edge1().Twin()->Mark();
        } else {
            faces[i].Edge1().Unmark();
        }
        if( faces[i].Edge2().Unmarked() ) {
                faces[i].Edge2().Compute_Direction_Length_Twin();
                faces[i].Edge2().Set_Origin_On_Plane_Twin();
                faces[i].Edge2().Twin()->Mark();
        } else {
            faces[i].Edge2().Unmark();
        }
        if( faces[i].Edge3().Unmarked() ) {
                faces[i].Edge3().Compute_Direction_Length_Twin();
                faces[i].Edge3().Set_Origin_On_Plane_Twin();
                faces[i].Edge3().Twin()->Mark();
        } else {
            faces[i].Edge3().Unmark();
        }
        // Compute face plane
        faces[i].Compute_Plane_From_Edges();
    }

    // Compute edge-face planes
    for( i = 0; i < Num_Faces(); i++ ) {
        // Compute edge-face planes
        faces[i].Edge1().Compute_Face_Plane();
        faces[i].Edge2().Compute_Face_Plane();
        faces[i].Edge3().Compute_Face_Plane();
    }
}

void SWIFT_Tri_Mesh::Translate_To( const SWIFT_Triple& t )
{   
    int i; 

    for( i = 0; i < Num_Vertices(); i++ ) {
        verts[i].Translate( t );
    }
    com += t;
    Compute_Geometry_After_Translate();
}       
    
// All edges should be unmarked when this function is called.  When this
// function terminates, they will still be unmarked.
void SWIFT_Tri_Mesh::Compute_Geometry_After_Translate( )
{
    int i;

    // Recompute face and edge distances
    for( i = 0; i < Num_Faces(); i++ ) {
        faces[i].Set_Point_On_Plane();
        faces[i].Edge1().Set_Origin_On_Plane();
        faces[i].Edge1().Set_Origin_On_Face_Plane();
        faces[i].Edge2().Set_Origin_On_Plane();
        faces[i].Edge2().Set_Origin_On_Face_Plane();
        faces[i].Edge3().Set_Origin_On_Plane(); 
        faces[i].Edge3().Set_Origin_On_Face_Plane();
    }
}

void SWIFT_Tri_Mesh::Compute_Center_Of_Mass( )
{
    int i;
    SWIFT_Real area;
    SWIFT_Real total_area;
    SWIFT_Triple areav;

    com.Set_Value( 0.0, 0.0, 0.0 );
    total_area = 0.0;
    for( i = 0; i < Num_Faces(); i++ ) {
        areav = (faces[i].Edge1().Origin()->Coords() -
                 faces[i].Edge2().Origin()->Coords()) %
                (faces[i].Edge1().Origin()->Coords() -
                 faces[i].Edge3().Origin()->Coords());
        area = 0.5 * areav.Length();
        total_area += area;
        com += area * (faces[i].Edge1().Origin()->Coords() +
                       faces[i].Edge2().Origin()->Coords() +
                       faces[i].Edge3().Origin()->Coords() );
    }

    com = com / (3.0 * total_area);
}

void SWIFT_Tri_Mesh::Compute_Radius( )
{
    int i;
    SWIFT_Real d;
    radius = 0.0;
    for( i = 0; i < Num_Vertices(); i++ ) {
        d = com.Dist_Sq( verts[i].Coords() );
        if( d > radius ) {
            radius = d;
        }
    }
    radius = sqrt( radius );
}

void SWIFT_Tri_Mesh::Compute_Volume( )
{
    int i;

    volume = 0.0;
    for( i = 0; i < Num_Faces(); i++ ) {
        volume += (faces[i].Edge1().Origin()->Coords() %
                   faces[i].Edge2().Origin()->Coords()) *
                   faces[i].Edge3().Origin()->Coords();
    }

    volume /= 6.0;
}

void SWIFT_Tri_Mesh::Process_Vertices(
            const SWIFT_Real* vs, int vn, int& vnn, const SWIFT_Real*& new_vs,
            SWIFT_Array<int>& vmap, SWIFT_Array<int>& vnewi, bool create_vids )
{
    int i, j;
    int initial_creation;
    SWIFT_Real xbuck_res;
    SWIFT_Real ybuck_res;
    SWIFT_Real zbuck_res;
    SWIFT_Triple minc, maxc;
    SWIFT_Array<SWIFT_Triple> vcoords;
    SWIFT_Array<int> buckets[21][21][21];
    const SWIFT_Real* vs_ptr;

    // Remove duplicate vertices

    // First copy all the coordinates to the vertices and compute the
    // bounding box
    vcoords.Create( vn ); 
    vmap.Create( vn );
    vnewi.Create( vn );
    minc = SWIFT_Triple( SWIFT_INFINITY, SWIFT_INFINITY, SWIFT_INFINITY );
    maxc = SWIFT_Triple( -SWIFT_INFINITY, -SWIFT_INFINITY, -SWIFT_INFINITY );
    for( i = 0, vs_ptr = vs; i < vn; i++, vs_ptr += 3 ) {
        vcoords[i] = SWIFT_Triple( vs_ptr[0], vs_ptr[1], vs_ptr[2] );
        if( vs_ptr[0] < minc.X() ) { minc.Set_X( vs_ptr[0] ); }
        if( vs_ptr[0] > maxc.X() ) { maxc.Set_X( vs_ptr[0] ); }
        if( vs_ptr[1] < minc.Y() ) { minc.Set_Y( vs_ptr[1] ); }
        if( vs_ptr[1] > maxc.Y() ) { maxc.Set_Y( vs_ptr[1] ); }
        if( vs_ptr[2] < minc.Z() ) { minc.Set_Z( vs_ptr[2] ); }
        if( vs_ptr[2] > maxc.Z() ) { maxc.Set_Z( vs_ptr[2] ); }
    }

    xbuck_res = (maxc.X() - minc.X()) / 20.0;
    ybuck_res = (maxc.Y() - minc.Y()) / 20.0;
    zbuck_res = (maxc.Z() - minc.Z()) / 20.0;

    initial_creation = vn / 8000 + 1;

    vnn = 0;
    no_dup_verts = true;
    for( i = 0; i < vn; i++ ) {
        int xbuck = (int)floor((vcoords[i].X() - minc.X()) / xbuck_res);
        int ybuck = (int)floor((vcoords[i].Y() - minc.Y()) / ybuck_res);
        int zbuck = (int)floor((vcoords[i].Z() - minc.Z()) / zbuck_res);
        SWIFT_Array<int>& buck_list = buckets[xbuck][ybuck][zbuck];

        if( buck_list.Length() == 0 ) {
            // First time accessing this bucket
            buck_list.Create( initial_creation );
            buck_list.Set_Length( 1 );
            buck_list.Set_Last( i );
            vmap[i] = i;
            vnewi[i] = vnn++;
        } else {
            // Search for the item in the list
            for( j = 0; j < buck_list.Length(); j++ ) {
                if( vcoords[i].X() == vcoords[ buck_list[j] ].X() &&
                    vcoords[i].Y() == vcoords[ buck_list[j] ].Y() &&
                    vcoords[i].Z() == vcoords[ buck_list[j] ].Z()
                ) {
                    break;
                }
            }

            if( j == buck_list.Length() ) {
                // Item not found
                if( buck_list.Length() == buck_list.Max_Length() ) {
                    buck_list.Grow_Double();
                }
                buck_list.Increment_Length();
                buck_list.Set_Last( i );
                vmap[i] = i;
                vnewi[i] = vnn++;
            } else {
                // Item is found at position j
                vmap[i] = buck_list[j];
                no_dup_verts = false;
            }
        }
    }

    // Create and fill in the new_vs and the orig_vids if duplicate vertices
    if( !no_dup_verts ) {
        SWIFT_Real* temp_vs = new SWIFT_Real[vnn*3];
        if( create_vids ) {
            int k;
            orig_vids.Create( vnn );
            for( i = 0, j = 0, k = 0; i < vn; i++ ) {
                if( i == vmap[i] ) {
                    temp_vs[j++] = vcoords[ vmap[i] ].X();
                    temp_vs[j++] = vcoords[ vmap[i] ].Y();
                    temp_vs[j++] = vcoords[ vmap[i] ].Z();
                    orig_vids[k++] = vmap[i];
                }
            }
        } else {
            for( i = 0, j = 0; i < vn; i++ ) {
                if( i == vmap[i] ) {
                    temp_vs[j++] = vcoords[ vmap[i] ].X();
                    temp_vs[j++] = vcoords[ vmap[i] ].Y();
                    temp_vs[j++] = vcoords[ vmap[i] ].Z();
                }
            }
        }
        new_vs = temp_vs;
    } else {
        new_vs = vs;
    }
}

void SWIFT_Tri_Mesh::Transform_Vertices( const SWIFT_Real* vs, int vn,
                                const SWIFT_Orientation& orient,
                                const SWIFT_Translation& trans, SWIFT_Real sc )
{
    int i, j;
    SWIFT_Triple T = SWIFT_Triple( trans[0], trans[1], trans[2] );
    SWIFT_Matrix33 R;

    R.Set_Value( orient );

    for( i = 0, j = 0; i < Num_Vertices(); i++, j += 3 ) {
        SWIFT_Triple coords( vs[j], vs[j+1], vs[j+2] );
        verts[i].Set_Coords( sc * (R * coords) + T );
    }
}

void SWIFT_Tri_Mesh::Transform_Vertices_And_COM( SWIFT_Tri_Mesh* msrc,
                                const SWIFT_Orientation& orient,
                                const SWIFT_Translation& trans, SWIFT_Real sc )
{
    int i;
    SWIFT_Triple T = SWIFT_Triple( trans[0], trans[1], trans[2] );
    SWIFT_Matrix33 R;

    R.Set_Value( orient );

    for( i = 0; i < Num_Vertices(); i++ ) {
        verts[i].Set_Coords( sc * (R * msrc->Vertices()[i].Coords()) + T );
    }

    // Transform the center of mass
    Set_Center_Of_Mass( sc * (R * msrc->Center_Of_Mass()) + T );
}

void SWIFT_Tri_Mesh::Process_Faces( const int* fs, int fn, const int* fv,
                                    int& tn, const int*& new_fs,
                                    SWIFT_Array<int>& triang_edges,
                                    bool create_fids )
{
    int i, j;
    int* temp_fs;

    only_tris = true;

    if( fv != NULL ) {
        tn = 0;
        for( i = 0; i < fn; i++ ) {
            if( fv[i] != 3 ) {
                only_tris = false;
            }
            tn += fv[i] - 2;
        }
    } else {
        tn = fn;
    }

    if( !only_tris ) {
        // Triangulate into the temp_fs array
        int b0, b1;
        int v, e, f, j_3;

        f = 0;
        e = 0;
        if( create_fids ) {
            orig_fids.Create( tn );
        }
        temp_fs = new int[tn*3];
        triang_edges.Create( tn - fn );
        for( i = 0, j = 0, j_3 = 0; i < fn; i++ ) {
            v = f + fv[i] - 1;  // Store the last index of this face
            b0 = f;
            b1 = f+1;

            while( true ) {
                if( create_fids ) {
                    orig_fids[j_3] = i;
                }

                // Create the left face
                temp_fs[j++] = fs[b0];
                temp_fs[j++] = fs[b1++];
                temp_fs[j++] = fs[b1];
                j_3++;

                // Test completion
                if( b1 == v ) {
                    break;
                }

                // Set an internal edge
                triang_edges[e++] = j_3-1;

                if( create_fids ) {
                    orig_fids[j_3] = i;
                }

                // Create the right face
                temp_fs[j++] = fs[v];
                temp_fs[j++] = fs[b0];
                temp_fs[j++] = fs[b1];
                j_3++;

                b0 = v;  // Advance b0
                v--;

                // Test completion
                if( b1 == v ) {
                    break;
                }

                // Set an internal edge
                triang_edges[e++] = j_3-1;
            }

            f += fv[i];
        }

        new_fs = temp_fs;
    } else {
        new_fs = fs;
    }
}


