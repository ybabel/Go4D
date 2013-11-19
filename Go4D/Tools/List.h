#ifndef _LIST_H_
#define _LIST_H_

/**
*AUTHOR    : Babel Yoann
*TITLE     : Tools.h
*DATE      : 11/12/95
*MODIF     : 17/12/00
*OBJECT    : This unit will contain some tools useful to program faster
*            like a template chained list ...
*/

#include "Base.h"

namespace Go4D
{


#define NOTFOUND -1

  template <class TypeElem> struct ListElem
  {
    TypeElem fValue;
    ListElem * fNext;
  };

  /**
  * A generic list class
  */
  template <class TypeElem> class List
  {
  protected :
    ListElem<TypeElem> * Last() const
    {
      ListElem<TypeElem> * result = fFirst;
      if (result==NULL) return NULL;
      while (result->fNext != NULL)
        result = result->fNext;
      return result;
    }

  public :

    typedef bool (*CmpFunc)(TypeElem, TypeElem);

    ///
    List(const List & aList)
    {
      fAggregate = aList.fAggregate;
      fUnique = aList.fUnique;
      fCmpFunc = aList.fCmpFunc;
      fFirst = NULL;
      fCurrent = NULL;
      Copy(aList);
    }

    ///
    List(bool aggregate=false, bool unique=false, CmpFunc aCmpFunc=NULL)
    {
      fAggregate = aggregate;
      fUnique = unique;
      fCmpFunc = aCmpFunc;
      fFirst = NULL;
      fCurrent = NULL;
    }

    ///
    ~List()
    {
      Clear();
    }

    ///
    List & Copy(const List & aList)
    {
      aList.First();
      while (aList.Current() != NULL)
      {
        Add(aList.Current());
        aList.Next();
      }
      return *this;
    }

    ///
    List & operator = (const List & aList)
    {
      return Copy(aList);
    }

    ///
    int Count() const
    {
      int result = 0;
      ListElem<TypeElem> * cur = fFirst;
      while (cur != NULL)
      {
        result++;
        cur = cur->fNext;
      }
      return result;
    }

    ///
    void Add(TypeElem value)
    {
      if (fUnique)
        if (IndexOf(value)!=NOTFOUND)
          return;
      ListElem<TypeElem> * newElem = new ListElem<TypeElem>;
      newElem->fValue = value;
      newElem->fNext = NULL;
      ListElem<TypeElem> * last = Last();
      if (last==NULL)
        fFirst = newElem;
      else
        last->fNext = newElem;
      fCurrent = newElem;
    }

    ///
    void Clear()
    {
      ListElem<TypeElem> * cur=fFirst;
      while (cur!=NULL)
      {
        ListElem<TypeElem> * prev = cur;
        cur = cur->fNext;
#ifndef UNDER_CE
        if (fAggregate)
          delete prev->fValue;
#endif
        delete prev;
      }
      fFirst = NULL;
    }

    ///
    TypeElem Get(int i) const
    {
      ListElem<TypeElem> * cur = fFirst;
      while ((i-->0) && (cur!=NULL))
        cur = cur->fNext;
      if (cur == NULL)
        return NULL;
      else
        return cur->fValue;
    }

    ///
    TypeElem operator [] (int i) const
    {
      return Get(i);
    }

    ///
    int IndexOf(TypeElem value) const
    {
      int index = 0;
      ListElem<TypeElem> * cur = fFirst;
      while (cur != NULL)
      {
        if (Compare(value, cur->fValue))
          return index;
        cur = cur->fNext;
        index++;
      }
      return NOTFOUND;
    }

    ///
    void Del(TypeElem value)
    {
      ListElem<TypeElem> * prev=fFirst;
      if (prev==NULL) return;
      while (prev->fNext!=NULL)
      {
        if (Compare(prev->fNext->fValue, value))
          break;
        prev = prev->fNext;
      }
      if (prev->fNext == NULL)
      {
        if (Compare(fFirst->fValue, value))
        {
          // it's the first
          ListElem<TypeElem> * next = fFirst->fNext; // can be NULL
          if (fAggregate) delete fFirst->fValue;
          delete fFirst;
          fFirst = next;
          return;
        }
        return;
      }
      ListElem<TypeElem> * next = prev->fNext->fNext;
      if (fAggregate) delete prev->fNext->fValue;
      delete prev->fNext;
      prev->fNext = next;
    }

    ///
    void First() const
    {
      fCurrent = fFirst;
    }

    ///
    TypeElem Current() const
    {
      if (fCurrent==NULL)
        return NULL;
      else
        return fCurrent->fValue;
    }

    ///
    bool Next() const
    {
      if (fCurrent==NULL)
        return false;
      else
      {
        fCurrent = fCurrent->fNext;
        return true;
      }
    }

  protected :
    bool fAggregate;
    bool fUnique;
    ListElem<TypeElem> * fFirst;
    mutable ListElem<TypeElem> * fCurrent;
    CmpFunc fCmpFunc;

    bool Compare(TypeElem a, TypeElem b) const
    {
      if (fCmpFunc != NULL)
        return (*fCmpFunc)(a, b);
      else
        return a==b;
    }
  };

