#include "GUIDemo.h"

MainApp::MainApp(Device & aDevice)
: inherited(aDevice),
drawer(fImage, fImage.Width(), fImage.Height()),
font("Font6x10.fon"),
layout(&font, true/*proportion*/, true/*animation*/),
gui(&fImage, guiRoot, &layout),
menu("Menu.xml")
{
  DBGTrace("Create");
  layout.ImportPalette(*fDevice->GetPalette());
  DBGTrace("Palette imported");
  gui.SetActionListener(static_cast<GUI::ActionListener *>(this));
  Init();
}

void MainApp::Init()
{
  DBGTrace("Initialize");
  guiRoot = new GUI::Box(&gui, "root", NULL, fImage.Width(), fImage.Height(), layout.FrameColor(), 2);
  reinterpret_cast<GUI::Box *>(guiRoot)->SetMargin(2);
  gui.SetRoot(guiRoot);
  GUI::Splitter * sp = new GUI::Splitter(&gui, "Splitter", 
    200,200,/*Vertical*/false, 120, 40);
  guiRoot->SetChild(sp);
  GUI::Splitter * spu = new GUI::Splitter(&gui, "SPUp", 200,200, false/*vertical ?*/, 180,2/*factor*/);
  GUI::ToolBox * tb = new GUI::ToolBox(&gui, "TB", 200, 10);
  tb->AddString("First");
  tb->AddString("Last");
  tb->AddString("Out");
  tb->AddString("In");
  tb->AddString("+");
  tb->AddString("-");
  tb->AddString("Up");
  tb->AddString("Clr");
  spu->SetOmega(tb);
  GUI::Switcher * sw = new GUI::Switcher(&gui, "SW", 200,200, false);
  GUI::StringList * sl = new GUI::StringList(&gui, "Menu", 
    50, 200, /*flat*/true, 1.0/*radius*/, 0.0/*decal*/);
  GUI::Tree * tv = new GUI::Tree(&gui, "TV", 50, 200, true/*showroot*/, 0.95, 0.0, 5);
  tv->SetRootNode(FillTree(NULL, menu.Root()->GetElement(1)));
  tv->SetComplete();
  sw->AddSubNode(tv);
  sw->AddSubNode(new GUI::String(&gui, "View1", "View1"));
  spu->SetAlpha(sw);
  sp->SetAlpha(spu);
  sp->SetOmega(sl);
  /*sl->AddString("Open a file");
  sl->AddString("Save into a file");
  sl->AddString("Copy selection");
  sl->AddString("Cut selection");
  sl->AddString("Paste selection");
  for (int i=0; i<8; i++)
  sl->AddString("Remplissage");
  sl->AddString("Exit");*/
  XML::Element * menuRoot = menu.Root()->GetElement(0);
  for (int i=0; i<menuRoot->ElementCount(); i++)
    sl->AddString(menuRoot->GetElement(i)->GetAttribute(0)->Value());
  sl->SetSelected(5);
  //for (int i=0; i<8; i++) sl->AddString("Remplissage");
  sl->AutoSize();

  spu = new GUI::Splitter(&gui, "ColorSP", 200,200, false/*vertical ?*/, 150,20/*factor*/);
  sw->AddSubNode(spu);
  GUI::Splitter * spsu = new GUI::Splitter(&gui, "OBSP", 200,100, false, 20, 0);
  /*GUI::OptionBox * ob = new GUI::OptionBox(&gui, "Options", 200, 20);
  ob->AddString("Plot");
  ob->AddString("Coord");
  ob->AddString("Line");
  ob->AddString("Segment");
  ob->AddString("Face");
  spsu->SetAlpha(ob);*/
  //spsu->SetAlpha(new GUI::SpinButton(&gui, "SPIN", 100, 20));
  //spsu->SetAlpha(new GUI::Gauge(&gui, "GAUGE", 100, 20, 0, 1));
  spsu->SetAlpha(new GUI::TextInput(&gui, "INPUT", 100, 40));
  //spsu->SetOmega(new GUI::Box(&gui, "ColorBox", NULL, 100, 100, 100));
  spsu->SetOmega(new GUI::AsciiTable(&gui, "ASCII", 100,100));
  spu->SetAlpha(spsu);
  spu->SetOmega(new GUI::ColorEditor(&gui, "CE", 100, 100));

  DBGTrace("Initialized");
  Draw();
  DBGTrace("Drawed");
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
  case kUP :
    break;
  case kDN :
    break;
  case kRI :
    break;
  case kLE :
    break;
  case kTB :
    break;
  default : inherited::OnKeyPressed(aKey);
  }
}

