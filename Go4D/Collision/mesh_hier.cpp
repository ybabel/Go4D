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
// mesh_hier.C
//
//////////////////////////////////////////////////////////////////////////////

#ifdef SWIFT_HIERARCHY
#include <math.h>

#include <SWIFT_config.h>
#include <SWIFT_common.h>
#include <SWIFT_linalg.h>
#include <SWIFT_array.h>
#include <SWIFT_mesh.h>
#include <SWIFT_fileio.h>

// Feature type identifiers
static const int SWIFT_VERTEX = 1;
static const int SWIFT_EDGE = 2;
static const int SWIFT_FACE = 3;

//////////////////////////////////////////////////////////////////////////////
// SWIFT_Tri_Mesh Hierarchy Specific functions
//////////////////////////////////////////////////////////////////////////////

int SWIFT_Tri_Mesh::Num_Levels( )
{
    SWIFT_Tri_Mesh* c;
    int i;

    for( i = 1, c = this; c->Parent() != NULL; i++, c = c->Parent() );

    return i;
}

SWIFT_Tri_Mesh* SWIFT_Tri_Mesh::Coarsest_Level( )
{   
    SWIFT_Tri_Mesh* stm = this;
 
    while( stm->Parent() != NULL ) {
        stm = stm->Parent();
    }
 
    return stm;
}