  /**
  * This class is an optimzed List. You can use it as a list for adding ...
  * But operator[] is optimized
  */
  template <class TypeElem> class Array : public List<TypeElem>
  {
    typedef List<TypeElem> inherited;
    typedef bool (*CmpFunc)(TypeElem, TypeElem);
  public :
    ///
    Array(bool aggregate=true, bool unique=false, CmpFunc aCmpFunc=NULL)
      : inherited(aggregate, unique, aCmpFunc)
    {
      fOptimized = false;
      fArray = NULL;
    };

    ///
    int Count() const
    {
      //assert(fOptimized);
      if (fOptimized)
        return fCount;
      else
        return inherited::Count();
    }

    ///
    void Add(TypeElem value)
    {
      inherited::Add(value);
      fOptimized = false;
    }

    ///
    void Complete()
    {
      fCount = inherited::Count();
      if (fArray != NULL) delete [] fArray;
      fArray = new TypeElem[fCount];
      inherited::First();
      fCount = 0;
      while (inherited::Current() != NULL)
      {
        fArray[fCount] = inherited::Current();
        inherited::Next(); fCount++;
      }
      fOptimized = true;
    }

    ///
    void Clear()
    {
      inherited::Clear();
      fOptimized = false;
    }

    ///
    TypeElem operator [] (int i) const
    {
      //assert(fOptimized);
      if (fOptimized)
        return fArray[i];
      else
        return inherited::Get(i);
    }

    ///
    void Del(TypeElem value)
    {
      inherited::Del(value);
      fOptimized = false;
    }

    ///
    Array & Copy(const Array & aArray)
    {
      inherited::Copy(aArray);
      Complete();
      return *this;
    }

  protected :
    TypeElem * fArray;
    int fCount;
    bool fOptimized;
  };

  /**
  * Clearly this class is not optimized at all !! so use it carrefully
  * interfaces are OK, but implementation can be greatly enhanced
  */
  template <class TypeElem> class Stack : public List<TypeElem>
  {
  protected :
    typedef List<TypeElem> inherited;
  public :
    ///
    Stack()
      : inherited(false)
    {
    }

    ///
    void Push(TypeElem value)
    {
      inherited::Add(value);
    }

    ///
    TypeElem Pop()
    {
      TypeElem result=inherited::Last()->fValue;
      inherited::Del(result);
      return result;
    }

    ///
    TypeElem Current()
    {
      ListElem<TypeElem> * result = inherited::Last();
      if (result != NULL)
        return inherited::Last()->fValue;
      else
        return NULL;
    }
  };

  typedef const char * String;

  /// an array of string predifined
  class StringArray : public Array<String>
  {
    typedef Array<String> inherited;
  public :
    ///
    StringArray();
  };

  /// a list of string, just a predefinition
  class StringList : public List<String>
  {
    typedef List<String> inherited;
  public :
    ///
    StringList();
  };

  bool CompareStr(String, String);


}

#endif
