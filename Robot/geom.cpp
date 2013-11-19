/******************************************************************************\
 * AUTHORS   : Babel Yoann
 * TITLE     : geom.cc, geom.h implementation
 * DATE      : 5/11/95
 * LAST REV  : 28/06/96
 * ACTUAL    : no convertion between quaterion, rotation, and axis rotation
 * BUGS      :
\******************************************************************************/


// matrix are now coded in the following way :
//
//   Matrix <n,m>
//
//      (0,0)   (0,1)   (0,2)  ...  (0,n-1)
//      (1,0)   (1,1)   (1,2)  ...  (1,n-1)
//      (2,0)   (2,1)   ...    ...    ...
//       ...     ...    ...    ...    ...
//      (m-1,0)  ...    ...    ... (m-1,n-1)

//   <-------n-------->
//               j
//   +----------------+   ^
//   |           .    |   |
//   |           .    |   |
// i | .  .  .  Mij . |   m
//   |           .    |   |
//   +----------------+   v


#include <assert.h>
#include <stdlib.h>
#include "geom.h"

//   <=> this(i,j)
#define elem2(i,j) fElems[j+fN*i]
#define elem1(i)   fElems[i]

/******************************************************************************\
 * Matrix
\******************************************************************************/


Matrix::Matrix(int n, int m)
{
  assert(n>0 && m>0);
  fN = n;
  fM = m;
  fNM = fN*fM;
  alloc(fElems = new real[fNM]);
  for (int i = 0; i<fNM; i++) fElems[i] = 0;
}

Matrix::Matrix(const Matrix & m)
{
  fN = m.fN;
  fM = m.fM;
  fNM = m.fNM;
  alloc(fElems = new real[fNM]);
  for (int i = 0; i<fNM; i++) fElems[i] = m.fElems[i];
}

Matrix::~Matrix()
{
  delete [] fElems;
}

real & Matrix::operator [] (int i) const
{
  assert(i>=0&&i<fNM);
  return elem1(i);
}

real & Matrix::operator () (int i, int j) const
{
  assert(i>=0&&i<fM);
  assert(j>=0&&j<fN);
  return elem2(i,j);
}

Matrix & Matrix::operator = (const Matrix & m)
{
  assert(fN==m.fN);
  assert(fM==m.fM);
  if (&m == this) return *this;
  for (int i = 0; i<fNM; i++)
    fElems[i] = m.fElems[i];
  return *this;
}

Matrix Matrix::operator+(const Matrix& m) const
{
  Matrix result(fN, fM);

  for(int i=0; i<fM; i++)
    for (int j=0; j<fN; j++)
      result(i,j)=elem2(i,j)+m(i,j);
  return result;
}

Matrix Matrix::operator-(const Matrix& m) const
{
  Matrix result(fN, fM);

  for(int i=0; i<fM; i++)
    for (int j=0; j<fN; j++)
      result(i,j)=elem2(i,j)-m(i,j);
  return result;
}

Matrix Matrix::operator * (const Matrix & m) const
{
  // result = this * m
  assert(fN==m.fM);
  Matrix result(m.fN, fM);

  for (int j=0; j<m.fN; j++)
    for (int i=0; i<fM; i++)
    {
	    real tmp = 0;
     	for (int k=0; k<fN; k++) //m.fM==fN
	      tmp += elem2(i,k)*m(k,j);
     	result(i,j) = tmp;
    }
  return result;
}

Matrix Matrix::operator*(real a) const
{
  Matrix result(fN, fM);

  for(int i=0; i<fM; i++)
    for (int j=0; j<fN; j++)
      result(i,j) = elem2(i,j)*a;
  return result;
}

Matrix Matrix::operator ! () const
{
  Matrix result(fM,fN);
  for (int i=0; i<fN; i++)
    for (int j=0; j<fM; j++)
      result(i,j) = elem2(j,i);
  return result;
}

Matrix Matrix::operator - () const
{
  Matrix result(fN,fM);
  for (int i=0; i<fM; i++)
    for (int j=0; j<fN; j++)
      result(i,j) = -elem2(i,j);
  return result;
}

ostream & operator << (ostream & os, const Matrix & m)
{
  os.setf(ios::dec);
  for (int i=0; i<m.fM; i++)
  {
    for (int j=0; j<m.fN; j++)
	    os << m(i,j) << " ";
    os << kCR;
  }
  return os;
}

Matrix & Matrix::operator += (const Matrix& m)
{
  assert(fN == m.fN);
  assert(fM == m.fM);
  for(int i=0; i<fM; i++)
    for (int j=0; j<fN; j++)
      elem2(i,j)+=m(i,j);
  return *this;
}

