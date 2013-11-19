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
// fileio.C
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>

#include <SWIFT_config.h>
#include <SWIFT_common.h>
#include <SWIFT_array.h>
#include <SWIFT_fileio.h>


///////////////////////////////////////////////////////////////////////////////
// SWIFT_Basic_File_Reader public functions
///////////////////////////////////////////////////////////////////////////////

bool SWIFT_Basic_File_Reader::Read( ifstream& fin, SWIFT_Real*& vs, int*& fs,
                                                   int& vn, int& fn, int*& fv
                                    )
{
    int i, j, k;
    char buffer[256];

    fin >> buffer;

    if( !strcmp( buffer, "TRI" ) ) {
        // Triangle file format
        fin >> vn >> fn;
        vs = new SWIFT_Real[vn*3];
        fs = new int[fn*3];
        fv = NULL;

        // Read in the vertex coordinates
        for( j = 0; j < vn*3; j += 3 ) {
            fin >> vs[j] >> vs[j+1] >> vs[j+2];
        }

        // Read in the face indices
        for( i = 0, j = 0; i < fn; i++, j += 3 ) {
            fin >> fs[j] >> fs[j+1] >> fs[j+2];
        }
    } else if( !strcmp( buffer, "POLY" ) ) {
        // Polygon file format
        fin >> vn >> fn;
        vs = new SWIFT_Real[vn*3];
        fs = new int[(vn+fn-2)<<1];
        fv = new int[fn];

        // Read in the vertex coordinates
        for( i = 0, j = 0; i < vn; i++, j += 3 ) {
            fin >> vs[j] >> vs[j+1] >> vs[j+2];
        }

        // Read in the face indices
        for( i = 0, j = 0; i < fn; i++ ) {
            fin >> fv[i];
            for( k = 0; k < fv[i]; k++ ) {
                fin >> fs[j++];
            }
        }
    } else {
        cerr << "Error: Do not recognize file type: \"" << buffer << "\".  "
             << "I was registered incorrectly" << endl;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// SWIFT_File_Read_Dispatcher public functions
///////////////////////////////////////////////////////////////////////////////

SWIFT_File_Read_Dispatcher::SWIFT_File_Read_Dispatcher( )
{
    magic_nums.Create( 10 );
    magic_nums.Set_Length( 0 );
    readers.Create( 10 );
    readers.Set_Length( 0 );
}

SWIFT_File_Read_Dispatcher::~SWIFT_File_Read_Dispatcher( )
{
    int i;
    for( i = 0; i < magic_nums.Length(); i++ ) {
        delete magic_nums[i];
    }
}

bool SWIFT_File_Read_Dispatcher::Register( const char* magic_number,
                                           SWIFT_File_Reader* file_reader )
{
    int i;
    const int mn_len = strlen( magic_number );

    // Check that the new magic_num is not a prefix of an existing one and
    // vice-versa
    for( i = 0; i < magic_nums.Length(); i++ ) {
        const int reg_mn_len = strlen( magic_nums[i] );
        if( reg_mn_len != mn_len ) {
            if( !strncmp( magic_nums[i], magic_number, mn_len ) ) {
                // The new magic number is a prefix of registered magic number
                cerr << "Error (Register_File_Reader): Given magic number: "
                     << "\"" << magic_number << "\"" << endl
                     << "      is a prefix of an already registered magic "
                     << "number: \"" << magic_nums[i] << "\"" << endl;
                return false;
            } else if( !strncmp( magic_nums[i], magic_number, reg_mn_len ) ) {
                // The registered magic number is a prefix of new magic number
                cerr << "Error (Register_File_Reader): Already registered magic"
                     << " number: \"" << magic_nums[i] << "\"" << endl
                     << "      is a prefix of the given magic number: "
                     << "\"" << magic_number << "\"" << endl;
                return false;
            }
        } else {
            if( !strcmp( magic_nums[i], magic_number ) ) {
                // Found it.  Already registered.  Replace it.
                cerr << "Warning (Register_File_Reader): Replacing already "
                     << "registered file reader for magic number: \""
                     << magic_number << "\"" << endl;
                readers[i] = file_reader;
                return true;
            }
        }
    }

    // Not found.  Register it.
    char* new_magic_num = new char[strlen(magic_number)+1];
    strcpy( new_magic_num, magic_number );
    magic_nums.Add_Grow( new_magic_num, 10 );
    readers.Add_Grow( file_reader, 10 );

    return true;
}

bool SWIFT_File_Read_Dispatcher::Read( const char* filename, SWIFT_Real*& vs,
                                       int*& fs, int& vn, int& fn, int*& fv
                                    )
{
    int i, j;
    bool match;
    char buffer[256];
    SWIFT_Array<int> cur_match;

    // Try to open the file
    if( filename == NULL ) {
        cerr << "Error: Invalid filename given to read file" << endl;
        return false;
    }

    fin.open( filename, ios::in );

    if( !fin.rdbuf()->is_open( ) ) {
        cerr << "Error: file could not be opened for reading \""
             << filename << "\"" << endl;
        return false;
    }

    cur_match.Create( magic_nums.Length() );
    for( i = 0; i <  magic_nums.Length(); i++ ) {
        cur_match[i] = i;
    }

    // Read the magic number
    match = false;
    for( j = 0; !match && !cur_match.Empty(); j++ ) {
        fin >> buffer[j];
        for( i = 0; i < cur_match.Length(); i++ ) {
            if( buffer[j] != magic_nums[cur_match[i]][j] ) {
                cur_match[i] = cur_match.Last();
                cur_match.Decrement_Length();
                i--;
            } else {
                if( (unsigned int)j+1 == strlen(magic_nums[cur_match[i]]) ) {
                    // Found it
                    match = true;
                    break;
                }
            }
        }
    }

    if( !match ) {
        // Didn't find it
        cerr << "Error: file could not be read because no file reader can "
             << "handle the type: \"" << buffer << "\".  Consider writing "
             << "one (see documentation)." << endl;
        return false;
    }

    // Dispatch to the file reader.
    fin.seekg( 0, ios::beg );
    bool result = readers[cur_match[i]]->Read( fin, vs, fs, vn, fn, fv
                                            );
    fin.close();

    return result;
}


