#ifndef _RTTI_H_
#define _RTTI_H_

namespace Go4D
{
  /**
  * Rules : all hierarchy root must inherit first IUnknown,
  *   if a root don't inherit IUnknown, then no dynamic_cast will be possible FROM that class
  *   RTTI leave class (mean supporting _dynamic_cast_) must have an RTTI_DECL in their
  *   declaration, and a RTTI_IMPL (recopying inheritance defintion) in implementation
  */

#ifndef UNDER_CE
  namespace RTTI
  {
#endif

    typedef const void * ClassID;
    template <class T> class UniqueClassID
    {
    public :
      static ClassID classid;
    };

    template <class T> ClassID UniqueClassID<T>::classid = 0;

    class IUnk
    {
    public :
      virtual void * QueryInterface(ClassID &)=0;
    };

    template <class T> inline T _dynamic_cast_(void * intf)
    {
      if (intf==0)
        return 0;
      else
        return
          reinterpret_cast<T>
            (reinterpret_cast<IUnk *>(intf)->QueryInterface(UniqueClassID<T>::classid));
    }

#ifndef UNDER_CE
#  define RTTI_USE using namespace RTTI
#else
#  define RTTI_USE
#endif

#define RTTI_BASE public IUnk

#define RTTI_DECL void * QueryInterface(ClassID&)

#define RTTI_IMPL_0(base) void * base::QueryInterface(ClassID& classid) { \
  if (&classid==&UniqueClassID<base*>::classid) \
  return static_cast<base *>(this); \
  else \
  return 0; \
}

#define RTTI_IMPL_1(base, i1) void * base::QueryInterface(ClassID& classid) { \
  if (&classid==&UniqueClassID<base*>::classid) \
  return static_cast<base*>(this); \
  else if (&classid==&UniqueClassID<i1*>::classid) \
  return static_cast<i1*>(this); \
  else \
  return 0; \
}

#define RTTI_IMPL_2(base, i1, i2) void * base::QueryInterface(ClassID& classid) { \
  if (&classid==&UniqueClassID<base*>::classid) \
  return static_cast<base*>(this); \
  else if (&classid==&UniqueClassID<i1*>::classid) \
  return static_cast<i1*>(this); \
  else if (&classid==&UniqueClassID<i2*>::classid) \
  return static_cast<i2*>(this); \
  else \
  return 0; \
}

#define RTTI_IMPL_3(base, i1, i2, i3) void * base::QueryInterface(ClassID& classid) { \
  if (&classid==&UniqueClassID<base*>::classid) \
  return static_cast<base*>(this); \
  else if (&classid==&UniqueClassID<i1*>::classid) \
  return static_cast<i1*>(this); \
  else if (&classid==&UniqueClassID<i2*>::classid) \
  return static_cast<i2*>(this); \
  else if (&classid==&UniqueClassID<i3*>::classid) \
  return static_cast<i3*>(this); \
  else \
  return 0; \
}

#define RTTI_IMPL_4(base, i1, i2, i3, i4) void * base::QueryInterface(ClassID& classid) { \
  if (&classid==&UniqueClassID<base*>::classid) \
  return static_cast<base*>(this); \
  else if (&classid==&UniqueClassID<i1*>::classid) \
  return static_cast<i1*>(this); \
  else if (&classid==&UniqueClassID<i2*>::classid) \
  return static_cast<i2*>(this); \
  else if (&classid==&UniqueClassID<i3*>::classid) \
  return static_cast<i3*>(this); \
  else if (&classid==&UniqueClassID<i4*>::classid) \
  return static_cast<i4*>(this); \
  else \
  return 0; \
}

#define RTTI_IMPL_5(base, i1, i2, i3, i4, i5) void * base::QueryInterface(ClassID& classid) { \
  if (&classid==&UniqueClassID<base*>::classid) \
  return static_cast<base*>(this); \
  else if (&classid==&UniqueClassID<i1*>::classid) \
  return static_cast<i1*>(this); \
  else if (&classid==&UniqueClassID<i2*>::classid) \
  return static_cast<i2*>(this); \
  else if (&classid==&UniqueClassID<i3*>::classid) \
  return static_cast<i3*>(this); \
  else if (&classid==&UniqueClassID<i4*>::classid) \
  return static_cast<i4*>(this); \
  else if (&classid==&UniqueClassID<i5*>::classid) \
  return static_cast<i5*>(this); \
  else \
  return 0; \
}

#ifndef UNDER_CE
  };
#endif

};

#endif