void MainApp::OnMouseClick(int mousecode, int x, int y)
{
  gui.SendClick(mousecode, x, y);
  Draw();
}

void MainApp::OnDragEnd(int mousecode, int x, int y)
{
  gui.SendClick(mousecode, x, y);
  Draw();
}

void MainApp::Cycle()
{
  // don't draw each cycle, wait a refreh event from the gui
  Draw();
}

void MainApp::Draw()
{
  gui.Display();
  inherited::Draw();
}


int MainApp::Execute(GUI::Action * action, const char * NodeName)
{
  cout << "event : " << NodeName << ", " << action->Name() << ", " << action->Sender()->Name() << endl << flush;
  //if (SameStr(action->Name(), "Click") && SameStr(NodeName, "Exit"))
#ifdef UNDER_CE
  GUI::StringList * sl = (GUI::StringList *)(action->Sender());
#else
  GUI::StringList * sl = reinterpret_cast<GUI::StringList *>(action->Sender());
#endif
  if (SameStr(action->Name(), "Refresh"))
    inherited::Draw();
  if (SameStr(action->Name(), "Click") && SameStr(action->Sender()->Name(), "Menu"))
    if (sl->GetSelected() == 5)
    {
      reinterpret_cast<GUI::Box *>(guiRoot)->SetMargin(100);
      Draw();
      exit(0);
    }
  if (SameStr(NodeName, "First") && SameStr(action->Name(), "Click"))
    reinterpret_cast<GUI::Switcher *>(gui.SearchNode("SW"))->SetCurrentNode(0);
  if (SameStr(NodeName, "Last") && SameStr(action->Name(), "Click"))
    reinterpret_cast<GUI::Switcher *>(gui.SearchNode("SW"))->SetCurrentNode(1);
  if (SameStr(NodeName, "Clr") && SameStr(action->Name(), "Click"))
    reinterpret_cast<GUI::Switcher *>(gui.SearchNode("SW"))->SetCurrentNode(2);
  if (SameStr(NodeName, "Out") && SameStr(action->Name(), "Click"))
  {
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetChildFactor(1.0);
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetComplete();
  }
  if (SameStr(NodeName, "In") && SameStr(action->Name(), "Click"))
  {
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetChildFactor(0.5);
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetComplete();
  }
  if (SameStr(NodeName, "+") && SameStr(action->Name(), "Click"))
  {
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetRelativeFactor(0.25);
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetComplete();
  }
  if (SameStr(NodeName, "-") && SameStr(action->Name(), "Click"))
  {
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetRelativeFactor(0.0);
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetComplete();
  }
  if (SameStr(NodeName, "TV") && SameStr(action->Name(), "Click"))
  {
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetSelectedAsDisplayRoot();
  }
  if (SameStr(NodeName, "Up") && SameStr(action->Name(), "Click"))
  {
    //reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->GoUp();
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->SetDisplayRoot( 
       reinterpret_cast<GUI::Tree *>(gui.SearchNode("TV"))->GetRootNode());
  }


  if (SameStr(NodeName, "CE") && SameStr(action->Name(), "Click"))
  {
    GUI::Box * box = reinterpret_cast<GUI::Box *>(gui.SearchNode("ColorBox"));
    if (box != NULL) 
    {
      box->SetColor(
        reinterpret_cast<GUI::ColorEditor *>(gui.SearchNode("CE"))->GetSelectedColor());
        Draw();
    }
  }

  return 0;
}

GUI::TreeNode * MainApp::FillTree(GUI::TreeNode * node, XML::Element * element)
{
  GUI::TreeNode * subNode = new GUI::TreeNode(node);
  subNode->SetValue(element->Name());
  for (int i=0; i<element->ElementCount(); i++)
    FillTree(subNode, element->GetElement(i));
  return subNode;
}
