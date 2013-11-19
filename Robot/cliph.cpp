/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : cliph.cc, horizontal line clipping
 *CREATED: 29/10/95
 *DATE   : 18/03/96
 *DEFS   : gouraud, flat, mapping, zbuffer, quadmapping, sbuffer, sbufferclip
 *         hypmapping
 *BUGS   : 
\*****************************************************************************/

/*****************************************************************************\
 *ClipH(realtype xb, realtype xe, int y,
 *      int color, //flat
 *      realtype cb, realtype ce, //gouraud
 *      realtype zb, realtype ze, //zbuffer, quadmapping
 *      realtype mxb, realtype myb, 
 *      realtype mxe, realtype mye, Image & ima // mapping
 *     )
 *
 *clipping is done between xmin, xmax, ymin, ymax
\*****************************************************************************/

  // Variables declaration
  //----------------------

  #ifndef sbuffer
  int xd, xf; // roundf(xb), roundf(xe) after clipping
  #ifdef zbuffer
  realtype z, iz;
  #endif
  #ifdef gouraud
  realtype c, ic;
  #endif
  #ifdef mapping
  realtype mx, imx, my, imy;
  #endif
  #ifdef quadmapping
  realtype l, mx, my, tx, ty, ax, ay;
  #endif
  #ifdef hypmapping
  #endif
  #endif

  // Clipping and ordering
  //----------------------

  int rxb = round(xb); 
  int rxe = round(xe);

  // realy need to draw ?
  if ( abs(xe-xb) < conv(1)) 
    #ifdef sbuffer 
    return TRUE;
    #else 
    return;
    #endif
  // test if the entire line is out of the screen
  if ( (y<ymin)||(y>=ymax) 
       || (rxb<xmin)&&(rxe<xmin) 
       || (rxb>=xmax)&&(rxe>=xmax) ) 
    #ifdef sbuffer
    return TRUE;
    #else 
    return;
    #endif

  // ensure that rxb < rxe
  if (xb>xe) 
    {
      Swap(rxb,rxe);
      SwapR(xb,xe);
      SwapR(zb, ze);
      #ifdef gouraud
      SwapR(cb, ce);
      #endif
      #ifdef mapping
      SwapR(mxb, mxe); SwapR(myb, mye);
      #endif
      #ifdef quadmapping
      SwapR(mxb, mxe); SwapR(myb, mye);
      #endif
      #ifdef hypmapping
      SwapR(mxb, mxe); SwapR(myb, mye);
      #endif
    }

  // coeficiants initilization
  #ifdef zbuffer
  iz = frapport( (ze-zb), (xe-xb) );
  z = zb;
  #endif
  #ifdef gouraud
  ic = frapport( (ce-cb), (xe-xb) );
  c = cb;
  #endif
  #ifdef mapping
  imx = frapport( (mxe-mxb), (xe-xb) );
  mx = mxb;
  imy = frapport( (mye-myb), (xe-xb) );
  my = myb;
  #endif
  #ifdef quadmapping
  assert(zb!=0);
  assert(ze!=0);
  assert( (zb>0&&ze>0)||(zb<0&&ze<0) );
  l = frapport(ze,zb);
  #endif
  #ifdef hypmapping
  #endif

  // clipping is effectivelly done here
  xd=rxb, xf=rxe;
  if (rxb<xmin)
    {
      #ifdef zbuffer
      z += iz*(xmin-rxb);
      #endif
      #ifdef gouraud
      c += ic*(xmin-rxb);
      #endif
      #ifdef mapping
      mx += imx*(xmin-rxb);
      my += imy*(xmin-rxb);
      #endif
      #ifdef quadmapping
      // clipping is automatically done with coef initialization
      #endif
      #ifdef hypmapping
      // no clipping for the moment
      #endif
      xd = xmin;
    }
  if (rxe>xmax-1) // because horiz go to xf+1  (because xf=round(xe))
    xf = xmax-1;

  if (xd==xf)
    #ifdef sbuffer 
    return TRUE;
    #else 
    return;
    #endif

  #ifdef sbuffer
  return FALSE;
  #endif
