#ifndef _RES_H_
#define _RES_H_

typedef enum { Eint,Ereal,Estr,Ertab } Etype ;

typedef struct constante
{
 char nom[255];
 Etype ty ;
 void *reference;
 struct constante *next ;

} cst ;


class ConstSet
{
public :
   ConstSet(char *);
   ~ConstSet();
   void Load();
   void Add(char *,Etype,void *);
private:
   int nbcst ;
   char filename[255];

   cst *root;
   cst *queue;

   cst *cherche(char *);
};

#endif
