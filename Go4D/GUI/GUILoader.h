#ifndef _GUILOADER_H_
#define _GUILOADER_H_

/**
*AUTHOR : Babel yoann
*TITLE  : GUILoader.h
*CREATED: 14/01/01
*DATE   : 14/01/01
*OBJECT : This a set of helpers that allows to build a gui hierrarchy
*         from a description in an XML file
*/

#include "Gui.h"
#include "GUIOutput.h"
#include "GUIInput.h"
#include "Xml/XMLParser.h"
#include "Platform/Files.h"
#include "Editor.h"
#include "Tools/apstring.h"


namespace Go4D
{

  namespace GUI
  {

    /// a class that can build a GUI from various sources
    class Loader
    {
    public :
      ///  hand coded, no intelligence here
      static ANode * Read(System * gui, XML::Element * xmlRoot);
      ///
      static TreeNode * FillTree(TreeNode * node, XML::Element * element);
      ///
      static TreeNode * FillTree(TreeNode * node, DirectoryDescriptor * directory);
      ///
      static apstring GetID(Editable * aEditable);
      ///
      static void DisplayNode(TreeNode *, Editable *);
      ///
      static Editable * FindEditableFromID(const char * ID, Editable * root);
    protected :
    };

  }

}

#endif
