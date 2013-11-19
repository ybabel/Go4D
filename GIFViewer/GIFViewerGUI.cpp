#include "GIFViewerGUI.h"
#include "GUILoader.h"

MainApp::MainApp(Device & aDevice)
: inherited(aDevice),
drawer(fImage, fImage.Width(), fImage.Height()),
font("Font6x10.fon"),
layout(&font, true/*proportion*/, true/*animation*/),
gui(&fImage, guiRoot, &layout),
xmlGui("GIFViewerGUI.xml"),
dirRoot(".")
{
  imageFile = NULL;
  layout.ImportPalette(*fDevice->GetPalette());
  gui.SetActionListener(static_cast<GUI::ActionListener *>(this));
  Init();
}

void MainApp::Init()
{
  guiRoot = GUI::Loader::Read(&gui, xmlGui.Root()->GetElement(0));
  gui.SetRoot(guiRoot);
  GUI::Tree * tv = reinterpret_cast<GUI::Tree *>(gui.SearchNode("DirTree"));
  tv->SetRootNode(GUI::Loader::FillTree(NULL, dirRoot.Root()));
  tv->SetComplete();
  RefreshFileList();

  //Draw();
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
  //inherited::OnMouseClick(mousecode, x, y);
}

void MainApp::OnDragEnd(int mousecode, int x, int y)
{
  gui.SendClick(mousecode, x, y);
  Draw();
}

void MainApp::Cycle()
{
  // don't draw each cycle, wait a refreh event from the gui
  //Draw();
}

void MainApp::Draw()
{
  gui.Display();
  inherited::Draw();
}


int MainApp::Execute(GUI::Action * action, const char * NodeName)
{
  if (SameStr(action->Sender()->Name(), "Quit") && SameStr(action->Name(), "Click"))
    fDevice->Close();
  if (SameStr(action->Name(), "Refresh"))
    inherited::Draw();
  if (SameStr(action->Sender()->Name(), "DirTree") && SameStr(action->Name(), "Click"))
    reinterpret_cast<GUI::Tree *>(gui.SearchNode("DirTree"))->SetSelectedAsDisplayRoot();
  if (SameStr(action->Sender()->Name(), "DirTree") && SameStr(action->Name(), "Change"))
    RefreshFileList();
  if (SameStr(action->Sender()->Name(), "FileList") && SameStr(action->Name(), "Click"))
    DisplayFile();
  return 0;
}

void MainApp::RefreshFileList()
{
  GUI::TreeNode * current = reinterpret_cast<GUI::Tree *>(gui.SearchNode("DirTree"))->GetSelected();
  GUI::StringList * fl = reinterpret_cast<GUI::StringList *>(gui.SearchNode("FileList"));
  fl->Clear();
  DirectoryDescriptor * dir = (DirectoryDescriptor *)current->CustomData;
  for (int i=0; i<dir->FileCount(); i++)
    fl->AddString(dir->GetFile(i)->Name());
  fl->AutoSize();
}

void MainApp::DisplayFile()
{
  GUI::TreeNode * current = reinterpret_cast<GUI::Tree *>(gui.SearchNode("DirTree"))->GetSelected();
  GUI::StringList * fl = reinterpret_cast<GUI::StringList *>(gui.SearchNode("FileList"));
  GUI::ImageFrame * imf = reinterpret_cast<GUI::ImageFrame *>(gui.SearchNode("Image"));
  DirectoryDescriptor * dir = (DirectoryDescriptor *)current->CustomData;
  fstring fileName;
  CopyStr(fileName, dir->FullName() );
  ConcatStr(fileName, "\\");
  ConcatStr(fileName, fl->GetSelectedText());
  UpperStr(fileName);
  if (imageFile != NULL)
    delete imageFile;
  if (FinishStr(fileName, ".PCX"))
    imageFile = new Pcx(fileName);
  if (FinishStr(fileName, ".BMP"))
    imageFile = new Bmp(fileName);
  fDevice->ChangeCurrentPalette(& imageFile->GetPalette());
  fDevice->AcceptImage(&fImage);
  imf->SetImage(imageFile);
  layout.ImportPalette(*fDevice->GetPalette());
}