Matrix & Matrix::operator -= (const Matrix& m)
{
  assert(fN == m.fN);
  assert(fM == m.fM);
  for(int i=0; i<fM; i++)
    for (int j=0; j<fN; j++)
      elem2(i,j)-=m(i,j);
  return *this;
}

Matrix & Matrix::operator *= (const Matrix & m)
{
  assert(fN == m.fN);
  assert(fM == m.fM);
  assert(fN == m.fM);
  Matrix result(fN, fM);

  for (int j=0; j<m.fN; j++)
    for (int i=0; i<fM; i++)
    {
	    real tmp = 0;
     	for (int k=0; k<fN; k++) //m.fM==fN
	      tmp += elem2(i,k)*m(k,j);
     	result(i,j) = tmp;
    }
  for (int j=0; j<m.fN; j++)
    for (int i=0; i<fM; i++)
      elem2(i,j) = result(i,j);
  return *this;
}

Matrix & Matrix::operator*=(real a)
{
  for(int i=0; i<fM; i++)
    for (int j=0; j<fN; j++)
      elem2(i,j)*=a;
  return *this;
}

/******************************************************************************\
 * Matrix4x4
\******************************************************************************/

/******************************************************************************\
 * Matrix3x3
\******************************************************************************/

/******************************************************************************\
 * Rotation
\******************************************************************************/

Rotation & Rotation::RotateX(real psy)
{
  elem2(0,0)=1;  elem2(0,1)=0;          elem2(0,2)=0;
  elem2(1,0)=0;  elem2(1,1)=cos(psy);   elem2(1,2)=-sin(psy);
  elem2(2,0)=0;  elem2(2,1)=sin(psy);   elem2(2,2)=cos(psy);
  return *this;
}

Rotation & Rotation::RotateY(real phi)
{
  elem2(0,0)=cos(phi);   elem2(0,1)=0;  elem2(0,2)=-sin(phi);
  elem2(1,0)=0;          elem2(1,1)=1;  elem2(1,2)=0;
  elem2(2,0)=sin(phi);   elem2(2,1)=0;  elem2(2,2)=cos(phi);
  return *this;
}

Rotation & Rotation::RotateZ(real teta)
{
  elem2(0,0)=cos(teta);  elem2(0,1)=-sin(teta);  elem2(0,2)=0;
  elem2(1,0)=sin(teta);  elem2(1,1)=cos(teta);   elem2(1,2)=0;
  elem2(2,0)=0;          elem2(2,1)=0;           elem2(2,2)=1;
  return *this;
}

Rotation & Rotation::RotateEuler(real teta, real phi, real psy)
{
  Rotation rX, rY, rZ;
  rX.RotateX(teta);
  rY.RotateY(phi);
  rZ.RotateZ(psy);
  *this = (rX*rY*rZ);
  return *this;
}

/******************************************************************************\
 * Vector
\******************************************************************************/


real & Vector::operator [] (int i) const
{
  assert(i>=0&&i<fM);
  return fElems[i];
}

real & Vector::operator () (int i) const
{
  assert(i>=0&&i<fM);
  return fElems[i];
}

Vector Vector::operator + (const Vector & v) const
{
  Vector result(fM);
  for (int i=0; i<fM; i++)
    result[i] = elem1(i)+v[i];
  return result;
}

Vector Vector::operator - (const Vector & v) const
{
  Vector result(fM);
  for (int i=0; i<fM; i++)
    result[i] = elem1(i)-v[i];
  return result;
}

Vector Vector::operator * (real r) const
{
  Vector result(fM);
  for (int i=0; i<fM; i++)
    result[i] = elem1(i)*r;
  return result;
}

real Vector::operator * (const Vector & v) const
{
  real result = 0;
  for (int i=0; i<fM; i++)
    result += elem1(i)*v[i];
  return result;
}

/******************************************************************************\
 *  Vector3
\******************************************************************************/

Vector3::Vector3(real a, real b, real c):Vector(3)
{
  elem1(0)=a;
  elem1(1)=b;
  elem1(2)=c;
}

Vector3 Vector3::operator^(const Vector3& v) const
{
  return Vector3(elem1(1)*v[2] - elem1(2)*v[1],
		 elem1(2)*v[0] - elem1(0)*v[2],
		 elem1(0)*v[1] - elem1(1)*v[0]);
}

void Vector3::Set(real a, real b, real c)
{
  elem1(0) = a;
  elem1(1) = b;
  elem1(2) = c;
}

real Vector3::Norme() const
{
  return sqrt(sqr(elem1(0))+sqr(elem1(1))+sqr(elem1(2)));
}

