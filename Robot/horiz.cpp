/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : horiz.cc, horizontal line drawing, no clipping (see cliph.cc)
 *CREATED: 22/10/95
 *DATE   : 18/03/96
 *DEFS   : gouraud, flat, mapping, zbuffer, quadmapping, hypmapping
 *BUGS   : xf+1 cause bug under linux vga
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


  // Asssertions (only for mapping)
  //-------------------------------

  #ifdef mapping
  assert(ima != NULL);
  assert(round(mxb)>=-1 && round(mxb)<ima->fWidth);
  assert(round(mxe)>=-1 && round(mxe)<ima->fWidth);
  assert(round(myb)>=-1 && round(myb)<ima->fHeight);
  assert(round(mye)>=-1 && round(mye)<ima->fHeight);
  assert(round(mxb)+ima->fWidth*round(myb)<ima->fLen);
  assert(round(mxe)+ima->fWidth*round(mye)<ima->fLen);
  int bl=ima->fWidth;
  #endif
  #ifdef quadmapping
  assert(ima != NULL);
  assert(round(mxb)>=-1 && round(mxb)<ima->fWidth);
  assert(round(mxe)>=-1 && round(mxe)<ima->fWidth);
  assert(round(myb)>=-1 && round(myb)<ima->fHeight);
  assert(round(mye)>=-1 && round(mye)<ima->fHeight);
  assert(round(mxb)+ima->fWidth*round(myb)<ima->fLen);
  assert(round(mxe)+ima->fWidth*round(mye)<ima->fLen);
  int bl=ima->fWidth;
  #endif
  #ifdef hypmapping
  assert(ima != NULL);
  assert(round(mxb)>=-1 && round(mxb)<ima->fWidth);
  assert(round(mxe)>=-1 && round(mxe)<ima->fWidth);
  assert(round(myb)>=-1 && round(myb)<ima->fHeight);
  assert(round(mye)>=-1 && round(mye)<ima->fHeight);
  assert(round(mxb)+ima->fWidth*round(myb)<ima->fLen);
  assert(round(mxe)+ima->fWidth*round(mye)<ima->fLen);
  int bl=ima->fWidth;
  #endif


  // Drawing of the line
  //--------------------

  // pointers declaration
  byte * Cur = fBuffer+fWidth*y+xd;
  #ifdef zbuffer
  realtype *CurZ = fZBuffer+fWidth*y+xd;
  #endif
  #ifdef mapping
  byte * Ima = ima->fBuffer;
  #endif
  #ifdef quadmapping
  byte * Ima = ima->fBuffer;
  QuadMapCoefsInit(xb, xe, l, conv(xd+1), mxb, mxe, myb, mye, 
		               mx, my, tx, ty, ax, ay);
  #endif
  #ifdef hypmapping
  byte * Ima = ima->fBuffer;
  int ux = 0, uy = 0;
  int smx=sign(mxe-mxb), smy=sign(mye-myb);
  int rmxe=round(mxe)-smx, rmye=round(mye)-smy;
  int rmxb=round(mxb), rmyb=round(myb);
  int rze=round(ze), rzb=round(zb);
  int rden=rzb*(rxb-(xd+1))+rze*((xd+1)-rxe);
  int rmx=(rzb*rmxe*(rxb-(xd+1))+rze*rmxb*((xd+1)-rxe))/rden;
  int rmy=(rzb*rmye*(rxb-(xd+1))+rze*rmyb*((xd+1)-rxe))/rden;
  int p=rzb*(rxb-(xd+1))+rze*((xd+1)-rxe);
  int ip=rze-rzb;
  int qx=smx*(rzb*(rmxe-rmx)+rze*(rmx-rmxb));
  int iqx=(rze-rzb);
  int qy=smy*(rzb*(rmye-rmy)+rze*(rmy-rmyb));
  int iqy=(rze-rzb);
  #endif

  // main loop
  //  xd+1 because round(xb)+1   x must be >= xb (k>=0)
  //---------------------------------------------------

  for (int x=xd+1; x<=xf; x++, Cur++)
    {
      #ifdef zbuffer
      if (z< *CurZ)
      	{
      #endif

          //assert(0 <= (Cur-fBuffer) && (Cur-fBuffer) < fLen);
      	  #ifdef flat
      	  *Cur = color;
      	  #endif
      	  #ifdef gouraud
      	  *Cur = byte(round(c));
      	  #endif
      	  #ifdef mapping
      	  *Cur = *(Ima+ round(mx)+ bl*round(my));
      	  #endif
      	  #ifdef quadmapping
      	  *Cur = *(Ima+ round(mx)+ bl*(round(my)));
      	  #endif
      	  #ifdef hypmapping
      	  *Cur = *(Ima+ rmx+ bl*rmy);
      	  #endif

      #ifdef zbuffer
      	  *CurZ = z;
       	}
      z += iz;
      CurZ++;
      #endif

      #ifdef gouraud
      c += ic;
      #endif
      #ifdef mapping
      mx += imx;
      my += imy;
      #endif
      #ifdef quadmapping
      mx += tx;
      tx += ax;
      my += ty;
      ty += ay;
      #endif
      #ifdef hypmapping
      //ux += smx*(rzb*(rmxe-rmx)+rze*(rmx-rmxb));
      ux += qx;
      while (ux>0)
      	{
      	  rmx+=smx;
      	  qx +=iqx;
      	  //ux += rzb*(rxb-x)+rze*(x-rxe);
      	  ux += p;
      	}
      //uy += smy*(rzb*(rmye-rmy)+rze*(rmy-rmyb));
      uy += qy;
      while (uy>0)
      	{
      	  rmy+=smy;
      	  qy +=iqy;
      	  //uy += rzb*(rxb-x)+rze*(x-rxe);
      	  uy += p;
      	}
      p += ip;
      #endif
    }
