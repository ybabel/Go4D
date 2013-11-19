#include "Editor.h"
#include <string.h>

namespace Go4D
{

#define NOTFOUND -1

  /******************************************************************************\
  * Editable
  \******************************************************************************/

  bool Editable::HaveProp(EditableID aPropName) const
  {
    for (int i=0; i<GetPropCount(); i++)
      if (strcmp(aPropName, GetProperty(i)))
        return true;
    return false;
  }

  bool Editable::Selected()
  {
    return fSelected;
  }

  void Editable::SetSelected(bool aSelected)
  {
    fSelected = aSelected;
  }

  void Editable::Refresh()
  {
  }

  int Editable::ID() const
  {
    return fID;
  }

  /******************************************************************************\
  * EditGroup
  \******************************************************************************/

  EditGroup::EditGroup()
    : fEditList(true)
  {
  }

  EditGroup::~EditGroup()
  {
  }

  void EditGroup::AddEditable(PEditable aEditable)
  {
    fEditList.Add(aEditable);
  }

  void EditGroup::DelEditable(PEditable aEditable)
  {
    fEditList.Del(aEditable);
  }

  EditGroup * EditGroup::SearchOwner(PEditable aSearched)
  {
    First();
    while (Current()!=NULL)
    {
      if (Current()==aSearched)
        return this;
      if (_dynamic_cast_<EditGroup *>(Current()))
      {
        EditGroup * newSearch =
          _dynamic_cast_<EditGroup *>(Current())->SearchOwner(aSearched);
        if (newSearch != NULL)
          return newSearch;
      }
      Next();
    }
    return NULL;
  }

  void EditGroup::First()
  {
    fEditList.First();
  }

  bool EditGroup::Next()
  {
    return fEditList.Next();
  }

  PEditable EditGroup::Current() const
  {
    return fEditList.Current();
  }

  void EditGroup::SetSelected(bool aSelected)
  {
    First();
    while (Current() != NULL)
    {
      Current()->SetSelected(aSelected);
      Next();
    }
  }

  /******************************************************************************\
  * DefaultEditGroup
  \******************************************************************************/

  RTTI_IMPL_2 (DefaultEditGroup, Editable, EditGroup);

  EditableID DefaultEditGroup::TypeName() const
  {
    return _TypeName();
  }

  EditableID DefaultEditGroup::_TypeName()
  {
    return "Group";
  }

  void DefaultEditGroup::AddEditable(PEditable aEditable)
  {
    inherited::AddEditable(aEditable);
    RefreshPropList();
    RefreshActionList();
  }

  void DefaultEditGroup::DelEditable(PEditable aEditable)
  {
    inherited::DelEditable(aEditable);
    RefreshPropList();
    RefreshActionList();
  }

  int DefaultEditGroup::GetPropCount() const
  {
    return fPropList.Count();
  }

  EditableID DefaultEditGroup::GetProperty(int i) const
  {
    return fPropList[i];
  }

  void DefaultEditGroup::SetValue(EditableID propName, Variant value)
  {
    First();
    while (Current()!=NULL)
    {
      Current()->SetValue(propName, value);
      Next();
    }
  }

  Variant DefaultEditGroup::GetValue(EditableID propName) const
  {
    Variant result(0);
    return result;
  }

  int DefaultEditGroup::GetActionCount() const
  {
    return fActionList.Count();
  }

  EditableID DefaultEditGroup::GetAction(int i) const
  {
    return fActionList[i];
  }

  void DefaultEditGroup::ApplyAction(EditableID actionName, Variant value)
  {
    First();
    while (Current()!=NULL)
    {
      Current()->ApplyAction(actionName, value);
      Next();
    }
  }

  void DefaultEditGroup::RefreshPropList()
  {
    fPropList.Clear();
    First();
    while (Current()!=NULL)
    {
      for (int i=0; i<Current()->GetPropCount(); i++)
        fPropList.Add(Current()->GetProperty(i));
      Next();
    }
  }

  void DefaultEditGroup::RefreshActionList()
  {
    fActionList.Clear();
    First();
    while (Current()!=NULL)
    {
      for (int i=0; i<Current()->GetActionCount(); i++)
        fActionList.Add(Current()->GetAction(i));
      Next();
    }
  }

  void DefaultEditGroup::AddLink(EditableID , Editable *)
  {
  }

  void DefaultEditGroup::DelLink(EditableID , Editable *)
  {
  }

  int DefaultEditGroup::GetLinkCount(EditableID) const
  {
    return 0;
  }

  Editable * DefaultEditGroup::GetLink(EditableID, int ) const
  {
    return NULL;
  }

  void DefaultEditGroup::SetSelected(bool selected)
  {
    inherited::SetSelected(selected);
  }

  /******************************************************************************\
  * EditNode
  \******************************************************************************/

  EditNode::EditNode()
  {
  }

  int EditNode::GetPropCount() const
  {
    return fProperties.Count();
  }

  EditableID EditNode::GetProperty(int i) const
  {
    return fProperties[i];
  }

