#ifndef _DNA_H_
#define _DNA_H_

#include "List.h"

using namespace Go4D;

///
class Coef {
protected:
  char *fname;
  real fvalue;

public:
  ///
  Coef(const char *name, real value=0.0);
  ///
  ~Coef();

  ///
  real Value();
  ///
  char *Name();
  ///
  void ValueIs(real value);
};



typedef Array<Array<Coef *> *> GenomeType;



///
class IDNA {
private:
  GenomeType _Genome;

public:
  /*
  virtual void ApplyOnArrayOfCoefs(Array<Coef *> *AofC);
  virtual void ApplyOnCoef(Coef *)=0;

  GenomeType & Clone();
  */
  ///
  GenomeType *GetDNA();
  ///
  virtual void ModifyDNA(real globalpercentage)=0;

#ifndef NDEBUG
public: 
  void showDNA();
#endif
};

#endif
