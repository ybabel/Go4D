/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : memmgm.h
 *CREATED: 22/10/95
 *DATE   : 08/03/96
 *OBJECT : memory managment (should be musch faster)
\*****************************************************************************/


/*****************************************************************************\
 *
 *MEMMANAGER : alocated and free ElemType. When you want to free some memory 
 *  the memory manager keep it in case you will need it later.
 *  free ElemType only when it's absolutely necessary. The pointer toward 
 *  the ElemTypes are allocated in an array that grow when it's necessary.
 *  elements memmananger acts like a stack.
 *FreeAll : free all the allocated memory
 *Purge : all the ElemType used are marked unused
 *New : get a new ElemType pointer
 *DeleteLast : as Delete, the ElemType marked is the last allocated by New
 *Grow : make the size of the array grow.
 *elems : the array containing the pointers on the elements
 *nballocmax : the current size of the array
 *
\*****************************************************************************/

#ifndef _MEMMGM_H_
#define _MEMMGM_H_

#define GROWSPEED 32;

template <class ElemType> class MemManager
{
  typedef ElemType * PElemType;
  public :
  MemManager()
    {
      pelems = NULL;
      nballoc = 0;
      nballocmax = 0;
      lastnew = 0;
    }

  ~MemManager()
    {
      FreeAll();
    }

  void Purge()
    {
      nballoc = 0;
    }

  ElemType * New(const ElemType & elem)
    {
      lastnew = FindFirstFree();
      *(pelems[lastnew]) = elem;
      return pelems[lastnew];
    }

  void DeleteLast()
    {
      nballoc--;
    }


  private :
  int FindFirstFree()
    {
      int i=nballoc++;
      if (i==nballocmax) Grow();
      return i;
    }

  void Grow()
    {
      int oldnballocmax = nballocmax;
      nballocmax+=GROWSPEED;
      PElemType * newpelems;
      alloc( newpelems = new PElemType[nballocmax] );
      // new call the default constructor so we must initialize the new array
      // with old pelems, the new elems are created.
      for (int i=0; i<oldnballocmax; i++)
	newpelems[i] = pelems[i];
      for (int j=oldnballocmax; j<nballocmax; j++)
	newpelems[j] = new ElemType;
      if (pelems != NULL) delete [] pelems;
      pelems = newpelems;
    }
  
  void FreeAll()
    {
      if (pelems==NULL) return;
      for (int i=0; i<nballocmax; i++)
	delete pelems[i];
      delete [] pelems;
    }
  
  PElemType * pelems; // array of PElems pointer
  int nballoc, nballocmax;
  int lastnew;
};

#endif