  int EditNode::GetActionCount() const
  {
    return fActions.Count();
  }

  EditableID EditNode::GetAction(int i) const
  {
    return fActions[i];
  }

  void EditNode::AddLink(EditableID linkName, Editable * aEditable)
  {
  }

  void EditNode::DelLink(EditableID linkName, Editable * aEditable)
  {
  }

  int EditNode::GetLinkCount(EditableID) const
  {
    return fLinks.Count();
  }

  Editable * EditNode::GetLink(EditableID, int i) const
  {
    return fLinks[i];
  }

  /******************************************************************************\
  * Editor
  \******************************************************************************/

  Editor::Editor()
  {
    fCurrentID = 0;
    fRoot = new DefaultEditGroup();
    SetEditor(fRoot);
    fCurrent = _dynamic_cast_<Editable *>(fRoot);
  }

  Editor::~Editor()
  {
    delete fRoot;
  }

  PEditable Editor::Root() const
  {
    return fRoot;
  }

  void Editor::SetCurrent(Editable * aEditable)
  {
    assert(aEditable != NULL);
    fCurrent->SetSelected(false);
    fCurrent = aEditable;
    fCurrent->SetSelected(true);
    fCurrent->Refresh();
  }

  PEditable Editor::Current() const
  {
    return _dynamic_cast_<Editable *>(fCurrent);
  }

  bool Editor::IsLinked(const Editable * aEditable)
  {
    return FindIsLinked(aEditable, _dynamic_cast_<EditGroup *>(fRoot));
  }

  bool Editor::FindIsLinked(const Editable * aEditable, EditGroup * findRoot)
  {
    findRoot->First();
    Editable * cur = findRoot->Current();
    while (cur !=  NULL)
    {
      for (int i=0; i<cur->GetLinkCount(""); i++)
        if (cur->GetLink("", i)==aEditable)
          return true;
      if (_dynamic_cast_<EditGroup *>(cur))
      {
        bool found = FindIsLinked(aEditable, _dynamic_cast_<EditGroup *>(cur));
        if (found) return found;
      }
      findRoot->Next();
      cur = findRoot->Current();
    }
    return false;
  }

  EditGroup * Editor::FindOwner(const Editable * aEditable, EditGroup * findRoot)
  {
    findRoot->First();
    while (findRoot->Current() !=  NULL)
    {
      if (aEditable == findRoot->Current())
        return findRoot;
      if (_dynamic_cast_<EditGroup *>(findRoot->Current()))
      {
        EditGroup * found =
          FindOwner(aEditable, _dynamic_cast_<EditGroup *>(findRoot->Current()));
        if (found != NULL)
          return found;
      }
      findRoot->Next();
    }
    return NULL;
  }

  Editable * Editor::FindLoadID(int loadID, EditGroup * findRoot)
  {
    if (loadID == _dynamic_cast_<Editable *>(findRoot)->fLoadID)
      return _dynamic_cast_<Editable *>(findRoot);
    findRoot->First();
    while (findRoot->Current() !=  NULL)
    {
      if (loadID == findRoot->Current()->fLoadID)
        return findRoot->Current();
      if (_dynamic_cast_<EditGroup *>(findRoot->Current()))
      {
        Editable * found =
          FindLoadID(loadID, _dynamic_cast_<EditGroup *>(findRoot->Current()));
        if (found != NULL)
          return found;
      }
      findRoot->Next();
    }
    return NULL;
  }

  void Editor::SetEditor(Editable * aEditable)
  {
    aEditable->fEditor = this;
    aEditable->fID = fCurrentID++;
  }

  int Editor::GetTypeCount() const
  {
    return fTypes.Count();
  }

  EditableID Editor::GetType(int i) const
  {
    return fTypes[i];
  }

  void Editor::Clear()
  {
    if (fCurrent==fRoot) return;
    EditGroup * aOwner=_dynamic_cast_<EditGroup *>(fRoot)->SearchOwner(fCurrent);
    assert(aOwner != NULL);
    aOwner->DelEditable(fCurrent);
    fCurrent = _dynamic_cast_<Editable *>(aOwner);
  }

  void Editor::Copy()
  {
    ofstream file("CopyPaste.tmp");
    Save(fCurrent, file);
  }

  void Editor::Paste()
  {
    if (!_dynamic_cast_<EditGroup *>(fCurrent)) return;
    ClearLoadID(_dynamic_cast_<EditGroup *>(fRoot));
    ifstream file("CopyPaste.tmp");
    Load(fCurrent, file);
    //file.seekg(0,ios_base::beg); // don't work in all the case
    ifstream samefile("CopyPaste.tmp");
    ResolveLinks(fRoot, samefile);
  }

  void Editor::Cut()
  {
    Copy();
    Clear();
  }

  void Editor::Save(const char * fileName)
  {
    ofstream file(fileName);
    file << reset;
    Save(fCurrent, file);
  }