bool Vector3::Normalize()
{
  real norme = Norme();
  assert(norme>0);
  elem1(0) /= norme;
  elem1(1) /= norme;
  elem1(2) /= norme;
  return true;
}

real Vector3::CosAngle(const Vector3& v) const
{
  real p = (this->Norme())*v.Norme();
  assert(p>0);
  return ((*this)*v)/p;
}

real Vector3::SinAngle(const Vector3& v) const
{
  real p = (this->Norme())*v.Norme();
  assert(p>0);
  return (((*this)^v).Norme())/p;
}

/******************************************************************************\
 * Point3D
\******************************************************************************/

real Point3D::Distance(const Point3D & A) const
{
  return sqrt(sqr(elem1(0)-A[0])+sqr(elem1(1)-A[1])+sqr(elem1(2)-A[2]));
}


/******************************************************************************\
 * Quaternion
\******************************************************************************/

Quaternion Quaternion::operator * (const Quaternion & q) const
{
  Quaternion result;
  result[0] = q[0]*elem1(0) - q[1]*elem1(1) - q[2]*elem1(2) - q[3]*elem1(3);
  result[0] = q[0]*elem1(1) + q[1]*elem1(0) - q[2]*elem1(3) + q[3]*elem1(2);
  result[0] = q[0]*elem1(2) + q[1]*elem1(3) + q[2]*elem1(0) - q[3]*elem1(1);
  result[0] = q[0]*elem1(3) - q[1]*elem1(2) + q[2]*elem1(1) + q[3]*elem1(0);
  return result; // = elem1(Q))*q
}

real Quaternion::Real() const
{
  return elem1(0);
}

Vector3 Quaternion::Imag() const
{
  Vector3 result;
  result[0] = elem1(1);
  result[1] = elem1(2);
  result[2] = elem1(3);
  return result;
}

Matrix4x4 Quaternion::Q() const
{
  Matrix4x4 result;
  result(0,0)=elem1(0);
  result(0,1)=-elem1(1);
  result(0,2)=-elem1(2);
  result(0,3)=-elem1(3);

  result(1,0)=elem1(1);
  result(1,1)= elem1(0);
  result(1,2)=-elem1(3);
  result(1,3)= elem1(2);

  result(2,0)=elem1(2);
  result(2,1)= elem1(3);
  result(2,2)= elem1(0);
  result(2,3)=-elem1(1);

  result(3,0)=elem1(3);
  result(3,1)=-elem1(2);
  result(3,2)= elem1(1);
  result(3,3)= elem1(0);
  return result;
}

Matrix4x4 Quaternion::W() const
{
  Matrix4x4 result;
  result(0,0)=elem1(0);
  result(0,1)=-elem1(1);
  result(0,2)=-elem1(2);
  result(0,3)=-elem1(3);

  result(1,0)=elem1(1);
  result(1,1)= elem1(0);
  result(1,2)= elem1(3);
  result(1,3)=-elem1(2);

  result(2,0)=elem1(2);
  result(2,1)=-elem1(3);
  result(2,2)= elem1(0);
  result(2,3)= elem1(1);

  result(3,0)=elem1(3);
  result(3,1)= elem1(2);
  result(3,2)=-elem1(1);
  result(3,3)= elem1(0);
  return result;
}


/******************************************************************************\
 * Plane
\******************************************************************************/

Plane::Plane(const Vector3 &v1, const Vector3 & v2, const Vector3 & v3):Vector(4)
{
  Vector3 tmp1 = v1-v2;
  Vector3 tmp2 = v1-v3;
  Vector3 tmp = tmp1^tmp2;
  real norme = tmp.Norme();
  assert(norme!=0);
  elem1(0) = tmp[0]/norme;
  elem1(1) = tmp[1]/norme;
  elem1(2) = tmp[2]/norme;
  elem1(3) = tmp*v1/norme;
}

Plane::Plane(const Vector3 & v, real d):Vector(4)
{
  elem1(0) = v[0];
  elem1(1) = v[1];
  elem1(2) = v[2];
  elem1(3) = d;
}

Plane::Plane(const Vector3 &n, const Vector3 & v):Vector(4)
{
  //assume that the vector is normed
  elem1(0) = n[0];
  elem1(1) = n[1];
  elem1(2) = n[2];
  elem1(3) = -(elem1(0)*v[0]+elem1(1)*v[1]+elem1(2)*v[2]);
}

Vector3 Plane::Normal() const
{
  return Vector3(elem1(0), elem1(1), elem1(2));
}

real Plane::GetDist() const
{
  return elem1(3);
}

real Plane::Dist(const Vector3 & v) const
{
  return elem1(0)*v[0]+elem1(1)*v[1]+elem1(2)*v[2]+elem1(3);
}

