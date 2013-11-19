#include "XMLParser.h"
#include <stdio.h>
#include "expat.h"
#include "Tools/FString.h"
#include "Tools/Base.h"


namespace Go4D
{

  namespace XML
  {

    Element::Element()
      : fElements(true),
      fAttributes(true)
    {
    }

    Element::~Element()
    {
    }

    void Element::SetName(const char * name)
    {
      fName = name;
    }

    void Element::SetValue(const char * value)
    {
      fValue = value;
    }

    void Element::AddElement(Element * subElement)
    {
      fElements.Add(subElement);
    }

    int Element::ElementCount() const
    {
      return fElements.Count();
    }

    Element * Element::GetElement(int index) const
    {
      return fElements[index];
    }

    void Element::AddAttribute(Attribute * attr)
    {
      fAttributes.Add(attr);
    }

    int Element::AttributeCount() const
    {
      return fAttributes.Count();
    }

    Attribute * Element::GetAttribute(int index) const
    {
      return fAttributes[index];
    }

    const char * Element::Value()
    {
      return fValue.c_str();
    }

    const char * Element::Name()
    {
      return fName.c_str();
    }

    void Element::CompleteAll()
    {
      fElements.Complete();
      fAttributes.Complete();
      for (int i=0; i<fElements.Count(); i++)
        fElements[i]->CompleteAll();
    }

    const char * Element::AttributeValue(const char * name) const
    {
      for (int i=0; i<fAttributes.Count(); i++)
        if (SameStr(name, fAttributes[i]->Name()))
          return fAttributes[i]->Value();
      error1("Attribute not found", name);
      return "";
    }

    bool Element::HaveAttribute(const char * name) const
    {
      for (int i=0; i<fAttributes.Count(); i++)
        if (SameStr(name, fAttributes[i]->Name()))
          return true;
      return false;
    }

    Attribute::Attribute(const char * name, const char * value)
    {
      fName = name;
      fValue = value;
    }

    Attribute::~Attribute()
    {
    }

    const char * Attribute::Name() const
    {
      return fName.c_str();
    }

    const char * Attribute::Value() const
    {
      return fValue.c_str();
    }


    Text::Text()
    {
    }

    Text::~Text()
    {
    }

    const char * Text::Value()
    {
      return fValue.c_str();
    }

    void parser_start(void *data, const char *el, const char **attr)
    {
      static_cast<Parser *>(data)->PushCurrent()->SetName(el);
      for (int i = 0; attr[i]; i += 2)
      {
        Attribute * attri = new Attribute(attr[i], attr[i + 1]);
        static_cast<Parser *>(data)->GetCurrent()->AddAttribute(attri);
      }
    }

    void parser_end(void *data, const char *el)
    {
      static_cast<Parser *>(data)->PopCurrent();
    }

    void parser_data(void *data, const XML_Char *s,	 int len)
    {
      char Buff[256];
      SubStr(Buff, s, 0, len);
      static_cast<Parser *>(data)->GetCurrent()->SetValue(Buff);
    }

    Parser::Parser(const char * fileName)
    {
      fRoot = NULL;
      Parse(fileName);
    }

    Parser::Parser()
    {
      fRoot = NULL;
    }

    Element * Parser::PushCurrent()
    {
      Element * result = new Element();
      Element * last = fElements.Current();
      if (last == NULL)
      {
        assert(fRoot == NULL);
        fRoot = result;
      }
      else
      {
        last->AddElement(result);
      }
      fElements.Push(result);
      return result;
    }

    void Parser::PopCurrent()
    {
      fElements.Pop();
    }

    Element * Parser::GetCurrent()
    {
      return fElements.Current();
    }

    Parser::~Parser()
    {
      if (fRoot != NULL)
        delete fRoot;
    }

    Element * Parser::Root()
    {
      return fRoot;
    }

    Element * Parser::Parse(const char * fileName)
    {
      if (fRoot != NULL)
        delete fRoot;
      fRoot = NULL;
      XML_Parser p = XML_ParserCreate(NULL);
      XML_SetElementHandler(p, parser_start, parser_end);
      XML_SetCharacterDataHandler(p, parser_data);
      XML_SetUserData(p, this);

#define BUFFSIZE	8192
      int done=0;
      char Buff[BUFFSIZE];
      FILE * file=fopen(fileName, "ra");
      if (file==NULL)
        error1("File doesn't exists", fileName);
      while (true)
      {
        int len = fread(Buff, 1, BUFFSIZE, file);
        done = feof(file);
        if (! XML_Parse(p, Buff, len, done))
          error(XML_ErrorString(XML_GetErrorCode(p))); //XML_GetCurrentLineNumber(p)
        if (done)
          break;
      }
      if (fRoot != NULL)
        fRoot->CompleteAll();
      fclose(file);
      return fRoot;
    }


  }

}
