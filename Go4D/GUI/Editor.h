#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Tools/Common.h"
#include "Tools/List.h"
#include "Tools/RTTI.h"

namespace Go4D
{
  RTTI_USE;

  typedef String EditableID;

  class Editor;

  /// an editable class, wich means that it's properties values can be
  /// accessed simply, by streaming or a user
  class Editable : RTTI_BASE
  {
    friend class Editor;
  public :
    // type information
    virtual EditableID TypeName() const =0;
    // properties management
    virtual int GetPropCount() const =0;
    virtual EditableID GetProperty(int i) const = 0;
    virtual void SetValue(EditableID propName, Variant )=0;
    virtual Variant GetValue(EditableID propName) const=0;
    bool HaveProp(EditableID) const;
    // actions management
    virtual int GetActionCount() const =0;
    virtual EditableID GetAction(int) const =0;
    virtual void ApplyAction(EditableID actionName, Variant value) = 0;
    // selection management
    virtual bool Selected();
    virtual void SetSelected(bool);
    // link with others editable management
    // for the moment only 1 link is handled, but it should be sufficient
    virtual void AddLink(EditableID linkName, Editable *) =0;
    virtual void DelLink(EditableID linkName, Editable *) =0;
    virtual int GetLinkCount(EditableID linkName) const =0;
    virtual Editable * GetLink(EditableID linkName, int ) const=0;
    virtual void Refresh(); // refresh properties if it use links
    int ID() const;

    void * fData;

  protected :
    Editor * fEditor;
    bool fSelected;
    int fID;
    int fLoadID;
  };

  /// pointer to an editable
  typedef Editable * PEditable;

  /// regroup some editables
  class EditGroup : RTTI_BASE
  {
  public :
    EditGroup();
    ~EditGroup();
    // list management
    virtual void DelEditable(PEditable);
    virtual void AddEditable(PEditable);
    virtual void First();
    virtual PEditable Current() const;
    virtual bool Next();
    virtual EditGroup * SearchOwner(PEditable);
    // selection
    virtual void SetSelected(bool);

  protected :
    List<PEditable> fEditList;
  };

  /// the default edit group is also an editable
  class DefaultEditGroup : public Editable, public EditGroup
  {
    typedef EditGroup inherited;
  public :
    RTTI_DECL;

    // list management
    virtual void DelEditable(PEditable);
    virtual void AddEditable(PEditable);
    // type information
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    // properties
    virtual int GetPropCount() const;
    virtual EditableID GetProperty(int i) const;
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    // actions
    virtual int GetActionCount() const;
    virtual EditableID GetAction(int) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    // links
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual int GetLinkCount(EditableID linkName) const;
    virtual Editable * GetLink(EditableID linkName, int ) const;
    // selection
    virtual void SetSelected(bool);

  protected :
    StringList fPropList;
    StringList fActionList;
    void RefreshPropList();
    void RefreshActionList();
  };

  /// the standard eidtable have facilities to implement propreties
  /// and links to other class. It has prebuild functions, and there
  /// a minimum to code for childs
  class EditNode : public Editable
  {
    typedef Editable inherited;
  public :
    typedef Editable Base;
    EditNode();
    // properties
    virtual int GetPropCount() const;
    virtual EditableID GetProperty(int i) const;
    // actions
    virtual int GetActionCount() const;
    virtual EditableID GetAction(int) const;
    // links
    // by default nothing is done in Add & Del, but a storage is provided (fLinks)
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual int GetLinkCount(EditableID ) const;
    virtual Editable * GetLink(EditableID, int ) const;

  protected :
    StringArray fProperties;
    StringArray fActions;
    List<PEditable> fLinks;
  };

  /// a high level manipulator for editables
  /// allows such operations like copy&paste, saving a loading from a file
  class Editor : RTTI_BASE
  {
    friend class Editable;
  public :
    Editor();
    ~Editor();
    Editable * Root() const;
    // Change the selection, all the sub elems of a group are also selected
    void SetCurrent(Editable *);
    Editable * Current() const;
    // insert it in the current (assumes it's a group)
    virtual Editable * NewEditable(EditableID name);
    int GetTypeCount() const;
    EditableID GetType(int) const;
    bool IsLinked(const Editable *);

    // applyed on current item !!!
    void Clear();
    void Copy();
    void Paste();
    void Cut();
    void Save(const char * fileName);
    void Load(const char * fileName);

    void ApplyAction(EditableID actionName, Variant value);
    void SetEditor(Editable * );
    virtual bool IsClearable();

  protected :
    StringArray fTypes;
    Editable * fRoot;
    Editable * fCurrent;
    int fCurrentID;

    // insert it in the current (assumes it's a group)
    virtual Editable * InternalNewEditable(EditableID name)=0;

    EditGroup * FindOwner(const Editable *, EditGroup * findRoot);
    Editable * FindLoadID(int loadID, EditGroup * findRoot);
    bool FindIsLinked(const Editable *, EditGroup * findRoot);
    void ClearLoadID(EditGroup * clearRoot);
    void ResolveLinks(Editable * solveRoot, istream &);

    void Save(Editable *, ostream &);
    void Load(Editable *, istream &);
  };

}

#endif