Vector3 Plane::Cut(const Vector3 &v1, const Vector3 & v2,
		  real d1, real d2, real & coord) const
{ //be careful to the cutprecision (common.h) value
  //it can cause some precision problems here
  Vector3 tmp = v2-v1;
  #ifndef NDEBUG
  real norme = tmp.Norme();
  #endif
  assert(d1*d2 <= 0); // the plane must cut the segment
  coord = 0;
  if ( fabs(d1) <cutprecision) return v1;
  coord = 1;
  if ( fabs(d2) <cutprecision) return v2;
  assert( norme > 2*cutprecision );
  assert( fabs(d1)<norme);
  assert( fabs(d2)<norme);

  real scal = elem1(0)*tmp[0]+elem1(1)*tmp[1]+elem1(2)*tmp[2];
  assert(scal!=0);
  coord = -d1/scal;
  assert(coord>0); assert(coord<1);

  Vector3 result;
  result[0] = coord*tmp[0]+v1[0];
  result[1] = coord*tmp[1]+v1[1];
  result[2] = coord*tmp[2]+v1[2];
  assert( fabs(Dist(result)) < cutprecision );
  return result;
}

/******************************************************************************\
 *  Base
\******************************************************************************/

Base::Base()
{
  origin[0]=0;  origin[1]=0;  origin[2]=0;
  matrx(0,0)=1; matrx(0,1)=0; matrx(0,2)=0;
  matrx(1,0)=0; matrx(1,1)=1; matrx(1,2)=0;
  matrx(2,0)=0; matrx(2,1)=0; matrx(2,2)=1;
}

Base::Base(real Ox,  real Oy,  real Oz,
  real a11, real a12, real a13,
  real a21, real a22, real a23,
	real a31, real a32, real a33)
: origin(Ox, Oy, Oz)
{
  matrx(0,0)=a11; matrx(0,1)=a12; matrx(0,2)=a13;
  matrx(1,0)=a21; matrx(1,1)=a22; matrx(1,2)=a23;
  matrx(2,0)=a31; matrx(2,1)=a32; matrx(2,2)=a33;
}

Base::Base(BaseCoefs coefs)
// a verifier !!
{
  origin[0]= coefs[0][0];  origin[1]= coefs[1][0];  origin[2]= coefs[2][0];
  matrx(0,0)=coefs[0][1];  matrx(0,1)=coefs[0][2];  matrx(0,2)=coefs[0][3];
  matrx(1,0)=coefs[1][1];  matrx(1,1)=coefs[1][2];  matrx(1,2)=coefs[1][3];
  matrx(2,0)=coefs[2][1];  matrx(2,1)=coefs[2][2];  matrx(2,2)=coefs[2][3];
}

Base::Base(real Ox,   real Oy,  real Oz,
	real teta, real phi, real psy)
:origin(Ox, Oy, Oz)
{
  matrx.RotateEuler(teta,phi,psy);
}

Base::Base(const Base& newbase)
: origin(newbase.origin), matrx(newbase.matrx)
{ }

Base::Base(const Point3D & o, const Rotation & r)
: origin(o), matrx(r)
{ }

Base::~Base()
{ }


void Base::ChgPointToOwnBase(Point3D& M) const
{
  M = M - origin;
  M = matrx * M;
}


void Base::ChgPointToMainBase(Point3D& M) const
{
  M = (!matrx) * M;
  M = M + origin;
}

void Base::ChgPointToBase(Point3D& M,Base& OldBase) const
{
  OldBase.ChgPointToMainBase(M);
  ChgPointToOwnBase(M);
}

void Base::ChgVectorToOwnBase(Vector3& v) const
{
  v = matrx * v;
}


void Base::ChgVectorToMainBase(Vector3& v) const
{
  v = (!(matrx)) * v;
}

void Base::ChgVectorToBase(Vector3& v,Base& OldBase) const
{
  OldBase.ChgVectorToMainBase(v);
  ChgVectorToOwnBase(v);
}


/******************************************************************************\
 *  Line
\******************************************************************************/

Line::Line()
{
  origin.Set(0,0,0);
  direction.Set(1,1,1);
}


Line::Line(Point3D& A,Vector3& u)
{
  origin = A;
  direction = u;
  assert(direction.Norme()>0);
  direction.Normalize(); // -> v.Norme() = 1
}

Line::~Line()
{ }

void Line::Set(Point3D& A,Vector3& u)
{
  origin.Set(A[0],A[1],A[2]);
  direction.Set(u[0],u[1],u[2]);
  assert(direction.Norme()>0);
  direction.Normalize();
}

real Line::Distance(const Point3D& A) const
{
  return (((Vector3)(A-origin))^direction).Norme();
}