bool SWIFT_Tri_Mesh::Create( const SWIFT_Real* const* vs, const int* const* fs,
                 const int* vn, const int* fn, int ln,
#ifdef SWIFT_QSLIM_HIER
                 bool creating_qslim_hier, SWIFT_Array<int>* qtriang_edges,
#endif
                 const SWIFT_Orientation* orient,
                 const SWIFT_Translation* trans,
                 const SWIFT_Real* sc, const int* const* fv )
{
    int i, j, k;
    int e, f;
    int vnn, tn;
    const SWIFT_Real* new_vs;
    const int* new_fs;
    SWIFT_Tri_Vertex* tv;
    SWIFT_Tri_Edge* te;
    SWIFT_Array<SWIFT_Tri_Edge*> sort_edges;
    SWIFT_Array<SWIFT_Tri_Face> temp_faces;
    SWIFT_Array<SWIFT_Tri_Mesh*> levels;
    SWIFT_Array<int> triang_edges;
    SWIFT_Array<int> vmap;
    SWIFT_Array<int> vnewi;

    if( ln > MAX_LEVELS ) {
        cerr << "Warning: Too many levels given for piece -- using maximum "
             << "allowed: " << MAX_LEVELS << endl;
        ln = MAX_LEVELS;
    }

    levels.Create( ln );

    levels[0] = this;

    for( k = 0; k < ln; k++ ) {
        if( k != 0 ) {
            levels[k] = new SWIFT_Tri_Mesh;
        }

#ifdef SWIFT_QSLIM_HIER
        if( creating_qslim_hier ) {
            // There are known to not be any duplicate vertices and the faces
            // are all triangulated. None of the levels' verts or faces have
            // been created yet.
            const SWIFT_Orientation* orientation
                    = (orient == NULL ? &DEFAULT_ORIENTATION : &(orient[k]));
            const SWIFT_Translation* translation
                    = (trans == NULL ? &DEFAULT_TRANSLATION : &(trans[k]));

            // Create the vertices
            levels[k]->Vertices().Create( vn[k] );

            // Transform the vertices
            levels[k]->Transform_Vertices( vs[k], vn[k], *orientation,
                                    *translation, (sc == NULL ? 1.0 : sc[k]) );

            // Create the faces
            levels[k]->Faces().Create( fn[k] );
            new_fs = fs[k];
            tn = fn[k];

            // Set the vmap and vnewi arrays to be the identity mapping
            vmap.Create( vn[k] );
            vnewi.Create( vn[k] );
            for( i = 0; i < vn[k]; i++ ) {
                vmap[i] = i;
                vnewi[i] = i;
            }
        } else {
#endif
            const SWIFT_Orientation* orientation
                    = (orient == NULL ? &DEFAULT_ORIENTATION : &(orient[k]));
            const SWIFT_Translation* translation
                    = (trans == NULL ? &DEFAULT_TRANSLATION : &(trans[k]));

            // Remove duplicate vertices
            levels[k]->Process_Vertices( vs[k], vn[k], vnn, new_vs,
                                         vmap, vnewi, (k == 0) );
            // Create the vertices
            levels[k]->Vertices().Create( vnn );

            // Transform the vertices
            levels[k]->Transform_Vertices( new_vs, vnn, *orientation,
                                    *translation, (sc == NULL ? 1.0 : sc[k]) );

            // Delete the vertex array copy if there were duplicate vertices
            if( !levels[k]->No_Duplicate_Vertices() ) {
                delete new_vs;
            }

            // Triangulate faces.
            Process_Faces( fs[k], fn[k], (fv == NULL ? NULL : fv[k]),
                           tn, new_fs, triang_edges, (k == 0) );

            // Create the faces
            levels[k]->Faces().Create( tn );
#ifdef SWIFT_QSLIM_HIER
        }
#endif

        temp_faces.Create( tn<<1 );
        sort_edges.Create( tn*6 );

        // For each face, create one oriented as given and one oriented the
        // opposite direction
        for( e = 0, f = 0, i = 0; i < tn; i++ ) {
            // Create the face oriented as given
            tv = levels[k]->Vertices()( vnewi[ vmap[ new_fs[f++] ] ] );
            tv->Set_Adj_Edge( levels[k]->Faces()[i].Edge1P() );
            levels[k]->Faces()[i].Edge1().Set_Origin( tv );
            levels[k]->Faces()[i].Edge1().Set_Twin( temp_faces[i].Edge2P() );
            sort_edges[e++] = levels[k]->Faces()[i].Edge1P();

            tv = levels[k]->Vertices()( vnewi[ vmap[ new_fs[f++] ] ] );
            tv->Set_Adj_Edge( levels[k]->Faces()[i].Edge2P() );
            levels[k]->Faces()[i].Edge2().Set_Origin( tv );
            levels[k]->Faces()[i].Edge2().Set_Twin( temp_faces[i].Edge1P() );
            sort_edges[e++] = levels[k]->Faces()[i].Edge2P();

            tv = levels[k]->Vertices()( vnewi[ vmap[ new_fs[f++] ] ] );
            tv->Set_Adj_Edge( levels[k]->Faces()[i].Edge3P() );
            levels[k]->Faces()[i].Edge3().Set_Origin( tv );
            levels[k]->Faces()[i].Edge3().Set_Twin( temp_faces[i].Edge3P() );
            sort_edges[e++] = levels[k]->Faces()[i].Edge3P();

            // Create the reversed face
            temp_faces[i].Edge1().Set_Origin(
                                    levels[k]->Faces()[i].Edge3().Origin() );
            temp_faces[i].Edge1().Set_Twin( levels[k]->Faces()[i].Edge2P() );
            sort_edges[e++] = temp_faces[i].Edge1P();

            temp_faces[i].Edge2().Set_Origin(
                                    levels[k]->Faces()[i].Edge2().Origin() );
            temp_faces[i].Edge2().Set_Twin( levels[k]->Faces()[i].Edge1P() );
            sort_edges[e++] = temp_faces[i].Edge2P();

            temp_faces[i].Edge3().Set_Origin(
                                    levels[k]->Faces()[i].Edge1().Origin() );
            temp_faces[i].Edge3().Set_Twin( levels[k]->Faces()[i].Edge3P() );
            sort_edges[e++] = temp_faces[i].Edge3P();
        }

        // Don't need the new_fs array anymore
#ifdef SWIFT_QSLIM_HIER
        if( !creating_qslim_hier && !levels[k]->Only_Triangles() ) {
#else
        if( !levels[k]->Only_Triangles() ) {
#endif
            delete new_fs;
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
            if( j < tn &&
                sort_edges[i]->Origin() == sort_edges[j]->Origin() &&
                sort_edges[i]->Twin()->Origin() ==
                    sort_edges[j]->Twin()->Origin()
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
                         << "       all edges must belong to exactly two "
                         << "triangles" << endl;
                    for( j = 1; j < ln; j++ ) {
                        delete levels[j];
                    }
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
                for( j = 1; j < ln; j++ ) {
                    delete levels[j];
                }
                return false;
            }
        }

        levels[k]->Compute_Geometry();
        levels[k]->Compute_Center_Of_Mass();
        levels[k]->Compute_Radius();
        levels[k]->Compute_Volume();

        // If there are new edges resulting from face triangulation, then fix
        // the edge-face planes on them to be twin-identical
#ifdef SWIFT_QSLIM_HIER
        if( creating_qslim_hier && k == 0 && qtriang_edges != NULL ) {
            for( i = 0; i < qtriang_edges->Length(); i++ ) {
                levels[k]->Faces()[(*qtriang_edges)[i]].Edge3().Twin()->
                        Set_Face_Distance( -levels[k]->Faces()[
                                (*qtriang_edges)[i]].Edge3().Face_Distance() );
                levels[k]->Faces()[(*qtriang_edges)[i]].Edge3().Twin()->
                        Set_Face_Normal( -levels[k]->Faces()[
                                (*qtriang_edges)[i]].Edge3().Face_Normal() );
            }
        } else {
#endif
            for( i = 0; i < tn/6 - fn[k]; i++ ) {
                levels[k]->Faces()[triang_edges[i]].Edge3().Twin()->
                        Set_Face_Distance( -levels[k]->Faces()[
                                triang_edges[i]].Edge3().Face_Distance() );
                levels[k]->Faces()[triang_edges[i]].Edge3().Twin()->
                        Set_Face_Normal( -levels[k]->Faces()[
                                triang_edges[i]].Edge3().Face_Normal() );
            }
#ifdef SWIFT_QSLIM_HIER
        }
#endif

        temp_faces.Destroy();
        sort_edges.Destroy();
        triang_edges.Destroy();
        vmap.Destroy();
        vnewi.Destroy();

#ifdef SWIFT_DEBUG
        cerr << "******* SWIFT_Tri_Mesh::Create()'2 Verification " << k
             << " *******" << endl;
        levels[k]->Verify();
#endif
    }

    deviation = 0.0;

    Create_Bounding_Volume_Hierarchy( levels );

    levels[ln-1]->Create_Lookup_Table();

    return true;
}

#ifndef SWIFT_QSLIM_HIER
void SWIFT_Tri_Mesh::Quicksort( SWIFT_Array<SWIFT_Tri_Vertex*>& vs,
                                SWIFT_Array<SWIFT_Real>& keys, int p, int r )
{
    if( p < r ) {
        // Compute a random element to use as the pivot
        int rn = (int) ((SWIFT_Real)(r-p+1) * drand48()) + p;
        int i = p-1;
        int j = r+1;
        SWIFT_Real x = keys[rn];
        SWIFT_Tri_Vertex* tv = vs[rn];
        SWIFT_Real tr;

        // Swap the random element into the first position
        keys[rn] = keys[p];
        keys[p] = x;
        vs[rn] = vs[p];
        vs[p] = tv;

        while( true ) {
            j--;
            while( keys[j] > x ) {
                j--;
            }
            i++;
            while( keys[i] < x ) {
                i++;
            }
            if( i < j ) {
                tr = keys[i];
                keys[i] = keys[j];
                keys[j] = tr;
                tv = vs[i];
                vs[i] = vs[j];
                vs[j] = tv;
            } else {
                break;
            }
        }

        Quicksort( vs, keys, p, j );
        Quicksort( vs, keys, j+1, r );
    }
}

void SWIFT_Tri_Mesh::Build_Heap( SWIFT_Array<SWIFT_Tri_Vertex*>& vs,
                                 SWIFT_Array<int>& indices,
                                 SWIFT_Array<SWIFT_Real>& keys )
{
    int i;

    for( i = ((indices.Length())>>1)-1; i >= 0; i-- ) {
        Heapify( vs, indices, keys, i );
    }
}

void SWIFT_Tri_Mesh::Heapify( SWIFT_Array<SWIFT_Tri_Vertex*>& vs,
                              SWIFT_Array<int>& indices,
                              SWIFT_Array<SWIFT_Real>& keys, int i )
{
    int l = (i<<1)+1;
    int r = l+1;
    int largest = (l < indices.Length() && keys[l] > keys[i]) ? l : i;

    if( r < indices.Length() && keys[r] > keys[largest] ) {
        largest = r;
    }

    while( largest != i ) {
        SWIFT_Real tempk = keys[i];
        SWIFT_Tri_Vertex* tempv = vs[i];

        keys[i] = keys[largest];
        keys[largest] = tempk;

        vs[i] = vs[largest];
        vs[largest] = tempv;

        // Cause the indices to be updated to be able to index to the swapped
        // vertices.
        indices[ Vertex_Id(vs[i]) ] = i;
        indices[ Vertex_Id(vs[largest]) ] = largest;

        i = largest;
        l = (i<<1)+1;
        r = l+1;
        largest = (l < indices.Length() && keys[l] > keys[i]) ? l : i;

        if( r < indices.Length() && keys[r] > keys[largest] ) {
            largest = r;
        }
    }
}

void SWIFT_Tri_Mesh::Up_Heap( SWIFT_Array<SWIFT_Tri_Vertex*>& vs,
                              SWIFT_Array<int>& indices,
                              SWIFT_Array<SWIFT_Real>& keys, int i )
{
    int parent = (i-1)>>1;

    while( i > 0 && keys[parent] < keys[i] ) {
        SWIFT_Real tempk = keys[i];
        SWIFT_Tri_Vertex* tempv = vs[i];

        keys[i] = keys[parent];
        keys[parent] = tempk;

        vs[i] = vs[parent];
        vs[parent] = tempv;

        // Cause the indices to be updated to be able to index to the swapped
        // vertices.
        indices[ Vertex_Id(vs[i]) ] = i;
        indices[ Vertex_Id(vs[parent]) ] = parent;

        i = parent;
        parent = (i-1)>>1;
    }
}

int SWIFT_Tri_Mesh::Create_Bounding_Volume_Hierarchy( int tcount )
{
    int i, j;
    int fnf;
    SWIFT_Real scale;
    SWIFT_Tri_Mesh* c;
    SWIFT_Tri_Mesh* p;
    SWIFT_Array<SWIFT_Tri_Vertex*> indep_verts;
    SWIFT_Array<SWIFT_Tri_Vertex**> fvs;
    SWIFT_Array<int> fvs_len;
    SWIFT_Array<SWIFT_Tri_Vertex**> cfvs;
    SWIFT_Array<int> cfvs_len;

    // Initialize the finest object
    child = NULL;

    // Create lists
    indep_verts.Create( Num_Vertices() );

    cfvs_len.Create( Num_Faces() );
    for( i = 0; i < Num_Faces(); i++ ) {
        faces[i].Set_Child( NULL );
        faces[i].Edge1().Set_Child( NULL );
        faces[i].Edge2().Set_Child( NULL );
        faces[i].Edge3().Set_Child( NULL );
        cfvs_len[i] = 0;
    }

    c = this;
    j = 1;
    while( true ) {
        if( j == MAX_LEVELS ) {
#ifdef SWIFT_DEBUG
            cerr << "MAX_LEVELS failed" << endl;
#endif
            break;
        }

        // Create the independent set and make sure that we can create level
        c->Create_Independent_Set( indep_verts );
        if( indep_verts.Length() == 0 ) {
#ifdef SWIFT_DEBUG
            cerr << "Independent set creation failed" << endl;
#endif
            break;
        }

        // Make sure that the new mesh will satisfy the triangle count
        // criterion.
        if( tcount != 0 &&
            c->Num_Faces() - 2 * indep_verts.Length() < tcount
        ) {
            // The new mesh will violate the triangle criteria.  Exit the loop.
#ifdef SWIFT_DEBUG
            cerr << "Triangle constraint failed" << endl;
#endif
            break;
        }

        // Create the parent mesh.
        p = new SWIFT_Tri_Mesh;

        // Copy the features from the child to the parent and take out the
        // independent set of vertices and their adjacent edges and faces.
        p->Create_And_Copy_With_Holes( indep_verts, fnf, *c );

        // Fill in the holes in the parent cause by removing the vertices from
        // the child.
        p->Fill_Holes( indep_verts, fnf, *c );

        // At this point, all the faces have the correct normal and distance,
        // edges have the correct direction and length, vertices are correct.

        // Set the center of mass of the parent to be the same as that of the
        // finest mesh.
        p->Set_Center_Of_Mass( com );

        // Compute the scaling factor required to scale the new mesh to bound
        // the finest mesh.
        scale = p->Compute_Pierced_Faces_And_Scaling_Factor(
                        indep_verts, fnf, *c, fvs, fvs_len, cfvs, cfvs_len );

        // Make sure that the center of mass is inside the parent otherwise the
        // scaling will fail.  Delete the newly created mesh if necessary.
        for( i = 0; i < p->Num_Faces(); i++ ) {
            if( p->Faces()[i].Distance( com ) > -EPSILON10 ) {
                // Center of mass is not well contained
#ifdef SWIFT_DEBUG
                cerr << "Center of mass containment failed" << endl;
#endif
                i = -1;
                break;
            }
        }
        if( i == -1 ) {
            delete p;
            break;
        }

        // Scale the mesh.
        p->Scale_From_Center_Of_Mass( scale );

        p->Compute_Geometry_After_Scale( scale );

        // All the vertices have moved in a scaled fashion so all the face
        // plane normals are still correct as are the edge directions.

        // Now relax the mesh so that it shrinks but maintains the convexity
        // and bounding properties and the center of mass regions.
        p->Relax( scale, fvs, fvs_len );

        // Relaxation is complete and all edge directions are correct and some
        // faces are also marked.  Face planes, edge-face planes, and edge
        // lengths and distances are incorrect.

        // Compute the geometry, radius, volume, and deviation
        p->Compute_Geometry();
        p->Compute_Radius();
        p->Compute_Volume();

        // Create the startvs array.  Can use the indep_verts array.
        indep_verts.Set_Length( indep_verts.Max_Length() );
        for( i = 0; i < p->Num_Vertices(); i++ ) {
            indep_verts[i] = p->Vertices()(i);
            while( indep_verts[i]->Child() != NULL ) {
                indep_verts[i] = indep_verts[i]->Child();
            }
        }

        // Have to assign the child to p before we can compute its deviation
        p->Set_Child( c );
        c->Set_Parent( p );
        c->Assign_Parents();
        c = p;
        p->Compute_Deviation( indep_verts );

#ifdef SWIFT_DEBUG
        cerr << "******* SWIFT_Tri_Mesh::Create_Bounding_Volume_Hierarchy()'1"
             << " Verification " << j << " *******" << endl;
        p->Verify( true );
#endif
        j++;

        // Destroy and swap the fvs lists
        for( i = 0; i < cfvs.Length(); i++ ) {
            delete cfvs[i];
        }
        cfvs.Destroy();
        cfvs_len.Destroy();
        cfvs = fvs;
        cfvs_len = fvs_len;
        fvs.Nullify();
        fvs_len.Nullify();
    }

    // Destroy the last fvs lists
    for( i = 0; i < cfvs.Length(); i++ ) {
        delete cfvs[i];
    }
    cfvs.Destroy();
    cfvs_len.Destroy();

#ifdef SWIFT_DEBUG
    cerr << "There are " << j << " levels in the BVH" << endl << endl;
#endif

    // Set all parents of the coarsest mesh NULL
    for( i = 0; i < c->Num_Vertices(); i++ ) {
        c->Vertices()[i].Set_Parent( NULL );
    }
    for( i = 0; i < c->Num_Faces(); i++ ) {
        c->Faces()[i].Set_Parent( NULL );
        c->Faces()[i].Edge1().Set_Parent( NULL );
        c->Faces()[i].Edge2().Set_Parent( NULL );
        c->Faces()[i].Edge3().Set_Parent( NULL );
    }

    return j;
}

void SWIFT_Tri_Mesh::Create_Independent_Set(
                                SWIFT_Array<SWIFT_Tri_Vertex*>& indep_verts )
{
    int i, j, len;
    SWIFT_Tri_Edge* e;
    SWIFT_Array<SWIFT_Tri_Vertex*> sort_verts;
    SWIFT_Array<bool> mark_verts;
    SWIFT_Array<SWIFT_Real> edge_convexities;
    SWIFT_Array<SWIFT_Real> importances;

    sort_verts.Create( Num_Vertices() );
    mark_verts.Create( Num_Vertices() );
    edge_convexities.Create( Num_Faces()*3 );
    importances.Create( Num_Vertices() );
    indep_verts.Set_Length( Num_Vertices() );

    // Compute the convexity of the edges
    for( i = 0, j = 0; i < Num_Faces(); i++ ) {
        edge_convexities[j++] = 1.0 - (faces[i].Normal() *
                                faces[i].Edge1().Twin()->Adj_Face()->Normal());
        edge_convexities[j++] = 1.0 - (faces[i].Normal() *
                                faces[i].Edge2().Twin()->Adj_Face()->Normal());
        edge_convexities[j++] = 1.0 - (faces[i].Normal() *
                                faces[i].Edge3().Twin()->Adj_Face()->Normal());
    }

    // Compute the importance of the vertices which is the sum of the
    // convexities of the neighboring edges.
    for( i = 0; i < Num_Vertices(); i++ ) {
        mark_verts[i] = true;
        sort_verts[i] = verts(i);
        importances[i] = edge_convexities[ Edge_Id( verts[i].Adj_Edge() ) ];
        for( e = verts[i].Adj_Edge()->Twin()->Next(); e != verts[i].Adj_Edge();
             e = e->Twin()->Next()
        ) {
            importances[i] += edge_convexities[ Edge_Id( e ) ];
        }
    }

    Quicksort( sort_verts, importances, 0, Num_Vertices()-1 );

    // Insert vertices into the independent set marking them and all their
    // neighbors in the process
    len = 0;
    for( i = 0; i < Num_Vertices(); i++ ) {
        if( mark_verts[ Vertex_Id( sort_verts[i] ) ] ) {
            if( Num_Vertices() - len == 4 ) {
                // Stop here because we don't want something with fewer
                // vertices than a tetrahedron
                break;
            } else {
                // Make sure that the importances are small so that we avoid
                // removing vertices that are at the top of sharp peaks.
                if( importances[i] < 1.0 ) {
                    // Go ahead... this importance is ok.  Store it and mark
                    // all the neighboring vertices as used.
                    indep_verts[len++] = sort_verts[i];
                    
                    e = sort_verts[i]->Adj_Edge()->Twin();
                    mark_verts[ Vertex_Id( e->Origin() ) ] = false;
                    for( e = e->Next()->Twin();
                         e != sort_verts[i]->Adj_Edge()->Twin();
                         e = e->Next()->Twin()
                    ) {
                        mark_verts[ Vertex_Id( e->Origin() ) ] = false;
                    }
                    sort_verts[i]->Adj_Edge()->Mark();
                } else {
                    // Done creating independent set.
                    break;
                }
            }
        }
    }

    indep_verts.Set_Length( len );
}

// Before this routine is called, the faces, edges, and vertices of the
// child should be unmarked.
//
// After this routine is complete, the faces, edges, and vertices of the
// child are unmarked.  The faces, edges, and vertices of this mesh are
// unmarked.
void SWIFT_Tri_Mesh::Create_And_Copy_With_Holes(
                                SWIFT_Array<SWIFT_Tri_Vertex*>& indep_verts,
                                int& first_new_face, SWIFT_Tri_Mesh& child )
{
    int i, len;

    verts.Create( child.Num_Vertices() - indep_verts.Length() );
    faces.Create( child.Num_Faces() - 2 * indep_verts.Length() );

    // Copy all the unmarked vertices which are kept and mark the edges and the
    // faces that are not kept.
    len = 0;
    for( i = 0; i < child.Num_Vertices(); i++ ) {
        if( child.Vertices()[i].Adj_Edge()->Unmarked() ) {
            // Vertex is kept
            child.Vertices()[i].Set_Parent( verts(len) );

            verts[len].Set_Coords( child.Vertices()[i].Coords() );
            verts[len].Set_Child( child.Vertices()(i) );
            verts[len].Set_Parent( child.Vertices()(i) );
            len++;
        } else {
            // Vertex is not kept.  This parent pointer is fixed up
            // later on when the holes are filled.
            SWIFT_Tri_Edge* e = child.Vertices()[i].Adj_Edge();

            e->Adj_Face()->Mark();
            for( e = e->Twin()->Next(); e != child.Vertices()[i].Adj_Edge();
                 e = e->Twin()->Next()
            ) {
                e->Adj_Face()->Mark();
            }
            child.Vertices()[i].Set_Parent( NULL );
            child.Vertices()[i].Adj_Edge()->Unmark();
        }
    }

#ifdef SWIFT_DEBUG
if( Num_Vertices() != len ) {
    cerr << "************** Num_Vertices() != len ****************" << endl;
    cerr << "Num_Vertices() = " << Num_Vertices() << endl;
    cerr << "len = " << len << endl;
}
#endif

    // Copy the marked faces which are kept.
    len = 0;
    for( i = 0; i < child.Num_Faces(); i++ ) {
        if( child.Faces()[i].Unmarked() ) {
            // Face is kept
            child.Faces()[i].Set_Parent( faces(len) );
            child.Faces()[i].Edge1().Set_Parent( faces[len].Edge1P() );
            child.Faces()[i].Edge2().Set_Parent( faces[len].Edge2P() );
            child.Faces()[i].Edge3().Set_Parent( faces[len].Edge3P() );

            faces[len].Set_Normal_N( child.Faces()[i].Normal() );
            faces[len].Set_Distance( child.Faces()[i].Distance() );
            faces[len].Set_Parent( child.Faces()(i) );
            faces[len].Set_Child( child.Faces()(i) );

            faces[len].Edge1().Set_Direction_N(
                                        child.Faces()[i].Edge1().Direction() );
            faces[len].Edge1().Set_Length( child.Faces()[i].Edge1().Length() );
            faces[len].Edge1().Set_Parent( child.Faces()[i].Edge1P() );
            faces[len].Edge1().Set_Child( child.Faces()[i].Edge1P() );

            faces[len].Edge2().Set_Direction_N(
                                        child.Faces()[i].Edge2().Direction() );
            faces[len].Edge2().Set_Length( child.Faces()[i].Edge2().Length() );
            faces[len].Edge2().Set_Parent( child.Faces()[i].Edge2P() );
            faces[len].Edge2().Set_Child( child.Faces()[i].Edge2P() );

            faces[len].Edge3().Set_Direction_N(
                                        child.Faces()[i].Edge3().Direction() );
            faces[len].Edge3().Set_Length( child.Faces()[i].Edge3().Length() );
            faces[len].Edge3().Set_Parent( child.Faces()[i].Edge3P() );
            faces[len].Edge3().Set_Child( child.Faces()[i].Edge3P() );

            len++;
        } else {
            // Face is not kept.  These parent pointers are fixed up
            // later on when the holes are filled.
            child.Faces()[i].Set_Parent( NULL );
            child.Faces()[i].Edge1().Set_Parent( NULL );
            child.Faces()[i].Edge2().Set_Parent( NULL );
            child.Faces()[i].Edge3().Set_Parent( NULL );
            child.Faces()[i].Unmark();
        }
    }
    faces.Set_Length( len );
    first_new_face = len;

    // Link features correctly at the new level.  Some of the pointers may be
    // NULL but they will be subsequently fixed.
    for( i = 0; i < Num_Faces(); i++ ) {
        faces[i].Edge1().Set_Origin(
                                faces[i].Parent()->Edge1().Origin()->Parent() );
        faces[i].Edge1().Set_Twin(
                                faces[i].Parent()->Edge1().Twin()->Parent() );
        faces[i].Edge1().Origin()->Set_Adj_Edge( faces[i].Edge1P() );

        faces[i].Edge2().Set_Origin(
                                faces[i].Parent()->Edge2().Origin()->Parent() );
        faces[i].Edge2().Set_Twin(
                                faces[i].Parent()->Edge2().Twin()->Parent() );
        faces[i].Edge2().Origin()->Set_Adj_Edge( faces[i].Edge2P() );

        faces[i].Edge3().Set_Origin(
                                faces[i].Parent()->Edge3().Origin()->Parent() );
        faces[i].Edge3().Set_Twin(
                                faces[i].Parent()->Edge3().Twin()->Parent() );
        faces[i].Edge3().Origin()->Set_Adj_Edge( faces[i].Edge3P() );
    }
}

// Before this routine is called, the faces, edges, and vertices of the
// child as well as this mesh should be unmarked.

// After this routine is complete, the faces, edges, and vertices of the
// child and this mesh are unmarked.

// This routine still works if some of the edges are exactly lined up such that
// the face normal generated by them is NAN.  If they were almost lined up, then
// the fp error may flip the normal causing failure.  There is no way to really
// solve this short of doing exact rational computation.
void SWIFT_Tri_Mesh::Fill_Holes( SWIFT_Array<SWIFT_Tri_Vertex*>& indep_verts,
                                 int first_new_face, SWIFT_Tri_Mesh& child )
{
    int i, j, k;
    int flen;
    int facei;
    SWIFT_Tri_Vertex* prevm;
    SWIFT_Tri_Vertex* nextm;
    SWIFT_Tri_Edge* laste;
    SWIFT_Tri_Edge* e;
    SWIFT_Tri_Edge* e1;
    SWIFT_Tri_Edge* e2;
    SWIFT_Tri_Edge* e3;
    SWIFT_Array<SWIFT_Tri_Face> change_faces;

    // Increase the lengths of the face list
    flen = first_new_face;
    faces.Set_Length( faces.Max_Length() );

    // Run through the independent set backwards so that if there are any
    // valence 2 vertices, then they will get handled last.
    for( i = indep_verts.Length()-1; i >= 0; i-- ) {

        // Make all vertices at the child level around the hole point to edges
        // on the boundary of the hole at the child level.  At the same time
        // figure out what this vertex's child should be.  Use the radius
        // and deviation variables temporarily.
        e = indep_verts[i]->Adj_Edge();
        e->Next()->Origin()->Set_Adj_Edge( e->Next() );
        nextm = e->Next()->Origin()->Parent();
        radius = indep_verts[i]->Coords().Dist_Sq(
                                            e->Next()->Origin()->Coords() );
        for( e = e->Twin()->Next();
             e != indep_verts[i]->Adj_Edge();
             e = e->Twin()->Next()
        ) {
            e->Next()->Origin()->Set_Adj_Edge( e->Next() );
            deviation = indep_verts[i]->Coords().Dist_Sq(
                                            e->Next()->Origin()->Coords() );
            if( deviation < radius ) {
                radius = deviation;
                nextm = e->Next()->Origin()->Parent();
            }
        }

        // Set the parent of the removed vertex
        indep_verts[i]->Set_Parent( nextm );

        // Create the initial triangle
        e = indep_verts[i]->Adj_Edge()->Twin();

        if( indep_verts[i]->Valence() == 2 ) {
            // We need to join the faces on each side of this hole.  We go
            // sideways in each direction until there are no more degenerate
            // faces (not that it is at all likely that the neighbors of this
            // hole are degenerate).

            e1 = e->Origin()->Adj_Edge()->Twin();
            while( e1->Adj_Face()->Parent() == NULL ) {
                e1 = e1->Next()->Twin()->Next()->Twin();
            }

            e2 = e->Prev()->Origin()->Adj_Edge()->Twin();
            while( e2->Adj_Face()->Parent() == NULL ) {
                e2 = e2->Next()->Twin()->Next()->Twin();
            }

            e1 = e1->Adj_Face()->Parent()->EdgeP(
                                            e->Prev()->Origin()->Parent() );
            e2 = e2->Adj_Face()->Parent()->EdgeP( e->Origin()->Parent() );
            e1->Set_Twin( e2 );
            e2->Set_Twin( e1 );
            e2->Set_Direction_N( -e1->Direction() );
            e2->Set_Length( e1->Length() );
            continue;
        } else if( indep_verts[i]->Valence() == 3 ) {
            // There is only one triangle needed
            e->Adj_Face()->Set_Parent( faces(flen) );
            e->Set_Parent( faces[flen].Edge1P() );
            e->Twin()->Set_Parent( faces[flen].Edge1P() );
            if( e->Prev()->Twin()->Adj_Face()->Parent() != NULL ) {
                // Twin for edge 3 exists
                e1 = e->Prev()->Twin()->Adj_Face()->Parent()->EdgeP(
                                                    e->Origin()->Parent() );
                faces[flen].Edge3().Set_Twin( e1 );
                e1->Set_Twin( faces[flen].Edge3P() );
            }
            e1 = e->Origin()->Adj_Edge();
            e1->Set_Parent( faces[flen].Edge1P() );
            faces[flen].Edge1().Set_Child( e1 );
            faces[flen].Edge1().Set_Direction_N( e1->Direction() );
            faces[flen].Edge1().Set_Length( e1->Length() );
            faces[flen].Edge1().Set_Origin( e->Origin()->Parent() );

            e = e->Next()->Twin();

            e->Adj_Face()->Set_Parent( faces(flen) );
            e->Set_Parent( faces[flen].Edge1P() );
            e->Twin()->Set_Parent( faces[flen].Edge1P() );
            if( e->Prev()->Twin()->Adj_Face()->Parent() != NULL ) {
                // Twin for edge 2 exists
                e1 = e->Prev()->Twin()->Adj_Face()->Parent()->EdgeP(
                                                    e->Origin()->Parent() );
                faces[flen].Edge2().Set_Twin( e1 );
                e1->Set_Twin( faces[flen].Edge2P() );
            }
            e1 = e->Origin()->Adj_Edge();
            e1->Set_Parent( faces[flen].Edge3P() );
            faces[flen].Edge3().Set_Child( e1 );
            faces[flen].Edge3().Set_Direction_N( e1->Direction() );
            faces[flen].Edge3().Set_Length( e1->Length() );
            faces[flen].Edge3().Set_Origin( e->Origin()->Parent() );

            e = e->Next()->Twin();

            e->Adj_Face()->Set_Parent( faces(flen) );
            e->Set_Parent( faces[flen].Edge1P() );
            e->Twin()->Set_Parent( faces[flen].Edge1P() );
            if( e->Prev()->Twin()->Adj_Face()->Parent() != NULL ) {
                // Twin for edge 1 exists
                e1 = e->Prev()->Twin()->Adj_Face()->Parent()->EdgeP(
                                                    e->Origin()->Parent() );
                faces[flen].Edge1().Set_Twin( e1 );
                e1->Set_Twin( faces[flen].Edge1P() );
            }
            e1 = e->Origin()->Adj_Edge();
            e1->Set_Parent( faces[flen].Edge2P() );
            faces[flen].Edge2().Set_Child( e1 );
            faces[flen].Edge2().Set_Direction_N( e1->Direction() );
            faces[flen].Edge2().Set_Length( e1->Length() );
            faces[flen].Edge2().Set_Origin( e->Origin()->Parent() );
            faces[flen].Compute_Plane_From_Edges();

            // Set the parent to be the removed vertex
            faces[flen].Set_Vertex_Child( indep_verts[i] );
            flen++;
            continue;
        }

        // More than 1 triangle is required.  Create the first one.
        facei = flen;

        e->Adj_Face()->Set_Parent( faces(facei) );
        faces[facei].Edge1().Set_Origin( e->Origin()->Parent() );

        e = e->Next()->Twin();

        e->Adj_Face()->Set_Parent( faces(facei) );
        faces[facei].Edge3().Set_Origin( e->Origin()->Parent() );
        faces[facei].Edge3().Set_Direction_N(
                                        e->Origin()->Adj_Edge()->Direction() );

        e = e->Next()->Twin();

        faces[facei].Edge2().Set_Origin( e->Origin()->Parent() );
        faces[facei].Edge2().Set_Direction_N(
                                        e->Origin()->Adj_Edge()->Direction() );

        faces[facei].Compute_Plane_From_Edges();

        laste = faces[facei].Edge1P();
        e = e->Next()->Twin();
        facei++;

        // Add the vertices around the hole fixing edges and faces as we go
        for( ; e != indep_verts[i]->Adj_Edge()->Twin(); e = e->Next()->Twin()
        ) {

            // Compare the new vertex being added to determine the side of
            // the faces that it falls on.  If it falls above a face, the face
            // is marked.  Mark all the vertices and edges that belong to the
            // face.
            k = 0;
            for( j = flen; j < facei; j++ ) {
                if( faces[j].Outside_Tol( e->Origin(), EPSILON12 ) ) {
                    faces[j].Mark();
                    k++;
                }
            }

            if( k == 0 ) {
                faces[facei].Edge1().Set_Origin( laste->Origin() );
                faces[facei].Edge2().Set_Origin( e->Origin()->Parent() );
                faces[facei].Edge3().Set_Origin( laste->Next()->Origin() );
                faces[facei].Edge3().Set_Twin( laste );
                laste->Set_Twin( faces[facei].Edge3P() );

                // Partially compute some edge directions
                faces[facei].Edge2().Set_Direction_N(
                                        e->Origin()->Adj_Edge()->Direction() );
                faces[facei].Edge3().Compute_Direction_U();
                laste->Set_Direction_N( -(faces[facei].Edge3().Direction()) );
                faces[facei].Compute_Plane_From_Edges();

                e->Origin()->Adj_Edge()->Adj_Face()->Set_Parent( faces(facei) );

                laste = faces[facei].Edge1P();
                facei++;
            } else {
                int kp;

                nextm = laste->Origin();
                e1 = laste;
                e3 = NULL;

                change_faces.Create( k+1 );
                faces[facei].Mark();
                facei++;

                j = 0;
                k = flen;
                while( true ) {
                    // Find the next face that will eventually be overwritten
                    // We set some of the fields here
                    for( ; k < facei && faces[k].Unmarked(); k++ );

                    // Pivot e1 about its tail CCW until it is not part of a
                    // marked face or until we reach the edge of the hole.
                    for( e2 = e1->Prev()->Twin();
                         e2 != NULL && e2->Adj_Face()->Marked();
                         e1 = e2, e2 = e1->Prev()->Twin() );

                    // Create the new triangle
                    prevm = nextm;
                    nextm = e1->Prev()->Origin();

                    change_faces[j].Edge1().Set_Origin( prevm );
                    if( e3 != NULL ) {
                        change_faces[j].Edge1().Set_Twin( e3 );
                        e3->Set_Twin( change_faces[j].Edge1P() );
                        faces[k].Edge1().Set_Direction_N(
                                    e->Origin()->Coords()-prevm->Coords() );
                        faces[kp].Edge2().Set_Direction_N(
                                            -(faces[k].Edge1().Direction()) );
                    }

                    change_faces[j].Edge2().Set_Origin( e->Origin()->Parent() );

                    change_faces[j].Edge3().Set_Origin( nextm );
                    if( e2 == NULL ) {
                        e1->Prev()->Origin()->Parent()->Adj_Edge()->
                                            Adj_Face()->Set_Parent( faces(k) );
                        faces[k].Edge3().Set_Direction_N( e1->Prev()->
                                Origin()->Child()->Adj_Edge()->Direction() );
                    } else {
                        // Set the special signal to indicate that the change
                        // faces twin pointer should be taken as is.
                        change_faces[j].Edge3().Set_Adj_Face( NULL );
                        change_faces[j].Edge3().Set_Twin( e2 );
                        e2->Set_Twin( faces[k].Edge3P() );
                        faces[k].Edge3().Set_Direction_N( -(e2->Direction()) );
                    }

                    if( nextm != laste->Next()->Origin() ) {
                        // Set the starting point for the next edge sweep to
                        // find the next horizon vertex
                        e1 = e1->Prev();
                        e3 = change_faces[j].Edge2P();
                        j++;
                        kp = k;
                        k++;
                    } else {
                        break;
                    }
                }

                e->Origin()->Adj_Edge()->Adj_Face()->Set_Parent( faces(k) );
                faces[k].Edge2().Set_Direction_N(
                                        e->Origin()->Adj_Edge()->Direction() );

                // Copy the twins and origins of the edges of the new faces
                // over corresponding pointers of the faces that got deleted.
                // Reuse the next pointers of the change_faces to point to the
                // real edges in faces.
                j = 0;
                for( k = flen; k < facei; k++ ) {
                    if( faces[k].Marked() ) {
                        change_faces[j].Edge1().Set_Next( faces[k].Edge1P() );
                        change_faces[j].Edge2().Set_Next( faces[k].Edge2P() );
                        change_faces[j].Edge3().Set_Next( faces[k].Edge3P() );
                        j++;
                    }
                }

                j = 0;
                for( k = flen; k < facei; k++ ) {
                    if( faces[k].Marked() ) {
                        // Unmark the face
                        faces[k].Unmark();

                        if( change_faces[j].Edge1().Twin() == NULL ) {
                            faces[k].Edge1().Set_Twin( NULL );
                        } else {
                            faces[k].Edge1().Set_Twin(
                                    change_faces[j].Edge1().Twin()->Next() );
                        }
                        faces[k].Edge1().Set_Origin(
                                            change_faces[j].Edge1().Origin() );

                        if( change_faces[j].Edge2().Twin() == NULL ) {
                            faces[k].Edge2().Set_Twin( NULL );
                        } else {
                            faces[k].Edge2().Set_Twin(
                                    change_faces[j].Edge2().Twin()->Next() );
                        }
                        faces[k].Edge2().Set_Origin(
                                            change_faces[j].Edge2().Origin() );

                        // This is the special signal to indicate to take
                        // the change_faces twin pointer as is.  This special
                        // signal can only happen for edge 3.
                        if( change_faces[j].Edge3().Adj_Face() == NULL ) {
                            faces[k].Edge3().Set_Twin(
                                            change_faces[j].Edge3().Twin() );
                        } else if( change_faces[j].Edge3().Twin() == NULL ) {
                            faces[k].Edge3().Set_Twin( NULL );
                        } else {
                            faces[k].Edge3().Set_Twin(
                                    change_faces[j].Edge3().Twin()->Next() );
                        }
                        faces[k].Edge3().Set_Origin(
                                            change_faces[j].Edge3().Origin() );

                        faces[k].Compute_Plane_From_Edges();

                        j++;
                    }
                }

                laste = change_faces[0].Edge1().Next();

                change_faces.Destroy();

#ifdef SWIFT_DEBUG
                // Find the next marked face
                for( ; k < facei; k++ ) {
                    if( faces[k].Marked() ) {
                        cerr << "********* There are still marked faces *******"
                             << endl;
                    }
                }
#endif
            }
        }

        // Set the things related to the very last edge of the hole
        laste->Set_Direction_N( e->Origin()->Adj_Edge()->Direction() );
        e->Origin()->Adj_Edge()->Adj_Face()->Set_Parent( laste->Adj_Face() );

        // Compute the edge twins and directions and lengths.  All the edges
        // should be unmarked at this point (actually, they were never marked
        // since creation).

        // Do the boundary edges
        do {
            e2 = e->Twin()->Adj_Face()->Parent()->EdgeP(
                                                    e->Origin()->Parent() );
            e->Origin()->Parent()->Set_Adj_Edge( e2 );
            e2->Set_Length( e->Origin()->Adj_Edge()->Length() );

            // Mark this edge since it is taken care of
            e2->Mark();

            // Set the edge parents and children

            // Interior edges that were not kept.  Set their parents to be
            // the longest interior edge of the parent sharing this vertex
            // Temporarily use the radius and deviation variables
            e1 = e2->Prev();
            e3 = e1;
            radius = -SWIFT_INFINITY;
            for( ; e1 != e->Adj_Face()->Parent()->EdgeP(
                   e->Prev()->Origin()->Parent() ); e1 = e1->Twin()->Prev()
            ) {
                deviation = e1->Origin()->Coords().Dist_Sq(
                                            e1->Next()->Origin()->Coords() );
                if( deviation > radius ) {
                    radius = deviation;
                    e3 = e1;
                }
            }
            if( e1 == e2->Prev() ) {
                // There is no interior edge from this vertex at the parent
                // level
                radius = e1->Origin()->Coords().Dist_Sq(
                                            e1->Next()->Origin()->Coords() );
                deviation = e2->Origin()->Coords().Dist_Sq(
                                            e2->Next()->Origin()->Coords() );
                if( deviation > radius ) {
                    e->Set_Parent( e2 );
                    e->Twin()->Set_Parent( e2 );
                } else {
                    e->Set_Parent( e1 );
                    e->Twin()->Set_Parent( e1 );
                }
            } else {
                e->Set_Parent( e3 );
                e->Twin()->Set_Parent( e3 );
            }

            // Border edge that was kept
            e->Twin()->Next()->Set_Parent( e2 );
            e2->Set_Child( e->Twin()->Next() );

            // Set the twin of this edge if possible
            if( e->Origin()->Adj_Edge()->Twin()->Adj_Face()->Parent() != NULL
            ) {
                e1 = e->Origin()->Adj_Edge()->Twin()->Adj_Face()->
                    Parent()->EdgeP( e->Twin()->Prev()->Origin()->Parent() );
                e1->Set_Twin( e2 );
                e2->Set_Twin( e1 );
            }

            e = e->Next()->Twin();
        } while( e != indep_verts[i]->Adj_Edge()->Twin() );

        // Compute the face childs and the edge geometry that has not yet
        // been computed
        for( j = flen; j < facei; j++ ) {
            // Set the child to be the removed vertex
            faces[j].Set_Vertex_Child( indep_verts[i] );
            if( faces[j].Edge1().Unmarked() ) {
                if( faces[j].Edge1().Child() == NULL ) {
                    faces[j].Edge1().Set_Vertex_Child( indep_verts[i] );
                    faces[j].Edge1().Twin()->Set_Vertex_Child( indep_verts[i] );
                }
                faces[j].Edge1().Compute_Norm_Direction_Length();
                faces[j].Edge1().Set_Direction_Length_To_Twin();
                faces[j].Edge1().Twin()->Mark();
            } else {
                faces[j].Edge1().Unmark();
            }
            if( faces[j].Edge2().Unmarked() ) {
                if( faces[j].Edge2().Child() == NULL ) {
                    faces[j].Edge2().Set_Vertex_Child( indep_verts[i] );
                    faces[j].Edge2().Twin()->Set_Vertex_Child( indep_verts[i] );
                }
                faces[j].Edge2().Compute_Norm_Direction_Length();
                faces[j].Edge2().Set_Direction_Length_To_Twin();
                faces[j].Edge2().Twin()->Mark();
            } else {
                faces[j].Edge2().Unmark();
            }
            if( faces[j].Edge3().Unmarked() ) {
                if( faces[j].Edge3().Child() == NULL ) {
                    faces[j].Edge3().Set_Vertex_Child( indep_verts[i] );
                    faces[j].Edge3().Twin()->Set_Vertex_Child( indep_verts[i] );
                }
                faces[j].Edge3().Compute_Norm_Direction_Length();
                faces[j].Edge3().Set_Direction_Length_To_Twin();
                faces[j].Edge3().Twin()->Mark();
            } else {
                faces[j].Edge3().Unmark();
            }
        }
        flen = facei;
    }

    // Note that at this point, the face planes, the edge directions and the
    // edge lengths have all been computed.  The edge distances and the edge
    // face planes still have to be computed.

#ifdef SWIFT_DEBUG
if( flen != Num_Faces() ) {
cerr << "*************** Face lengths don't match up *****************" << endl;
cerr << "Num_Faces() = " << Num_Faces() << endl;
cerr << "flen = " << flen << endl;
}
#endif
}

// Compute the scaling factor to bound the child.  The pierced faces are marked
// for subsequent use by the relaxation algorithm.
SWIFT_Real SWIFT_Tri_Mesh::Compute_Pierced_Faces_And_Scaling_Factor(
                                SWIFT_Array<SWIFT_Tri_Vertex*>& indep_verts,
                                int first_new_face, SWIFT_Tri_Mesh& child,
                                SWIFT_Array<SWIFT_Tri_Vertex**>& fvs,
                                SWIFT_Array<int>& fvs_len,
                                SWIFT_Array<SWIFT_Tri_Vertex**>& cfvs,
                                SWIFT_Array<int>& cfvs_len )
{
    int i, j, k;
    SWIFT_Real s;
    SWIFT_Real scale;
    SWIFT_Real comd;

    SWIFT_Array<SWIFT_Triple> edge_normals;
    SWIFT_Array<SWIFT_Real> edge_distances;

    edge_normals.Create( (Num_Faces()-first_new_face)*3 );
    edge_distances.Create( (Num_Faces()-first_new_face)*3 );

    scale = -SWIFT_INFINITY;

    // Create the region normals for the new edges
    j = 0;
    for( i = first_new_face; i < Num_Faces(); i++ ) {
        if( faces[i].Edge1().Unmarked() ) {
            edge_normals[j] = faces[i].Edge1().Direction() %
                              (com - faces[i].Edge1().Origin()->Coords());
            edge_normals[j].Normalize();
            edge_distances[j] = com * edge_normals[j];
            k = Face_Id( faces[i].Edge1().Twin()->Adj_Face() );
            if( k >= first_new_face ) {
                k = (k-first_new_face)*3 +
                    faces[k].Edge_Id( faces[i].Edge1().Twin() );
                edge_normals[k] = -(edge_normals[j]);
                edge_distances[k] = -(edge_distances[j]);
                faces[i].Edge1().Twin()->Mark();
            }
        } else {
            faces[i].Edge1().Unmark();
        }
        j++;
        if( faces[i].Edge2().Unmarked() ) {
            edge_normals[j] = faces[i].Edge2().Direction() %
                              (com - faces[i].Edge2().Origin()->Coords());
            edge_normals[j].Normalize();
            edge_distances[j] = com * edge_normals[j];
            k = Face_Id( faces[i].Edge2().Twin()->Adj_Face() );
            if( k >= first_new_face ) {
                k = (k-first_new_face)*3 +
                    faces[k].Edge_Id( faces[i].Edge2().Twin() );
                edge_normals[k] = -(edge_normals[j]);
                edge_distances[k] = -(edge_distances[j]);
                faces[i].Edge2().Twin()->Mark();
            }
        } else {
            faces[i].Edge2().Unmark();
        }
        j++;
        if( faces[i].Edge3().Unmarked() ) {
            edge_normals[j] = faces[i].Edge3().Direction() %
                              (com - faces[i].Edge3().Origin()->Coords());
            edge_normals[j].Normalize();
            edge_distances[j] = com * edge_normals[j];
            k = Face_Id( faces[i].Edge3().Twin()->Adj_Face() );
            if( k >= first_new_face ) {
                k = (k-first_new_face)*3 +
                    faces[k].Edge_Id( faces[i].Edge3().Twin() );
                edge_normals[k] = -(edge_normals[j]);
                edge_distances[k] = -(edge_distances[j]);
                faces[i].Edge3().Twin()->Mark();
            }
        } else {
            faces[i].Edge3().Unmark();
        }
        j++;
    }

    SWIFT_Array<SWIFT_Tri_Vertex*> cverts;
    SWIFT_Array<SWIFT_Tri_Vertex*> cnewverts;
    SWIFT_Tri_Face* f;
    SWIFT_Tri_Edge* e;
    int m, n, o, p, nv;

    fvs.Create( Num_Faces() );
    fvs_len.Create( Num_Faces() );

    // Copy the face vertices for the kept faces
    for( i = 0; i < first_new_face; i++ ) {
        o = child.Face_Id( faces[i].Child() );
        fvs_len[i] = cfvs_len[o];
        if( fvs_len[i] == 0 ) {
            fvs[i] = NULL;
        } else {
            fvs[i] = new SWIFT_Tri_Vertex*[fvs_len[i]];
            for( j = 0; j < fvs_len[i]; j++ ) {
                fvs[i][j] = cfvs[o][j];

                // Compute a scaling factor.
                comd = -faces[i].Distance( com );
                s = (faces[i].Distance( fvs[i][j] ) + comd) / comd;

                // If it is the largest so far, set it.
                if( s > scale ) {
                    scale = s;
                }
            }
        }
    }

    k = 0;
    j = first_new_face;
    for( i = indep_verts.Length()-1; i >= 0; i-- ) {
        // Count up the total number of vertices contained in this hole
        e = indep_verts[i]->Adj_Edge();
        f = indep_verts[i]->Adj_Edge()->Adj_Face();
        nv = 0;
        for( n = 0; n < indep_verts[i]->Valence(); n++ ) {
            nv += cfvs_len[child.Face_Id( f )];
            e = e->Twin()->Next();
            f = e->Adj_Face();
        }

        cverts.Create( nv+1 );
        cnewverts.Create( nv+1 );


        // Find the vertex at the finest level and add it to the list
        cverts[0] = indep_verts[i];
        SWIFT_Tri_Mesh* finest = &child;
        while( cverts[0]->Child() != NULL ) {
            cverts[0] = cverts[0]->Child();
            finest = finest->Child();
        }

        e = indep_verts[i]->Adj_Edge();
        f = indep_verts[i]->Adj_Edge()->Adj_Face();
        nv = 1;
        for( n = 0; n < indep_verts[i]->Valence(); n++ ) {
            // Put the vertices into the temp list
            o = child.Face_Id( f );
            for( m = 0; m < cfvs_len[o]; m++ ) {
                cverts[nv++] = cfvs[o][m];
            }
            e = e->Twin()->Next();
            f = e->Adj_Face();
        }

        for( n = j; n < j+indep_verts[i]->Valence()-2; n++ ) {
#ifdef SWIFT_DEBUG
if( indep_verts[i] != faces[n].Vertex_Child() ) {
cerr << "*************** Child does not match" << endl;
}
#endif
            // For each new face find out which vertices it gets.
            o = 0;
            p = 0;
            for( m = 0; m < nv; m++ ) {
                if( (edge_distances[k] - EPSILON12) <=
                    cverts[m]->Coords() * edge_normals[k] &&
                    (edge_distances[k+1] - EPSILON12) <=
                    cverts[m]->Coords() * edge_normals[k+1] &&
                    (edge_distances[k+2] - EPSILON12) <=
                    cverts[m]->Coords() * edge_normals[k+2]
                ) {
                    cnewverts[o++] = cverts[m];

                    // Compute a scaling factor.
                    comd = -faces[n].Distance( com );
                    s = (faces[n].Distance( cverts[m] ) + comd) / comd;

                    // If it is the largest so far, set it.
                    if( s > scale ) {
                        scale = s;
                    }
                } else {
                    // Pack the array for efficiency
                    cverts[p++] = cverts[m];
                }
            }

            // Because of the array packing
            nv -= o;

            // Create the new face vertex list for this level
            if( o == 0 ) {
                fvs[n] = NULL;
                fvs_len[n] = 0;
            } else {
                fvs[n] = new SWIFT_Tri_Vertex*[o];
                fvs_len[n] = o;

                // Copy over the face vertex list
                for( m = 0; m < o; m++ ) {
                    fvs[n][m] = cnewverts[m];
                }
            }

            k += 3;
        }

        j = n;

#ifdef SWIFT_DEBUG
        if( nv != 0 ) {
            for( n = 0; n < nv; n++ ) {
                cerr << "  addr = " << (void*) cverts[n] << endl;
                cerr << "  coords = " << cverts[n]->Coords() << endl;
            }
        }
#endif

        cverts.Destroy();
        cnewverts.Destroy();
    }

    return scale;
}

SWIFT_Real SWIFT_Tri_Mesh::Relax_Parameter(
                                    int i, SWIFT_Real mint, SWIFT_Real maxt,
                                    SWIFT_Array<SWIFT_Tri_Vertex**>& fvs,
                                    SWIFT_Array<int>& fvs_len )
{
    int j, k, m;
    SWIFT_Real t;
    SWIFT_Real result = mint;
    SWIFT_Tri_Edge* e = verts[i].Adj_Edge();

    for( j = 0; j < verts[i].Valence(); j++, e = e->Twin()->Next() ) {
        // Compute the convexity criteria
        SWIFT_Triple v2xv1 = e->Twin()->Prev()->Direction() %
                             e->Next()->Direction();
        t = v2xv1 * verts[i].Coords();
        if( t >= 0.0 ) {
            t = (e->Twin()->Origin()->Coords() * v2xv1) / t;
            if( t > maxt ) {
                result = maxt;
            } else if( t > result ) {
                result = t;
            }
        }

        // Compute the face bounding criterion

        // For each neighboring face, check all the vertices that are contained
        // in their com-regions
        m = Face_Id( e->Adj_Face() );
        for( k = 0; k < fvs_len[m]; k++ ) {
            v2xv1 = fvs[m][k]->Coords() -
                    e->Twin()->Origin()->Coords() - com;

            SWIFT_Triple pBe = v2xv1 - ((v2xv1 * e->Next()->Direction()) *
                                        e->Next()->Direction());
            v2xv1 = verts[i].Coords() - e->Twin()->Origin()->Coords();
            SWIFT_Triple p1e = v2xv1 - ((v2xv1 * e->Next()->Direction()) *
                                        e->Next()->Direction());
            SWIFT_Triple p10 = verts[i].Coords() -
                               ((verts[i].Coords() * e->Next()->Direction()) *
                                e->Next()->Direction());
            SWIFT_Real pBe_dot_p10 = pBe * p10;
            SWIFT_Real pBe_len_sq = pBe * pBe;

            t = 1.0 -
                ( pBe_len_sq * (p1e * p10) - pBe_dot_p10 * (p1e * pBe) ) /
                ( pBe_len_sq * p10.Length_Sq() - pBe_dot_p10 * pBe_dot_p10 );

            // If the face touches the removed vertex before we reach the
            // current movement parameter then we must stop when the face
            // touches the removed vertex otherwise the parent will not
            // bound the child
            if( t > maxt ) {
                result = maxt;
            } else if( t > result ) {
                result = t;
            }
        }
    }

    return result;
}

void SWIFT_Tri_Mesh::Relax( SWIFT_Real s, SWIFT_Array<SWIFT_Tri_Vertex**>& fvs,
                                          SWIFT_Array<int>& fvs_len )
{
    const SWIFT_Real one_scale = 1.0 / s;
    int i, j;
    SWIFT_Real old_dist, old_parm;
    SWIFT_Tri_Edge* e;
    SWIFT_Tri_Vertex* v;

    SWIFT_Array<SWIFT_Tri_Vertex*> heap_verts;
    SWIFT_Array<int> heap_indices;
    SWIFT_Array<SWIFT_Real> parms;
    SWIFT_Array<SWIFT_Real> dists;
    SWIFT_Array<SWIFT_Real> lengths;
    SWIFT_Array<SWIFT_Triple> directions;

    heap_verts.Create( Num_Vertices() );
    heap_indices.Create( Num_Vertices() );
    parms.Create( Num_Vertices() );
    dists.Create( Num_Vertices() );
    lengths.Create( Num_Vertices() );
    directions.Create( Num_Vertices() );

    // Translate all the vertices to the center of mass, compute the lengths
    // and directions of the vertex rays emanating from the com.
    for( i = 0; i < Num_Vertices(); i++ ) {
        heap_verts[i] = verts(i);
        heap_indices[i] = i;
        verts[i].Set_Coords( verts[i].Coords() - com );
        lengths[i] = verts[i].Coords().Length();
        directions[i] = verts[i].Coords() / lengths[i];
    }

    // Compute all the initial parameter values
    for( i = 0; i < Num_Vertices(); i++ ) {
        // Compute the parameter value for this vertex.  This is the percentage
        // of the distance from the com that it must maintain in order to not
        // violate any criteria.
        parms[i] = Relax_Parameter( i, one_scale, 1.0, fvs, fvs_len );

        // Compute the distance that the vertex can move
        dists[i] = (1.0 - parms[i]) * lengths[i];
    }

    // Create a heap with the parameter values
    Build_Heap( heap_verts, heap_indices, dists );

    while( true ) {
        v = heap_verts[0];
        i = Vertex_Id(v);
#ifdef SWIFT_USE_FLOAT
        if( (dists[0] / lengths[i]) < EPSILON4 ) {
#else
        if( (dists[0] / lengths[i]) < EPSILON6 ) {
#endif
            break;
        }

        // Move the vertex the largest possible distance
        v->Set_Coords( lengths[i] * parms[i] * directions[i] );

        // Fix the heap now that the vertex cannot move anymore.
        // Set its distance to 0.0.
        dists[0] = 0.0;
        Heapify( heap_verts, heap_indices, dists, 0 );

        // Compute new edge lengths and directions
        v->Adj_Edge()->Compute_Direction_Length_Twin();
        e = v->Adj_Edge()->Twin()->Next();
        for( j = 1; e != v->Adj_Edge(); e = e->Twin()->Next(), j++ ) {
            e->Compute_Direction_Length_Twin();
        }

        // Now fix up the distance values of the neighbors of the removed
        // vertex and reheap them.
        e = v->Adj_Edge();
        for( ; j > 0; j--, e = e->Twin()->Next() ) {
            i = Vertex_Id( e->Twin()->Origin() );
            old_parm = parms[i];
            parms[i] = Relax_Parameter( i, one_scale, old_parm, fvs, fvs_len );

            // Compute the distance that the vertex can move
            old_dist = dists[heap_indices[i]];
            dists[heap_indices[i]] = (old_parm - parms[i]) * lengths[i];

            if( dists[heap_indices[i]] < old_dist ) {
                Heapify( heap_verts, heap_indices, dists, heap_indices[i] );
            } else {
                Up_Heap( heap_verts, heap_indices, dists, heap_indices[i] );
            }
        }
    }

    // Move all the vertices back to the world frame
    for( i = 0; i < Num_Vertices(); i++ ) {
        verts[i].Set_Coords( verts[i].Coords() + com );
    }

    // Unmark all the faces and compute their new plane equations.
    for( i = 0; i < Num_Faces(); i++ ) {
        if( faces[i].Edge1().Unmarked() ) {
            // Compute edge distance
            faces[i].Edge1().Set_Origin_On_Plane_Twin();
            faces[i].Edge1().Twin()->Mark();
        } else {
            faces[i].Edge1().Unmark();
        }
        if( faces[i].Edge2().Unmarked() ) {
            faces[i].Edge2().Set_Origin_On_Plane_Twin();
            faces[i].Edge2().Twin()->Mark();
        } else {
            faces[i].Edge2().Unmark();
        }
        if( faces[i].Edge3().Unmarked() ) {
            faces[i].Edge3().Set_Origin_On_Plane_Twin();
            faces[i].Edge3().Twin()->Mark();
        } else {
            faces[i].Edge3().Unmark();
        }
        // Compute face plane
        faces[i].Compute_Plane_From_Edges();
        faces[i].Unmark();
    }

    // Compute edge-face planes
    for( i = 0; i < Num_Faces(); i++ ) {
        // Compute edge-face planes
        faces[i].Edge1().Compute_Face_Plane();
        faces[i].Edge2().Compute_Face_Plane();
        faces[i].Edge3().Compute_Face_Plane();
    }
}
#endif

void SWIFT_Tri_Mesh::Scale_From_Center_Of_Mass( SWIFT_Real s )
{
    int i;

    for( i = 0; i < Num_Vertices(); i++ ) {
        verts[i].Set_Coords( (s * (verts[i].Coords() - com)) + com );
    }
}

// All edges should be unmarked when this function is called.  When this
// function terminates, they will still be unmarked.
void SWIFT_Tri_Mesh::Compute_Geometry_After_Scale( SWIFT_Real s )
{
    int i;

    // Recompute face and edge distances
    for( i = 0; i < Num_Faces(); i++ ) {
        faces[i].Set_Point_On_Plane( faces[i].Edge1().Origin()->Coords() );
        if( faces[i].Edge1().Unmarked() ) {
            faces[i].Edge1().Scale_Length( s );
            faces[i].Edge1().Set_Origin_On_Plane();
            faces[i].Edge1().Set_Origin_On_Face_Plane();
            faces[i].Edge1().Twin()->Set_Length( faces[i].Edge1().Length() );
            faces[i].Edge1().Twin()->Set_Origin_On_Plane();
            faces[i].Edge1().Twin()->Set_Origin_On_Face_Plane();
            faces[i].Edge1().Twin()->Mark();
        } else {
            faces[i].Edge1().Unmark();
        }
        if( faces[i].Edge2().Unmarked() ) {
            faces[i].Edge2().Scale_Length( s );
            faces[i].Edge2().Set_Origin_On_Plane();
            faces[i].Edge2().Set_Origin_On_Face_Plane();
            faces[i].Edge2().Twin()->Set_Length( faces[i].Edge2().Length() );
            faces[i].Edge2().Twin()->Set_Origin_On_Plane();
            faces[i].Edge2().Twin()->Set_Origin_On_Face_Plane();
            faces[i].Edge2().Twin()->Mark();
        } else {
            faces[i].Edge2().Unmark();
        }
        if( faces[i].Edge3().Unmarked() ) {
            faces[i].Edge3().Scale_Length( s );
            faces[i].Edge3().Set_Origin_On_Plane();
            faces[i].Edge3().Set_Origin_On_Face_Plane();
            faces[i].Edge3().Twin()->Set_Length( faces[i].Edge3().Length() );
            faces[i].Edge3().Twin()->Set_Origin_On_Plane();
            faces[i].Edge3().Twin()->Set_Origin_On_Face_Plane();
            faces[i].Edge3().Twin()->Mark();
        } else {
            faces[i].Edge3().Unmark();
        }
    }
}

int SWIFT_Tri_Mesh::Create_Bounding_Volume_Hierarchy(
                                        SWIFT_Array<SWIFT_Tri_Mesh*>& levels )
{
    int i;
    SWIFT_Real scale;
    SWIFT_Array<SWIFT_Tri_Vertex*> startvs;

    // Set the parent and child pointers of the meshes
    levels[0]->Set_Child( NULL );
    if( levels.Length() > 1 ) {
        levels[0]->Set_Parent( levels[1] );
    }
    for( i = 1; i < levels.Length()-1; i++ ) {
        levels[i]->Set_Child( levels[i-1] );
        levels[i]->Set_Parent( levels[i+1] );
    }
    if( levels.Length() > 1 ) {
        levels[levels.Length()-1]->Set_Child( levels[levels.Length()-2] );
    }
    levels[levels.Length()-1]->Set_Parent( NULL );

    // Compute center of mass for each level and translate them so that the
    // finest mesh's center of mass is equal.
    for( i = 1; i < levels.Length(); i++ ) {
        levels[i]->Translate_To( levels[0]->Center_Of_Mass() -
                                 levels[i]->Center_Of_Mass() );
        levels[i]->Set_Center_Of_Mass( levels[0]->Center_Of_Mass() );
    }

    // Compute and apply scaling factors so that each level bounds the original
    // level.  Also, compute parents, children and deviations.
    for( i = 1; i < levels.Length(); i++ ) {
        startvs.Create( levels[i]->Num_Vertices() );
        scale = levels[i]->Compute_Scaling_Factor( startvs, levels[0] );
        levels[i]->Scale_From_Center_Of_Mass( scale );
        levels[i]->Compute_Geometry_After_Scale( scale );
        levels[i]->Set_Radius( scale * levels[i]->Radius() );
        levels[i]->Set_Volume( scale * scale * scale * levels[i]->Volume() );
        levels[i]->Assign_Children();
        levels[i-1]->Assign_Parents();
        levels[i]->Compute_Deviation( startvs );
        startvs.Destroy();
#ifdef SWIFT_DEBUG
        cerr << "******* SWIFT_Tri_Mesh::Create_Bounding_Volume_Hierarchy()'2"
             << " Verification " << i-1 << " *******" << endl;
        levels[i-1]->Verify( true );
#endif
    }
#ifdef SWIFT_DEBUG
    cerr << "******* SWIFT_Tri_Mesh::Create_Bounding_Volume_Hierarchy()'2"
         << " Verification " << i-1 << " *******" << endl;
    levels[i-1]->Verify( true );
#endif

    return levels.Length();
}

SWIFT_Real SWIFT_Tri_Mesh::Compute_Scaling_Factor(
                                        SWIFT_Array<SWIFT_Tri_Vertex*>& svs,
                                        SWIFT_Tri_Mesh* finest )
{
    int i, top;
    SWIFT_Real scale;
    SWIFT_Tri_Vertex* v = finest->Vertices()(0);
    SWIFT_Array<SWIFT_Tri_Face*> stack_faces;

    stack_faces.Create( Num_Faces() );

    scale = 0.0;
    top = 0;
    stack_faces[0] = faces(0);

    while( top != -1 ) {
        if( stack_faces[top]->Unmarked() ) {
            // Process this face
            SWIFT_Real s = stack_faces[top]->Normal() * Center_Of_Mass();
            s = (finest->Extremal_Vertex(
                    stack_faces[top]->Normal(), v ) - s) /
                (stack_faces[top]->Distance() - s);
            svs[ Vertex_Id(stack_faces[top]->Edge1().Origin()) ] = v;
            svs[ Vertex_Id(stack_faces[top]->Edge2().Origin()) ] = v;
            svs[ Vertex_Id(stack_faces[top]->Edge3().Origin()) ] = v;
            if( s > scale ) {
                scale = s;
            }
            stack_faces[top]->Mark();
        }
        if( stack_faces[top]->Edge1().Twin()->Adj_Face()->Unmarked() ) {
            top++;
            stack_faces[top] = stack_faces[top-1]->Edge1().Twin()->Adj_Face();
        } else if( stack_faces[top]->Edge2().Twin()->Adj_Face()->Unmarked()
        ) {
            top++;
            stack_faces[top] = stack_faces[top-1]->Edge2().Twin()->Adj_Face();
        } else if( stack_faces[top]->Edge3().Twin()->Adj_Face()->Unmarked()
        ) {
            top++;
            stack_faces[top] = stack_faces[top-1]->Edge3().Twin()->Adj_Face();
        } else {
            top--;
        }
    }

    // Unmark all the faces
    for( i = 0; i < Num_Faces(); i++ ) {
        faces[i].Unmark();
    }

    return scale;
}

// Computes the maximum deviation between this mesh (coarse) and the mesh that
// owns the vertices in startvs (finest).   This is done by finding the maximum
// of the minimum of the distances of each of the coarse vertices to the finest
// mesh.
void SWIFT_Tri_Mesh::Compute_Deviation(
                                    SWIFT_Array<SWIFT_Tri_Vertex*>& startvs )
{
    int i;
    void* near_f;
    int type;
    SWIFT_Real dist;
    SWIFT_Tri_Mesh* finest;

    if( child == NULL ) {
        deviation = 0.0;
        return;
    }

    // Find the finest child
    finest = child;
    while( finest->Child() != NULL ) {
        finest = finest->Child();
    }

    deviation = 0.0;
    for( i = 0; i < Num_Vertices(); i++ ) {
        near_f = finest->Closest_Feature( verts[i].Coords(), startvs[i],
                                          type, dist );

        if( near_f == NULL ) {
            cerr << "Error (SWIFT_Tri_Mesh::Compute_Deviation): Could not find "
                 << "Closest_Feature: inside mesh" << endl;
            continue;
        }

        switch( type ) {
        case SWIFT_VERTEX:
            if( dist > deviation ) {
                deviation = dist;
            }
            break;
        case SWIFT_EDGE:
            if( dist > deviation ) {
                deviation = dist;
            }
            break;
        case SWIFT_FACE:
            if( dist*dist > deviation ) {
                deviation = dist*dist;
            }
            break;
        default:
            cerr << "Error (SWIFT_Tri_Mesh::Compute_Deviation): Could not find "
                 << "Closest_Feature: bad feature type" << endl;
            break;
        }
    }
    deviation = sqrt( deviation );
}

void SWIFT_Tri_Mesh::Assign_Children( )
{
    int i;
    void* near_f;
    int type;
    SWIFT_Real dist, fd, minfd;
    SWIFT_Tri_Vertex* startv = child->Vertices()(0);
    SWIFT_Tri_Vertex* v;
    SWIFT_Tri_Edge* e;
    SWIFT_Tri_Face* f;
    SWIFT_Tri_Edge* edge;
    
    for( i = 0; i < Num_Vertices(); i++ ) {
        near_f = child->Closest_Feature( verts[i].Coords(), startv,
                                         type, dist );

        if( near_f == NULL ) {
            cerr << "Error (SWIFT_Tri_Mesh::Compute_Deviation): Could not find "
                 << "Closest_Feature: inside mesh" << endl;
            continue;
        }

        switch( type ) {
        case SWIFT_VERTEX:
            v = (SWIFT_Tri_Vertex*)near_f;
            e = verts[i].Adj_Edge();
            verts[i].Set_Child( v );
            e->Set_Vertex_Child( v );
            e->Adj_Face()->Set_Vertex_Child( v );
            for( e = e->Twin()->Next(); e != verts[i].Adj_Edge();
                 e = e->Twin()->Next()
            ) {
                e->Set_Vertex_Child( v );
                e->Adj_Face()->Set_Vertex_Child( v );
            }
            // Initialize for the next iteration
            startv = v;
            break;
        case SWIFT_EDGE:
            e = ((SWIFT_Tri_Edge*)near_f);
            if( verts[i].Coords().Dist_Sq( e->Origin()->Coords() ) <
                verts[i].Coords().Dist_Sq( e->Twin()->Origin()->Coords() )
            ) {
                v = e->Origin();
            } else {
                v = e->Twin()->Origin();
            }

            edge = verts[i].Adj_Edge();
            verts[i].Set_Child( v );
            edge->Set_Child( e );
            edge->Adj_Face()->Set_Vertex_Child( v );
            for( edge = edge->Twin()->Next();
                 edge != verts[i].Adj_Edge();
                 edge = edge->Twin()->Next()
            ) {
                edge->Set_Child( e );
                edge->Adj_Face()->Set_Vertex_Child( v );
            }
            // Initialize for the next iteration
            startv = e->Origin();
            break;
        case SWIFT_FACE:
            f = ((SWIFT_Tri_Face*)near_f);
            fd = verts[i].Coords().Dist_Sq( f->Edge1().Origin()->Coords() );
            minfd = verts[i].Coords().Dist_Sq( f->Edge2().Origin()->Coords() );
            if( fd < minfd ) {
                v = f->Edge1().Origin();
                minfd = fd;
            } else {
                v = f->Edge2().Origin();
            }
            fd = verts[i].Coords().Dist_Sq( f->Edge3().Origin()->Coords() );
            if( fd < minfd ) {
                v = f->Edge3().Origin();
            }

            e = verts[i].Adj_Edge();
            verts[i].Set_Child( v );
            e->Set_Vertex_Child( v );
            e->Adj_Face()->Set_Child( f );
            for( e = e->Twin()->Next(); e != verts[i].Adj_Edge();
                 e = e->Twin()->Next()
            ) {
                e->Set_Vertex_Child( v );
                e->Adj_Face()->Set_Child( f );
            }
            // Initialize for the next iteration
            startv = f->Edge1().Origin();
            break;
        default:
            cerr << "Error (SWIFT_Tri_Mesh::Compute_Deviation): Could not find "
                 << "Closest_Feature: bad feature type" << endl;
            break;
        }
    }
}

void SWIFT_Tri_Mesh::Assign_Parents( )
{
    int i, top;
    SWIFT_Tri_Vertex* v = parent->Vertices()(0);
    SWIFT_Array<SWIFT_Tri_Face*> stack_faces;

    stack_faces.Create( Num_Faces() );

    top = 0;
    stack_faces[0] = faces(0);

    while( top != -1 ) {
        if( stack_faces[top]->Unmarked() ) {
            // Process this face
            parent->Extremal_Vertex( stack_faces[top]->Normal(), v );
            // Assign the extremal vertex as the parent to the face, its
            // edges, and its vertices
            stack_faces[top]->Set_Parent( (SWIFT_Tri_Face*)v );
            stack_faces[top]->Edge1().Set_Parent( (SWIFT_Tri_Edge*)v );
            stack_faces[top]->Edge1().Origin()->Set_Parent( v );
            stack_faces[top]->Edge2().Set_Parent( (SWIFT_Tri_Edge*)v );
            stack_faces[top]->Edge2().Origin()->Set_Parent( v );
            stack_faces[top]->Edge3().Set_Parent( (SWIFT_Tri_Edge*)v );
            stack_faces[top]->Edge3().Origin()->Set_Parent( v );
            stack_faces[top]->Mark();
        }
        if( stack_faces[top]->Edge1().Twin()->Adj_Face()->Unmarked() ) {
            top++;
            stack_faces[top] = stack_faces[top-1]->Edge1().Twin()->Adj_Face();
        } else if( stack_faces[top]->Edge2().Twin()->Adj_Face()->Unmarked() ) {
            top++;
            stack_faces[top] = stack_faces[top-1]->Edge2().Twin()->Adj_Face();
        } else if( stack_faces[top]->Edge3().Twin()->Adj_Face()->Unmarked() ) {
            top++;
            stack_faces[top] = stack_faces[top-1]->Edge3().Twin()->Adj_Face();
        } else {
            top--;
        }
    }

    // Unmark all the faces
    for( i = 0; i < Num_Faces(); i++ ) {
        faces[i].Unmark();
    }
}
#endif