  void Editor::Load(const char * fileName)
  {
    if (!_dynamic_cast_<EditGroup *>(fCurrent)) return;
    ClearLoadID(_dynamic_cast_<EditGroup *>(fRoot));
    ifstream file(fileName);
    Load(fCurrent, file);
    //file.close();
    //file.seekg(0,ios_base::beg); // don't work in all the case
    ifstream savefile(fileName);
    ResolveLinks(fRoot, savefile);
  }

  void Editor::ClearLoadID(EditGroup * clearRoot)
  {
    _dynamic_cast_<Editable *>(clearRoot)->fLoadID = -1;
    clearRoot->First();
    while (clearRoot->Current() != NULL)
    {
      clearRoot->Current()->fLoadID = -1;
      if (_dynamic_cast_<EditGroup *>(clearRoot->Current()))
        ClearLoadID(_dynamic_cast_<EditGroup *>(clearRoot->Current()));
      clearRoot->Next();
    }
  }

  void Editor::Save(Editable * aEditable, ostream & os)
  {
    int i;
    os << "obj " << aEditable->TypeName() << " " << aEditable->ID() << incendl;

    for (i=0; i<aEditable->GetPropCount(); i++)
    {
      EditableID propName = aEditable->GetProperty(i);
      EditableID propValue = aEditable->GetValue(propName).ToString();
      if (StrLen(propValue)==0)
        os << "prop " << propName << " _NOVALUE_" << iendl;
      else
        os << "prop " << propName << " " << propValue << iendl;
    }

    for (i=0; i<aEditable->GetLinkCount(""); i++)
    {
      os << "link" << " noname " << aEditable->GetLink("", i)->ID() << iendl;
    }

    if (_dynamic_cast_<EditGroup *>(aEditable))
    {
      EditGroup * aGroup = _dynamic_cast_<EditGroup *>(aEditable);
      aGroup->First();
      while (aGroup->Current() != NULL)
      {
        Save(const_cast<Editable *>(aGroup->Current()), os);
        aGroup->Next();
      }
    }

    os << "end" << decendl;
  }

  void Editor::Load(Editable * aEditable, istream &is)
  {
    fstring token;
    fstring typeName;
    fstring propName;
    fstring linkName;
    fstring value;
    int loadID;
    int linkID;

    while (!is.eof())
    {
      ClearStr(token);
      is >> token;
      if (StrLen(token)==0) return; // nothing more to read

      if (strcmp(token, "obj")==0)
      {
        is >> typeName >> loadID;
        if (_dynamic_cast_<EditGroup *>(aEditable))
          fCurrent = aEditable;
        Editable * newEditable = NewEditable(typeName);
        newEditable->fLoadID = loadID;
        Load(newEditable, is);
        // restore fCurrent that can have been modified by recursive calls
        if (_dynamic_cast_<EditGroup *>(aEditable))
          fCurrent = aEditable;
      }
      else if (strcmp(token, "end")==0)
        return;
      else if (strcmp(token, "prop")==0)
      {
        is >> propName >> value;
        if (CompareStr("_NOVALUE_", value))
          aEditable->SetValue(propName, Variant(""));
        else
          aEditable->SetValue(propName, Variant(value));
      }
      else if (strcmp(token, "link")==0)
      {
        is >> linkName >> linkID;
        // linking is done later, after loading all the new instances
      }
    }
  }

  void Editor::ResolveLinks(Editable * solveRoot, istream &is)
  {
    fstring token;
    fstring typeName;
    fstring propName;
    fstring linkName;
    fstring value;
    int loadID;
    int linkID;

    while (!is.eof())
    {
      ClearStr(token);
      is >> token;
      if (StrLen(token)==0) return; // nothing more to read

      if (strcmp(token, "obj")==0)
      {
        is >> typeName >> loadID;
        Editable * newEditable = FindLoadID(loadID, _dynamic_cast_<EditGroup *>(fRoot));
        assert(newEditable != NULL);
        assert(newEditable->fLoadID == loadID);
        ResolveLinks(newEditable, is);
      }
      else if (strcmp(token, "end")==0)
        return;
      else if (strcmp(token, "prop")==0)
      {
        is >> propName >> value;
      }
      else if (strcmp(token, "link")==0)
      {
        is >> linkName >> linkID;
        Editable * linked = FindLoadID(linkID, _dynamic_cast_<EditGroup *>(fRoot));
        assert(linked != NULL);
        solveRoot->AddLink(linkName, linked);
      }
    }
  }

  void Editor::ApplyAction(EditableID actionName, Variant value)
  {
    fCurrent->ApplyAction(actionName, value);
  }

  bool Editor::IsClearable()
  {
    if (fCurrent == fRoot) return false;
    if (IsLinked(fCurrent)) return false;
    return true;
  }

  Editable * Editor::NewEditable(EditableID name)
  {
    if (!_dynamic_cast_<EditGroup *>(fCurrent)) return NULL;
    Editable * result=NULL;

    result = InternalNewEditable(name);

    _dynamic_cast_<EditGroup *>(fCurrent)->EditGroup::AddEditable(result);
    SetEditor(result);
    return result;
  }


}
