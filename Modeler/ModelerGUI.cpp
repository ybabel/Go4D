#include "ModelerGUI.h"

ModelerGUI::ModelerGUI(Device & dev)
: inherited(dev),
guiParser("ModelerGUI.xml"),
font("font6x10.fon"),
layout(&font, true/*proportion*/, true/*animation*/),
gui(&fImage, guiRoot, &layout),
dirRoot(".")
{
  layout.ImportPalette(*fDevice->GetPalette());
  gui.SetActionListener(static_cast<GUI::ActionListener *>(this));

  // modeler initialization code
  GUI::Modeler3DView * modeler = new GUI::Modeler3DView(&gui, "Modeler", fDevice);
  guiRoot = modeler;
  guiRoot = GUI::Loader::Read(&gui, guiParser.Root()->GetElement(0));
  gui.SetRoot(guiRoot);
  GUI::Switcher * uv = reinterpret_cast<GUI::Switcher *>(gui.SearchNode("UPView"));
  uv->AddSubNode(modeler);
  RefreshTree();
  RefreshTypes();
  RefreshOptions();

  // dir browser init code
  GUI::Tree * tv = reinterpret_cast<GUI::Tree *>(gui.SearchNode("DirTree"));
  tv->SetRootNode(GUI::Loader::FillTree(NULL, dirRoot.Root()));
  tv->SetComplete();
  RefreshFileList();

#ifndef UNDER_CE
  Draw();
#endif
}

ModelerGUI::~ModelerGUI()
{
}


void ModelerGUI::OnDragStart(int mousecode, int x, int y)
{
  gui.SendStartDrag(mousecode, x, y);
  Draw();
}

void ModelerGUI::OnDragOver(int mousecode, int dx, int dy)
{
  gui.SendDragOver(mousecode, dx, dy);
  Draw();
}

void ModelerGUI::OnDragEnd(int mousecode, int x, int y)
{
  gui.SendEndDrag(mousecode, x, y);
  Draw();
}

void ModelerGUI::OnMouseClick(int mousecode, int x, int y)
{
  gui.SendClick(mousecode, x, y);
  Draw();
}

Editable * ModelerGUI::GetCurrent() const
{
  void * data = GetNodes()->GetSelected()->CustomData;
  return static_cast<Editable *>(data);
}

void ModelerGUI::Cycle()
{
  if (GetModeler()->Cycle())
    Draw();
}

void ModelerGUI::Draw()
{
  gui.Display();
  inherited::Draw();
}

GUI::Modeler3DView * ModelerGUI::GetModeler() const
{
  return reinterpret_cast<GUI::Modeler3DView *>(gui.SearchNode("Modeler"));
}

GUI::Tree * ModelerGUI::GetNodes() const
{
  return reinterpret_cast<GUI::Tree *>(gui.SearchNode("Nodes"));
}

void ModelerGUI::RefreshTree() const
{
  GUI::Tree * tv = GetNodes();
  tv->Clear();
  GUI::Modeler3DView * modeler = GetModeler();
  GUI::TreeNode * root = tv->GetRootNode();
  if (root == NULL)
  {
    root = new GUI::TreeNode();
    root->SetValue("Nodes");
    root->CustomData = NULL;
  }
  tv->SetRootNode(root);
  GUI::Loader::DisplayNode(root, modeler->GetEditor().Root());
  GUI::Loader::DisplayNode(root, modeler->GetEditor().fDefaultCamera);
  tv->SetComplete();
  tv->SetSelected(root);
}

void ModelerGUI::RefreshProperties() const
{
  GUI::StringList * names = reinterpret_cast<GUI::StringList *>(gui.SearchNode("Prp_Name"));
  GUI::StringList * values = reinterpret_cast<GUI::StringList *>(gui.SearchNode("Prp_Value"));
  names->Clear();
  values->Clear();

  Editable * cur = GetModeler()->GetEditor().Current();

  int i;
  for (i=0; i<cur->GetPropCount(); i++)
    names->AddString(cur->GetProperty(i));

  for (i=0; i<cur->GetPropCount(); i++)
  {
    EditableID propName = cur->GetProperty(i);
    values->AddString(cur->GetValue(propName).ToString());
  }

}

void ModelerGUI::RefreshLinks() const
{
  Editable * cur = GetModeler()->GetEditor().Current();
  GUI::StringList * links = reinterpret_cast<GUI::StringList *>(gui.SearchNode("Lnk"));
  links->Clear();
  for (int i=0; i<cur->GetLinkCount(""); i++)
  {
    links->AddString(GUI::Loader::GetID(cur->GetLink("", i)).c_str());
  }
}

void ModelerGUI::RefreshActions() const
{
  Editable * cur = GetModeler()->GetEditor().Current();
  GUI::StringList * actions = reinterpret_cast<GUI::StringList *>(gui.SearchNode("Act_List"));
  actions->Clear();
  for (int i=0; i<cur->GetActionCount(); i++)
  {
    actions->AddString(cur->GetAction(i));
  }
}

