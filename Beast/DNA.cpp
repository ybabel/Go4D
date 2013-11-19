#include "Common.h"
#include "DNA.h"
#include <string.h>

/*****************************************************************************\
*  Coef
\*****************************************************************************/

Coef::Coef(const char *name, real value) {
  assert(name!=NULL);
  assert(strlen(name)>0);
  fname = NewString(name);
  fvalue = value;
}


Coef::~Coef() {
  delete fname;
}


real Coef::Value() {
  return fvalue;
}


char *Coef::Name() {
  return fname;
}

void Coef::ValueIs(real value) {
  fvalue = value;
}



/*****************************************************************************\
*  IDNA
\*****************************************************************************/

GenomeType * IDNA::GetDNA() 
{ 
  return &_Genome; 
}

/*
GenomeType & IDNA::Clone()
{
//return GetDNA().clone(); 
}
*/

#ifndef NDEBUG
void IDNA::showDNA() 
{
  Array<Coef *> *cChromosome = NULL;
  Coef * cCoef = NULL;

  cout << endl << "DNA: {" << incendl;  
  for(int idx=0; idx<_Genome.Count(); idx++)
  {
    cChromosome = _Genome[idx];
    int count = cChromosome->Count();
    if(count>0) {
      cout << "[RULE:" << idx << "]" << incendl;
      for(int jdx=0; jdx<count; jdx++)
      {
        cCoef = (*cChromosome)[jdx];
        cout << cCoef->Name() << "=" << cCoef->Value() << iendl;
      }
      cout << decendl;
    }
  }
  cout << "}" << decendl;
}
#endif
