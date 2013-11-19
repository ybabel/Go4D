/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : image.c, image.h implementation
 *CREATED: 22/10/95
 *DATE   : 18/03/96
 *DEFS   : 'gouraud' for shading
 *         'mapping' for mapping (mpoints and ima must exist)
 *	       'flat' for normal filling (color must exist)
 *         'quadmapping' for quadradtic mapping (cf horiz.cc)
 *         'hypmapping for hypperbolic mapping (cf horiz.cc)
 *BUGS   :
\*****************************************************************************/

/*****************************************************************************\
 *  description of the alogrithm FILL SHAPE WITH N POINTS
 * 
 *           1  *-------------------* 2
 *             /                     \
 *            /          Z1           \
 *        3  *-------------------------\
 *           |                          \                      |
 *   |       |             Z2            \                     |  xe
 *xb |       |                            \ 4                  |
 *   |       |-----------------------------*-----------* 5     v
 *   v       |              Z3                        /
 *        6  *---------------------------------------/
 *            \                                     /
 *             \              Z5                   /
 *              \                                 /
 *           7   *-------------------------------/
 *                      --------      Z6        /
 *                              --------       /
 *                                      ------* 8
 *
 * -  The plots are sorted, and the shape is cutted in zones, each zone is
 *draw basically. the first point P1 is the point with the minimal y.
 *the last point P8 is the point with the maximal y. 
 * -  There is 2 virtual points that are following the edges : one folow
 *the left edges (indiced bye 'l'), the other follow the right edges 
 *(indiced by 'r') : (1,3,6,7) are the lefts points, whereas (2,4,5,8) 
 *are the right points.
 * 
 *[Q] How are the points determined to be left or right ?
 *[A] The plots are sorted in the 'points' list, at the begining the current 
 *'left' point and the current 'right' point are equal to points[i=0], then
 *next left point will be points[i++], next right point will be points[i--]
 *(assuming i--=npoints if i==0)
 *
 *[Q] How do i draw a 'zone'?
 *[A] The limit of the zone is determine by the old left and right points
 *and the coordinates of the news left and right points. The you can calculate
 *the coef dir of the left and right edges. You have to draw an horizontal
 *line between xl and xr, and when its done calculate the new xl and xr for
 *the next line (simply increment xl and xr by ixl and ixr).
 *
 *                   *--------------------------*
 *            ixl   /                          /  ixr (coef of the line)
 *                 /                          /
 *         xl ---->           ry             <---  xr
 * xl+=ixl ->    /          ry=ry+1         /   <-xe+=ixr
 *              /            ...           /
 *             /                          /
 *            *--------------------------* ry==ryf
 *
 *
 * -  Then when you reach the end of the zone a new ixl (or ixr) must be
 *calculated (due to the changing left (or right) point)
 * -  You had to iterates these step until left==right
 *
 *[Q] How do i draw a gouraud shaded shape, or a mapped shape ?
 *[A] for gouraud shading and linear mapping its very simple : you have to
 *determine the coefs ic (or imx, imy) to increment the color of the current
 *point (or the coordinate of the new mapping point).
 *
 *note : to calculate the coef dir of the lines, the macro frapport is used
 *rather than div or rapport because if precision is two great it will return
 *zero (it's not well handled by gcc). this is a little bit slower (the 
 *division is made in float, instead of being made in int) due to the 
 *conversion
 *but it's more secure.
 *
 *
 *the subroutine behind must be defined
 *  Horiz
 *  ShadeHoriz
 *  MapHoriz
 *  QuadMapHoriz
 *  HypMapHoriz
 *
 *
 *void Drawer::Fill(int npoints, 
 *  Point * points, // the list of points required
 *  MapPoint * mpoints, // the list of points in the image
 *  const Image * ima, // the image itself
 *	color // the color for flat filling
 *	)
 *
\*****************************************************************************/

  #ifndef FILL_POINTS_ROT
  #define FILL_POINTS_ROT
  #endif

  #ifndef FILL_LOCALS
  #define FILL_LOCALS
  #endif

  #ifndef FILL_CALL_HORIZ
  #define FILL_CALL_HORIZ
  #endif

  #ifndef FILL_NEXT_LINE
  #define FILL_NEXT_LINE
  #endif

  #ifndef FILL_LEFT_CHANGING
  #define FILL_LEFT_CHANGING
  #endif

  #ifndef FILL_LEFT_CHANGED
  #define FILL_LEFT_CHANGED
  #endif

  #ifndef FILL_LEFT_SAME_COEF
  #define FILL_LEFT_SAME_COEF
  #endif

  #ifndef FILL_LEFT_NOT_SAME_LOCALS
  #define FILL_LEFT_NOT_SAME_LOCALS
  #endif

  #ifndef FILL_LEFT_NOT_SAME_COEF
  #define FILL_LEFT_NOT_SAME_COEF
  #endif

  #ifndef FILL_RIGHT_CHANGING
  #define FILL_RIGHT_CHANGING
  #endif

  #ifndef FILL_RIGHT_CHANGED
  #define FILL_RIGHT_CHANGED
  #endif

  #ifndef FILL_RIGHT_SAME_COEF
  #define FILL_RIGHT_SAME_COEF
  #endif

  #ifndef FILL_RIGHT_NOT_SAME_LOCALS
  #define FILL_RIGHT_NOT_SAME_LOCALS
  #endif

  #ifndef FILL_RIGHT_NOT_SAME_COEF
  #define FILL_RIGHT_NOT_SAME_COEF
  #endif


  // becarful the list can be modified (rotated)
  //don't verify that the shape is convex
  assert(npoints>2); //at least 3 points

  //search the point with the y max
  //-------------------------------

  int minp = 0;
  int miny = points[0].y;
  for (int ks=1; ks<npoints; ks++)
    if (points[ks].y < miny) { miny=points[ks].y; minp = ks; }

  // the first point of the list must be the one with the y min
  // so we make a rotation in the list
  //-----------------------------------------------------------

  for (int kr=0; kr<minp; kr++)
    {
       PointZ tmp = points[0];
       for (int i=0; i<npoints-1; i++)
         points[i] = points[i+1];
       points[npoints-1] = tmp;

       FILL_POINTS_ROT;

    }

  // coef's initialization
  //----------------------

  int right=0, left=0;
  int ry=points[0].y;
  int ryf;  //initialized later, <=> y when the next pallier is reached

  realtype xl=conv(points[0].x);
  realtype xr=conv(points[0].x);
  realtype ixl=0, ixr=0;

  realtype zl=fconv(points[0].z);
  realtype zr=fconv(points[0].z);
  realtype izl=0, izr=0;

  FILL_LOCALS;


  // draw the differents zones
  //--------------------------

  do
    {

      // ryf = the limit of the zone
      //---------------------------

      if (points[left].y < points[right].y)
      	ryf  = points[left].y;
      else
      	ryf  = points[right].y;

      // draw the zone (depending on zbuffer)
      //-------------------------------------

      if (ryf != ry)
      	{
      	  while (ry<ryf)
      	    {

              FILL_CALL_HORIZ;

      	      ry++;
      	      xl+=ixl; xr+=ixr;
              zl+=izl; zr+=izr;

              FILL_NEXT_LINE;

      	    }
      	}

      // calculate the new coef for the next zone
      //-----------------------------------------

      if (points[left].y < points[right].y)
        {

          FILL_LEFT_CHANGING;

      	  left++;

          FILL_LEFT_CHANGED;

      	  if (points[left].y == ry)
      	    {
      	      xl = conv(points[left].x);
      	      zl = fconv(points[left].z);

              FILL_LEFT_SAME_COEF;

      	    }
      	  else
      	    {
      	      realtype deltax = conv(points[left].x) - xl;
      	      realtype deltay = conv(points[left].y - ry);
      	      realtype deltaz = fconv(points[left].z) - zl;

              FILL_LEFT_NOT_SAME_LOCALS;

      	      ixl = frapport(deltax, deltay);
      	      izl = frapport(deltaz, deltay);

              FILL_LEFT_NOT_SAME_COEF;

      	    }
       	}
      else
      	{

          FILL_RIGHT_CHANGING;

      	  if (right==0) right = npoints-1; else right--;

          FILL_RIGHT_CHANGED;

      	  if (points[right].y == ry)
      	    {
      	      xr = conv(points[right].x);
      	      zr = fconv(points[right].z);

              FILL_RIGHT_SAME_COEF;

      	    }
      	  else
      	    {
      	      realtype deltax = conv(points[right].x) - xr;
      	      realtype deltay = conv(points[right].y - ry);
      	      realtype deltaz = fconv(points[right].z) -zr;

              FILL_RIGHT_NOT_SAME_LOCALS;

      	      ixr = frapport(deltax, deltay);
      	      izr = frapport(deltaz, deltay);

              FILL_RIGHT_NOT_SAME_COEF;

      	    }
      	}
    }
  while (right >= left
         && !(right==0 && left == npoints-1)
         && !(right==0 && left==0)
        );

  #undef FILL_POINTS_ROT
  #undef FILL_LOCALS
  #undef FILL_CALL_HORIZ
  #undef FILL_NEXT_LINE
  #undef FILL_LEFT_CHANGING
  #undef FILL_LEFT_CHANGED
  #undef FILL_LEFT_SAME_COEF
  #undef FILL_LEFT_NOT_SAME_LOCALS
  #undef FILL_LEFT_NOT_SAME_COEF
  #undef FILL_RIGHT_CHANGING
  #undef FILL_RIGHT_CHANGED
  #undef FILL_RIGHT_SAME_COEF
  #undef FILL_RIGHT_NOT_SAME_LOCALS
  #undef FILL_RIGHT_NOT_SAME_COEF

