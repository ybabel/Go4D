#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "common.h"
#include "res.h"

ConstSet::ConstSet(char *nom)
{
   strcpy(filename,nom);
   nbcst=0;
   root=NULL;
   queue=NULL;
}

ConstSet::~ConstSet()
{
  cst * tmp;
  for (int i=0 ; i< nbcst ; i++ )
    {
      tmp=root->next ;
      delete root ;
      root=tmp ;
    }
}
void ConstSet::Add(char *nom,Etype ty,void *ref)
{
 if (queue==NULL)
   {
	 queue=root=new cst ;
	 strcpy(root->nom,nom);
	 root->ty=ty;
	 root->reference=ref ;

	 root->next=NULL ;
   }
 else
   {
	 cst *tmp=new cst ;
	 strcpy(tmp->nom,nom);
	 tmp->ty=ty ;
	 tmp->reference=ref ;
	 tmp->next=NULL ;
	 queue->next=tmp ;
	 queue=tmp ;
   }

 nbcst++ ;
}

cst *ConstSet::cherche(char *chaine)
{
 if (root==NULL) return NULL ;
 for (cst *tmp=root ; tmp!=NULL ; tmp=tmp->next )
 {

  if (!stricmp(chaine,tmp->nom))
	   return tmp ;
 }
 return NULL ;
}

void ConstSet::Load()
{
  ::string chaine;
  real * k;
  int * i;
  char * s;
  real *rtab ;
  int nb,w ;

 ifstream h(filename);
 while (1)
   {
	 h >> chaine;
	 if (chaine[0]=='-') break;
	 cst *trouve=cherche(chaine);
	 if (trouve==NULL)
	 {
	   //cout << "chaine :" << chaine << " non trouvee\n" ;
           //MessageBox(0,chaine,"string not found",MB_OK);
           return;
	 }
	 switch (trouve->ty)
   	{

      case Ereal :
        k = (real *)trouve->reference ;
			  h >> *k;
			  break ;
		 case Eint  : i =  (int *)trouve->reference ;
					  h >> *i ;
					  break;
		 case Estr  : s = (char *)trouve->reference ;
					  //h >> s ;
            h.getline(s, 255);
					  break;
    case Ertab : rtab= (float *)trouve->reference ;
                  h >> nb ;
                 for (w=0 ; w<nb ; w++)
                   {
                       h >> rtab[w];
                   } ; break;
		 default : //cout << " type inconnu : " << trouve->ty <<"\n" ;
        //MessageBox(0,chaine,"unknown type",MB_OK);
        return;
		}
   }
}