void ModelerGUI::RefreshTypes() const
{
  GUI::StringList * types = reinterpret_cast<GUI::StringList *>(gui.SearchNode("New"));
  types->Clear();
  for (int i=0; i<GetModeler()->GetEditor().GetTypeCount(); i++)
  {
    types->AddString(GetModeler()->GetEditor().GetType(i));
  }
  types->AddString("Delete");
}

void ModelerGUI::RefreshOptions() const
{
  GUI::OptionBox * options = reinterpret_cast<GUI::OptionBox *>(gui.SearchNode("Cmd_Opt"));
  GetModeler()->fDrawPlotNumbers = options->GetIsSelected("Numbr");
  GetModeler()->fDrawPlots = options->GetIsSelected("Plot");
  GetModeler()->fDrawSegments = options->GetIsSelected("Line");
  GetModeler()->fDrawFaces = options->GetIsSelected("Face");
  GetModeler()->fDrawPlotCoord = options->GetIsSelected("Coord");
  if (options->GetIsSelected("Simul"))
    GetModeler()->StartSimul();
  else
    GetModeler()->StopSimul();
}

void ModelerGUI::RefreshFileList() const
{
  GUI::TreeNode * current = reinterpret_cast<GUI::Tree *>(gui.SearchNode("DirTree"))->GetSelected();
  GUI::StringList * fl = reinterpret_cast<GUI::StringList *>(gui.SearchNode("FileList"));
  fl->Clear();
  DirectoryDescriptor * dir = (DirectoryDescriptor *)current->CustomData;
  for (int i=0; i<dir->FileCount(); i++)
    fl->AddString(dir->GetFile(i)->Name());
  fl->AutoSize();
}

