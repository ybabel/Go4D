#include <math.h>

#define EPSILON 0.000001

#define CROSS(dest,v1,v2){                 \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];}

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2){       \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2];}

/*
 * A function for creating a rotation matrix that rotates a vector called
 * "from" into another vector called "to".
 * Input : from[3], to[3] which both must be *normalized* non-zero vectors
 * Output: mtx[3][3] -- a 3x3 matrix in colum-major form
 * Author: Tomas Möller, 1999
 */
void fromToRotation(float from[3],float to[3],float mtx[3][3])
{
  float v[3];
  float e,h;
  CROSS(v,from,to);
  e=DOT(from,to);
  if(e>1.0-EPSILON)     /* "from" almost or equal to "to"-vector? */
  {
    /* return identity */
    mtx[0][0]=1.0; mtx[0][1]=0.0; mtx[0][2]=0.0;
    mtx[1][0]=0.0; mtx[1][1]=1.0; mtx[1][2]=0.0;
    mtx[2][0]=0.0; mtx[2][1]=0.0; mtx[2][2]=1.0;
  }
  else if(e<-1.0+EPSILON) /* "from" almost or equal to negated "to"? */
  {
    float up[3],left[3];
    float invlen;
    float fxx,fyy,fzz,fxy,fxz,fyz;
    float uxx,uyy,uzz,uxy,uxz,uyz;
    float lxx,lyy,lzz,lxy,lxz,lyz;
    /* left=CROSS(from, (1,0,0)) */
    left[0]=0.0; left[1]=from[2]; left[2]=-from[1];
    if(DOT(left,left)<EPSILON) /* was left=CROSS(from,(1,0,0)) a good choice? */
    {
      /* here we now that left = CROSS(from, (1,0,0)) will be a good choice */
      left[0]=-from[2]; left[1]=0.0; left[2]=from[0];
    }
    /* normalize "left" */
    invlen=1.0/sqrt(DOT(left,left));
    left[0]*=invlen;
    left[1]*=invlen;
    left[2]*=invlen;
    CROSS(up,left,from);
    /* now we have a coordinate system, i.e., a basis;    */
    /* M=(from, up, left), and we want to rotate to:      */
    /* N=(-from, up, -left). This is done with the matrix:*/
    /* N*M^T where M^T is the transpose of M              */
    fxx=-from[0]*from[0]; fyy=-from[1]*from[1]; fzz=-from[2]*from[2];
    fxy=-from[0]*from[1]; fxz=-from[0]*from[2]; fyz=-from[1]*from[2];

    uxx=up[0]*up[0]; uyy=up[1]*up[1]; uzz=up[2]*up[2];
    uxy=up[0]*up[1]; uxz=up[0]*up[2]; uyz=up[1]*up[2];

    lxx=-left[0]*left[0]; lyy=-left[1]*left[1]; lzz=-left[2]*left[2];
    lxy=-left[0]*left[1]; lxz=-left[0]*left[2]; lyz=-left[1]*left[2];
    /* symmetric matrix */
    mtx[0][0]=fxx+uxx+lxx; mtx[0][1]=fxy+uxy+lxy; mtx[0][2]=fxz+uxz+lxz;
    mtx[1][0]=mtx[0][1];   mtx[1][1]=fyy+uyy+lyy; mtx[1][2]=fyz+uyz+lyz;
    mtx[2][0]=mtx[0][2];   mtx[2][1]=mtx[1][2];   mtx[2][2]=fzz+uzz+lzz;
  }
  else  /* the most common case, unless "from"="to", or "from"=-"to" */
  {
#if 0
    /* unoptimized version - a good compiler will optimize this. */
    h=(1.0-e)/DOT(v,v);
    mtx[0][0]=e+h*v[0]*v[0];    mtx[0][1]=h*v[0]*v[1]-v[2]; mtx[0][2]=h*v[0]*v[2]+v[1];
    mtx[1][0]=h*v[0]*v[1]+v[2]; mtx[1][1]=e+h*v[1]*v[1];    mtx[1][2]=h*v[1]*v[2]-v[0];
    mtx[2][0]=h*v[0]*v[2]-v[1]; mtx[2][1]=h*v[1]*v[2]+v[0]; mtx[2][2]=e+h*v[2]*v[2];
#else
    /* ...otherwise use this hand optimized version (9 mults less) */
    float hvx,hvz,hvxy,hvxz,hvyz;
    h=(1.0-e)/DOT(v,v);
    hvx=h*v[0];
    hvz=h*v[2];
    hvxy=hvx*v[1];
    hvxz=hvx*v[2];
    hvyz=hvz*v[1];
    mtx[0][0]=e+hvx*v[0]; mtx[0][1]=hvxy-v[2];     mtx[0][2]=hvxz+v[1];
    mtx[1][0]=hvxy+v[2];  mtx[1][1]=e+h*v[1]*v[1]; mtx[1][2]=hvyz-v[0];
    mtx[2][0]=hvxz-v[1];  mtx[2][1]=hvyz+v[0];     mtx[2][2]=e+hvz*v[2];
#endif
  }
}
