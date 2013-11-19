#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

/*
*AUTHOR : Babel yoann
*TITLE  : XMLParsre.h, an XML parser, based on expat
*CREATED: 12/01/01
*DATE   : 12/01/01
*OBJECT : XML parsing
*TODO   : parse the text ! for the moment only elements and attributes are parsed !
*/

#include "Tools/apstring.h"
#include "Tools/List.h"

namespace Go4D
{

  namespace XML
  {

    class Elements;
    class Attribute;
    class Text;

    /// an XLM element
    class Element
    {
      friend class Parser;
    public :
      ///
      Element();
      ///
      ~Element();
      ///
      void SetName(const char *);
      ///
      void SetValue(const char *);
      ///
      void AddElement(Element * subElement);
      ///
      int ElementCount() const;
      ///
      Element * GetElement(int index) const;
      ///
      void AddAttribute(Attribute * attr);
      ///
      int AttributeCount() const;
      ///
      Attribute * GetAttribute(int index) const;
      ///
      const char * Value();
      ///
      const char * Name();
      ///
      const char * AttributeValue(const char * name) const;
      ///
      bool HaveAttribute(const char * name) const;

    protected :
      apstring fName;
      apstring fValue;
      Array<Element *> fElements;
      Array<Attribute *> fAttributes;

    private :
      void CompleteAll();
    };

    /// an XML attribute
    class Attribute
    {
    public :
      ///
      Attribute(const char * name, const char * value);
      ///
      ~Attribute();
      ///
      const char * Name() const;
      ///
      const char * Value() const;

    protected :
      apstring fName;
      apstring fValue;
    };

    /// an XML piece of text : not used for the moment
    class Text
    {
    public :
      ///
      Text();
      ///
      ~Text();
      ///
      const char * Value();

    protected :
      apstring fValue;
    };

    /**
    * This class provides an XML parser. It return a simple structure composed of
    * elements and attributes.
    */
    class Parser
    {
    public :
      ///
      Parser(const char * fileName);
      ///
      Parser();
      ///
      ~Parser();
      ///
      Element * Parse(const char * fileName);
      ///
      Element * Root();

      ///  use internaly
      Element * PushCurrent();
      ///  use internaly
      Element * GetCurrent();
      ///  use internaly
      void PopCurrent();

    protected :
      Element * fRoot;
      Stack<Element *> fElements;
    };


  }

}
#endif
