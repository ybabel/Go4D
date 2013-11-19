#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "FString.h"


namespace Go4D
{


  /**
  *  This unit will contain some tools useful to program faster like a template
  * chained list ...
  */

  /// Variant Like enumeration type
  enum VariantType {vtUnk, vtInt, vtString, vtReal};

  /**
  * A variant is used to stored a value in various format. For example you can
  * can use an integer as an int, a string (char *) or a real.
  * This class help you make the convertion. It's not optimzed for memory usage
  * not speed, so use it carefully.
  */
  struct Variant
  {
  protected:
    /// internal structure for storing variants
    struct Variant_
    {
      int fInteger;
      real fReal;
      fstring fString;
    };

  public:
    ///  return the native format of the variant, if it's a real/int/string
    VariantType fType;
    ///  direct access to the value (you'd better use the conversion routines)
    Variant_ fValue;
    ///  create a (int)0 variant
    Variant();
    ///
    Variant(int );
    ///
    Variant(const char * );
    ///
    Variant(real );
    ///  convert the variant into a string
    char * ToString();
    /** convert the variant from a string, the conversion depends on the
    * current type of the variant
    */
    void FromString(const char *);
    ///  convert the variant into a real
    real ToReal();
    ///  convert the variant into a integer
    int ToInt();
  };

}
#endif