int ModelerGUI::Execute(GUI::Action * action, const char * NodeName)
{
  // GENERAL
  if (SameStr(action->Name(), "Refresh"))
    inherited::Draw();

  // MODELER
  if (SameStr(action->Name(), "Click"))
  {
    if (SameStr(action->Sender()->Name(), "Panel") )
      reinterpret_cast<GUI::Switcher *>(gui.SearchNode("Panels"))->SetCurrentNode(
        reinterpret_cast<GUI::ToolBox *>(action->Sender())->GetSelectedIndex());

    if (SameStr(action->Sender()->Name(), "New"))
    {
      if (SameStr(NodeName, "Delete"))
      {
        if (!GetModeler()->GetEditor().IsClearable()) return 0;
        GetModeler()->GetEditor().Clear();
        GetNodes()->DeleteSelected();
        GetNodes()->SetComplete();
        gui.RefreshNode(GetNodes());
        inherited::Draw();
      }
      else
      {
        EditableID typeName = NodeName;
        Editable * newNode = GetModeler()->GetEditor().NewEditable(typeName);
        if (newNode == NULL) return 0;
        GUI::Loader::DisplayNode(GetNodes()->GetSelected(), newNode);
        GetNodes()->SetComplete();
        gui.RefreshNode(GetNodes());
        inherited::Draw();
      }
    }

    if ( SameStr(NodeName, "Add Link") )
    {
      if (GetModeler()->GetEditor().fDefault != NULL)
        GetModeler()->GetEditor().fDefault->AddLink("", GetModeler()->GetEditor().Current() );
      RefreshLinks();
      gui.RefreshNode(gui.SearchNode("Lnk"));
      inherited::Draw();
    }

    if ( SameStr(action->Sender()->Name(), "Lnk") )
    {
      Editable * editable = GUI::Loader::FindEditableFromID(NodeName, GetModeler()->GetEditor().Root());
      assert(editable != NULL);
      GetModeler()->GetEditor().Current()->DelLink("", editable);
      RefreshLinks();
      gui.RefreshNode(gui.SearchNode("Lnk"));
      inherited::Draw();
    }

    if ( SameStr(action->Sender()->Name(), "Act_Spin") )
    {
      Editable * cur = GetModeler()->GetEditor().Current();
      EditableID actionName = NodeName;
      Variant value(real(0.0));
      if (SameStr(NodeName, "Inc"))
        value = real(1.0);
      if (SameStr(NodeName, "Dec"))
        value = real(-1.0);
      cur->ApplyAction(actionName, value);
      gui.RefreshNode(GetModeler());
      RefreshProperties();
      inherited::Draw();
    }

    if ( SameStr(NodeName, "Exit") )
      fDevice->Close();

    if ( SameStr(NodeName, "Default") )
    {
      if ( GetModeler()->GetEditor().fDefault == reinterpret_cast<Editable *>(GetModeler()->GetEditor().Current()))
        GetModeler()->GetEditor().fDefault = NULL;
      else
        GetModeler()->GetEditor().fDefault = reinterpret_cast<Editable *>(GetModeler()->GetEditor().Current());
    }

    if ( SameStr(action->Sender()->Name(), "Cmd_Opt") )
      RefreshOptions();

    if ( SameStr(NodeName, "Copy") )
    {
      GetModeler()->GetEditor().Copy();
    }

    if ( SameStr(NodeName, "Cut") )
    {
      GetModeler()->GetEditor().Cut();
      RefreshTree();
    }

    if ( SameStr(NodeName, "Paste") )
    {
      GetModeler()->GetEditor().Paste();
      RefreshTree();
    }

    if ( SameStr(NodeName, "Load") )
    {
      fLoading = true;
      GUI::Switcher * uv = reinterpret_cast<GUI::Switcher *>(gui.SearchNode("UPView"));
      if (uv->GetCurrentNode()==0) // give a way to cancel operation
        uv->SetCurrentNode(uv->NodesCount()-1);
      else
        uv->SetCurrentNode(0);
    }

    if ( SameStr(NodeName, "Save") )
    {
      fLoading = false;
      GUI::Switcher * uv = reinterpret_cast<GUI::Switcher *>(gui.SearchNode("UPView"));
      if (uv->GetCurrentNode()==0) // give a way to cancel operation
        uv->SetCurrentNode(uv->NodesCount()-1);
      else
        uv->SetCurrentNode(0);
    }

  }

  if (SameStr(action->Name(), "Inc") || SameStr(action->Name(), "Dec") )
  {
    Editable * cur = GetModeler()->GetEditor().Current();
    GUI::StringList * acts = reinterpret_cast<GUI::StringList *>(gui.SearchNode("Act_List"));
    EditableID actionName = acts->GetSelectedText();
    Variant value(real(0.0));
    if (SameStr(action->Name(), "Inc"))
      value = real(1.0);
    if (SameStr(action->Name(), "Dec"))
      value = real(-1.0);
    cur->ApplyAction(actionName, value);
    gui.RefreshNode(GetModeler());
    RefreshProperties();
    inherited::Draw();
  }

  if (SameStr(action->Name(), "Key"))
  {
    if (SameStr(action->Sender()->Name(), "Edt_Txt"))
    {
      GUI::TextInput * ti = reinterpret_cast<GUI::TextInput *>(gui.SearchNode("Edt_Txt"));
      GUI::StringList * names = reinterpret_cast<GUI::StringList *>(gui.SearchNode("Prp_Name"));
      GUI::StringList * values = reinterpret_cast<GUI::StringList *>(gui.SearchNode("Prp_Value"));
      values->SetSelected(names->GetSelected());
      Editable * cur = GetModeler()->GetEditor().Current();
      apstring value=values->GetSelectedText();
      if (ti->ApplyLastKey(value))
        cur->SetValue(names->GetSelectedText(), value.c_str());
      else
        values->SetSelectedText(value.c_str());
      gui.RefreshNode(values);
      inherited::Draw();
    }
  }

  if (SameStr(action->Sender()->Name(), "Nodes") && SameStr(action->Name(), "Change"))
  {
    Editable * cur = GetCurrent();
    if (cur != NULL)
    {
      GetModeler()->GetEditor().SetCurrent(cur);
      GetModeler()->GetDrawPlotCoord() = reinterpret_cast<EditablePlot *>(GetCurrent())!=NULL;
      RefreshProperties();
      RefreshLinks();
      RefreshActions();
    }
  }

  // DIRECTORY BROWSER
  if (SameStr(action->Sender()->Name(), "DirTree") && SameStr(action->Name(), "Change"))
    RefreshFileList();
  if (SameStr(action->Sender()->Name(), "FileList") && SameStr(action->Name(), "Click"))
    OpenFile();

  return -1;
}

void ModelerGUI::OpenFile()
{
  GUI::TreeNode * current = reinterpret_cast<GUI::Tree *>(gui.SearchNode("DirTree"))->GetSelected();
  GUI::StringList * fl = reinterpret_cast<GUI::StringList *>(gui.SearchNode("FileList"));
  DirectoryDescriptor * dir = (DirectoryDescriptor *)current->CustomData;
  fstring fileName;
  CopyStr(fileName, dir->FullName() );
  ConcatStr(fileName, DirSep.c_str());
  ConcatStr(fileName, fl->GetSelectedText());
  UpperStr(fileName);
  if (FinishStr(fileName, ".ENV"))
  {
    if (fLoading)
      LoadFile(fileName);
    else
      SaveFile(fileName);
  }
  GUI::Switcher * uv = reinterpret_cast<GUI::Switcher *>(gui.SearchNode("UPView"));
  uv->SetCurrentNode(uv->NodesCount()-1);
}

void ModelerGUI::LoadFile(const char * filename)
{
  GetModeler()->GetEditor().Load(filename);
  RefreshTree();
}

void ModelerGUI::SaveFile(const char * filename)
{
  GetModeler()->GetEditor().Save(filename);
}
