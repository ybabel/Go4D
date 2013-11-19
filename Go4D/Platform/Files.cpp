#include "Files.h"
#include "Tools/FString.h"

#ifdef VSNET
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#endif


namespace Go4D
{

#ifdef VSNET
#ifdef UNDER_CE
TCHAR buf3[512];
LPCTSTR StrToUnicode3(const char * mess)
{
  MultiByteToWideChar(
            CP_ACP,
            0,
            mess,
            strlen(mess)+1,
            (LPWSTR) buf3,
            sizeof(buf3));
  return buf3;
}

char buf4[512];
const char * UnicodeToStr(LPCTSTR mess)
{
  WideCharToMultiByte(CP_ACP, 0, mess,
    -1, buf4, sizeof(buf4), NULL, NULL);
  return buf4;
}
#endif
#endif

  //******************************************************

  FileDescriptor::FileDescriptor(const char * name, DirectoryDescriptor * directory)
  {
    fName = name;
    fDirectory = directory;
    fDirectory->AddFile(this);
  }

  const char * FileDescriptor::Name() const
  {
    return fName.c_str();
  }

  DirectoryDescriptor * FileDescriptor::Directory() const
  {
    return fDirectory;
  }

  const char * FileDescriptor::FullName() const
  {
    return fFullName.c_str();
  }

  void FileDescriptor::SetComplete()
  {
    fFullName = apstring(fDirectory->FullName())+DirSep+fName;
  }

  //******************************************************

  DirectoryDescriptor::DirectoryDescriptor(const char * name, DirectoryDescriptor * parent)
  {
    fName = name;
    fParent = parent;
    if (fParent != NULL)
      fParent->AddChild(this);
  }

  int DirectoryDescriptor::ChildCount() const
  {
    return fChilds.Count();
  }

  DirectoryDescriptor * DirectoryDescriptor::GetChild(int index) const
  {
    return fChilds[index];
  }

  int DirectoryDescriptor::FileCount() const
  {
    return fFiles.Count();
  }

  FileDescriptor * DirectoryDescriptor::GetFile(int index) const
  {
    return fFiles[index];
  }

  const char * DirectoryDescriptor::Name() const
  {
    return fName.c_str();
  }

  const char * DirectoryDescriptor::FullName() const
  {
    return fFullName.c_str();
  }

  void DirectoryDescriptor::AddFile(FileDescriptor * file)
  {
    fFiles.Add(file);
  }

  void DirectoryDescriptor::AddChild(DirectoryDescriptor * directory)
  {
    fChilds.Add(directory);
  }

  void DirectoryDescriptor::SetComplete()
  {
    if (fParent != NULL)
      fFullName = apstring(fParent->FullName())+DirSep+fName;
    else
      fFullName = fName;
    fFiles.Complete();
    fChilds.Complete();
    for (int i=0; i<fChilds.Count(); i++)
      fChilds[i]->SetComplete();
    for (int j=0; j<fFiles.Count(); j++)
      fFiles[j]->SetComplete();
  }

  //******************************************************

  DirectoryBrowser::DirectoryBrowser(const char * root)
  {
    fRoot = ReadDir(root);
    fRoot->SetComplete();
  }

  DirectoryDescriptor * DirectoryBrowser::Root()
  {
    return fRoot;
  }

  void DirectoryBrowser::Get(const char * name, bool isDir, DirectoryDescriptor * parent)
  {
    if (SameStr(name, "."))
      return;
    if (SameStr(name, ".."))
      return;
    if (isDir)
      ReadDir(name, parent);
    else
      new FileDescriptor(name, parent);
  }

#ifdef VSNET
  DirectoryDescriptor * DirectoryBrowser::ReadDir(const char * name, DirectoryDescriptor * parent)
  {
    DirectoryDescriptor * result = new DirectoryDescriptor(name, parent);
    result->SetComplete();
    apstring dirname = apstring(result->FullName())+DirSep+apstring("*.*");

    WIN32_FIND_DATA findData;
    HANDLE fileHandle;

#ifdef UNDER_CE
    fileHandle = FindFirstFile(StrToUnicode3(dirname.c_str()), &findData);
#else
    fileHandle = FindFirstFile(dirname.c_str(), &findData);
#endif
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
      // Loop on all remaining entries in the directory
#ifdef UNDER_CE
      Get(UnicodeToStr(findData.cFileName), (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0, result);
#else
      Get(findData.cFileName, (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0, result);
#endif
      while (FindNextFile(fileHandle, &findData))
      {
#ifdef UNDER_CE
        Get(UnicodeToStr(findData.cFileName), (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0, result);
#else
        Get(findData.cFileName, (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0, result);
#endif
      }
    }
    else
    if (fileHandle != INVALID_HANDLE_VALUE )
      FindClose(fileHandle);
    return result;
  }
#else
  DirectoryDescriptor * DirectoryBrowser::ReadDir(const char * name, DirectoryDescriptor * parent)
  {
    DirectoryDescriptor * result = new DirectoryDescriptor(name, parent);
    result->SetComplete();
    apstring dirname = apstring(result->FullName());
    struct dirent * dp;
    DIR * dd;
    dd = opendir(name);
    while((dp = readdir(dd)) != NULL)
    {
        apstring fullname = dirname+DirSep+apstring(dp->d_name);
        struct stat file_info;
        stat(fullname.c_str(), &file_info);
        bool is_dir = S_ISDIR(file_info.st_mode);
        Get(dp->d_name, is_dir, result);
    }
    closedir(dd);
    return result;
  }
#endif

}


