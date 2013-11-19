#ifndef _FILES_H_
#define _FILES_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Files.h, directory and files browser
*CREATED: 14/01/01
*DATE   : 14/01/01
*OBJECT : browse a directory structure, build an information tree
*/

#include "Tools/apstring.h"
#include "Tools/List.h"

namespace Go4D
{

#ifdef VSNET
#define DirSep apstring("\\")
#else
#define DirSep apstring("/")
#endif


  class FileDescriptor;
  class DirectoryDescriptor;

  /// a file descriptor, to get it's name and directory
  class FileDescriptor
  {
    friend class DirectoryDescriptor;
    friend class DirectoryBrowser;
  public :
    ///
    FileDescriptor(const char * name, DirectoryDescriptor * directory);
    ///
    const char * Name() const;
    ///
    DirectoryDescriptor * Directory() const;
    ///
    const char * FullName() const;

  protected :
    apstring fName;
    apstring fFullName;
    DirectoryDescriptor * fDirectory;
    void SetComplete();
  };

  /// a directory descriptor, just a layer to describe directories content
  class DirectoryDescriptor
  {
    friend class DirectoryBrowser;
    friend class FileDescriptor;
  public :
    ///
    DirectoryDescriptor(const char * name, DirectoryDescriptor * parent);
    ///
    int ChildCount() const;
    ///
    DirectoryDescriptor * GetChild(int index) const;
    ///
    int FileCount() const;
    ///
    FileDescriptor * GetFile(int index) const;
    ///
    const char * Name() const;
    ///
    const char * FullName() const;

  protected :
    Array<DirectoryDescriptor *> fChilds;
    Array<FileDescriptor *> fFiles;
    DirectoryDescriptor * fParent;
    apstring fName;
    apstring fFullName;
    void AddFile(FileDescriptor * file);
    void AddChild(DirectoryDescriptor * directory);
    void SetComplete();
  };

  /// build a hierarchy of directory and files
  class DirectoryBrowser
  {
  public :
    ///  browse the root
    DirectoryBrowser(const char * root=".");
    ///
    DirectoryDescriptor * Root();
  protected :
    DirectoryDescriptor * ReadDir(const char * name, DirectoryDescriptor * parent=NULL);
    DirectoryDescriptor * fRoot;
    void Get(const char * name, bool isDir, DirectoryDescriptor * parent);
  };

}

#endif
