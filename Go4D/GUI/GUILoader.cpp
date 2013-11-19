#include "GUILoader.h"
#include "Tools/FString.h"


namespace Go4D
{


  namespace GUI
  {

    int StrAsInt(const char * value)
    {
      int result;
      Str2Int(result, value);
      return result;
    }

    real StrAsReal(const char * value)
    {
      real result;
      Str2Real(result, value);
      return result;
    }

    ANode * Loader::Read(System * gui, XML::Element * xmlRoot)
    {
      ANode * result=NULL;
      const char * name = xmlRoot->Name();

#define NodeIs(v) SameStr(name, v)
#define SubNode(i) xmlRoot->GetElement(i)
#define GetStr(attr) xmlRoot->AttributeValue(attr)
#define GetBool(attr) SameStr("True", GetStr(attr))?true:false
#define GetInt(attr) StrAsInt(GetStr(attr))
#define GetReal(attr) StrAsReal(GetStr(attr))
#define HaveAttr(attr) xmlRoot->HaveAttribute(attr)

      if (NodeIs("Splitter"))
      {
        Splitter * node = new Splitter(gui, GetStr("Name"),
          GetInt("Width"), GetInt("Height"), GetBool("Vertical"),
          GetInt("Separation"), GetInt("Factor") );
        node->SetAlpha(Read(gui, SubNode(0)));
        node->SetOmega(Read(gui, SubNode(1)));
        result = node;
      }
      else if (NodeIs("String"))
      {
        String * node;
        if (HaveAttr("Value"))
          node = new String(gui, GetStr("Name"), GetStr("Value"));
        else
          node = new String(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"));
        result = node;
      }
      else if (NodeIs("StringList"))
      {
        StringList * node;
        node = new StringList(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"), GetBool("Flat") );
        for (int i=0; i<xmlRoot->ElementCount(); i++)
          node->AddString(xmlRoot->GetElement(i)->Value());
        result = node;
      }
      else if (NodeIs("Tree"))
      {
        Tree * node;
        if (xmlRoot->AttributeCount()==3)
          node = new Tree(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"));
        else
          node = new Tree(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"),
          GetBool("ShowRoot"), GetReal("ChildFactor"), GetReal("RelativeFactor"));
        result = node;
      }
      else if (NodeIs("ToolBox"))
      {
        ToolBox * node;
        node = new ToolBox(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"), GetBool("ShowSelected"));
        for (int i=0; i<xmlRoot->ElementCount(); i++)
          node->AddString(xmlRoot->GetElement(i)->Value());
        result = node;
      }
      else if (NodeIs("Switcher"))
      {
        Switcher * node;
        node = new Switcher(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"), GetBool("Vertical"));
        for (int i=0; i<xmlRoot->ElementCount(); i++)
          node->AddSubNode(Read(gui, SubNode(i)));
        result = node;
      }
      else if (NodeIs("Image"))
      {
        ImageFrame * node;
        node = new ImageFrame(gui, GetStr("Name"));
        result = node;
      }
      else if (NodeIs("AsciiTable"))
      {
        AsciiTable * node;
        node = new AsciiTable(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"));
        result = node;
      }
      else if (NodeIs("SpinButton"))
      {
        SpinButton * node;
        node = new SpinButton(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"), GetBool("Vertical"));
        result = node;
      }
      else if (NodeIs("ColorEditor"))
      {
        ColorEditor * node;
        node = new ColorEditor(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"));
        result = node;
      }
      else if (NodeIs("TextInput"))
      {
        TextInput * node;
        node = new TextInput(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"));
        result = node;
      }
      else if (NodeIs("Gauge"))
      {
        Gauge * node;
        node = new Gauge(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"), 0.0, 1.0);
        result = node;
      }
      else if (NodeIs("OptionBox"))
      {
        OptionBox * node;
        node = new OptionBox(gui, GetStr("Name"), GetInt("Width"), GetInt("Height"));
        for (int i=0; i<xmlRoot->ElementCount(); i++)
        {
          const char * value = xmlRoot->GetElement(i)->Value();
          node->AddString(value);
          if (xmlRoot->GetElement(i)->HaveAttribute("Checked"))
            node->SetIsSelected(value, SameStr("True", xmlRoot->GetElement(i)->AttributeValue("Checked")));
        }
        result = node;
      }
      assert(result != NULL);
      return result;
    }

    TreeNode * Loader::FillTree(TreeNode * node, XML::Element * element)
    {
      TreeNode * subNode = new TreeNode(node);
      subNode->SetValue(element->Name());
      subNode->CustomData = (void *)element;
      for (int i=0; i<element->ElementCount(); i++)
        FillTree(subNode, element->GetElement(i));
      return subNode;
    }

    TreeNode * Loader::FillTree(TreeNode * node, DirectoryDescriptor * directory)
    {
      TreeNode * subNode = new TreeNode(node);
      subNode->SetValue(directory->Name());
      subNode->CustomData = (void *)directory;
      for (int i=0; i<directory->ChildCount(); i++)
        FillTree(subNode, directory->GetChild(i));
      return subNode;
    }


    apstring Loader::GetID(Editable * aEditable)
    {
      apstring typeName = aEditable->TypeName();
      apstring id = IntAsStr(aEditable->ID());
      if (!aEditable->HaveProp("Name"))
        return typeName+":"+id;
      else
      {
        apstring name=aEditable->GetValue("Name").ToString();
        if (name.length()==0)
          return typeName+":"+id;
        else
          return typeName+":"+name+":"+id;
      }
    }

    void Loader::DisplayNode(TreeNode * root, Editable * aEditable)
    {
      TreeNode * newNode = new TreeNode(root);
      newNode->SetValue(GetID(aEditable).c_str());
      newNode->CustomData = (void *)aEditable;
      if (_dynamic_cast_<EditGroup *>(aEditable))
      {
        EditGroup * cur = _dynamic_cast_<EditGroup *>(aEditable);
        cur->First();
        while (cur->Current() != NULL)
        {
          DisplayNode(newNode, cur->Current());
          cur->Next();
        }
      }
    }

    Editable * Loader::FindEditableFromID(const char * ID, Editable * root)
    {
      if (SameStr(GetID(root).c_str(), ID))
        return root;
      else
      {
        EditGroup * group = _dynamic_cast_<EditGroup *>(root);
        if (group == NULL) return NULL;
        group->First();
        while (group->Current() !=  NULL)
        {
          Editable * found = FindEditableFromID(ID, group->Current());
          if (found != NULL)
            return found;
          group->Next();
        }
        return NULL;
      }
    }


  }

}
