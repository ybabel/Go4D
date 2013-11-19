/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : horiz.cc, horizontal line drawing, no clipping (see cliph.cc)
 *CREATED: 22/10/95
 *DATE   : 18/03/96
 *DEFS   : gouraud, flat, mapping, zbuffer, quadmapping, hypmapping
 *BUGS   : xf+1 cause bug when imx or imx is great
\*****************************************************************************/

/*****************************************************************************\
 *Horiz(realtype xb, realtype xe, int y,
 *      int color, //flat
 *      realtype cb, realtype ce, //gouraud
 *      realtype zb, realtype ze, //zbuffer, quadmapping, hypmapping
 *      realtype mxb, realtype myb, //mapping
 *      realtype mxe, realtype mye, const Image * ima // mapping
 *     )
 *
 *local variables :
 *int xb, xe, xd, xf;
 *realtype z, iz; // zbuffer
 *realtype c, ic; // gouraud shading
 *realtype mx, imx, my, imy; //mapping
 *realtype l, mx, my, tx, ty, ax, ay; //quadmapping
 *int ux,uy,smx,smy=s,rmxe,rmye,rmxb,rmyb,rze,rzb,rden,rmx,rmy; //hypmapping
 *int p,ip,qx,iqx,qy,iqy; //hypmapping
 *
\*****************************************************************************/

  #ifndef HORIZ_ASSERTIONS
  #define HORIZ_ASSERTIONS
  #endif

  #ifndef HORIZ_LOCALS
  #define HORIZ_LOCALS
  #endif

  #ifndef HORIZ_SWAP
  #define HORIZ_SWAP
  #endif

  #ifndef HORIZ_COEF_INIT
  #define HORIZ_COEF_INIT
  #endif

  #ifndef HORIZ_CLIP
  #define HORIZ_CLIP
  #endif

  #ifndef HORIZ_LOCALS_CLIPPED
  #define HORIZ_LOCALS_CLIPPED
  #endif

  #ifndef HORIZ_PUT_PIXEL
  #define HORIZ_PUT_PIXEL
  #endif

  #ifndef HORIZ_NEXT_PIXEL
  #define HORIZ_NEXT_PIXEL
  #endif





  // Asssertions (only for mapping)
  //-------------------------------
  HORIZ_ASSERTIONS;

  // Variables declaration
  //----------------------

  int xd, xf; // roundf(xb), roundf(xe) after clipping

  HORIZ_LOCALS;

  #ifdef zbuffer
  realtype z, iz;
  #endif

  // Clipping and ordering
  //----------------------

  int rxb = round(xb);
  int rxe = round(xe);

  // realy need to draw ?
  if ( abs(xe-xb) < conv(1))
    return;
  // test if the entire line is out of the screen
  if ( (y<ymin)||(y>=ymax)
       || (rxb<xmin)&&(rxe<xmin)
       || (rxb>=xmax)&&(rxe>=xmax) )
    return;

  // ensure that rxb < rxe
  if (xb>xe)
  {
    Swap(rxb,rxe);
    SwapR(xb,xe);
    SwapR(zb, ze);

    HORIZ_SWAP;

  }

  HORIZ_COEF_INIT;

  // coeficiants initilization
  #ifdef zbuffer
  iz = frapport( (ze-zb), (xe-xb) );
  z = zb;
  #endif

  // clipping is effectivelly done here
  xd=rxb, xf=rxe;
  if (rxb<xmin)
  {

    HORIZ_CLIP;

    #ifdef zbuffer
    z += iz*(xmin-rxb);
    #endif
    xd = xmin;
  }
  if (rxe>xmax-1) // because horiz go to xf+1  (because xf=round(xe))
    xf = xmax-1;

  if (xd==xf)
    return;


  // Drawing of the line
  //--------------------

  HORIZ_LOCALS_CLIPPED;

  // pointers declaration
  Color * Cur = fBuffer+Width()*y+xd;
  #ifdef zbuffer
  realtype *CurZ = fZBuffer+Width()*y+xd;
  #endif

  // main loop
  //  xd+1 because round(xb)+1   x must be >= xb (k>=0)
  //---------------------------------------------------
  for (int x=xd+1; x<=xf; x++, Cur++)
  //for (int x=xd+1; x<=xf+1; x++, Cur++) fill the lacking space but bug some time
  {

    HORIZ_PUT_PIXEL;

    #ifdef zbuffer
    if (z< *CurZ)
      {
    #endif

        assert(int(Cur-GetBuffer())>=0);
        assert(int(Cur-GetBuffer())<Len());

    #ifdef zbuffer
        *CurZ = z;
      }
    z += iz;
    CurZ++;
    #endif

    HORIZ_NEXT_PIXEL;

  }

  #undef HORIZ_ASSERTIONS
  #undef HORIZ_LOCALS
  #undef HORIZ_SWAP
  #undef HORIZ_COEF_INIT
  #undef HORIZ_CLIP
  #undef HORIZ_LOCALS_CLIPPED
  #undef HORIZ_PUT_PIXEL
  #undef HORIZ_NEXT_PIXEL

